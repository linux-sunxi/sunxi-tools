/*
 * (C) Copyright 2016 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fel_lib.h"
#include "progress.h"

#include "fel-remotefunc-spi-data-transfer.h"

/*****************************************************************************/

typedef struct {
	uint32_t  id;
	uint8_t   write_enable_cmd;
	uint8_t   large_erase_cmd;
	uint32_t  large_erase_size;
	uint8_t   small_erase_cmd;
	uint32_t  small_erase_size;
	uint8_t   program_cmd;
	uint32_t  program_size;
	char     *text_description;
} spi_flash_info_t;

spi_flash_info_t spi_flash_info[] = {
	{ .id = 0xEF40, .write_enable_cmd = 0x6,
	  .large_erase_cmd = 0xD8, .large_erase_size = 64 * 1024,
	  .small_erase_cmd = 0x20, .small_erase_size =  4 * 1024,
	  .program_cmd = 0x02, .program_size = 256,
	  .text_description = "Winbond W25Qxx" },
	{ .id = 0xC220, .write_enable_cmd = 0x6,
	  .large_erase_cmd = 0xD8, .large_erase_size = 64 * 1024,
	  .small_erase_cmd = 0x20, .small_erase_size =  4 * 1024,
	  .program_cmd = 0x02, .program_size = 256,
	  .text_description = "Macronix MX25Lxxxx" },
};

spi_flash_info_t default_spi_flash_info = {
	.id = 0x0000, .write_enable_cmd = 0x6,
	.large_erase_cmd = 0xD8, .large_erase_size = 64 * 1024,
	.small_erase_cmd = 0x20, .small_erase_size =  4 * 1024,
	.program_cmd = 0x02, .program_size = 256,
	.text_description = "Unknown",
};

/*****************************************************************************/

uint32_t fel_readl(feldev_handle *dev, uint32_t addr);
void fel_writel(feldev_handle *dev, uint32_t addr, uint32_t val);
#define readl(addr)                 fel_readl(dev, (addr))
#define writel(val, addr)           fel_writel(dev, (addr), (val))

#define PA                          (0)
#define PB                          (1)
#define PC                          (2)

#define CCM_SPI0_CLK                (0x01C20000 + 0xA0)
#define CCM_AHB_GATING0             (0x01C20000 + 0x60)
#define CCM_AHB_GATE_SPI0           (1 << 20)
#define SUN6I_BUS_SOFT_RST_REG0     (0x01C20000 + 0x2C0)
#define SUN6I_SPI0_RST              (1 << 20)

#define SUNXI_GPC_SPI0              (3)
#define SUN50I_GPC_SPI0             (4)

#define SUN4I_CTL_ENABLE            (1 << 0)
#define SUN4I_CTL_MASTER            (1 << 1)
#define SUN4I_CTL_TF_RST            (1 << 8)
#define SUN4I_CTL_RF_RST            (1 << 9)
#define SUN4I_CTL_XCH               (1 << 10)

#define SUN6I_TCR_XCH               (1 << 31)

#define SUN4I_SPI0_CCTL             (0x01C05000 + 0x1C)
#define SUN4I_SPI0_CTL              (0x01C05000 + 0x08)
#define SUN4I_SPI0_RX               (0x01C05000 + 0x00)
#define SUN4I_SPI0_TX               (0x01C05000 + 0x04)
#define SUN4I_SPI0_FIFO_STA         (0x01C05000 + 0x28)
#define SUN4I_SPI0_BC               (0x01C05000 + 0x20)
#define SUN4I_SPI0_TC               (0x01C05000 + 0x24)

#define SUN6I_SPI0_CCTL             (0x01C68000 + 0x24)
#define SUN6I_SPI0_GCR              (0x01C68000 + 0x04)
#define SUN6I_SPI0_TCR              (0x01C68000 + 0x08)
#define SUN6I_SPI0_FIFO_STA         (0x01C68000 + 0x1C)
#define SUN6I_SPI0_MBC              (0x01C68000 + 0x30)
#define SUN6I_SPI0_MTC              (0x01C68000 + 0x34)
#define SUN6I_SPI0_BCC              (0x01C68000 + 0x38)
#define SUN6I_SPI0_TXD              (0x01C68000 + 0x200)
#define SUN6I_SPI0_RXD              (0x01C68000 + 0x300)

#define CCM_SPI0_CLK_DIV_BY_2       (0x1000)
#define CCM_SPI0_CLK_DIV_BY_4       (0x1001)
#define CCM_SPI0_CLK_DIV_BY_6       (0x1002)

/*
 * Configure pin function on a GPIO port
 */
static void gpio_set_cfgpin(feldev_handle *dev, int port_num, int pin_num,
			    int val)
{
	uint32_t port_base = 0x01C20800 + port_num * 0x24;
	uint32_t cfg_reg   = port_base + 4 * (pin_num / 8);
	uint32_t pin_idx   = pin_num % 8;
	uint32_t x = readl(cfg_reg);
	x &= ~(0x7 << (pin_idx * 4));
	x |= val << (pin_idx * 4);
	writel(x, cfg_reg);
}

static bool spi_is_sun6i(feldev_handle *dev)
{
	soc_info_t *soc_info = dev->soc_info;
	switch (soc_info->soc_id) {
	case 0x1623: /* A10 */
	case 0x1625: /* A13 */
	case 0x1651: /* A20 */
		return false;
	default:
		return true;
	}
}

/*
 * Init the SPI0 controller and setup pins muxing.
 */
static bool spi0_init(feldev_handle *dev)
{
	uint32_t reg_val;
	soc_info_t *soc_info = dev->soc_info;
	if (!soc_info) {
		printf("Unable to fetch device information. "
		       "Possibly unknown device.\n");
		return false;
	}

	/* Setup SPI0 pins muxing */
	switch (soc_info->soc_id) {
	case 0x1625: /* Allwinner A13 */
	case 0x1680: /* Allwinner H3 */
	case 0x1718: /* Allwinner H5 */
		gpio_set_cfgpin(dev, PC, 0, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 1, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 2, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 3, SUNXI_GPC_SPI0);
		break;
	case 0x1651: /* Allwinner A20 */
		gpio_set_cfgpin(dev, PC, 0, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 1, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 2, SUNXI_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 23, SUNXI_GPC_SPI0);
		break;
	case 0x1689: /* Allwinner A64 */
		gpio_set_cfgpin(dev, PC, 0, SUN50I_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 1, SUN50I_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 2, SUN50I_GPC_SPI0);
		gpio_set_cfgpin(dev, PC, 3, SUN50I_GPC_SPI0);
		break;
	default: /* Unknown/Unsupported SoC */
		printf("SPI support not implemented yet for %x (%s)!\n",
		       soc_info->soc_id, soc_info->name);
		return false;
	}

	reg_val = readl(CCM_AHB_GATING0);
	reg_val |= CCM_AHB_GATE_SPI0;
	writel(reg_val, CCM_AHB_GATING0);

	/* 24MHz from OSC24M */
	writel((1 << 31), CCM_SPI0_CLK);
	/* divide by 4 */
	writel(CCM_SPI0_CLK_DIV_BY_4, spi_is_sun6i(dev) ? SUN6I_SPI0_CCTL :
							  SUN4I_SPI0_CCTL);

	if (spi_is_sun6i(dev)) {
		/* Deassert SPI0 reset */
		reg_val = readl(SUN6I_BUS_SOFT_RST_REG0);
		reg_val |= SUN6I_SPI0_RST;
		writel(reg_val, SUN6I_BUS_SOFT_RST_REG0);
		/* Enable SPI in the master mode and do a soft reset */
		reg_val = readl(SUN6I_SPI0_GCR);
		reg_val |= (1 << 31) | 3;
		writel(reg_val, SUN6I_SPI0_GCR);
		/* Wait for completion */
		while (readl(SUN6I_SPI0_GCR) & (1 << 31)) {}
	} else {
		reg_val = readl(SUN4I_SPI0_CTL);
		reg_val |= SUN4I_CTL_MASTER;
		reg_val |= SUN4I_CTL_ENABLE | SUN4I_CTL_TF_RST | SUN4I_CTL_RF_RST;
		writel(reg_val, SUN4I_SPI0_CTL);
	}

	return true;
}

/*
 * Backup/restore the initial portion of the SRAM, which can be used as
 * a temporary data buffer.
 */
static void *backup_sram(feldev_handle *dev)
{
	soc_info_t *soc_info = dev->soc_info;
	size_t bufsize = soc_info->scratch_addr - soc_info->spl_addr;
	void *buf = malloc(bufsize);
	aw_fel_read(dev, soc_info->spl_addr, buf, bufsize);
	return buf;
}

static void restore_sram(feldev_handle *dev, void *buf)
{
	soc_info_t *soc_info = dev->soc_info;
	size_t bufsize = soc_info->scratch_addr - soc_info->spl_addr;
	aw_fel_write(dev, buf, soc_info->spl_addr, bufsize);
	free(buf);
}

static void prepare_spi_batch_data_transfer(feldev_handle *dev, uint32_t buf)
{
	if (spi_is_sun6i(dev)) {
		aw_fel_remotefunc_prepare_spi_batch_data_transfer(dev,
							    buf,
							    SUN6I_SPI0_TCR,
							    SUN6I_TCR_XCH,
							    SUN6I_SPI0_FIFO_STA,
							    SUN6I_SPI0_TXD,
							    SUN6I_SPI0_RXD,
							    SUN6I_SPI0_MBC,
							    SUN6I_SPI0_MTC,
							    SUN6I_SPI0_BCC);
	} else {
		aw_fel_remotefunc_prepare_spi_batch_data_transfer(dev,
							    buf,
							    SUN4I_SPI0_CTL,
							    SUN4I_CTL_XCH,
							    SUN4I_SPI0_FIFO_STA,
							    SUN4I_SPI0_TX,
							    SUN4I_SPI0_RX,
							    SUN4I_SPI0_BC,
							    SUN4I_SPI0_TC,
							    0);
	}
}

/*
 * Read data from the SPI flash. Use the first 4KiB of SRAM as the data buffer.
 */
void aw_fel_spiflash_read(feldev_handle *dev,
			  uint32_t offset, void *buf, size_t len,
			  progress_cb_t progress)
{
	soc_info_t *soc_info = dev->soc_info;
	void *backup = backup_sram(dev);
	uint8_t *buf8 = (uint8_t *)buf;
	size_t max_chunk_size = soc_info->scratch_addr - soc_info->spl_addr;
	if (max_chunk_size > 0x1000)
		max_chunk_size = 0x1000;
	uint8_t *cmdbuf = malloc(max_chunk_size);
	memset(cmdbuf, 0, max_chunk_size);
	aw_fel_write(dev, cmdbuf, soc_info->spl_addr, max_chunk_size);

	if (!spi0_init(dev))
		return;

	prepare_spi_batch_data_transfer(dev, soc_info->spl_addr);

	progress_start(progress, len);
	while (len > 0) {
		size_t chunk_size = len;
		if (chunk_size > max_chunk_size - 8)
			chunk_size = max_chunk_size - 8;

		memset(cmdbuf, 0, max_chunk_size);
		cmdbuf[0] = (chunk_size + 4) >> 8;
		cmdbuf[1] = (chunk_size + 4);
		cmdbuf[2] = 3;
		cmdbuf[3] = offset >> 16;
		cmdbuf[4] = offset >> 8;
		cmdbuf[5] = offset;

		if (chunk_size == max_chunk_size - 8)
			aw_fel_write(dev, cmdbuf, soc_info->spl_addr, 6);
		else
			aw_fel_write(dev, cmdbuf, soc_info->spl_addr, chunk_size + 8);
		aw_fel_remotefunc_execute(dev, NULL);
		aw_fel_read(dev, soc_info->spl_addr + 6, buf8, chunk_size);

		len -= chunk_size;
		offset += chunk_size;
		buf8 += chunk_size;
		progress_update(chunk_size);
	}

	free(cmdbuf);
	restore_sram(dev, backup);
}

/*
 * Write data to the SPI flash. Use the first 4KiB of SRAM as the data buffer.
 */

#define CMD_WRITE_ENABLE 0x06

void aw_fel_spiflash_write_helper(feldev_handle *dev,
				  uint32_t offset, void *buf, size_t len,
				  size_t erase_size, uint8_t erase_cmd,
				  size_t program_size, uint8_t program_cmd)
{
	soc_info_t *soc_info = dev->soc_info;
	uint8_t *buf8 = (uint8_t *)buf;
	size_t max_chunk_size = soc_info->scratch_addr - soc_info->spl_addr;
	size_t cmd_idx;

	if (max_chunk_size > 0x1000)
		max_chunk_size = 0x1000;
	uint8_t *cmdbuf = malloc(max_chunk_size);
	cmd_idx = 0;

	prepare_spi_batch_data_transfer(dev, soc_info->spl_addr);

	while (len > 0) {
		while (len > 0 && max_chunk_size - cmd_idx > program_size + 64) {
			if (offset % erase_size == 0) {
				/* Emit write enable command */
				cmdbuf[cmd_idx++] = 0;
				cmdbuf[cmd_idx++] = 1;
				cmdbuf[cmd_idx++] = CMD_WRITE_ENABLE;
				/* Emit erase command */
				cmdbuf[cmd_idx++] = 0;
				cmdbuf[cmd_idx++] = 4;
				cmdbuf[cmd_idx++] = erase_cmd;
				cmdbuf[cmd_idx++] = offset >> 16;
				cmdbuf[cmd_idx++] = offset >> 8;
				cmdbuf[cmd_idx++] = offset;
				/* Emit wait for completion */
				cmdbuf[cmd_idx++] = 0xFF;
				cmdbuf[cmd_idx++] = 0xFF;
			}
			/* Emit write enable command */
			cmdbuf[cmd_idx++] = 0;
			cmdbuf[cmd_idx++] = 1;
			cmdbuf[cmd_idx++] = CMD_WRITE_ENABLE;
			/* Emit page program command */
			size_t write_count = program_size;
			if (write_count > len)
				write_count = len;
			cmdbuf[cmd_idx++] = (4 + write_count) >> 8;
			cmdbuf[cmd_idx++] = 4 + write_count;
			cmdbuf[cmd_idx++] = program_cmd;
			cmdbuf[cmd_idx++] = offset >> 16;
			cmdbuf[cmd_idx++] = offset >> 8;
			cmdbuf[cmd_idx++] = offset;
			memcpy(cmdbuf + cmd_idx, buf8, write_count);
			cmd_idx += write_count;
			buf8    += write_count;
			len     -= write_count;
			offset  += write_count;
			/* Emit wait for completion */
			cmdbuf[cmd_idx++] = 0xFF;
			cmdbuf[cmd_idx++] = 0xFF;
		}
		/* Emit the end marker */
		cmdbuf[cmd_idx++] = 0;
		cmdbuf[cmd_idx++] = 0;

		/* Flush */
		aw_fel_write(dev, cmdbuf, soc_info->spl_addr, cmd_idx);
		aw_fel_remotefunc_execute(dev, NULL);
		cmd_idx = 0;
	}

	free(cmdbuf);
}

void aw_fel_spiflash_write(feldev_handle *dev,
			   uint32_t offset, void *buf, size_t len,
			   progress_cb_t progress)
{
	void *backup = backup_sram(dev);
	uint8_t *buf8 = (uint8_t *)buf;

	spi_flash_info_t *flash_info = &default_spi_flash_info; /* FIXME */

	if ((offset % flash_info->small_erase_size) != 0) {
		fprintf(stderr, "aw_fel_spiflash_write: 'addr' must be %d bytes aligned\n",
		        flash_info->small_erase_size);
		exit(1);
	}

	if (!spi0_init(dev))
		return;

	progress_start(progress, len);
	while (len > 0) {
		size_t write_count;
		if ((offset % flash_info->large_erase_size) != 0 ||
							len < flash_info->large_erase_size) {

			write_count = flash_info->small_erase_size;
			if (write_count > len)
				write_count = len;
			aw_fel_spiflash_write_helper(dev, offset, buf8,
				write_count,
				flash_info->small_erase_size, flash_info->small_erase_cmd,
				flash_info->program_size, flash_info->program_cmd);
		} else {
			write_count = flash_info->large_erase_size;
			if (write_count > len)
				write_count = len;
			aw_fel_spiflash_write_helper(dev, offset, buf8,
				write_count,
				flash_info->large_erase_size, flash_info->large_erase_cmd,
				flash_info->program_size, flash_info->program_cmd);
		}

		len    -= write_count;
		offset += write_count;
		buf8   += write_count;
		progress_update(write_count);
	}

	restore_sram(dev, backup);
}

/*
 * Use the read JEDEC ID (9Fh) command.
 */
void aw_fel_spiflash_info(feldev_handle *dev)
{
	soc_info_t *soc_info = dev->soc_info;
	const char *manufacturer;
	unsigned char buf[] = { 0, 4, 0x9F, 0, 0, 0, 0x0, 0x0 };
	void *backup = backup_sram(dev);

	if (!spi0_init(dev))
		return;

	aw_fel_write(dev, buf, soc_info->spl_addr, sizeof(buf));
	prepare_spi_batch_data_transfer(dev, soc_info->spl_addr);
	aw_fel_remotefunc_execute(dev, NULL);
	aw_fel_read(dev, soc_info->spl_addr, buf, sizeof(buf));

	restore_sram(dev, backup);

	/* Assume that the MISO pin is either pulled up or down */
	if (buf[5] == 0x00 || buf[5] == 0xFF) {
		printf("No SPI flash detected.\n");
		return;
	}

	switch (buf[3]) {
	case 0xEF:
		manufacturer = "Winbond";
		break;
	case 0xC2:
		manufacturer = "Macronix";
		break;
	default:
		manufacturer = "Unknown";
		break;
	}

	printf("Manufacturer: %s (%02Xh), model: %02Xh, size: %d bytes.\n",
	       manufacturer, buf[3], buf[4], (1 << buf[5]));
}

/*
 * Show a help message about the available "spiflash-*" commands.
 */
void aw_fel_spiflash_help(void)
{
	printf("	spiflash-info			Retrieves basic information\n"
	       "	spiflash-read addr length file	Write SPI flash contents into file\n"
	       "	spiflash-write addr file	Store file contents into SPI flash\n");
}

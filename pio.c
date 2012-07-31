/* needs _BSD_SOURCE for htole and letoh  */
#define _BSD_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <endian.h>

#define PIO_REG_SIZE 0x228 /*0x300*/
#define PIO_PORT_SIZE 0x24

#define errf(...)	fprintf(stderr, __VA_ARGS__)

static inline int _read(int fd, char *buf, size_t l)
{
	ssize_t rc = 0;
	while (l > 0) {
		rc = read(fd, buf, l);
		if (rc >= 0)
			break;
		else if (rc < 0 && errno != EINTR) {
			errf("read: %s\n", strerror(errno));
			break;
		}
	}

	return rc;
}

static int read_fixed_stdin(char *buf, size_t wanted)
{
	ssize_t rc;
	char eof;

	while (wanted > 0) {
		rc = _read(0, buf, wanted);
		if (rc > 0) {
			wanted -= rc;
			buf += rc;
		} else if (rc == 0) {
			errf("read: input too small\n");
			goto fail;
		} else if (rc < 0) {
			goto fail;
		}
	}

	if (_read(0, &eof, 1) == 0)
		return 1;

	errf("read: input too large\n");
fail:
	return 0;
}

struct pio_status {
	int mul_sel;
	int pull;
	int drv_level;
	int data;
};

#define PIO_REG_CFG(B, N, I)	((B) + (N)*0x24 + ((I)<<2) + 0x00)
#define PIO_REG_DLEVEL(B, N, I)	((B) + (N)*0x24 + ((I)<<2) + 0x14)
#define PIO_REG_PULL(B, N, I)	((B) + (N)*0x24 + ((I)<<2) + 0x1C)
#define PIO_REG_DATA(B, N)	((B) + (N)*0x24 + 0x10)

#define LE32TOH(X)		le32toh(*((uint32_t*)(X)))

static int pio_get(const char *buf, uint32_t port, uint32_t port_num, struct pio_status *pio)
{
	uint32_t val;
	uint32_t port_num_func, port_num_pull;
	uint32_t offset_func, offset_pull;

	port_num_func = port_num >> 3;
	offset_func = ((port_num & 0x07) << 2);

	port_num_pull = port_num >> 4;
	offset_pull = ((port_num & 0x0f) << 1);

	/* func */
	val = LE32TOH(PIO_REG_CFG(buf, port, port_num_func));
	pio->mul_sel = (val>>offset_func) & 0x07;

	/* pull */
	val = LE32TOH(PIO_REG_PULL(buf, port, port_num_pull));
	pio->pull = (val>>offset_pull) & 0x03;

	/* dlevel */
	val = LE32TOH(PIO_REG_DLEVEL(buf, port, port_num_pull));
	pio->drv_level = (val>>offset_pull) & 0x03;

	/* i/o data */
	if (pio->mul_sel > 1)
		pio->data = -1;
	else {
		val = LE32TOH(PIO_REG_DATA(buf, port));
		pio->data = (val >> port_num) & 0x01;
	}
	return 1;
}

static int pio_set(char *buf, uint32_t port, uint32_t port_num, struct pio_status *pio)
{
	uint32_t *addr, val;
	uint32_t port_num_func, port_num_pull;
	uint32_t offset_func, offset_pull;

	port_num_func = port_num >> 3;
	offset_func = ((port_num & 0x07) << 2);

	port_num_pull = port_num >> 4;
	offset_pull = ((port_num & 0x0f) << 1);

	/* func */
	if (pio->mul_sel >= 0) {
		addr = (uint32_t*)PIO_REG_CFG(buf, port, port_num_func);
		val = le32toh(*addr);
		val &= ~(0x07 << offset_func);
		val |=  (pio->mul_sel & 0x07) << offset_func;
		*addr = htole32(val);
	}

	/* pull */
	if (pio->pull >= 0) {
		addr = (uint32_t*)PIO_REG_PULL(buf, port, port_num_pull);
		val = le32toh(*addr);
		val &= ~(0x03 << offset_pull);
		val |=  (pio->pull & 0x03) << offset_pull;
		*addr = htole32(val);
	}

	/* dlevel */
	if (pio->drv_level >= 0) {
		addr = (uint32_t*)PIO_REG_DLEVEL(buf, port, port_num_pull);
		val = le32toh(*addr);
		val &= ~(0x03 << offset_pull);
		val |=  (pio->pull & 0x03) << offset_pull;
		*addr = htole32(val);
	}

	/* data */
	if (pio->data >= 0) {
		addr = (uint32_t*)PIO_REG_DATA(buf, port);
		val = le32toh(*addr);
		if (pio->data)
			val |= (0x01 << port_num);
		else
			val &= ~(0x01 << port_num);
		*addr = htole32(val);
	}

	return 1;
}

static void print(const char *buf)
{
	int port, i;
	struct pio_status pio;
	for (port=0; port < 10 /*PIO_REG_SIZE/0x24*/; port++) {
		for (i=0; i<28; i++) {
			if (pio_get(buf, port, i, &pio)) {
				printf("P%c%d", 'A'+port, i+1);
				printf("<%x>", pio.mul_sel);
				printf("<%x>", pio.pull);
				printf("<%x>", pio.drv_level);
				if (pio.data >= 0)
					printf("<%x>", pio.data);

				fputc('\n', stdout);
			}
		}
	}
}
static int read_and_print(void)
{
	char buf[PIO_REG_SIZE];

	if (read_fixed_stdin(buf, PIO_REG_SIZE)) {
		print(buf);
		return 0;
	}
	return -1;
}

static inline int usage(const char *argv0)
{
	fprintf(stderr, "usage: %s print < PIO\n", argv0);
	return 1;
}

int main(int argc, char **argv)
{
	if (argc == 2 && (strncmp("print", argv[1], 5) == 0))
		return read_and_print();
	else
		return usage(argv[0]);
}


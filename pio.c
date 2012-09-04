/* needs _BSD_SOURCE for htole and letoh  */
#define _BSD_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "endian_compat.h"

#define PIO_REG_SIZE 0x228 /*0x300*/
#define PIO_PORT_SIZE 0x24

#define errf(...)	fprintf(stderr, __VA_ARGS__)

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
#define PIO_NR_PORTS		9 /* A-I */

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
		val |=  (pio->drv_level & 0x03) << offset_pull;
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

static void pio_print(int port, int port_nr, struct pio_status *pio)
{
	printf("P%c%d", 'A'+port, port_nr);
	printf("<%x>", pio->mul_sel);
	printf("<%x>", pio->pull);
	printf("<%x>", pio->drv_level);
	if (pio->data >= 0)
		printf("<%x>", pio->data);
	fputc('\n', stdout);
}

static void print(const char *buf)
{
	int port, i;
	struct pio_status pio;
	for (port=0; port < PIO_NR_PORTS; port++) {
		for (i=0; i<32; i++) {
			if (pio_get(buf, port, i, &pio)) {
				pio_print(port, i, &pio);
			}
		}
	}
}

static const char *argv0;

static void usage(int rc )
{
			
	fprintf(stderr, "usage: %s -i input [-o output] pin..\n", argv0);
	fprintf(stderr," print				Show all pins\n");
	fprintf(stderr," Pxx				Show pin\n");
	fprintf(stderr," Pxx<mode><pull><drive><data>	Configure pin\n");
	fprintf(stderr," Pxx=data,drive			Configure GPIO output\n");
	fprintf(stderr," Pxx?pull			Configure GPIO input\n");
	fprintf(stderr," clean				Clean input pins\n");
	fprintf(stderr, "\n	mode 0-7, 0=input, 1=ouput, 2-7 I/O function\n");
	fprintf(stderr, "	pull 0=none, 1=up, 2=down\n");
	fprintf(stderr, "	drive 0-3, I/O drive level\n");
	
	exit(rc);
}

static void parse_pin(int *port, int *pin, const char *name)
{
	if (*name == 'P') name++;
	*port = *name++ - 'A';
	*pin = atoi(name);
}

static void cmd_show_pin(char *buf, const char *pin)
{
	int port, port_nr;
	struct pio_status pio;
	parse_pin(&port, &port_nr, pin);
	if (!pio_get(buf, port, port_nr, &pio))
		usage(1);
    	pio_print(port, port_nr, &pio);
}

static int parse_int(int *dst, const char *in)
{
	int value;
	char *next;
	errno = 0;
	value = strtol(in, &next, 0);
	if (!errno && next != in) {
		*dst = value;
		return 0;
	}
	return -1;
}

static void cmd_set_pin(char *buf, const char *pin)
{
	int port, port_nr;
	const char *t = pin;
	struct pio_status pio;
	parse_pin(&port, &port_nr, pin);
	if (!pio_get(buf, port, port_nr, &pio))
		usage(1);
	if ((t = strchr(pin, '='))) {
		pio.mul_sel = 1;
		if (t) {
			t++;
			parse_int(&pio.data, t);
		}
		if (t)
			t = strchr(t, ',');
		if (t) {
			t++;
			parse_int(&pio.drv_level, t);
		}
	} else if ((t = strchr(pin, '?'))) {
		pio.mul_sel = 0;
		pio.data = 0;
		pio.drv_level = 0;
		if (t) {
			t++;
			parse_int(&pio.pull, t);
		}
	} else if ((t = strchr(pin, '<'))) {
		if (t) {
			t++;
			parse_int(&pio.mul_sel, t);
		}
		if (t)
			t = strchr(t, '<');
		if (t) {
			t++;
			parse_int(&pio.pull, t);
		}
		if (t)
			t = strchr(t, '<');
		if (t) {
			t++;
			parse_int(&pio.drv_level, t);
		}
		if (t)
			t = strchr(t, '<');
		if (t) {
			t++;
			parse_int(&pio.data, t);
		}
	}
	pio_set(buf, port, port_nr, &pio);
}

static void cmd_clean(char *buf)
{
	int port, i;
	struct pio_status pio;
	for (port=0; port < PIO_NR_PORTS; port++) {
		for (i=0; i<32; i++) {
			if (pio_get(buf, port, i, &pio)) {
				if (pio.mul_sel == 0) {
					pio.data = 0;
					pio_set(buf, port, i, &pio);
				}
			}
		}
	}
}

static int do_command(char *buf, const char **args, int argc)
{
	const char *command = args[0];
	if (*command == 'P') {
		if (strchr(command, '<'))
			cmd_set_pin(buf, command);
		else if (strchr(command, '='))
			cmd_set_pin(buf, command);
		else if (strchr(command, '?'))
			cmd_set_pin(buf, command);
		else
			cmd_show_pin(buf, command);
	}
	else if (strcmp(command, "print") == 0)
		print(buf);
	else if (strcmp(command, "clean") == 0)
		cmd_clean(buf);
	else	usage(1);
	return 1;
}

int main(int argc, char **argv)
{
	int opt;
	FILE *in = NULL;
	FILE *out = NULL;
	const char *in_name = NULL;
	const char *out_name = NULL;
	char buf[PIO_REG_SIZE];

	argv0 = argv[0];

	while ((opt = getopt(argc, argv, "i:o:")) != -1) {
		switch(opt) {
		case '?':
			usage(0);
		case 'i':
			in_name = optarg;
			break;
		case 'o':
			out_name = optarg;
			break;
		}
	}
	if (!in_name)
		usage(1);
	if (in_name) {
		if (strcmp(in_name, "-") == 0) {
			in = stdin;
		} else {
			in = fopen(in_name, "rb");
			if (!in) {
				perror("open input");
				exit(1);
			}
		}
	}
	if (fread(buf, sizeof(buf), 1, in) != 1) {
		perror("read input");
		exit(1);
	}
	if (in != stdin)
		fclose(in);

	while(optind < argc) {
		optind += do_command(buf, (const char **)(argv + optind), argc - optind);
	}

	if (out_name) {
		if (strcmp(out_name, "-") == 0) {
			out = stdout;
		} else {
			out = fopen(out_name, "wb");
			if (!out) {
				perror("open output");
				exit(1);
			}
		}
		if (fwrite(buf, sizeof(buf), 1, out) != 1) {
			perror("write output");
			exit(1);
		}
	}
}

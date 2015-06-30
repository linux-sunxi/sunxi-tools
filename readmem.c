#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	void *map_base;
	off_t map_addr;
	off_t size;
	int fp;
	size_t i;

	if(argc < 2)
	{
		fprintf(stderr, "Usage %s: <start_address> <size> > file\n", argv[0]);
		return -1;
	}
	
	map_addr = strtoul(argv[1], NULL, 0);
	size = strtoul(argv[2], NULL, 0);

	fprintf(stderr, "Address 0x%x Size 0x%x\n", (unsigned long)map_addr, size);
	fp = open("/dev/mem", O_RDONLY);

	if(fp == -1)
	{
		fprintf(stderr, "Can 't open /dev/mem!");
	}

	map_base = mmap(NULL, size, PROT_READ, MAP_SHARED, fp, map_addr);
	for(i = 0; i < size; i++)
	{
		putchar(*((unsigned char *)map_base + i));
	}

	munmap(map_base, size);
	close(fp);
}

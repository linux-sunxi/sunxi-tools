/*
 * Copyright (C) 2015 Olliver Schinagl <oliver@schinagl.nl>
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SCRIPT_START	0x43000000
#define SCRIPT_SIZE	0x20000

int main(void) {
	char *addr;
	int fd;
	int i;

	fd = open("/dev/mem", O_RDONLY);
	addr = (char *)mmap(NULL, SCRIPT_SIZE, PROT_READ, MAP_SHARED, fd, SCRIPT_START);
	for (i = 0; i < SCRIPT_SIZE; i++)
		putchar(addr[i]);
	munmap(addr, SCRIPT_SIZE);
	close(fd);

	return 0;
}

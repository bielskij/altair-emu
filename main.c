/*
 * main.c
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common/utils/ihex.h"

#include "emulator/cpu.h"
#include "emulator/memory.h"
#include "emulator/io.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

#define MEM_SIZE (64 * 1024)

struct _IO {
	_U8 reserved;
};

struct _Memory {
	_U8 space[MEM_SIZE];
};


static struct _Memory _mem;
static struct _IO     _io;

static Cpu _cpu;


_U8  IO_read_byte(IO io, _U8 address) {
	DBG(("CALL <- [%#02x]", address));

	return 0;
}


void IO_write_byte(IO io, _U8 address, _U8 data) {
	DBG(("CALL %#02x -> [%#02x (%c)]", address, data, data));
}


extern _U8  memory_read_byte(Memory memory, _U16 address) {
	return memory->space[address];
}


extern void memory_write_byte(Memory memory, _U16 address, _U8 data) {
	memory->space[address] = data;
}


static _U8 _loadHex(char *path) {
	_U8 ret = 0;

	DBG(("Opening file: %s", path));

	{
		FILE *fd = fopen(path, "rb");
		if (fd) {
			char *line = NULL;
			size_t n;

			while (getline(&line, &n, fd) > 0) {
				ihex_line_to_binary(line, _mem.space, sizeof(_mem.space));

				free(line);
				line = NULL;
			}

			fclose(fd);
		}
	}

	return ret;
}


static void _dumpMemory(Memory mem) {
	_U32 i = 0;
	_U8 j;

	const _U8 lineLength = 32;

	for (i = 0; i < sizeof(mem->space); i += lineLength) {
		printf("%04x:", i);

		for (j = 0; j < lineLength; j++) {
			printf(" %02x", mem->space[i + j]);
		}

		printf("   ");

		for (j = 0; j < lineLength; j++) {
			char c = mem->space[i + j];

			if (! isalnum(c)) {
				c = '.';
			}

			printf("%c", c);
		}

		printf("\n");
	}
}


int main(int argc, char *argv[]) {
	DBG(("START"));

	// Clear memory
	memset(_mem.space, 0, sizeof(_mem.space));

	if (argc > 1) {
		_loadHex(argv[1]);
	}

	cpu_init(&_cpu, &_mem, &_io);
	cpu_reset(&_cpu);

	cpu_loop(&_cpu);
//	_dumpMemory(&_mem);
}

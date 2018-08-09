/*******************************************************************************
 * Copyright (C) 2018 Jaroslaw Bielski (bielski.j@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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

static Cpu _cpu;
static _U8 _memory[MEM_SIZE];



static _U8 _loadHex(char *path) {
	_U8 ret = 0;

	DBG(("Opening file: %s", path));

	{
		FILE *fd = fopen(path, "rb");
		if (fd) {
			char *line = NULL;
			size_t n;

			while (getline(&line, &n, fd) > 0) {
				ihex_line_to_binary(line, _memory, sizeof(_memory));

				free(line);
				line = NULL;
			}

			fclose(fd);
		}
	}

	return ret;
}


static void _dumpMemory(_U8 *mem, _U32 memSize) {
	_U32 i = 0;
	_U8 j;

	const _U8 lineLength = 32;

	for (i = 0; i < memSize; i += lineLength) {
		printf("%04x:", i);

		for (j = 0; j < lineLength; j++) {
			printf(" %02x", mem[i + j]);
		}

		printf("   ");

		for (j = 0; j < lineLength; j++) {
			char c = mem[i + j];

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
	memset(_memory, 0, sizeof(_memory));

	if (argc > 1) {
		_loadHex(argv[1]);
	}

	cpu_init(&_cpu);
	cpu_reset(&_cpu);

	_U8 cycles = 0;

	while (1) {

		if (_cpu.cycleType == 0 && _cpu.cycle == 1 && _cpu.state == 1) {
			DBG(("Last instruction cycles: %u", cycles));
			cycles = 0;
		}

		// p1
		cpu_phase(&_cpu);

//		DBG(("T: %u, P1: %u, CYCLE: %u, TYPE: %u, STATE: %u, SYNC: %u, DBIN: %u, DATA: %02x, ADDRESS: %04x, PC: %04x",
//			_cpu.ticks, _cpu.p1, _cpu.cycle, _cpu.cycleType, _cpu.state, _cpu.pins.SYNC,
//			_cpu.pins.DBIN, _cpu.pins.DATA, _cpu.pins.ADDRESS, _cpu.PC
//		));

		// Read from memory
		if (_cpu.pins.DBIN) {
			_cpu.pins.DATA = _memory[_cpu.pins.ADDRESS];
		}

		// p2
		cpu_phase(&_cpu);

		DBG(("T: %u, P1: %u, CYCLE: %u, TYPE: %u, STATE: %u, SYNC: %u, DBIN: %u, DATA: %02x, ADDRESS: %04x, PC: %04x",
			_cpu.ticks, _cpu.p1, _cpu.cycle, _cpu.cycleType, _cpu.state, _cpu.pins.SYNC,
			_cpu.pins.DBIN, _cpu.pins.DATA, _cpu.pins.ADDRESS, _cpu.PC
		));

		cycles++;
	}
	cpu_loop(&_cpu);
//	_dumpMemory(&_mem);
}

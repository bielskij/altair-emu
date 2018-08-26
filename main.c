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
#include "emulator/altair/mainBoard.h"
#include "emulator/altair/module/cpu.h"
#include "emulator/altair/module/sram.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


#define MEM_SIZE (48 * 1024)

static _U8 _memory[MEM_SIZE];

Cpu          cpu;
AltairModule cpuModule;

AltairSramParameter sramAParameters;
AltairModule        sramAModule;
AltairSramParameter sramBParameters;
AltairModule        sramBModule;
AltairSramParameter sramCParameters;
AltairModule        sramCModule;


static _U8 _sramRead(_U16 addr) {
	DBG(("%02x = [R](%04x)", _memory[addr], addr));

	return _memory[addr];
}

static void _sramWrite(_U16 addr, _U8 val) {
	DBG(("%02x = [W](%04x)", val, addr));

	_memory[addr] = val;
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
				ihex_line_to_binary(line, _memory, sizeof(_memory));

				free(line);
				line = NULL;
			}

			fclose(fd);
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	altair_mainBoard_initialize();

	// Modules

	// CPU
	{
		altair_module_cpu_init(&cpuModule, &cpu);

		altair_mainBoard_addModule(&cpuModule);
	}

	// SRAM
	{
		sramAParameters.bank          = 0;
		sramAParameters.readCallback  = _sramRead;
		sramAParameters.writeCallback = _sramWrite;

		altair_module_sram_init(&sramAModule, &sramAParameters);

		sramBParameters.bank          = 1;
		sramBParameters.readCallback  = _sramRead;
		sramBParameters.writeCallback = _sramWrite;

		altair_module_sram_init(&sramBModule, &sramBParameters);

		sramCParameters.bank          = 2;
		sramCParameters.readCallback  = _sramRead;
		sramCParameters.writeCallback = _sramWrite;

		altair_module_sram_init(&sramCModule, &sramCParameters);

		altair_mainBoard_addModule(&sramAModule);
		altair_mainBoard_addModule(&sramBModule);
		altair_mainBoard_addModule(&sramCModule);
	}

	// Clear memory
	memset(_memory, 0, sizeof(_memory));

	// Load image
	if (argc > 1) {
		_loadHex(argv[1]);
	}

	while (1) {
		altair_mainBoard_tick();
	}

	return 0;
}


#if 0


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

		if (! _cpu.pins.WR) {
			_memory[_cpu.pins.ADDRESS] = _cpu.pins.DATA;

			DBG(("MEM[%04x] <- %02x", _cpu.pins.ADDRESS, _cpu.pins.DATA));
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

			DBG(("MEM[%04x] -> %02x", _cpu.pins.ADDRESS, _cpu.pins.DATA));
		}

		// p2
		cpu_phase(&_cpu);

//		DBG(("T: %u, P1: %u, CYCLE: %u, TYPE: %u, STATE: %u, SYNC: %u, DBIN: %u, DATA: %02x, ADDRESS: %04x, PC: %04x",
//			_cpu.ticks, _cpu.p1, _cpu.cycle, _cpu.cycleType, _cpu.state, _cpu.pins.SYNC,
//			_cpu.pins.DBIN, _cpu.pins.DATA, _cpu.pins.ADDRESS, _cpu.PC
//		));

		cycles++;
	}
	cpu_loop(&_cpu);
//	_dumpMemory(&_mem);
}
#endif

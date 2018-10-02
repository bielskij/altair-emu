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
#include "lib/emulator/inc/emulator/altair/module/88-2sio.h"

#define DEBUG_LEVEL 1
#include "common/debug.h"


#define MEM_SIZE (64 * 1024)

static _U8 _memory[MEM_SIZE];

Cpu          cpu;
AltairModule cpuModule;

AltairSramParameter sramAParameters;
AltairModule        sramAModule;
AltairSramParameter sramBParameters;
AltairModule        sramBModule;
AltairSramParameter sramCParameters;
AltairModule        sramCModule;
AltairSramParameter sramDParameters;
AltairModule        sramDModule;

Altair882SioParameters _882sioParameters;
AltairModule           _882sioModule;


static _U8 _sramRead(_U16 addr) {
	DBG(("%02x = [R](%04x)", _memory[addr], addr));

	return _memory[addr];
}


static void _sramWrite(_U16 addr, _U8 val) {
	DBG(("%02x = [W](%04x)", val, addr));

	_memory[addr] = val;
}


static void _882SioStatusRegCallback(_U8 port, Altair882SioStatusRegister *reg) {
	LOG(("SIO STATUS[%02x]", port));
}


static void _882SioControlRegCallback(_U8 port, Altair882SioControlRegister *reg) {
	LOG(("SIO CONTROL[%02x]", port));
}


static _U8  _882SioInputDataCallback (_U8 port) {
	_U8 ret = 0;

	LOG(("SIO INPUT[%02x] <- %02x (%c)", port, ret, ret));

	return ret;
}


static void _882SioOutputDataCallback(_U8 port, _U8 data) {
	LOG(("SIO OUTPUT[%02x] -> %02x (%c)", port, data, data));
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
		// Clear memory
		memset(_memory, 0, sizeof(_memory));

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

		sramDParameters.bank          = 3;
		sramDParameters.readCallback  = _sramRead;
		sramDParameters.writeCallback = _sramWrite;

		altair_module_sram_init(&sramDModule, &sramDParameters);

		altair_mainBoard_addModule(&sramAModule);
		altair_mainBoard_addModule(&sramBModule);
		altair_mainBoard_addModule(&sramCModule);
		altair_mainBoard_addModule(&sramDModule);
	}

	// 2sio
	{
		_882sioParameters.address = 0x10;

		_882sioParameters.controlRegCallback = _882SioControlRegCallback;
		_882sioParameters.inputDataCallback  = _882SioInputDataCallback;
		_882sioParameters.outputDataCallback = _882SioOutputDataCallback;
		_882sioParameters.statusRegCallback  = _882SioStatusRegCallback;

		altair_module_882sio_init(&_882sioModule, &_882sioParameters);

		altair_mainBoard_addModule(&_882sioModule);
	}

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
#endif

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

#include "arch/module.h"

#include "emulator/altair/module/88-16mcs.h"

#define DEBUG_LEVEL 1
#include "common/debug.h"


#define MEM_SIZE (64 * 1024)

static _U8 _memory[MEM_SIZE];


static _U8 _sramRead(_U16 addr) {
	DBG(("%02x = [R](%04x)", _memory[addr], addr));

	return _memory[addr];
}


static void _sramWrite(_U16 addr, _U8 val) {
	DBG(("%02x = [W](%04x)", val, addr));

	_memory[addr] = val;
}


AltairModule *arch_create_module_8816Mcs(_U8 bank) {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair8816mcsParameter *parameters = malloc(sizeof(Altair8816mcsParameter));

		parameters->bank          = bank;
		parameters->readCallback  = _sramRead;
		parameters->writeCallback = _sramWrite;

		altair_module_8816mcs_init(ret, parameters);
	}

	return ret;
}


void internal_sram_getMemory(_U8 **memory, _U32 *memorySize) {
	*memory     = _memory;
	*memorySize = MEM_SIZE;
}

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

#include "emulator/altair/module/cpu.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static void _onClock(_U8 phase1, S100Bus *busState, void *privateData) {
	Cpu *core = (Cpu *) privateData;

	// TODO: all inputs
	core->pins.READY = busState->PRDY;

	if (busState->PDBIN) {
		core->pins.DATA = busState->DI;
	}

	cpu_phase(core, phase1);

	busState->PINTE = core->pins.INTE;
	busState->PDBIN = core->pins.DBIN;
	busState->PSYNC = core->pins.SYNC;
	busState->_PWR  = core->pins._WR;

	if (busState->PSYNC) {
		busState->SINTA  = (core->pins.DATA & 0x01) != 0;
		busState->SWO    = (core->pins.DATA & 0x02) != 0;
		busState->SSTACK = (core->pins.DATA & 0x04) != 0;
		busState->SHLTA  = (core->pins.DATA & 0x08) != 0;
		busState->SOUT   = (core->pins.DATA & 0x10) != 0;
		busState->SM1    = (core->pins.DATA & 0x20) != 0;
		busState->SINP   = (core->pins.DATA & 0x40) != 0;
		busState->SMEMR  = (core->pins.DATA & 0x80) != 0;
	}

	busState->MWRT = ! busState->_PWR && ! busState->SOUT;

	busState->A = core->pins.ADDRESS;

	if (busState->MWRT || ! busState->_PWR) {
		busState->DO = core->pins.DATA;
	}

	core->pins.INT = ! busState->_PINT;
}


void altair_module_cpu_init(AltairModule *module, Cpu *core) {
	cpu_init(core);

	module->clockCallback = _onClock;
	module->privateData   = core;
}

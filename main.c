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
#include "emulator/altair/module/88-16mcs.h"

#include "arch/arch.h"
#include "arch/module.h"

#define DEBUG_LEVEL 1
#include "common/debug.h"


Cpu          cpu;
AltairModule cpuModule;


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
		altair_mainBoard_addModule(
			arch_create_module_8816Mcs(0)
		);

		altair_mainBoard_addModule(
			arch_create_module_8816Mcs(1)
		);

		altair_mainBoard_addModule(
			arch_create_module_8816Mcs(2)
		);
	}

	// 2sio
	altair_mainBoard_addModule(
		arch_create_module_882Sio(4)
	);

	// sio
	altair_mainBoard_addModule(
		arch_create_module_88Sio(0)
	);

	// prom
	altair_mainBoard_addModule(
		arch_create_module_88Pmc(0x1f)
	);

	// minidisk
	altair_mainBoard_addModule(
		arch_create_module_88mcs()
	);

	// Load image
	if (argc > 1) {
		arch_loadHex(argv[1]);
	}

	_U32 timeUs;
	_U32 sleepUs = (TICKS_PER_LOOP * 1000000UL) / ALTAIR_F_CPU ;

	while (1) {
		arch_timer_start();
		{
			altair_mainBoard_ticks(TICKS_PER_LOOP);
		}
		timeUs = arch_timer_stop();

		if (timeUs < sleepUs) {
			arch_waitUs(sleepUs - timeUs);
		}
	}

	return 0;
}

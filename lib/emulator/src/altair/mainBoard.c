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

#include "emulator/altair/mainBoard.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static AltairModule *_modules[ALTAIR_MODULES_COUNT];
static _U8           _modulesConnected = 0;
static S100Bus       _bus;

void altair_mainBoard_initialize() {
	for (_U8 i = 0; i < ALTAIR_MODULES_COUNT; i++) {
		_modules[i] = NULL;
	}
}


void altair_mainBoard_addModule(AltairModule *module) {
	_modules[_modulesConnected++] = module;
}


void altair_mainBoard_tick() {
	_U8 i = 0;

	// phase1
	while (_modules[i] != NULL) {
		_modules[i]->clockCallback(TRUE, &_bus, _modules[i]->privateData);
		i++;
	}

	// phase2
	i = 0;
	while (_modules[i] != NULL) {
		_modules[i]->clockCallback(FALSE, &_bus, _modules[i]->privateData);
		i++;
	}
}


void altair_mainBoard_ticks(_U32 ticksCount) {
	for (_U32 i = 0; i < ticksCount; i++) {
		altair_mainBoard_tick();
	}
}

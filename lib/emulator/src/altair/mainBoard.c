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

	_bus._PINT = 1;
}


void altair_mainBoard_addModule(AltairModule *module) {
	_modules[_modulesConnected++] = module;
}


void altair_mainBoard_tick() {
	AltairModule *m;
	_U8 i = 0;

	// phase1
	while (_modules[i] != NULL) {
		m = _modules[i++];

		if (m->clockCallback) {
			m->clockCallback(TRUE, &_bus, m->privateData);
		}

		if (_bus.PDBIN) {
			// Memory read
			if (_bus.SMEMR) {
				if (m->readMemoryCallback) {
					m->readMemoryCallback(&_bus, m->privateData);
				}

			// IO read
			} else if (_bus.SINP) {
				if (m->readIoCallback) {
					m->readIoCallback(&_bus, m->privateData);
				}
			}

		// Memory write
		} else if (_bus.MWRT) {
			if (m->writeMemoryCallback) {
				m->writeMemoryCallback(&_bus, m->privateData);
			}

		// IO write
		} else if (_bus.SOUT && _bus._PWR) {
			if (m->writeIoCallback) {
				m->writeIoCallback(&_bus, m->privateData);
			}
		}
	}

	// phase2
	i = 0;
	while (_modules[i] != NULL) {
		m = _modules[i++];

		if (m->clockCallback) {
			m->clockCallback(FALSE, &_bus, m->privateData);
		}
	}
}


void altair_mainBoard_ticks(_U32 ticksCount) {
	for (_U32 i = 0; i < ticksCount; i++) {
		altair_mainBoard_tick();
	}
}

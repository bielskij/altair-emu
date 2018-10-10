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

#include "emulator/altair/module/88-pmc.h"


static void _onReadMemory(S100Bus *bus, void *privateData) {
	Altair88PmcParameters *params = (Altair88PmcParameters *) privateData;

	_U8 patternValue = bus->A >> 11;
	if (patternValue == params->addressPattern) {
		_U8 address = (bus->A);
		_U8 bank    = (bus->A >> 8) & 0x07;

		bus->DI = params->readCallback(bank, bus->A, params->callbackData);
	}
}


void altair_module_88pmc_init(AltairModule *module, Altair88PmcParameters *params) {
	module->clockCallback       = NULL;
	module->readIoCallback      = NULL;
	module->writeIoCallback     = NULL;
	module->readMemoryCallback  = _onReadMemory;
	module->writeMemoryCallback = NULL;

	module->privateData   = params;
}

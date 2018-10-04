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


static void _onTick(_U8 phase1, S100Bus *busState, void *privateData) {
	Altair88PmcParameters *params = (Altair88PmcParameters *) privateData;

	if (phase1) {
		if (busState->SMEMR && busState->PDBIN) {
			_U8 patternValue = busState->A >> 11;
			if (patternValue == params->addressPattern) {
				_U8 address = (busState->A);
				_U8 bank    = (busState->A >> 8) & 0x07;

				busState->DI = params->readCallback(bank, address, params->callbackData);
			}
		}
	}
}


void altair_module_88pmc_init(AltairModule *module, Altair88PmcParameters *params) {
	module->clockCallback = _onTick;
	module->privateData   = params;
}

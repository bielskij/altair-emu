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

#include "emulator/altair/module/88-16mcs.h"

#define DEBUG_LEVEL 1
#include "common/debug.h"

static void _onTick(_U8 phase1, S100Bus *busState, void *privateData) {
	Altair8816mcsParameter *params = (Altair8816mcsParameter *) privateData;

	if (phase1) {
		if (busState->SMEMR && busState->PDBIN) {
			if (
				(busState->A >= (params->bank << 14)) && (busState->A < ((params->bank + 1) << 14))
			) {
				busState->DI = params->readCallback(busState->A);
			}

		} else if (busState->MWRT) {
			if (
				(busState->A >= (params->bank << 14)) && (busState->A < ((params->bank + 1) << 14))
			) {
				params->writeCallback(busState->A, busState->DO);
			}
		}
	}
}


void altair_module_8816mcs_init(AltairModule *module, Altair8816mcsParameter *parameters) {
	module->clockCallback = _onTick;
	module->privateData   = parameters;
}

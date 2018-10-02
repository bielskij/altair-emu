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

#include "../../../inc/emulator/altair/module/88-2sio.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

/*
 * | address | port | output  | input  |
 * | P0 | P1 |      |         |        |
 * |  0 |  0 |   1  | control | status |
 * |  1 |  0 |   1  | output  | input  |
 * |  0 |  1 |   2  | control | status |
 * |  1 |  1 |   2  | output  | input  |
 */

static void _onTick(_U8 phase1, S100Bus *busState, void *privateData) {
	Altair882SioParameters *params = (Altair882SioParameters *) privateData;

	if (phase1) {
		if (busState->SOUT && ! busState->_PWR) {
			DBG(("--------------------- OUT %u, addr: %04x, data: %02x", busState->_PWR, busState->A, busState->DO));

		} else if (busState->SINP && busState->PDBIN) {
			busState->DI = 0x00;

			DBG(("--------------------- IN %u, addr: %04x, data: %02x", busState->PDBIN, busState->A, busState->DI));
		}
	}
}


void altair_module_882sio_init(AltairModule *module, Altair882SioParameters *params) {
	module->clockCallback = _onTick;
	module->privateData   = params;
}

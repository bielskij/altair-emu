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

#include "emulator/altair/module/88-sio.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static void _onReadIo(S100Bus *bus, void *privateData) {
	Altair88SioParameters *params = (Altair88SioParameters *) privateData;

	_U8 address = bus->A >> 8;

	if ((address >= params->address) && (address < params->address + 4)) {
		if (address & 0x01) {
			bus->DI = params->inputDataCallback(params->callbackData);

		} else {
			Altair88SioStatusRegister status;

			params->statusRegCallback(&status, params->callbackData);

			bus->DI = 0;

			if (! status.ODR) bus->DI |= 0x80;
			if (status.DA   ) bus->DI |= 0x20;
			if (status.DO   ) bus->DI |= 0x10;
			if (status.FE   ) bus->DI |= 0x08;
			if (status.PE   ) bus->DI |= 0x04;
			if (status.XBE  ) bus->DI |= 0x02;
			if (! status.IDR) bus->DI |= 0x01;
		}
	}
}


static void _onWriteIo(S100Bus *bus, void *privateData) {
	Altair88SioParameters *params = (Altair88SioParameters *) privateData;

	_U8 address = bus->A >> 8;

	if ((address >= params->address) && (address < params->address + 2)) {
		if (address & 0x01) {
			params->outputDataCallback(bus->DO, params->callbackData);

		} else {
			// Control
		}
	}
}


void altair_module_88sio_init(AltairModule *module, Altair88SioParameters *params) {
	module->clockCallback       = NULL;
	module->readIoCallback      = _onReadIo;
	module->writeIoCallback     = _onWriteIo;
	module->readMemoryCallback  = NULL;
	module->writeMemoryCallback = NULL;

	module->privateData   = params;
}

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

#include "emulator/altair/module/88-2sio.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static void _882SioStatusRegCallback(_U8 port, Altair882SioStatusRegister *reg) {
	LOG(("SIO STATUS[%02x]", port));
}


static void _882SioControlRegCallback(_U8 port, Altair882SioControlRegister *reg) {
	LOG(("SIO CONTROL[%02x]", port));
}


static _U8  _882SioInputDataCallback (_U8 port) {
	_U8 ret = 0;

	LOG(("SIO INPUT[%02x] <- %02x (%c)", port, ret, ret));

	return ret;
}


static void _882SioOutputDataCallback(_U8 port, _U8 data) {
	LOG(("SIO OUTPUT[%02x] -> %02x (%c)", port, data, data));
}


AltairModule *arch_create_module_882Sio(_U8 port) {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair882SioParameters *parameters = malloc(sizeof(Altair882SioParameters));

		parameters->address            = port * 4;
		parameters->controlRegCallback = _882SioControlRegCallback;
		parameters->inputDataCallback  = _882SioInputDataCallback;
		parameters->outputDataCallback = _882SioOutputDataCallback;
		parameters->statusRegCallback  = _882SioStatusRegCallback;

		altair_module_882sio_init(ret, parameters);
	}

	return ret;
}

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
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "arch/module.h"
#include "serial.h"

#include "emulator/altair/module/88-2sio.h"

#define DEBUG_LEVEL 0
#include "common/debug.h"


typedef struct _Sio882Data {
	SerialContext ports[2];
} Sio882Data;


static void _882SioStatusRegCallback(_U8 port, Altair882SioStatusRegister *reg, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	LOG(("SIO STATUS[%02x]", port));

	reg->RDRF = serial_canRead(&ctx->ports[port]);
	reg->TDRE = serial_canWrite(&ctx->ports[port]);
}


static void _882SioControlRegCallback(_U8 port, Altair882SioControlRegister *reg, void *privateData) {
	LOG(("SIO CONTROL[%02x]", port));
}


static _U8  _882SioInputDataCallback (_U8 port, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	return serial_readByte(&ctx->ports[port]);
}


static void _882SioOutputDataCallback(_U8 port, _U8 data, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	serial_writeByte(&ctx->ports[port], data);
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

		{
			Sio882Data *sioData = malloc(sizeof(Sio882Data));

			for (int i = 0; i < 2; i++) {
				serial_open(&sioData->ports[i]);
			}

			parameters->callbackData = sioData;
		}

		altair_module_882sio_init(ret, parameters);
	}

	return ret;
}

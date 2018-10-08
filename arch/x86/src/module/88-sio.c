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

#include "emulator/altair/module/88-sio.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


typedef struct _Sio88Data {
	SerialContext port;
} Sio88Data;


static void _88SioStatusRegCallback(Altair88SioStatusRegister *reg, void *privateData) {
	Sio88Data *ctx = (Sio88Data *) privateData;

	reg->DA  = 0;
	reg->DO  = 0;
	reg->FE  = 0;
	reg->PE  = 0;
	reg->ODR = 0;
	reg->IDR = serial_canRead(&ctx->port);
	reg->XBE = serial_canWrite(&ctx->port);
}


static void _88SioControlRegCallback(Altair88SioControlRegister *reg, void *privateData) {
	LOG(("SIO CONTROL[]"));
}


static _U8  _88SioInputDataCallback (void *privateData) {
	Sio88Data *ctx = (Sio88Data *) privateData;

	return serial_readByte(&ctx->port);
}


static void _88SioOutputDataCallback(_U8 data, void *privateData) {
	Sio88Data *ctx = (Sio88Data *) privateData;

	serial_writeByte(&ctx->port, data);
}


AltairModule *arch_create_module_88Sio(_U8 port) {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair88SioParameters *parameters = malloc(sizeof(Altair88SioParameters));

		parameters->address            = port * 2;
		parameters->controlRegCallback = _88SioControlRegCallback;
		parameters->inputDataCallback  = _88SioInputDataCallback;
		parameters->outputDataCallback = _88SioOutputDataCallback;
		parameters->statusRegCallback  = _88SioStatusRegCallback;

		{
			Sio88Data *sioData = malloc(sizeof(Sio88Data));

			serial_open(&sioData->port);

			parameters->callbackData = sioData;
		}

		altair_module_88sio_init(ret, parameters);
	}

	return ret;
}

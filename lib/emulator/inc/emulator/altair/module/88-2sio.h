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

// 88-2SIO implementation
#ifndef EMULATOR_ALTAIR_MODULE_88_2SIO_H_
#define EMULATOR_ALTAIR_MODULE_88_2SIO_H_

#include "emulator/altair/module.h"

typedef struct _Altair882SioStatusRegister {

} Altair882SioStatusRegister;

typedef struct _Altair882SioControlRegister {

} Altair882SioControlRegister;

typedef void (*Altair882SioStatusRegCallback) (_U8 port, Altair882SioStatusRegister *reg);
typedef void (*Altair882SioControlRegCallback)(_U8 port, Altair882SioControlRegister *reg);
typedef _U8  (*Altair882SioInputDataCallback) (_U8 port);
typedef void (*Altair882SioOutputDataCallback)(_U8 port, _U8 data);

typedef struct _Altair882SioParameters {
	_U8 address; // 0 - 63

	Altair882SioControlRegCallback controlRegCallback;
	Altair882SioStatusRegCallback  statusRegCallback;
	Altair882SioInputDataCallback  inputDataCallback;
	Altair882SioOutputDataCallback outputDataCallback;
} Altair882SioParameters;


void altair_module_882sio_init(AltairModule *module, Altair882SioParameters *params);

#endif /* EMULATOR_ALTAIR_MODULE_88_2SIO_H_ */

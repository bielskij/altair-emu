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
	_U8 RDRF : 1; // Receive Data Register Full
	_U8 TDRE : 1; // Transmit Data Register Empty
	_U8 _DCD : 1; // Data Carrier Detect
	_U8 _CTS : 1; // Clear-To-Send
	_U8 FE   : 1; // Framing Error
	_U8 OVRN : 1; // Receiver Overrun
	_U8 PE   : 1; // Parity Error
	_U8 IRQ  : 1; // Interrupt Request
} Altair882SioStatusRegister;

typedef struct _Altair882SioControlRegister {

} Altair882SioControlRegister;

typedef void (*Altair882SioStatusRegCallback) (_U8 port, Altair882SioStatusRegister *reg, void *privateData);
typedef void (*Altair882SioControlRegCallback)(_U8 port, Altair882SioControlRegister *reg, void *privateData);
typedef _U8  (*Altair882SioInputDataCallback) (_U8 port, void *privateData);
typedef void (*Altair882SioOutputDataCallback)(_U8 port, _U8 data, void *privateData);

typedef struct _Altair882SioParameters {
	_U8 address; // 0 - 63

	Altair882SioControlRegCallback controlRegCallback;
	Altair882SioStatusRegCallback  statusRegCallback;
	Altair882SioInputDataCallback  inputDataCallback;
	Altair882SioOutputDataCallback outputDataCallback;

	void *callbackData;
} Altair882SioParameters;


void altair_module_882sio_init(AltairModule *module, Altair882SioParameters *params);

#endif /* EMULATOR_ALTAIR_MODULE_88_2SIO_H_ */

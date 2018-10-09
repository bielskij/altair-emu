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

#ifndef EMULATOR_ALTAIR_MODULE_88_SIO_H_
#define EMULATOR_ALTAIR_MODULE_88_SIO_H_

#include "emulator/altair/module.h"


typedef struct _Altair88SioStatusRegister {
	_U8 ODR : 1; // Output device ready
	_U8 DA  : 1; // Data available;
	_U8 DO  : 1; // Data overflow;
	_U8 FE  : 1; // Framming Error
	_U8 PE  : 1; // Parity error
	_U8 XBE : 1; // X-mitter buffer empty
	_U8 IDR : 1; // Input device ready
} Altair88SioStatusRegister;


typedef struct _Altair88SioControlRegister {
	_U8 ININT  : 1; // Input interrupt
	_U8 OUTINT : 1; // Output interrupt
} Altair88SioControlRegister;


typedef void (*Altair88SioStatusRegCallback) (Altair88SioStatusRegister *reg, void *privateData);
typedef void (*Altair88SioControlRegCallback)(Altair88SioControlRegister *reg, void *privateData);
typedef _U8  (*Altair88SioInputDataCallback) (void *privateData);
typedef void (*Altair88SioOutputDataCallback)(_U8 data, void *privateData);


typedef struct _Altair88SioParameters {
	_U8 address; // 0 - 127

	Altair88SioControlRegCallback controlRegCallback;
	Altair88SioStatusRegCallback  statusRegCallback;
	Altair88SioInputDataCallback  inputDataCallback;
	Altair88SioOutputDataCallback outputDataCallback;

	_U8 intTroughIv  : 1;
	_U8 intIvIdx     : 5;
	_U8 intInEnable  : 1;
	_U8 intOutEnable : 1;

	void *callbackData;
} Altair88SioParameters;


void altair_module_88sio_init(AltairModule *module, Altair88SioParameters *params);

#endif /* EMULATOR_ALTAIR_MODULE_88_SIO_H_ */

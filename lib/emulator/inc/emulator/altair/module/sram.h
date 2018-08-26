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
/*
 * It is an abstract implementation of 16KB SRAM memory module (88-16MCS)
 */
#ifndef EMULATOR_ALTAIR_MODULE_SRAM_H_
#define EMULATOR_ALTAIR_MODULE_SRAM_H_

#include "emulator/altair/module.h"

typedef _U8  (*AltairSramReadCallback)(_U16 address);
typedef void (*AltairSramWriteCallback)(_U16 address, _U8 data);

typedef struct _AltairSramParameters {
	AltairSramReadCallback  readCallback;
	AltairSramWriteCallback writeCallback;

	// Alternative to original switch on the module board, defines address offset of the card memory.
	// 0:     0 - 16383
	// 1: 16384 - 32767
	// 2: 32768 - 49151
	// 3: 49152 - 65535
	_U8 bank;
} AltairSramParameter;

void altair_module_sram_init(AltairModule *module, AltairSramParameter *parameters);

#endif /* EMULATOR_ALTAIR_MODULE_SRAM_H_ */

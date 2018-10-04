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

#include "arch/module.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static _U8 _mdblProm[] = {
	0x21, 0x13, 0xff, 0x11, 0x00, 0x4c, 0x0e, 0xe3, 0x7e, 0x12, 0x23, 0x13, 0x0d, 0xc2, 0x08, 0xff,
	0xc3, 0x00, 0x4c, 0xf3, 0xaf, 0xd3, 0x22, 0x2f, 0xd3, 0x23, 0x3e, 0x2c, 0xd3, 0x22, 0x3e, 0x03,
	0xd3, 0x10, 0xdb, 0xff, 0xe6, 0x11, 0x0f, 0x0f, 0xc6, 0x10, 0xd3, 0x10, 0x31, 0x71, 0x4d, 0xaf,
	0xd3, 0x08, 0xdb, 0x08, 0xe6, 0x08, 0xc2, 0x1c, 0x4c, 0x3e, 0x04, 0xd3, 0x09, 0xc3, 0x38, 0x4c,
	0xdb, 0x08, 0xe6, 0x02, 0xc2, 0x2d, 0x4c, 0x3e, 0x02, 0xd3, 0x09, 0xdb, 0x08, 0xe6, 0x40, 0xc2,
	0x2d, 0x4c, 0x11, 0x00, 0x00, 0x06, 0x00, 0x3e, 0x10, 0xf5, 0xd5, 0xc5, 0xd5, 0x11, 0x86, 0x80,
	0x21, 0xe3, 0x4c, 0xdb, 0x09, 0x1f, 0xda, 0x50, 0x4c, 0xe6, 0x1f, 0xb8, 0xc2, 0x50, 0x4c, 0xdb,
	0x08, 0xb7, 0xfa, 0x5c, 0x4c, 0xdb, 0x0a, 0x77, 0x23, 0x1d, 0xc2, 0x5c, 0x4c, 0xe1, 0x11, 0xe6,
	0x4c, 0x01, 0x80, 0x00, 0x1a, 0x77, 0xbe, 0xc2, 0xc3, 0x4c, 0x80, 0x47, 0x13, 0x23, 0x0d, 0xc2,
	0x71, 0x4c, 0x1a, 0xfe, 0xff, 0xc2, 0x88, 0x4c, 0x13, 0x1a, 0xb8, 0xc1, 0xeb, 0xc2, 0xba, 0x4c,
	0xf1, 0xf1, 0x2a, 0xe4, 0x4c, 0xcd, 0xdd, 0x4c, 0xd2, 0xb3, 0x4c, 0x04, 0x04, 0x78, 0xfe, 0x10,
	0xda, 0x44, 0x4c, 0x06, 0x01, 0xca, 0x44, 0x4c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0xa5, 0x4c, 0x3e,
	0x01, 0xd3, 0x09, 0xc3, 0x42, 0x4c, 0x3e, 0x80, 0xd3, 0x08, 0xc3, 0x00, 0x00, 0xd1, 0xf1, 0x3d,
	0xc2, 0x46, 0x4c, 0x3e, 0x43, 0x01, 0x3e, 0x4d, 0xfb, 0x32, 0x00, 0x00, 0x22, 0x01, 0x00, 0x47,
	0x3e, 0x80, 0xd3, 0x08, 0x78, 0xd3, 0x01, 0xd3, 0x11, 0xd3, 0x05, 0xd3, 0x23, 0xc3, 0xd2, 0x4c,
	0x7a, 0xbc, 0xc0, 0x7b, 0xbd, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static _U8 _readCallback(_U8 promNumber, _U8 address, void *callbackData) {
	DBG(("READ port: %u, address: %02x", promNumber, address));

	if (promNumber == 7) {
		return _mdblProm[address];
	}

	return 0;
}


AltairModule *arch_create_module_88Pmc(_U8 addressPattern) {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair88PmcParameters *params = malloc(sizeof(Altair88PmcParameters));

		params->addressPattern = addressPattern;
		params->readCallback   = _readCallback;
		params->callbackData   = NULL;

		altair_module_88pmc_init(ret, params);
	}

	return ret;
}

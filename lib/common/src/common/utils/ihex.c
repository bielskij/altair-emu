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

#include "common/utils/ihex.h"

#define DEBUG_LEVEL 1
#include "common/debug.h"


static _U8 _hexToBin(const char hex[2]) {
	_U8 ret = 0;
	_U8 i = 0;

	while (i != 2) {
		char x = hex[i];
		_U8 shift = 4 * (1 - i);

		if (x >= '0' && x <= '9') {
			ret |= ((x - '0') << shift);

		} else if (x >= 'A' && x <= 'F') {
			ret |= ((x - 'A' + 10) << shift);

		} else {
			ret |= ((x - 'a' + 10) << shift);
		}

		i++;
	}

	return ret;
}


_U8 ihex_line_to_binary(char *ihex, void *buffer, _U32 bufferSize) {
	_U8 ret = 0;

	_U8  bytesCount;
	_U16 address;
	_U8  type;

	if (ihex[0] != ':') {
		ERR(("Start code missing!"));

		ret = 1;
	}

	bytesCount = _hexToBin(ihex + 1);
	address    = _hexToBin(ihex + 3) << 8;
	address   |= _hexToBin(ihex + 5);
	type       = _hexToBin(ihex + 7);

	// TODO: Check CRC

	DBG(("Line type: %x, length: %zd, address: %04x", type, bytesCount, address));

	switch (type) {
		case 0x00:
			for (_U16 i = 0; i < bytesCount; i++) {
				((_U8 *)buffer)[address + i] = _hexToBin(ihex + 9 + (i * 2));
			}
			break;

		case 0x01:
			LOG(("IHEX EOF"));
			break;

		case 0x02:
			ERR(("Extended Segment Address is not supported!"));
			break;

		case 0x03:
			ERR(("Start Segment Address is not supported"));
			break;

		case 0x04:
			ERR(("Extended Linear Address is not supported"));
			break;

		case 0x05:
			ERR(("Start Linear Address is not supported"));
			break;

		default:
			ERR(("Unknown type of record! %#02X", type));
	}

	return ret;
}

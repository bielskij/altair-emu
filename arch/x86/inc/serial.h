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

#ifndef X86_SERIAL_H_
#define X86_SERIAL_H_

#include <stdio.h>

#include "common/types.h"


typedef struct _SerialContext {
	int   ptyMasterFd;
	char  ptyPath[FILENAME_MAX];
	pid_t xtermPid;
} SerialContext;


_U8  serial_open(SerialContext *ctx);
void serial_close(SerialContext *ctx);

_U8 serial_canRead(SerialContext *ctx);
_U8 serial_canWrite(SerialContext *ctx);

_U8  serial_readByte(SerialContext *ctx);
void serial_writeByte(SerialContext *ctx, _U8 data);


#endif /* X86_SERIAL_H_ */

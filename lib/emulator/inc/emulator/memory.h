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

#ifndef EMULATOR_INC_MEMORY_H_
#define EMULATOR_INC_MEMORY_H_

#include "common/types.h"

typedef struct _Memory *Memory;

extern _U8  memory_read_byte(Memory memory, _U16 address);
extern void memory_write_byte(Memory memory, _U16 address, _U8 data);

#endif /* EMULATOR_INC_MEMORY_H_ */

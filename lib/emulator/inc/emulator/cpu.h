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

#ifndef EMULATOR_CPU_H_
#define EMULATOR_CPU_H_

#include "common/types.h"

#include "emulator/memory.h"
#include "emulator/io.h"

#define FLAG_SIGN   ((_U8)0x80)
#define FLAG_ZERO   ((_U8)0x40)
#define FLAG_AC     ((_U8)0x10)
#define FLAG_PARITY ((_U8)0x04)
#define FLAG_CARRY  ((_U8)0x01)

#define FLAG_IS_SET(__cpu, __flag)((__cpu->PSW.r.b.l & __flag) != 0)

typedef struct _RegPair {
	union {
		struct {
			_U8 l;
			_U8 h;
		} b;

		_U16 w;
	} r;
} RegPair;

typedef struct _Cpu {
	RegPair PSW;
	RegPair B;
	RegPair D;
	RegPair H;
	RegPair SP;

	_U16 PC;

	_U32 cycles;
	_U8 halt;

	Memory memory;
	IO     io;
} Cpu;

void cpu_init(Cpu *cpu, Memory mem, IO io);

_U8 cpu_step(Cpu *cpu);

void cpu_loop(Cpu *cpu);

void cpu_reset(Cpu *cpu);

#endif /* EMULATOR_CPU_H_ */

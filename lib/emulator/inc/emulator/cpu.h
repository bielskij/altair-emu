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

typedef struct _Cpu Cpu;

typedef struct _RegPair {
	union {
		struct {
			_U8 l;
			_U8 h;
		} b;

		_U16 w;
	} r;
} RegPair;

typedef void (*OpCodeCallback)(Cpu *cpu);

struct _Cpu {
	RegPair PSW;
	RegPair B;
	RegPair D;
	RegPair H;
	RegPair SP;
	_U16 PC;

	// Temporary register, non addressable
	RegPair W;

	// Internal ALU registers
	_U8 ACT; // temporary accumulator
	_U8 TMP; // temporary register
	_U8 ALUOP; // ALU pending operation

	struct {
		// Output
		_U8 WAIT  : 1;
		_U8 HLDA  : 1;
		_U8 SYNC  : 1;
		_U8 INTE  : 1;
		_U8 DBIN  : 1;

		// Input
		_U8 READY : 1;
		_U8 INT   : 1;
		_U8 HOLD  : 1;
		_U8 RESET : 1;

		// Data & address bus, equal to 0 when tri-state
		_U8  DATA;
		_U16 ADDRESS;
	} pins;

	// 1 on phase1, 0 on phase2 clk
	_U8  p1;
	// clock ticks
	_U32 ticks;
	// Holds read cycle number in a row
	_U8  readCycleCount;

	// Status of internal address bus, used for delayed jumps
	_U16 internalAddress;

	// Instruction register - holds current opcode
	_U8            ir;
	// opcode callback pointer
	OpCodeCallback irCallback;

	// Current state (1-5)
	_U8 state;
	// Current cycle (1
	_U8 cycle;
	// Current cycle type (fetch, read memory/stack etc)
	_U8 cycleType;
};

void cpu_init(Cpu *cpu);

void cpu_phase(Cpu *cpu);

void cpu_tick(Cpu *cpu);

void cpu_state(Cpu *cpu);

void cpu_cycle(Cpu *cpu);

void cpu_nexti(Cpu *cpu);

void cpu_reset(Cpu *cpu);

#endif /* EMULATOR_CPU_H_ */

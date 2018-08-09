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

#include "emulator/cpu.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

// ------------------ CONFIG ------------------

#define REPORT_OPCODES

//#define PARITY_ALG_LUT
//#define PARITY_ALG_NAIVE
#define PARITY_ALG_XOR

// --------------------------------------------

#define BIT_SET(__byte, __bit)    {(__byte) |= (__bit);}
#define BIT_CLEAR(__byte, __bit)  {(__byte) &= ~(__bit);}
#define BIT_TOGGLE(__byte, __bit) {(__byte) ^= (__bit);}

#define FLAG_SET(__cpu, __flag)    BIT_SET(__cpu->PSW.r.b.l, __flag)
#define FLAG_CLEAR(__cpu, __flag)  BIT_CLEAR(__cpu->PSW.r.b.l, __flag)
#define FLAG_TOGGLE(__cpu, __flag) BIT_TOGGLE(__cpu->PSW.r.b.l, __flag)

#define PC_INC(__cpu, __val) {__cpu->PC += __val;}
#define FETCH_OFFSET(__cpu, __offset) memory_read_byte(__cpu->memory, __cpu->PC + __offset)

#define SET_PAIR_HB(__pair, __h) { (__pair).r.b.h = (__h); }
#define SET_PAIR_LB(__pair, __l) { (__pair).r.b.l = (__l); }
#define SET_PAIR_B(__pair, __h, __l) { (__pair).r.b.h = __h; (__pair).r.b.l = (__l);}
#define SET_PAIR_W(__pair, __w) { (__pair).r.w = __w; }
#define COPY_PAIR_W(__pairDst, __pairSrc) { (__pairDst).r.w = (__pairSrc).r.w; }

#define INC_PAIR_W(__pair) {(__pair).r.w++;}
#define DEC_PAIR_W(__pair) {(__pair).r.w--;}
#define GET_PAIR_W(__pair) ((__pair).r.w)
#define GET_PAIR_LB(__pair) ((__pair).r.b.l)
#define GET_PAIR_HB(__pair) ((__pair).r.b.h)
#define GET_A(__cpu)(__cpu->PSW.r.b.h)
#define GET_B(__cpu)(__cpu->B.r.b.h)
#define GET_C(__cpu)(__cpu->B.r.b.l)
#define GET_D(__cpu)(__cpu->D.r.b.h)
#define GET_E(__cpu)(__cpu->D.r.b.l)
#define GET_H(__cpu)(__cpu->H.r.b.h)
#define GET_L(__cpu)(__cpu->H.r.b.l)
#define GET_Z(__cpu)(__cpu->W.r.b.h)
#define GET_W(__cpu)(__cpu->W.r.b.l)

#define SET_A(__cpu, __val){__cpu->PSW.r.b.h = __val; }
#define SET_B(__cpu, __val){__cpu->B.r.b.h = __val; }
#define SET_C(__cpu, __val){__cpu->B.r.b.l = __val; }
#define SET_D(__cpu, __val){__cpu->D.r.b.h = __val; }
#define SET_E(__cpu, __val){__cpu->D.r.b.l = __val; }
#define SET_H(__cpu, __val){__cpu->H.r.b.h = __val; }
#define SET_L(__cpu, __val){__cpu->H.r.b.l = __val; }
#define SET_Z(__cpu, __val){__cpu->W.r.b.h = __val; }
#define SET_W(__cpu, __val){__cpu->W.r.b.l = __val; }

#define STATUS_FLAG_INTA  0x01
#define STATUS_FLAG_WO    0x02
#define STATUS_FLAG_STACK 0x04
#define STATUS_FLAG_HLTA  0x08
#define STATUS_FLAG_OUT   0x10
#define STATUS_FLAG_M1    0x20
#define STATUS_FLAG_INP   0x40
#define STATUS_FLAG_MEMR  0x80

#define CYCLE_TYPE_INSTRUCTION_FETCH 0
#define CYCLE_TYPE_MEMORY_READ       1
#define CYCLE_TYPE_MEMORY_WRITE      2
#define CYCLE_TYPE_STACK_READ        3
#define CYCLE_TYPE_STACK_WRITE       4
#define CYCLE_TYPE_INPUT_READ        5
#define CYCLE_TYPE_OUTPUT_WRITE      6
#define CYCLE_TYPE_INT_ACK           7
#define CYCLE_TYPE_HALT_ACK          8
#define CYCLE_TYPE_INT_HALT_ACK      9

#define ALU_OP_NONE 0
#define ALU_OP_AND  1

#if defined(REPORT_OPCODES)
#define DECLARE_OPCODE(__value, __handler, __name) {_opHandlers[__value] = __handler; _opcodesNames[__value] = __name;}
#else
#define DECLARE_OPCODE(__value, __handler, __name) {_opHandlers[__value] = __handler;}
#endif

typedef void (*OpCodeCallback)(Cpu *cpu);

static _U8 _initialized = 0;
static OpCodeCallback _opHandlers[256];
static char *_opcodesNames[256];

#if defined(PARITY_ALG_LUT)
static const _U8 _parityValue[256] = {
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
};

static _U8 _getParity(_U8 byte) {
	return _parityValue[byte];
}
#endif

#if defined(PARITY_ALG_NAIVE)
static _U8 _getParity(_U8 byte) {
	_U8 ret = 0;

	while (byte) {
		ret = ! ret;
		byte = byte & (byte - 1);
	}

	return ret;
}
#endif

#if defined(PARITY_ALG_XOR)
static _U8 _getParity(_U8 byte) {
	byte ^= (byte >> 4);
	byte ^= (byte >> 2);
	byte ^= (byte >> 1);

	return (byte) & 1;
}
#endif

static void _flagsCommon(Cpu *cpu, _U8 result) {
	if (result & 0x80) {
		FLAG_SET(cpu, FLAG_SIGN);
	} else {
		FLAG_CLEAR(cpu, FLAG_SIGN);
	}

	if (result == 0) {
		FLAG_SET(cpu, FLAG_ZERO);
	} else {
		FLAG_CLEAR(cpu, FLAG_ZERO);
	}

	if (_getParity(result)) {
		FLAG_CLEAR(cpu, FLAG_PARITY);
	} else {
		FLAG_SET(cpu, FLAG_PARITY);
	}
}

static void _flagsLogic(Cpu *cpu, _U8 result) {
	_flagsCommon(cpu, result);

	FLAG_CLEAR(cpu, FLAG_CARRY);
	FLAG_CLEAR(cpu, FLAG_AC);
}

static _U8 _flagsAdd(Cpu *cpu, _U8 leftOp, _U8 rightOp, _U8 addCarry, _U8 preserveCarry) {
	_U16 result;
	_U8  carryVal = 0;

	if (addCarry) {
		carryVal = FLAG_IS_SET(cpu, FLAG_CARRY);
	}

	result = leftOp + rightOp + carryVal;

	_flagsCommon(cpu, result);

	if (! preserveCarry) {
		if (result >> 8) {
			FLAG_SET(cpu, FLAG_CARRY);
		} else {
			FLAG_CLEAR(cpu, FLAG_CARRY);
		}
	}

	if (((leftOp & 0x0f) + (rightOp & 0x0f) + carryVal) >> 4) {
		FLAG_SET(cpu, FLAG_AC);
	} else {
		FLAG_CLEAR(cpu, FLAG_AC);
	}

	return result;
}

static _U8 _flagsSub(Cpu *cpu, _U8 leftOp, _U8 rightOp, _U8 subCarry, _U8 preserveCarry) {
	_U16 result;
	_U8  carryVal = 0;

	if (subCarry) {
		carryVal = FLAG_IS_SET(cpu, FLAG_CARRY);
	}

	result = leftOp - rightOp - carryVal;

	_flagsCommon(cpu, result);

	if (! preserveCarry) {
		if (result >> 8) {
			FLAG_SET(cpu, FLAG_CARRY);
		} else {
			FLAG_CLEAR(cpu, FLAG_CARRY);
		}
	}

	if (((leftOp & 0x0f) - (rightOp & 0x0f) - carryVal) >> 4) {
		FLAG_SET(cpu, FLAG_AC);
	} else {
		FLAG_CLEAR(cpu, FLAG_AC);
	}

	return result;
}


static __inline _cpu_next_instruction(Cpu *cpu, _U16 address) {
	cpu->ir              = 0;
	cpu->irCallback      = NULL;
	cpu->cycle           = 1;
	cpu->state           = 1;
	cpu->cycleType       = CYCLE_TYPE_INSTRUCTION_FETCH;
	cpu->internalAddress = address;
}

static __inline _cpu_next_cycle(Cpu *cpu, _U8 cycleType, _U16 address) {
	cpu->cycle           = cpu->cycle + 1;
	cpu->state           = 1;
	cpu->cycleType       = cycleType;
	cpu->internalAddress = address;
}

#if 0
static _U8 _h_nop(Cpu *cpu, _U8 opcode) {
	PC_INC(cpu, 1);
	return 4;
}
#else
static void _h_nop(Cpu *cpu) {
	switch (cpu->state) {
		case 4:
			_cpu_next_instruction(cpu, cpu->PC);
			break;
	}
}
#endif

#if 0
static _U8 _h_lxi(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	SET_PAIR_LB(pair, FETCH_OFFSET(cpu, 1));
	SET_PAIR_HB(pair, FETCH_OFFSET(cpu, 2));

	switch (opcode & 0x30) {
		case 0x00: COPY_PAIR_W(cpu->B,  pair); break;
		case 0x10: COPY_PAIR_W(cpu->D,  pair); break;
		case 0x20: COPY_PAIR_W(cpu->H,  pair); break;
		case 0x30: COPY_PAIR_W(cpu->SP, pair); break;
		default:
			break;
	}

	PC_INC(cpu, 3);
	return 10;
}
#else
static void _h_lxi(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				switch (cpu->ir & 0x30) {
					case 0x00: COPY_PAIR_W(cpu->B,  cpu->W); break;
					case 0x10: COPY_PAIR_W(cpu->D,  cpu->W); break;
					case 0x20: COPY_PAIR_W(cpu->H,  cpu->W); break;
					case 0x30: COPY_PAIR_W(cpu->SP, cpu->W); break;
					default:
						break;
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}
#endif

#if 0
static _U8 _h_sta(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	SET_PAIR_LB(pair, FETCH_OFFSET(cpu, 1));
	SET_PAIR_HB(pair, FETCH_OFFSET(cpu, 2));

	memory_write_byte(cpu->memory, GET_PAIR_W(pair), GET_A(cpu));

	PC_INC(cpu, 3);
	return 13;
}

static _U8 _h_stax(Cpu *cpu, _U8 opcode) {
	_U16 address;

	if (opcode & 0x10) {
		address = GET_PAIR_W(cpu->D);
	} else {
		address = GET_PAIR_W(cpu->B);
	}

	memory_write_byte(cpu->memory, address, GET_A(cpu));

	PC_INC(cpu, 1);
	return 7;
}

static _U8 _h_lda(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	SET_PAIR_LB(pair, FETCH_OFFSET(cpu, 1));
	SET_PAIR_HB(pair, FETCH_OFFSET(cpu, 2));

	SET_A(cpu, memory_read_byte(cpu->memory, GET_PAIR_W(pair)));

	PC_INC(cpu, 3);
	return 13;
}

static _U8 _h_lhld(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	SET_PAIR_LB(pair, FETCH_OFFSET(cpu, 1));
	SET_PAIR_HB(pair, FETCH_OFFSET(cpu, 2));

	SET_L(cpu, memory_read_byte(cpu->memory, GET_PAIR_W(pair)));
	SET_H(cpu, memory_read_byte(cpu->memory, GET_PAIR_W(pair) + 1));

	PC_INC(cpu, 3);
	return 16;
}

static _U8 _h_shld(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	SET_PAIR_LB(pair, FETCH_OFFSET(cpu, 1));
	SET_PAIR_HB(pair, FETCH_OFFSET(cpu, 2));

	memory_write_byte(cpu->memory, GET_PAIR_W(pair), GET_L(cpu));
	memory_write_byte(cpu->memory, GET_PAIR_W(pair) + 1, GET_H(cpu));

	PC_INC(cpu, 3);
	return 16;
}

static _U8 _h_ldax(Cpu *cpu, _U8 opcode) {
	_U16 address;

	if (opcode & 0x10) {
		address = GET_PAIR_W(cpu->D);
	} else {
		address = GET_PAIR_W(cpu->B);
	}

	SET_A(cpu, memory_read_byte(cpu->memory, address));

	PC_INC(cpu, 1);
	return 7;
}

static _U8 _h_movMr(Cpu *cpu, _U8 opcode) {
	_U8 data;

	switch (opcode & 0x07) {
		case 0: data = GET_B(cpu); break;
		case 1: data = GET_C(cpu); break;
		case 2: data = GET_D(cpu); break;
		case 3: data = GET_E(cpu); break;
		case 4: data = GET_H(cpu); break;
		case 5: data = GET_L(cpu); break;
		case 7: data = GET_A(cpu); break;
		default:
			data = 0;
	}

	memory_write_byte(cpu->memory, GET_PAIR_W(cpu->H), data);

	PC_INC(cpu, 1);
	return 7;
}

static _U8 _h_movrM(Cpu *cpu, _U8 opcode) {
	_U8 data = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H));

	switch ((opcode & 0x38) >> 3) {
		case 0: SET_B(cpu, data); break;
		case 1: SET_C(cpu, data); break;
		case 2: SET_D(cpu, data); break;
		case 3: SET_E(cpu, data); break;
		case 4: SET_H(cpu, data); break;
		case 5: SET_L(cpu, data); break;
		case 7: SET_A(cpu, data); break;
	}

	PC_INC(cpu, 1);
	return 7;
}

static _U8 _h_movrr(Cpu *cpu, _U8 opcode) {
	switch (opcode & 0x3f) {
		case 0x00: SET_B(cpu, GET_B(cpu)); break;
		case 0x01: SET_B(cpu, GET_C(cpu)); break;
		case 0x02: SET_B(cpu, GET_D(cpu)); break;
		case 0x03: SET_B(cpu, GET_E(cpu)); break;
		case 0x04: SET_B(cpu, GET_H(cpu)); break;
		case 0x05: SET_B(cpu, GET_L(cpu)); break;
		case 0x07: SET_B(cpu, GET_A(cpu)); break;
		case 0x08: SET_C(cpu, GET_B(cpu)); break;
		case 0x09: SET_C(cpu, GET_C(cpu)); break;
		case 0x0a: SET_C(cpu, GET_D(cpu)); break;
		case 0x0b: SET_C(cpu, GET_E(cpu)); break;
		case 0x0c: SET_C(cpu, GET_H(cpu)); break;
		case 0x0d: SET_C(cpu, GET_L(cpu)); break;
		case 0x0f: SET_C(cpu, GET_A(cpu)); break;
		case 0x10: SET_D(cpu, GET_B(cpu)); break;
		case 0x11: SET_D(cpu, GET_C(cpu)); break;
		case 0x12: SET_D(cpu, GET_D(cpu)); break;
		case 0x13: SET_D(cpu, GET_E(cpu)); break;
		case 0x14: SET_D(cpu, GET_H(cpu)); break;
		case 0x15: SET_D(cpu, GET_L(cpu)); break;
		case 0x17: SET_D(cpu, GET_A(cpu)); break;
		case 0x18: SET_E(cpu, GET_B(cpu)); break;
		case 0x19: SET_E(cpu, GET_C(cpu)); break;
		case 0x1a: SET_E(cpu, GET_D(cpu)); break;
		case 0x1b: SET_E(cpu, GET_E(cpu)); break;
		case 0x1c: SET_E(cpu, GET_H(cpu)); break;
		case 0x1d: SET_E(cpu, GET_L(cpu)); break;
		case 0x1f: SET_E(cpu, GET_A(cpu)); break;
		case 0x20: SET_H(cpu, GET_B(cpu)); break;
		case 0x21: SET_H(cpu, GET_C(cpu)); break;
		case 0x22: SET_H(cpu, GET_D(cpu)); break;
		case 0x23: SET_H(cpu, GET_E(cpu)); break;
		case 0x24: SET_H(cpu, GET_H(cpu)); break;
		case 0x25: SET_H(cpu, GET_L(cpu)); break;
		case 0x27: SET_H(cpu, GET_A(cpu)); break;
		case 0x28: SET_L(cpu, GET_B(cpu)); break;
		case 0x29: SET_L(cpu, GET_C(cpu)); break;
		case 0x2a: SET_L(cpu, GET_D(cpu)); break;
		case 0x2b: SET_L(cpu, GET_E(cpu)); break;
		case 0x2c: SET_L(cpu, GET_H(cpu)); break;
		case 0x2d: SET_L(cpu, GET_L(cpu)); break;
		case 0x2f: SET_L(cpu, GET_A(cpu)); break;
		case 0x38: SET_A(cpu, GET_B(cpu)); break;
		case 0x39: SET_A(cpu, GET_C(cpu)); break;
		case 0x3a: SET_A(cpu, GET_D(cpu)); break;
		case 0x3b: SET_A(cpu, GET_E(cpu)); break;
		case 0x3c: SET_A(cpu, GET_H(cpu)); break;
		case 0x3d: SET_A(cpu, GET_L(cpu)); break;
		case 0x3f: SET_A(cpu, GET_A(cpu)); break;
	}

	PC_INC(cpu, 1);
	return 5;
}

static _U8 _h_ori(Cpu *cpu, _U8 opcode) {
	_U8 result = GET_A(cpu) | FETCH_OFFSET(cpu, 1);

	_flagsLogic(cpu, result);
	SET_A(cpu, result);

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_xra(Cpu *cpu, _U8 opcode) {
	_U8 ret = 4;

	_U8 data = GET_A(cpu);

	switch(opcode & 0x07) {
		case 0x00: data ^= GET_B(cpu); break;
		case 0x01: data ^= GET_C(cpu); break;
		case 0x02: data ^= GET_D(cpu); break;
		case 0x03: data ^= GET_E(cpu); break;
		case 0x04: data ^= GET_H(cpu); break;
		case 0x05: data ^= GET_L(cpu); break;
		case 0x06:
			data ^= memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H));
			ret = 7;
			break;
		case 0x07: data ^= GET_A(cpu); break;
		default:
			data = 0;
	}

	_flagsLogic(cpu, data);
	SET_A(cpu, data);

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_xri(Cpu *cpu, _U8 opcode) {
	_U8 result = GET_A(cpu) ^ FETCH_OFFSET(cpu, 1);

	_flagsLogic(cpu, result);
	SET_A(cpu, result);

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_ora(Cpu *cpu, _U8 opcode) {
	_U8 ret = 4;

	_U8 data = GET_A(cpu);

	switch (opcode & 0x07) {
		case 0x00: data |= GET_B(cpu); break;
		case 0x01: data |= GET_C(cpu); break;
		case 0x02: data |= GET_D(cpu); break;
		case 0x03: data |= GET_E(cpu); break;
		case 0x04: data |= GET_H(cpu); break;
		case 0x05: data |= GET_L(cpu); break;
		case 0x06:
			data |= memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H));
			ret = 7;
			break;

		case 0x07: data |= GET_A(cpu); break;
		default:
			data = 0;
	}

	_flagsLogic(cpu, data);
	SET_A(cpu, data);

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_out(Cpu *cpu, _U8 opcode) {
	IO_write_byte(cpu->io, FETCH_OFFSET(cpu, 1), GET_A(cpu));

	PC_INC(cpu, 2);
	return 10;
}

static _U8 _h_inx(Cpu *cpu, _U8 opcode) {
	switch (opcode & 0x30) {
		case 0x00: INC_PAIR_W(cpu->B);  break;
		case 0x10: INC_PAIR_W(cpu->D);  break;
		case 0x20: INC_PAIR_W(cpu->H);  break;
		case 0x30: INC_PAIR_W(cpu->SP); break;
	}

	PC_INC(cpu, 1);
	return 5;
}

static _U8 _h_dcx(Cpu *cpu, _U8 opcode) {
	switch (opcode & 0x30) {
		case 0x00: DEC_PAIR_W(cpu->B);  break;
		case 0x10: DEC_PAIR_W(cpu->D);  break;
		case 0x20: DEC_PAIR_W(cpu->H);  break;
		case 0x30: DEC_PAIR_W(cpu->SP); break;
	}

	PC_INC(cpu, 1);
	return 5;
}

static _U8 _h_inr(Cpu *cpu, _U8 opcode) {
	_U8 ret = 5;

	_U8 left;

	switch (opcode & 0x38) {
		case 0x00: left = GET_B(cpu); break;
		case 0x10: left = GET_D(cpu); break;
		case 0x20: left = GET_H(cpu); break;
		case 0x30: left = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H)); break;
		case 0x08: left = GET_C(cpu); break;
		case 0x18: left = GET_E(cpu); break;
		case 0x28: left = GET_L(cpu); break;
		case 0x38: left = GET_A(cpu); break;
		default:
			left = 0;
	}

	left = _flagsAdd(cpu, left, 1, 0, 1);

	switch (opcode & 0x38) {
		case 0x00: SET_B(cpu, left); break;
		case 0x10: SET_D(cpu, left); break;
		case 0x20: SET_H(cpu, left); break;
		case 0x30: memory_write_byte(cpu->memory, GET_PAIR_W(cpu->H), left); ret = 10; break;
		case 0x08: SET_C(cpu, left); break;
		case 0x18: SET_E(cpu, left); break;
		case 0x28: SET_L(cpu, left); break;
		case 0x38: SET_A(cpu, left); break;
		default:
			break;
	}

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_dcrc(Cpu *cpu, _U8 opcode) {
	_U8 ret = 5;

	_U8 left;

	switch (opcode & 0x38) {
		case 0x00: left = GET_B(cpu); break;
		case 0x10: left = GET_D(cpu); break;
		case 0x20: left = GET_H(cpu); break;
		case 0x30: left = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H)); break;
		case 0x08: left = GET_C(cpu); break;
		case 0x18: left = GET_E(cpu); break;
		case 0x28: left = GET_L(cpu); break;
		case 0x38: left = GET_A(cpu); break;
		default:
			left = 0;
	}

	left = _flagsSub(cpu, left, 1, 0, 1);

	switch (opcode & 0x38) {
		case 0x00: SET_B(cpu, left); break;
		case 0x10: SET_D(cpu, left); break;
		case 0x20: SET_H(cpu, left); break;
		case 0x30: memory_write_byte(cpu->memory, GET_PAIR_W(cpu->H), left); ret = 10; break;
		case 0x08: SET_C(cpu, left); break;
		case 0x18: SET_E(cpu, left); break;
		case 0x28: SET_L(cpu, left); break;
		case 0x38: SET_A(cpu, left); break;
		default:
			break;
	}

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_hlt(Cpu *cpu, _U8 opcode) {
	PC_INC(cpu, 1);

	cpu->halt = 1;

	return 7;
}

static _U8 _h_ana(Cpu *cpu, _U8 opcode) {
	_U8 ret = 4;

	_U8 data = GET_A(cpu);

	switch (opcode & 0x07) {
		case 0x00: data &= GET_B(cpu); break;
		case 0x01: data &= GET_C(cpu); break;
		case 0x02: data &= GET_D(cpu); break;
		case 0x03: data &= GET_E(cpu); break;
		case 0x04: data &= GET_H(cpu); break;
		case 0x05: data &= GET_L(cpu); break;
		case 0x06:
			data &= memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H));
			ret = 7;
			break;
		case 0x07: data &= GET_A(cpu); break;
	}

	_flagsLogic(cpu, data);
	SET_A(cpu, data);

	PC_INC(cpu, 1);
	return ret;
}
#endif

#if 0
static _U8 _h_ani(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, GET_A(cpu) & FETCH_OFFSET(cpu, 1));

	_flagsLogic(cpu, GET_A(cpu));

	PC_INC(cpu, 2);
	return 7;
}
#else
static void _h_ani(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP   = GET_Z(cpu);
				cpu->ALUOP = ALU_OP_AND;

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}
#endif

#if 0
static _U8 _h_jmp(Cpu *cpu, _U8 opcode) {
	return _jumpCond(cpu, opcode, 1);
}
#else

static void _h_jmp(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				_cpu_next_instruction(cpu, GET_PAIR_W(cpu->W));
			}
			break;

		default:
			break;
	}
}
#endif

static void _h_jcnd(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				_U8 jump = 0;

				switch (cpu->ir & 0x38) {
					case 0x00: jump = ! FLAG_IS_SET(cpu, FLAG_ZERO);   break;
					case 0x08: jump =   FLAG_IS_SET(cpu, FLAG_ZERO);   break;
					case 0x10: jump = ! FLAG_IS_SET(cpu, FLAG_CARRY);  break;
					case 0x18: jump =   FLAG_IS_SET(cpu, FLAG_CARRY);  break;
					case 0x20: jump = ! FLAG_IS_SET(cpu, FLAG_PARITY); break;
					case 0x28: jump =   FLAG_IS_SET(cpu, FLAG_PARITY); break;
					case 0x30: jump = ! FLAG_IS_SET(cpu, FLAG_SIGN);   break;
					case 0x38: jump =   FLAG_IS_SET(cpu, FLAG_SIGN);   break;
				}

				if (jump) {
					_cpu_next_instruction(cpu, GET_PAIR_W(cpu->W));

				} else {
					_cpu_next_instruction(cpu, cpu->PC);
				}
			}
			break;

		default:
			break;
	}
}

#if 0


static _U8 _callCond(Cpu *cpu, _U8 cond) {
	_U8 ret = 11;

	if (cond) {
		RegPair pcNew;
		RegPair pcRet;

		SET_PAIR_LB(pcNew, FETCH_OFFSET(cpu, 1));
		SET_PAIR_HB(pcNew, FETCH_OFFSET(cpu, 2));

		SET_PAIR_W(pcRet, cpu->PC + 3);

		DEC_PAIR_W(cpu->SP);
		memory_write_byte(cpu->memory, GET_PAIR_W(cpu->SP), GET_PAIR_HB(pcRet));

		DEC_PAIR_W(cpu->SP);
		memory_write_byte(cpu->memory, GET_PAIR_W(cpu->SP), GET_PAIR_LB(pcRet));

		cpu->PC = GET_PAIR_W(pcNew);
		ret = 17;

	} else {
		PC_INC(cpu, 3);
	}

	return ret;
}

static _U8 _h_cc(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, FLAG_IS_SET(cpu, FLAG_CARRY));
}

static _U8 _h_cnc(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, ! FLAG_IS_SET(cpu, FLAG_CARRY));
}

static _U8 _h_cz(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, FLAG_IS_SET(cpu, FLAG_ZERO));
}

static _U8 _h_cnz(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, ! FLAG_IS_SET(cpu, FLAG_ZERO));
}

static _U8 _h_cpe(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, FLAG_IS_SET(cpu, FLAG_PARITY));
}

static _U8 _h_cpo(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, ! FLAG_IS_SET(cpu, FLAG_PARITY));
}

static _U8 _h_cm(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, FLAG_IS_SET(cpu, FLAG_SIGN));
}

static _U8 _h_cp(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, ! FLAG_IS_SET(cpu, FLAG_SIGN));
}

static void _ret(Cpu *cpu) {
	RegPair pair;

	SET_PAIR_LB(pair, memory_read_byte(cpu->memory, GET_PAIR_W(cpu->SP)));
	INC_PAIR_W(cpu->SP);

	SET_PAIR_HB(pair, memory_read_byte(cpu->memory, GET_PAIR_W(cpu->SP)));
	INC_PAIR_W(cpu->SP);

	cpu->PC = GET_PAIR_W(pair);
}

static _U8 _retCond(Cpu *cpu, _U8 shouldRet) {
	if (shouldRet) {
		_ret(cpu);
		return 11;

	} else {
		PC_INC(cpu, 1);
		return 5;
	}
}

static _U8 _h_rpe(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, FLAG_IS_SET(cpu, FLAG_PARITY));
}

static _U8 _h_rpo(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, ! FLAG_IS_SET(cpu, FLAG_PARITY));
}

static _U8 _h_rm(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, FLAG_IS_SET(cpu, FLAG_SIGN));
}

static _U8 _h_rp(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, ! FLAG_IS_SET(cpu, FLAG_SIGN));
}

static _U8 _h_rc(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, FLAG_IS_SET(cpu, FLAG_CARRY));
}

static _U8 _h_rnc(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, ! FLAG_IS_SET(cpu, FLAG_CARRY));
}

static _U8 _h_rz(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, FLAG_IS_SET(cpu, FLAG_ZERO));
}

static _U8 _h_rnz(Cpu *cpu, _U8 opcode) {
	return _retCond(cpu, ! FLAG_IS_SET(cpu, FLAG_ZERO));
}

static _U8 _h_call(Cpu *cpu, _U8 opcode) {
	return _callCond(cpu, 1);
}

static void _push(Cpu *cpu, RegPair *pair) {
	DEC_PAIR_W(cpu->SP);
	memory_write_byte(cpu->memory, GET_PAIR_W(cpu->SP), GET_PAIR_HB(*pair));

	DEC_PAIR_W(cpu->SP);
	memory_write_byte(cpu->memory, GET_PAIR_W(cpu->SP), GET_PAIR_LB(*pair));
}

static _U8 _h_push(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	switch (opcode & 0x30) {
		case 0x00: COPY_PAIR_W(pair, cpu->B);   break;
		case 0x10: COPY_PAIR_W(pair, cpu->D);   break;
		case 0x20: COPY_PAIR_W(pair, cpu->H);   break;
		case 0x30: COPY_PAIR_W(pair, cpu->PSW); break;
	}

	_push(cpu, &pair);

	PC_INC(cpu, 1);
	return 11;
}

static void _pop(Cpu *cpu, RegPair *pair) {
	SET_PAIR_LB(*pair, memory_read_byte(cpu->memory, GET_PAIR_W(cpu->SP)));
	INC_PAIR_W(cpu->SP);

	SET_PAIR_HB(*pair, memory_read_byte(cpu->memory, GET_PAIR_W(cpu->SP)));
	INC_PAIR_W(cpu->SP);
}

static _U8 _h_pop(Cpu *cpu, _U8 opcode) {
	RegPair pair;

	_pop(cpu, &pair);

	switch (opcode & 0x30) {
		case 0x00: COPY_PAIR_W(cpu->B,   pair); break;
		case 0x10: COPY_PAIR_W(cpu->D,   pair); break;
		case 0x20: COPY_PAIR_W(cpu->H,   pair); break;
		case 0x30: COPY_PAIR_W(cpu->PSW, pair); break;
	}

	PC_INC(cpu, 1);
	return 11;
}

static _U8 _h_sphl(Cpu *cpu, _U8 opcode) {
	COPY_PAIR_W(cpu->SP, cpu->H);

	PC_INC(cpu, 1);
	return 5;
}

static _U8 _h_xthl(Cpu *cpu, _U8 opcode) {
	RegPair tmp;

	_pop(cpu, &tmp);
	_push(cpu, &cpu->H);
	COPY_PAIR_W(cpu->H, tmp);

	PC_INC(cpu, 1);
	return 18;
}

static _U8 _h_pchl(Cpu *cpu, _U8 opcode) {
	cpu->PC = GET_PAIR_W(cpu->H);
	return 5;
}

static _U8 _h_xchg(Cpu *cpu, _U8 opcode) {
	RegPair tmp;

	COPY_PAIR_W(tmp, cpu->H);
	COPY_PAIR_W(cpu->H, cpu->D);
	COPY_PAIR_W(cpu->D, tmp);

	PC_INC(cpu, 1);
	return 5;
}

static _U8 _sub(Cpu *cpu, _U8 opcode, _U8 carry) {
	_U8 ret = 4;

	_U8 right;

	switch(opcode & 0x07) {
		case 0x00: right = GET_B(cpu); break;
		case 0x01: right = GET_C(cpu); break;
		case 0x02: right = GET_D(cpu); break;
		case 0x03: right = GET_E(cpu); break;
		case 0x04: right = GET_H(cpu); break;
		case 0x05: right = GET_L(cpu); break;
		case 0x06: right = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H)); ret = 7; break;
		case 0x07: right = GET_A(cpu); break;
	}

	SET_A(cpu, _flagsSub(cpu, GET_A(cpu), right, carry, 0));

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_sub(Cpu *cpu, _U8 opcode) {
	return _sub(cpu, opcode, 0);
}

static _U8 _h_sbb(Cpu *cpu, _U8 opcode) {
	return _sub(cpu, opcode, 1);
}

static _U8 _add(Cpu *cpu, _U8 opcode, _U8 carry) {
	_U8 ret = 4;

	_U16 left;

	switch(opcode & 0x07) {
		case 0x00: left = GET_B(cpu); break; // ADC B
		case 0x01: left = GET_C(cpu); break; // ADC C
		case 0x02: left = GET_D(cpu); break; // ADC D
		case 0x03: left = GET_E(cpu); break; // ADC E
		case 0x04: left = GET_H(cpu); break; // ADC H
		case 0x05: left = GET_L(cpu); break; // ADC L
		case 0x06: left = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H)); ret = 7; break; // ADC M
		case 0x07: left = GET_A(cpu); break; // ADC A
	}

	SET_A(cpu, _flagsAdd(cpu, left, GET_A(cpu), carry, 0));

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_add(Cpu *cpu, _U8 opcode) {
	return _add(cpu, opcode, 0);
}

static _U8 _h_adc(Cpu *cpu, _U8 opcode) {
	return _add(cpu, opcode, 1);
}

static _U8 _h_dad(Cpu *cpu, _U8 opcode) {
	_U32 val = GET_PAIR_W(cpu->H);

	switch (opcode & 0x30) {
		case 0x00: val += (_U32)GET_PAIR_W(cpu->B);  break;
		case 0x10: val += (_U32)GET_PAIR_W(cpu->D);  break;
		case 0x20: val += (_U32)GET_PAIR_W(cpu->H);  break;
		case 0x30: val += (_U32)GET_PAIR_W(cpu->SP); break;
		default:
			break;
	}

	SET_PAIR_W(cpu->H, val);

	// Only carry flag is affected
	if ((val >> 16) != 0) {
		FLAG_SET(cpu, FLAG_CARRY);
	} else {
		FLAG_CLEAR(cpu, FLAG_CARRY);
	}

	PC_INC(cpu, 1);
	return 10;
}

static _U8 _h_daa(Cpu *cpu, _U8 opcode) {
	_U16 a = GET_A(cpu);

	{
		_U8 lb = a & 0x0f;

		if ((lb > 9) || FLAG_IS_SET(cpu, FLAG_AC)) {
			lb += 6;

			if (lb >> 4) {
				FLAG_SET(cpu, FLAG_AC);
			} else {
				FLAG_CLEAR(cpu, FLAG_AC);
			}

			a = (a + 6) & 0xff;
		}
	}

	{
		_U8 hb = a >> 4;

		if ((hb > 9) || FLAG_IS_SET(cpu, FLAG_CARRY)) {
			hb += 6;

			if (hb >> 4) {
				FLAG_SET(cpu, FLAG_CARRY);
			} else {
				FLAG_CLEAR(cpu, FLAG_CARRY);
			}

			a = (a + (6 << 4)) & 0xff;
		}
	}

	SET_A(cpu, a);

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_adi(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, _flagsAdd(cpu, GET_A(cpu), FETCH_OFFSET(cpu, 1), 0, 0));

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_sbi(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, _flagsSub(cpu, GET_A(cpu), FETCH_OFFSET(cpu, 1), 1, 0));

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_sui(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, _flagsSub(cpu, GET_A(cpu), FETCH_OFFSET(cpu, 1), 0, 0));

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_aci(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, _flagsAdd(cpu, GET_A(cpu), FETCH_OFFSET(cpu, 1), 1, 0));

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_cpi(Cpu *cpu, _U8 opcode) {
	_flagsSub(cpu, GET_A(cpu), FETCH_OFFSET(cpu, 1), 0, 0);

	PC_INC(cpu, 2);
	return 7;
}

static _U8 _h_cmp(Cpu *cpu, _U8 opcode) {
	_U8 ret = 4;

	_U16 right;

	switch(opcode & 0x07) {
		case 0x00: right = GET_B(cpu); break;
		case 0x01: right = GET_C(cpu); break;
		case 0x02: right = GET_D(cpu); break;
		case 0x03: right = GET_E(cpu); break;
		case 0x04: right = GET_H(cpu); break;
		case 0x05: right = GET_L(cpu); break;
		case 0x06: right = memory_read_byte(cpu->memory, GET_PAIR_W(cpu->H)); ret = 7; break;
		case 0x07: right = GET_A(cpu); break;
	}

	_flagsSub(cpu, GET_A(cpu), right, 0, 0);

	PC_INC(cpu, 1);
	return ret;
}

static _U8 _h_mvi(Cpu *cpu, _U8 opcode) {
	_U8 data   = FETCH_OFFSET(cpu, 1);
	_U8 cycles = 7;

	switch (opcode & 0x38) {
		case 0x00: SET_B(cpu, data); break;
		case 0x10: SET_D(cpu, data); break;
		case 0x20: SET_H(cpu, data); break;
		case 0x30:
			memory_write_byte(cpu->memory, GET_PAIR_W(cpu->H), data);
			cycles = 10;
			break;
		case 0x08: SET_C(cpu, data); break;
		case 0x18: SET_E(cpu, data); break;
		case 0x28: SET_L(cpu, data); break;
		case 0x38: SET_A(cpu, data); break;
	}

	PC_INC(cpu, 2);
	return cycles;
}

static _U8 _h_stc(Cpu *cpu, _U8 opcode) {
	FLAG_SET(cpu, FLAG_CARRY);

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_cmc(Cpu *cpu, _U8 opcode) {
	FLAG_TOGGLE(cpu, FLAG_CARRY);

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_cma(Cpu *cpu, _U8 opcode) {
	SET_A(cpu, ~GET_A(cpu));

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_rlc(Cpu *cpu, _U8 opcode) {
	_U8 carry = (GET_A(cpu) & 0x80) != 0;

	SET_A(cpu, (GET_A(cpu) << 1) | carry);

	if (carry) {
		FLAG_SET(cpu, FLAG_CARRY);
	} else {
		FLAG_CLEAR(cpu, FLAG_CARRY);
	}

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_rrc(Cpu *cpu, _U8 opcode) {
	_U8 carry = GET_A(cpu) & 0x01;

	SET_A(cpu, (GET_A(cpu) >> 1) | (carry << 7));

	if (carry) {
		FLAG_SET(cpu, FLAG_CARRY);
	} else {
		FLAG_CLEAR(cpu, FLAG_CARRY);
	}

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_ral(Cpu *cpu, _U8 opcode) {
	_U8 carry = FLAG_IS_SET(cpu, FLAG_CARRY);

	if (GET_A(cpu) & 0x80) {
		FLAG_SET(cpu, FLAG_CARRY);
	} else {
		FLAG_CLEAR(cpu, FLAG_CARRY);
	}

	SET_A(cpu, (GET_A(cpu) << 1) | carry);

	PC_INC(cpu, 1);
	return 4;
}

static _U8 _h_rar(Cpu *cpu, _U8 opcode) {
	_U8 carry = FLAG_IS_SET(cpu, FLAG_CARRY);

	if (GET_A(cpu) & 0x01) {
		FLAG_SET(cpu, FLAG_CARRY);
	} else {
		FLAG_CLEAR(cpu, FLAG_CARRY);
	}

	SET_A(cpu, (GET_A(cpu) >> 1) | (carry << 7));

	PC_INC(cpu, 1);
	return 4;
}
#endif

void cpu_init(Cpu *cpu) {
	SET_PAIR_W(cpu->PSW, 0);
	SET_PAIR_W(cpu->B,   0);
	SET_PAIR_W(cpu->D,   0);
	SET_PAIR_W(cpu->H,   0);
	SET_PAIR_W(cpu->SP,  0);

	_flagsCommon(cpu, 0);
	// second bit is always set to one
	FLAG_SET(cpu, 0x02);

	cpu->PC             = 0;
	cpu->ticks          = 0;
	cpu->readCycleCount = 0;
	cpu->ALUOP          = ALU_OP_NONE;

	_cpu_next_instruction(cpu, cpu->PC);

	if (! _initialized) {
		for (_U16 i = 0; i < sizeof(_opHandlers) / sizeof(*_opHandlers); i++) {
			_opHandlers[i]   = NULL;
#if defined(REPORT_OPCODES)
			_opcodesNames[i] = NULL;
#endif
		}

		DECLARE_OPCODE(0x00, _h_nop, "NOP");

		DECLARE_OPCODE(0x01, _h_lxi,  "LXI B,d16");
		DECLARE_OPCODE(0x11, _h_lxi,  "LXI D,d16");
		DECLARE_OPCODE(0x21, _h_lxi,  "LXI H,d16");
		DECLARE_OPCODE(0x31, _h_lxi,  "LXI SP,d16");
//		DECLARE_OPCODE(0x32, _h_sta,  "STA a16");
//		DECLARE_OPCODE(0x02, _h_stax, "STAX B");
//		DECLARE_OPCODE(0x12, _h_stax, "STAX D");
//		DECLARE_OPCODE(0x3a, _h_lda,  "LDA a16");
//		DECLARE_OPCODE(0x2a, _h_lhld, "LHLD a16");
//		DECLARE_OPCODE(0x22, _h_shld, "SHLD a16");
//
//		DECLARE_OPCODE(0x0a, _h_ldax, "LDAX B");
//		DECLARE_OPCODE(0x1a, _h_ldax, "LDAX D");
//
//		DECLARE_OPCODE(0xc5, _h_push, "PUSH B");
//		DECLARE_OPCODE(0xd5, _h_push, "PUSH D");
//		DECLARE_OPCODE(0xe5, _h_push, "PUSH H");
//		DECLARE_OPCODE(0xf5, _h_push, "PUSH PSW");
//
//		DECLARE_OPCODE(0xc1, _h_pop, "POP B");
//		DECLARE_OPCODE(0xd1, _h_pop, "POP D");
//		DECLARE_OPCODE(0xe1, _h_pop, "POP H");
//		DECLARE_OPCODE(0xf1, _h_pop, "POP PSW");
//
//		DECLARE_OPCODE(0xf9, _h_sphl, "SPHL");
//		DECLARE_OPCODE(0xe3, _h_xthl, "XTHL");
//		DECLARE_OPCODE(0xe9, _h_pchl, "PCHL");
//
//		DECLARE_OPCODE(0x03, _h_inx, "INX B");
//		DECLARE_OPCODE(0x13, _h_inx, "INX D");
//		DECLARE_OPCODE(0x23, _h_inx, "INX H");
//		DECLARE_OPCODE(0x33, _h_inx, "INX SP");
//
//		DECLARE_OPCODE(0x0b, _h_dcx, "DCX B");
//		DECLARE_OPCODE(0x1b, _h_dcx, "DCX D");
//		DECLARE_OPCODE(0x2b, _h_dcx, "DCX H");
//		DECLARE_OPCODE(0x3b, _h_dcx, "DCX SP");
//
//		DECLARE_OPCODE(0x04, _h_inr, "INR B");
//		DECLARE_OPCODE(0x14, _h_inr, "INR D");
//		DECLARE_OPCODE(0x24, _h_inr, "INR H");
//		DECLARE_OPCODE(0x34, _h_inr, "INR M");
//		DECLARE_OPCODE(0x0c, _h_inr, "INR C");
//		DECLARE_OPCODE(0x1c, _h_inr, "INR E");
//		DECLARE_OPCODE(0x2c, _h_inr, "INR L");
//		DECLARE_OPCODE(0x3c, _h_inr, "INR A");
//
//		DECLARE_OPCODE(0x05, _h_dcrc, "DCR B");
//		DECLARE_OPCODE(0x15, _h_dcrc, "DCR D");
//		DECLARE_OPCODE(0x25, _h_dcrc, "DCR H");
//		DECLARE_OPCODE(0x35, _h_dcrc, "DCR M");
//		DECLARE_OPCODE(0x0d, _h_dcrc, "DCR C");
//		DECLARE_OPCODE(0x1d, _h_dcrc, "DCR E");
//		DECLARE_OPCODE(0x2d, _h_dcrc, "DCR L");
//		DECLARE_OPCODE(0x3d, _h_dcrc, "DCR A");
//
//		DECLARE_OPCODE(0x46, _h_movrM, "MOV B,M");
//		DECLARE_OPCODE(0x4e, _h_movrM, "MOV C,M");
//		DECLARE_OPCODE(0x56, _h_movrM, "MOV D,M");
//		DECLARE_OPCODE(0x5e, _h_movrM, "MOV E,M");
//		DECLARE_OPCODE(0x66, _h_movrM, "MOV H,M");
//		DECLARE_OPCODE(0x6e, _h_movrM, "MOV L,M");
//		DECLARE_OPCODE(0x7e, _h_movrM, "MOV A,M");
//		DECLARE_OPCODE(0x70, _h_movMr, "MOV M,B");
//		DECLARE_OPCODE(0x71, _h_movMr, "MOV M,C");
//		DECLARE_OPCODE(0x72, _h_movMr, "MOV M,D");
//		DECLARE_OPCODE(0x73, _h_movMr, "MOV M,E");
//		DECLARE_OPCODE(0x74, _h_movMr, "MOV M,H");
//		DECLARE_OPCODE(0x75, _h_movMr, "MOV M,L");
//		DECLARE_OPCODE(0x77, _h_movMr, "MOV M,A");
//
//		DECLARE_OPCODE(0x40, _h_movrr, "MOV B,B");
//		DECLARE_OPCODE(0x41, _h_movrr, "MOV B,C");
//		DECLARE_OPCODE(0x42, _h_movrr, "MOV B,D");
//		DECLARE_OPCODE(0x43, _h_movrr, "MOV B,E");
//		DECLARE_OPCODE(0x44, _h_movrr, "MOV B,H");
//		DECLARE_OPCODE(0x45, _h_movrr, "MOV B,L");
//		DECLARE_OPCODE(0x47, _h_movrr, "MOV B,A");
//		DECLARE_OPCODE(0x48, _h_movrr, "MOV C,B");
//		DECLARE_OPCODE(0x49, _h_movrr, "MOV C,C");
//		DECLARE_OPCODE(0x4a, _h_movrr, "MOV C,D");
//		DECLARE_OPCODE(0x4b, _h_movrr, "MOV C,E");
//		DECLARE_OPCODE(0x4c, _h_movrr, "MOV C,H");
//		DECLARE_OPCODE(0x4d, _h_movrr, "MOV C,L");
//		DECLARE_OPCODE(0x4f, _h_movrr, "MOV C,A");
//		DECLARE_OPCODE(0x50, _h_movrr, "MOV D,B");
//		DECLARE_OPCODE(0x51, _h_movrr, "MOV D,C");
//		DECLARE_OPCODE(0x52, _h_movrr, "MOV D,D");
//		DECLARE_OPCODE(0x53, _h_movrr, "MOV D,E");
//		DECLARE_OPCODE(0x54, _h_movrr, "MOV D,H");
//		DECLARE_OPCODE(0x55, _h_movrr, "MOV D,L");
//		DECLARE_OPCODE(0x57, _h_movrr, "MOV D,A");
//		DECLARE_OPCODE(0x58, _h_movrr, "MOV E,B");
//		DECLARE_OPCODE(0x59, _h_movrr, "MOV E,C");
//		DECLARE_OPCODE(0x5a, _h_movrr, "MOV E,D");
//		DECLARE_OPCODE(0x5b, _h_movrr, "MOV E,E");
//		DECLARE_OPCODE(0x5c, _h_movrr, "MOV E,H");
//		DECLARE_OPCODE(0x5d, _h_movrr, "MOV E,L");
//		DECLARE_OPCODE(0x5f, _h_movrr, "MOV E,A");
//		DECLARE_OPCODE(0x60, _h_movrr, "MOV H,B");
//		DECLARE_OPCODE(0x61, _h_movrr, "MOV H,C");
//		DECLARE_OPCODE(0x62, _h_movrr, "MOV H,D");
//		DECLARE_OPCODE(0x63, _h_movrr, "MOV H,E");
//		DECLARE_OPCODE(0x64, _h_movrr, "MOV H,H");
//		DECLARE_OPCODE(0x65, _h_movrr, "MOV H,L");
//		DECLARE_OPCODE(0x67, _h_movrr, "MOV H,A");
//		DECLARE_OPCODE(0x68, _h_movrr, "MOV L,B");
//		DECLARE_OPCODE(0x69, _h_movrr, "MOV L,C");
//		DECLARE_OPCODE(0x6a, _h_movrr, "MOV L,D");
//		DECLARE_OPCODE(0x6b, _h_movrr, "MOV L,E");
//		DECLARE_OPCODE(0x6c, _h_movrr, "MOV L,H");
//		DECLARE_OPCODE(0x6d, _h_movrr, "MOV L,L");
//		DECLARE_OPCODE(0x6f, _h_movrr, "MOV L,A");
//		DECLARE_OPCODE(0x78, _h_movrr, "MOV A,B");
//		DECLARE_OPCODE(0x79, _h_movrr, "MOV A,C");
//		DECLARE_OPCODE(0x7a, _h_movrr, "MOV A,D");
//		DECLARE_OPCODE(0x7b, _h_movrr, "MOV A,E");
//		DECLARE_OPCODE(0x7c, _h_movrr, "MOV A,H");
//		DECLARE_OPCODE(0x7d, _h_movrr, "MOV A,L");
//		DECLARE_OPCODE(0x7f, _h_movrr, "MOV A,A");
//
//		DECLARE_OPCODE(0x06, _h_mvi,   "MVI B,d8");
//		DECLARE_OPCODE(0x16, _h_mvi,   "MVI D,d8");
//		DECLARE_OPCODE(0x26, _h_mvi,   "MVI H,d8");
//		DECLARE_OPCODE(0x36, _h_mvi,   "MVI M,d8");
//		DECLARE_OPCODE(0x0e, _h_mvi,   "MVI C,d8");
//		DECLARE_OPCODE(0x1e, _h_mvi,   "MVI E,d8");
//		DECLARE_OPCODE(0x2e, _h_mvi,   "MVI L,d8");
//		DECLARE_OPCODE(0x3e, _h_mvi,   "MVI A,d8");
//
//		DECLARE_OPCODE(0xb0, _h_ora, "ORA B");
//		DECLARE_OPCODE(0xb1, _h_ora, "ORA C");
//		DECLARE_OPCODE(0xb2, _h_ora, "ORA D");
//		DECLARE_OPCODE(0xb3, _h_ora, "ORA E");
//		DECLARE_OPCODE(0xb4, _h_ora, "ORA H");
//		DECLARE_OPCODE(0xb5, _h_ora, "ORA L");
//		DECLARE_OPCODE(0xb6, _h_ora, "ORA M");
//		DECLARE_OPCODE(0xb7, _h_ora, "ORA A");
//
//		DECLARE_OPCODE(0xa0, _h_ana,  "ANA B");
//		DECLARE_OPCODE(0xa1, _h_ana,  "ANA C");
//		DECLARE_OPCODE(0xa2, _h_ana,  "ANA D");
//		DECLARE_OPCODE(0xa3, _h_ana,  "ANA E");
//		DECLARE_OPCODE(0xa4, _h_ana,  "ANA H");
//		DECLARE_OPCODE(0xa5, _h_ana,  "ANA L");
//		DECLARE_OPCODE(0xa6, _h_ana,  "ANA M");
//		DECLARE_OPCODE(0xa7, _h_ana,  "ANA A");
//
		DECLARE_OPCODE(0xe6, _h_ani,  "ANI d8");
//		DECLARE_OPCODE(0xf6, _h_ori,  "ORI d8");
//
//		DECLARE_OPCODE(0xa8, _h_xra,  "XRA B");
//		DECLARE_OPCODE(0xa9, _h_xra,  "XRA C");
//		DECLARE_OPCODE(0xaa, _h_xra,  "XRA D");
//		DECLARE_OPCODE(0xab, _h_xra,  "XRA E");
//		DECLARE_OPCODE(0xac, _h_xra,  "XRA H");
//		DECLARE_OPCODE(0xad, _h_xra,  "XRA L");
//		DECLARE_OPCODE(0xae, _h_xra,  "XRA M");
//		DECLARE_OPCODE(0xaf, _h_xra,  "XRA A");
//		DECLARE_OPCODE(0xee, _h_xri,  "XRI d8");
//
//		DECLARE_OPCODE(0xd3, _h_out,  "OUT, d8");
//		DECLARE_OPCODE(0x76, _h_hlt,  "HLT");
//		DECLARE_OPCODE(0xcd, _h_call, "CALL a16");
//		DECLARE_OPCODE(0xeb, _h_xchg, "XCHG");
//
		DECLARE_OPCODE(0xc3, _h_jmp,  "JMP a16");
		DECLARE_OPCODE(0xca, _h_jcnd, "JZ  a16");
		DECLARE_OPCODE(0xd2, _h_jcnd, "JNC a16");
		DECLARE_OPCODE(0xea, _h_jcnd, "JPE a16");
		DECLARE_OPCODE(0xf2, _h_jcnd, "JP  a16");
		DECLARE_OPCODE(0xda, _h_jcnd, "JC  a16");
		DECLARE_OPCODE(0xfa, _h_jcnd, "JM  a16");
		DECLARE_OPCODE(0xc2, _h_jcnd, "JNZ a16");
		DECLARE_OPCODE(0xe2, _h_jcnd, "JPO a16");
//		DECLARE_OPCODE(0xdc, _h_cc,   "CC  a16");
//		DECLARE_OPCODE(0xe4, _h_cpo,  "CPO a16");
//		DECLARE_OPCODE(0xfc, _h_cm,   "CM  a16");
//		DECLARE_OPCODE(0xc4, _h_cnz,  "CNZ a16");
//		DECLARE_OPCODE(0xd4, _h_cnc,  "CNC a16");
//		DECLARE_OPCODE(0xec, _h_cpe,  "CPE a16");
//		DECLARE_OPCODE(0xf4, _h_cp,   "CP  a16");
//		DECLARE_OPCODE(0xcc, _h_cz,   "CZ  a16");
//
//		DECLARE_OPCODE(0xe0, _h_rpo,  "RPO");
//		DECLARE_OPCODE(0xe8, _h_rpe,  "RPE");
//		DECLARE_OPCODE(0xf0, _h_rp,   "RP");
//		DECLARE_OPCODE(0xf8, _h_rm,   "RM");
//		DECLARE_OPCODE(0xd0, _h_rnc,  "RNC");
//		DECLARE_OPCODE(0xd8, _h_rc,   "RC");
//		DECLARE_OPCODE(0xc0, _h_rnz,  "RNZ");
//		DECLARE_OPCODE(0xc8, _h_rz,   "RZ");
//
//		DECLARE_OPCODE(0xde, _h_sbi,  "SBI");
//		DECLARE_OPCODE(0xd6, _h_sui,  "SUI");
//		DECLARE_OPCODE(0xce, _h_aci,  "ACI");
//		DECLARE_OPCODE(0xfe, _h_cpi,  "CPI");
//
//		DECLARE_OPCODE(0xb8, _h_cmp,  "CMP B");
//		DECLARE_OPCODE(0xb9, _h_cmp,  "CMP C");
//		DECLARE_OPCODE(0xba, _h_cmp,  "CMP D");
//		DECLARE_OPCODE(0xbb, _h_cmp,  "CMP E");
//		DECLARE_OPCODE(0xbc, _h_cmp,  "CMP H");
//		DECLARE_OPCODE(0xbd, _h_cmp,  "CMP L");
//		DECLARE_OPCODE(0xbe, _h_cmp,  "CMP M");
//		DECLARE_OPCODE(0xbf, _h_cmp,  "CMP A");
//
//		DECLARE_OPCODE(0xc6, _h_adi,  "ADI");
//
//		DECLARE_OPCODE(0x80, _h_add,  "ADD B");
//		DECLARE_OPCODE(0x81, _h_add,  "ADD C");
//		DECLARE_OPCODE(0x82, _h_add,  "ADD D");
//		DECLARE_OPCODE(0x83, _h_add,  "ADD E");
//		DECLARE_OPCODE(0x84, _h_add,  "ADD H");
//		DECLARE_OPCODE(0x85, _h_add,  "ADD L");
//		DECLARE_OPCODE(0x86, _h_add,  "ADD M");
//		DECLARE_OPCODE(0x87, _h_add,  "ADD A");
//
//		DECLARE_OPCODE(0x88, _h_adc,  "ADC B");
//		DECLARE_OPCODE(0x89, _h_adc,  "ADC C");
//		DECLARE_OPCODE(0x8a, _h_adc,  "ADC D");
//		DECLARE_OPCODE(0x8b, _h_adc,  "ADC E");
//		DECLARE_OPCODE(0x8c, _h_adc,  "ADC H");
//		DECLARE_OPCODE(0x8d, _h_adc,  "ADC L");
//		DECLARE_OPCODE(0x8e, _h_adc,  "ADC M");
//		DECLARE_OPCODE(0x8f, _h_adc,  "ADC A");
//
//		DECLARE_OPCODE(0x09, _h_dad,  "DAD B");
//		DECLARE_OPCODE(0x19, _h_dad,  "DAD D");
//		DECLARE_OPCODE(0x29, _h_dad,  "DAD H");
//		DECLARE_OPCODE(0x39, _h_dad,  "DAD SP");
//
//		DECLARE_OPCODE(0x27, _h_daa,  "DAA");
//
//		DECLARE_OPCODE(0x90, _h_sub,  "SUB B");
//		DECLARE_OPCODE(0x91, _h_sub,  "SUB C");
//		DECLARE_OPCODE(0x92, _h_sub,  "SUB D");
//		DECLARE_OPCODE(0x93, _h_sub,  "SUB E");
//		DECLARE_OPCODE(0x94, _h_sub,  "SUB H");
//		DECLARE_OPCODE(0x95, _h_sub,  "SUB L");
//		DECLARE_OPCODE(0x96, _h_sub,  "SUB M");
//		DECLARE_OPCODE(0x97, _h_sub,  "SUB A");
//
//		DECLARE_OPCODE(0x98, _h_sbb,  "SBB B");
//		DECLARE_OPCODE(0x99, _h_sbb,  "SBB C");
//		DECLARE_OPCODE(0x9a, _h_sbb,  "SBB D");
//		DECLARE_OPCODE(0x9b, _h_sbb,  "SBB E");
//		DECLARE_OPCODE(0x9c, _h_sbb,  "SBB H");
//		DECLARE_OPCODE(0x9d, _h_sbb,  "SBB L");
//		DECLARE_OPCODE(0x9e, _h_sbb,  "SBB M");
//		DECLARE_OPCODE(0x9f, _h_sbb,  "SBB A");
//
//		DECLARE_OPCODE(0x37, _h_stc,  "STC");
//		DECLARE_OPCODE(0x3f, _h_cmc,  "CMC");
//		DECLARE_OPCODE(0x2f, _h_cma,  "CMA");
//
//		DECLARE_OPCODE(0x07, _h_rlc,  "RLC");
//		DECLARE_OPCODE(0x0f, _h_rrc,  "RRC");
//		DECLARE_OPCODE(0x17, _h_ral,  "RAL");
//		DECLARE_OPCODE(0x1f, _h_rar,  "RAR");
	}
}


void cpu_reset(Cpu *cpu) {
	cpu->pins.RESET = 1;
}

static _U8 _statusBitsPerCycle[] = {
	STATUS_FLAG_WO | STATUS_FLAG_M1 | STATUS_FLAG_MEMR,                   // INST FETCH
	STATUS_FLAG_WO | STATUS_FLAG_MEMR,                                    // MEM READ
	0,                                                                    // MEM WRITE
	STATUS_FLAG_WO | STATUS_FLAG_STACK | STATUS_FLAG_MEMR,                // STACK READ
	STATUS_FLAG_STACK,                                                    // STACK WRITE
	STATUS_FLAG_WO | STATUS_FLAG_INP,                                     // INPUT READ
	STATUS_FLAG_OUT,                                                      // OUTPUT WRITE
	STATUS_FLAG_INTA | STATUS_FLAG_WO | STATUS_FLAG_M1,                   // INT ACK
	STATUS_FLAG_WO | STATUS_FLAG_HLTA | STATUS_FLAG_MEMR,                 // HALT ACK
	STATUS_FLAG_INTA | STATUS_FLAG_WO | STATUS_FLAG_HLTA | STATUS_FLAG_M1 // HALT + INT ACK
};

void cpu_phase(Cpu *cpu) {
	if (cpu->p1) {
		// phase1
		switch (cpu->state) {
			case 1:
				{
					if (cpu->cycleType != CYCLE_TYPE_MEMORY_READ) {
						cpu->readCycleCount = 0;
					}
				}
				break;

			case 3:
				{
					if (cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) {
						cpu->ir         = cpu->pins.DATA;
						cpu->irCallback = _opHandlers[cpu->ir];

						if (cpu->irCallback == NULL) {
							ERR(("Opcode %02x is not implemented!", cpu->pins.DATA));

							abort();
						}

						DBG(("OPCODE: %s", _opcodesNames[cpu->pins.DATA]));

					} else if (cpu->cycleType == CYCLE_TYPE_MEMORY_READ) {
						if (cpu->readCycleCount == 1) {
							SET_Z(cpu, cpu->pins.DATA);

						} else {
							SET_W(cpu, cpu->pins.DATA);
						}

						// Increase reg offset fot memory read
						cpu->readCycleCount++;
					}
				}
				break;
		}

	} else {
		// phase2
		switch (cpu->state) {
			case 1:
				{
					// ALU operation
					if (cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) {
						if (cpu->ALUOP == ALU_OP_AND) {
							cpu->ACT = cpu->ACT & cpu->TMP;
						}
					}

					if (
						(cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ)
					) {
						cpu->pins.ADDRESS = cpu->internalAddress;
					}

					cpu->pins.SYNC = 1;
					cpu->pins.DATA = _statusBitsPerCycle[cpu->cycleType];
				}
				break;

			case 2:
				{
					// ALU flags and copy
					if (cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) {
						if (cpu->ALUOP == ALU_OP_AND) {
							_flagsLogic(cpu, cpu->ACT);

							SET_A(cpu, cpu->ACT);

							cpu->ALUOP = ALU_OP_NONE;
						}
					}

					if (
						(cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ)
					) {
						cpu->pins.DBIN = 1;

						cpu->PC = cpu->internalAddress + 1;
					}

					cpu->pins.SYNC = 0;
					cpu->pins.DATA = 0;
				}
				break;

			case 3:
				{
					if (
						(cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ)
					) {
						cpu->pins.DBIN = 0;
					}
				}
				break;

			case 4:
				{
					cpu->pins.ADDRESS = 0;
				}
				break;
		}

		cpu->ticks++;
	}

	if (! cpu->p1) {
		_U8 currentState = cpu->state;

		if (cpu->irCallback) {
			cpu->irCallback(cpu);
		}

		if (cpu->state == currentState) {
			if (cpu->state == 5) {
				cpu->state = 1;
				cpu->cycle++;

			} else {
				cpu->state++;
			}
		}
	}

	cpu->p1 = !cpu->p1;
}

void cpu_state(Cpu *cpu) {
	cpu_phase(cpu); // phase1
	cpu_phase(cpu); // phase2
}

void cpu_cycle(Cpu *cpu) {
	do {
		cpu_state(cpu);
	} while (cpu->state != 1);
}

void cpu_nexti(Cpu *cpu) {
	do {
		cpu_cycle(cpu);
	} while ((cpu->cycleType != CYCLE_TYPE_INSTRUCTION_FETCH) && (cpu->state != 1));
}


void cpu_loop(Cpu *cpu) {
//	while (1) {
//		if (! cpu_step(cpu) || cpu->halt) {
//			break;
//		}
//	}
}

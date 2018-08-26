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

#define GET_FLG(__cpu)(__cpu->PSW.r.b.l)
#define GET_A(__cpu)(__cpu->PSW.r.b.h)
#define GET_B(__cpu)(__cpu->B.r.b.h)
#define GET_C(__cpu)(__cpu->B.r.b.l)
#define GET_D(__cpu)(__cpu->D.r.b.h)
#define GET_E(__cpu)(__cpu->D.r.b.l)
#define GET_H(__cpu)(__cpu->H.r.b.h)
#define GET_L(__cpu)(__cpu->H.r.b.l)
#define GET_W(__cpu)(__cpu->W.r.b.h)
#define GET_Z(__cpu)(__cpu->W.r.b.l)

#define SET_FLG(__cpu, __val){__cpu->PSW.r.b.l = __val; }
#define SET_A(__cpu, __val){__cpu->PSW.r.b.h = __val; }
#define SET_B(__cpu, __val){__cpu->B.r.b.h = __val; }
#define SET_C(__cpu, __val){__cpu->B.r.b.l = __val; }
#define SET_D(__cpu, __val){__cpu->D.r.b.h = __val; }
#define SET_E(__cpu, __val){__cpu->D.r.b.l = __val; }
#define SET_H(__cpu, __val){__cpu->H.r.b.h = __val; }
#define SET_L(__cpu, __val){__cpu->H.r.b.l = __val; }
#define SET_W(__cpu, __val){__cpu->W.r.b.h = __val; }
#define SET_Z(__cpu, __val){__cpu->W.r.b.l = __val; }

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

// false cycles used by internal logic only
#define CYCLE_TYPE_MEMORY_READ_OPCODE 10
#define CYCLE_TYPE_ALU_OPERATION      11

#define ALU_OP_NONE  0
#define ALU_OP_AND   1
#define ALU_OP_OR    2
#define ALU_OP_XOR   3
#define ALU_OP_ADD   4
#define ALU_OP_SUB   5
#define ALU_OP_ROT_R 6
#define ALU_OP_ROT_L 7

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


static __inline void _cpu_next_instruction(Cpu *cpu, _U16 address) {
	cpu->ir              = 0;
	cpu->irCallback      = NULL;
	cpu->cycle           = 1;
	cpu->state           = 1;
	cpu->cycleType       = CYCLE_TYPE_INSTRUCTION_FETCH;
	cpu->readOpcode      = 1;
	cpu->internalAddress = address;
}

static __inline void _cpu_next_cycle(Cpu *cpu, _U8 cycleType, _U16 address) {
	cpu->cycle           = cpu->cycle + 1;
	cpu->state           = 1;
	cpu->cycleType       = cycleType;
	cpu->internalAddress = address;

	if (cpu->cycleType == CYCLE_TYPE_MEMORY_READ_OPCODE) {
		cpu->readOpcode = 1;
		cpu->cycleType  = CYCLE_TYPE_MEMORY_READ;

	} else {
		if (cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) {
			cpu->readOpcode = 1;

		} else {
			cpu->readOpcode = 0;
		}
	}
}

static __inline void _alu_op(Cpu *cpu, _U8 op, _U8 delay, _U8 copyAccu, _U8 includeCarry, _U8 preserveCarry) {
	cpu->ALUOP            = op;
	cpu->ALUDEL           = delay;
	cpu->ALUcarry         = includeCarry;
	cpu->ALUpreserveCarry = preserveCarry;
	cpu->ALUcopyAccu      = copyAccu;
}

static void _h_nop(Cpu *cpu) {
	switch (cpu->state) {
		case 4:
			_cpu_next_instruction(cpu, cpu->PC);
			break;
	}
}

static void _h_lxi(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
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

static void _xta(Cpu *cpu, _U8 load) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				if (! load) {
					cpu->internalData = GET_A(cpu);
				}

				_cpu_next_cycle(cpu, load ? CYCLE_TYPE_MEMORY_READ : CYCLE_TYPE_MEMORY_WRITE, GET_PAIR_W(cpu->W));
			}
			break;

		case 4:
			if (cpu->state == 3) {
				if (load) {
					SET_A(cpu, GET_Z(cpu));
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_sta(Cpu *cpu) {
	_xta(cpu, 0);
}

static void _h_lda(Cpu *cpu) {
	_xta(cpu, 1);
}

static void _xhld(Cpu *cpu, _U8 load) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				if (! load) {
					cpu->internalData = GET_L(cpu);
				}

				_cpu_next_cycle(
					cpu, load ? CYCLE_TYPE_MEMORY_READ : CYCLE_TYPE_MEMORY_WRITE, GET_PAIR_W(cpu->W)
				);
			}
			break;

		case 4:
			if (cpu->state == 3) {
				if (load) {
					SET_L(cpu, GET_Z(cpu));

				} else {
					cpu->internalData = GET_H(cpu);
				}

				_cpu_next_cycle(
					cpu, load ? CYCLE_TYPE_MEMORY_READ : CYCLE_TYPE_MEMORY_WRITE, cpu->internalAddress + 1
				);
			}
			break;

		case 5:
			if (cpu->state == 3) {
				if (load) {
					SET_H(cpu, GET_W(cpu));
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_lhld(Cpu *cpu) {
	_xhld(cpu, 1);
}

static void _h_shld(Cpu *cpu) {
	_xhld(cpu, 0);
}

static void _xdax(Cpu *cpu, _U8 load) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_U16 address;

				if (cpu->ir & 0x10) {
					address = GET_PAIR_W(cpu->D);
				} else {
					address = GET_PAIR_W(cpu->B);
				}

				if (! load) {
					cpu->internalData = GET_A(cpu);
				}

				_cpu_next_cycle(cpu, load ? CYCLE_TYPE_MEMORY_READ : CYCLE_TYPE_MEMORY_WRITE, address);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				if (load) {
					SET_A(cpu, GET_Z(cpu));
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_ldax(Cpu *cpu) {
	_xdax(cpu, 1);
}

static void _h_stax(Cpu *cpu) {
	_xdax(cpu, 0);
}

static void _h_movrr(Cpu *cpu) {
	if (cpu->state == 4) {
		switch (cpu->ir & 0x7) {
			case 0x00: cpu->TMP = GET_B(cpu); break;
			case 0x01: cpu->TMP = GET_C(cpu); break;
			case 0x02: cpu->TMP = GET_D(cpu); break;
			case 0x03: cpu->TMP = GET_E(cpu); break;
			case 0x04: cpu->TMP = GET_H(cpu); break;
			case 0x05: cpu->TMP = GET_L(cpu); break;
			case 0x07: cpu->TMP = GET_A(cpu); break;
		}

	} else if (cpu->state == 5) {
		switch (cpu->ir & 0x38) {
			case 0x00: SET_B(cpu, cpu->TMP); break;
			case 0x08: SET_C(cpu, cpu->TMP); break;
			case 0x10: SET_D(cpu, cpu->TMP); break;
			case 0x18: SET_E(cpu, cpu->TMP); break;
			case 0x20: SET_H(cpu, cpu->TMP); break;
			case 0x28: SET_L(cpu, cpu->TMP); break;
			case 0x38: SET_A(cpu, cpu->TMP); break;
		}

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_movMr(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				switch (cpu->ir & 0x7) {
					case 0x00: cpu->TMP = GET_B(cpu); break;
					case 0x01: cpu->TMP = GET_C(cpu); break;
					case 0x02: cpu->TMP = GET_D(cpu); break;
					case 0x03: cpu->TMP = GET_E(cpu); break;
					case 0x04: cpu->TMP = GET_H(cpu); break;
					case 0x05: cpu->TMP = GET_L(cpu); break;
					case 0x07: cpu->TMP = GET_A(cpu); break;
				}

				cpu->internalData = cpu->TMP;

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_WRITE, GET_PAIR_W(cpu->H));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}

static void _h_movrM(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, GET_PAIR_W(cpu->H));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				switch (cpu->ir & 0x38) {
					case 0x00: SET_B(cpu, GET_Z(cpu)); break;
					case 0x08: SET_C(cpu, GET_Z(cpu)); break;
					case 0x10: SET_D(cpu, GET_Z(cpu)); break;
					case 0x18: SET_E(cpu, GET_Z(cpu)); break;
					case 0x20: SET_H(cpu, GET_Z(cpu)); break;
					case 0x28: SET_L(cpu, GET_Z(cpu)); break;
					case 0x38: SET_A(cpu, GET_Z(cpu)); break;
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

static void _logicI(Cpu *cpu, _U8 op, _U8 readFromPC) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				if (readFromPC) {
					_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
				} else {
					_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, GET_PAIR_W(cpu->H));
				}
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = GET_Z(cpu);

				_alu_op(cpu, op, 2, 1, 0, 0);
				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_ori(Cpu *cpu) {
	_logicI(cpu, ALU_OP_OR, 1);
}

static void _h_ani(Cpu *cpu) {
	_logicI(cpu, ALU_OP_AND, 1);
}

static void _h_xri(Cpu *cpu) {
	_logicI(cpu, ALU_OP_XOR, 1);
}

static void _h_anaM(Cpu *cpu) {
	_logicI(cpu, ALU_OP_AND, 0);
}

static void _h_oraM(Cpu *cpu) {
	_logicI(cpu, ALU_OP_OR, 0);
}

static void _h_xraM(Cpu *cpu) {
	_logicI(cpu, ALU_OP_XOR, 0);
}

static void _logicA(Cpu *cpu, _U8 op) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				switch (cpu->ir & 0x7) {
					case 0x00: cpu->TMP = GET_B(cpu); break;
					case 0x01: cpu->TMP = GET_C(cpu); break;
					case 0x02: cpu->TMP = GET_D(cpu); break;
					case 0x03: cpu->TMP = GET_E(cpu); break;
					case 0x04: cpu->TMP = GET_H(cpu); break;
					case 0x05: cpu->TMP = GET_L(cpu); break;
					case 0x07: cpu->TMP = GET_A(cpu); break;
				}

				_alu_op(cpu, op, 2, 1, 0, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}

static void _h_xra(Cpu *cpu) {
	_logicA(cpu, ALU_OP_XOR);
}

static void _h_ora(Cpu *cpu) {
	_logicA(cpu, ALU_OP_OR);
}

static void _h_ana(Cpu *cpu) {
	_logicA(cpu, ALU_OP_AND);
}


#if 0


static _U8 _h_out(Cpu *cpu, _U8 opcode) {
	IO_write_byte(cpu->io, FETCH_OFFSET(cpu, 1), GET_A(cpu));

	PC_INC(cpu, 2);
	return 10;
}
#endif
static void _h_inx(Cpu *cpu) {
	if (cpu->state == 5) {
		switch (cpu->ir & 0x30) {
			case 0x00: INC_PAIR_W(cpu->B);  break;
			case 0x10: INC_PAIR_W(cpu->D);  break;
			case 0x20: INC_PAIR_W(cpu->H);  break;
			case 0x30: INC_PAIR_W(cpu->SP); break;
		}

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_dcx(Cpu *cpu) {
	if (cpu->state == 5) {
		switch (cpu->ir & 0x30) {
			case 0x00: DEC_PAIR_W(cpu->B);  break;
			case 0x10: DEC_PAIR_W(cpu->D);  break;
			case 0x20: DEC_PAIR_W(cpu->H);  break;
			case 0x30: DEC_PAIR_W(cpu->SP); break;
		}

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _inrDcr(Cpu *cpu, _U8 inc) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->TMP = 1;

				switch (cpu->ir & 0x38) {
					case 0x00: cpu->ACT = GET_B(cpu); break;
					case 0x08: cpu->ACT = GET_C(cpu); break;
					case 0x10: cpu->ACT = GET_D(cpu); break;
					case 0x18: cpu->ACT = GET_E(cpu); break;
					case 0x20: cpu->ACT = GET_H(cpu); break;
					case 0x28: cpu->ACT = GET_L(cpu); break;
					case 0x38: cpu->ACT = GET_A(cpu); break;
					default:
						break;
				}

				_alu_op(cpu, inc ? ALU_OP_ADD : ALU_OP_SUB, 1, 0, 0, 1);

			} else if (cpu->state == 5) {
				switch (cpu->ir & 0x38) {
					case 0x00: SET_B(cpu, cpu->ACT); break;
					case 0x08: SET_C(cpu, cpu->ACT); break;
					case 0x10: SET_D(cpu, cpu->ACT); break;
					case 0x18: SET_E(cpu, cpu->ACT); break;
					case 0x20: SET_H(cpu, cpu->ACT); break;
					case 0x28: SET_L(cpu, cpu->ACT); break;
					case 0x38: SET_A(cpu, cpu->ACT); break;
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

static void _h_inr(Cpu *cpu) {
	_inrDcr(cpu, 1);
}

static void _h_dcr(Cpu *cpu) {
	_inrDcr(cpu, 0);
}

static void _inrDcrM(Cpu *cpu, _U8 inc) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, GET_PAIR_W(cpu->H));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = 1;
				cpu->ACT = GET_Z(cpu);

				_alu_op(cpu, inc ? ALU_OP_ADD : ALU_OP_SUB, 1, 0, 0, 1);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_WRITE, GET_PAIR_W(cpu->H));
			}
			break;

		case 3:
			if (cpu->state == 1) {
				cpu->internalData = cpu->ACT;

			} else if (cpu->state == 3) {
				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_inrM(Cpu *cpu) {
	_inrDcrM(cpu, 1);
}

static void _h_dcrM(Cpu *cpu) {
	_inrDcrM(cpu, 0);
}

#if 0
static _U8 _h_hlt(Cpu *cpu, _U8 opcode) {
	PC_INC(cpu, 1);

	cpu->halt = 1;

	return 7;
}
#endif

static void _h_jmp(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
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

static void _h_jcnd(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
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

static void _call(Cpu *cpu, _U8 cond) {
	if (cond) {
		switch (cpu->cycle) {
			case 3:
			case 4:
				if (cpu->state == 3) {
					RegPair ret;

					ret.r.w = cpu->PC;

					if (cpu->cycle == 3) {
						cpu->internalData = GET_PAIR_HB(ret);
					} else {
						cpu->internalData = GET_PAIR_LB(ret);
					}
					_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_WRITE, GET_PAIR_W(cpu->SP));
				}
				break;

			case 5:
				if (cpu->state == 3) {
					_cpu_next_instruction(cpu, GET_PAIR_W(cpu->W));
				}
		}

	} else {
		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_ccnd(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 5) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
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

				_call(cpu, jump);
			}
			break;

		case 4:
		case 5:
			_call(cpu, 1);
			break;

		default:
			break;
	}
}

static void _h_call(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 5) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 3) {
				_call(cpu, 1);
			}
			break;

		case 4:
		case 5:
			_call(cpu, 1);
			break;
	}
}

static void _ret(Cpu *cpu, _U8 cond) {
	if (cond) {
		switch (cpu->cycle) {
			case 1:
				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
				break;

			case 2:
				if (cpu->state == 3) {
					_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
				}
				break;

			case 3:
				if (cpu->state == 3) {
					_cpu_next_instruction(cpu, GET_PAIR_W(cpu->W));
				}
				break;
		}

	} else {
		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_retCond(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 5) {
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

				_ret(cpu, jump);
			}
			break;

		case 2:
		case 3:
			_ret(cpu, 1);
			break;
	}
}

static void _h_push(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				switch (cpu->ir & 0x30) {
					case 0x00: cpu->internalData = GET_PAIR_HB(cpu->B);   break;
					case 0x10: cpu->internalData = GET_PAIR_HB(cpu->D);   break;
					case 0x20: cpu->internalData = GET_PAIR_HB(cpu->H);   break;
					case 0x30: cpu->internalData = GET_PAIR_HB(cpu->PSW); break;
				}

				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_WRITE, GET_PAIR_W(cpu->SP));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				switch (cpu->ir & 0x30) {
					case 0x00: cpu->internalData = GET_PAIR_LB(cpu->B);   break;
					case 0x10: cpu->internalData = GET_PAIR_LB(cpu->D);   break;
					case 0x20: cpu->internalData = GET_PAIR_LB(cpu->H);   break;
					case 0x30: cpu->internalData = GET_PAIR_LB(cpu->PSW); break;
				}

				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_WRITE, GET_PAIR_W(cpu->SP));
			}
			break;

		case 3:
			if (cpu->state == 3) {
				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_pop(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
			}
			break;

		case 3:
			if (cpu->state == 3) {
				switch (cpu->ir & 0x30) {
					case 0x00: COPY_PAIR_W(cpu->B,   cpu->W); break;
					case 0x10: COPY_PAIR_W(cpu->D,   cpu->W); break;
					case 0x20: COPY_PAIR_W(cpu->H,   cpu->W); break;
					case 0x30: COPY_PAIR_W(cpu->PSW, cpu->W); break;
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_sphl(Cpu *cpu) {
	if (cpu->state == 5) {
		COPY_PAIR_W(cpu->SP, cpu->H);

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_xthl(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_READ, GET_PAIR_W(cpu->SP));
			}
			break;

		case 3:
			if (cpu->state == 3) {
				cpu->internalData = GET_H(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_WRITE, GET_PAIR_W(cpu->SP));
			}
			break;

		case 4:
			if (cpu->state == 3) {
				cpu->internalData = GET_L(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_STACK_WRITE, GET_PAIR_W(cpu->SP));
			}
			break;

		case 5:
			if (cpu->state == 5) {
				COPY_PAIR_W(cpu->H, cpu->W);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_pchl(Cpu *cpu) {
	if (cpu->state == 5) {
		_cpu_next_instruction(cpu, GET_PAIR_W(cpu->H));
	}
}

static void _h_xchg(Cpu *cpu) {
	if (cpu->state == 4) {
		COPY_PAIR_W(cpu->W, cpu->H);
		COPY_PAIR_W(cpu->H, cpu->D);
		COPY_PAIR_W(cpu->D, cpu->W);

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _addSubM(Cpu *cpu, _U8 op, _U8 carry) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, GET_PAIR_W(cpu->H));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = GET_Z(cpu);

				_alu_op(cpu, op, 2, 1, carry, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}

static void _h_addM(Cpu *cpu) {
	_addSubM(cpu, ALU_OP_ADD, 0);
}

static void _h_adcM(Cpu *cpu) {
	_addSubM(cpu, ALU_OP_ADD, 1);
}

static void _h_subM(Cpu *cpu) {
	_addSubM(cpu, ALU_OP_SUB, 0);
}

static void _h_sbbM(Cpu *cpu) {
	_addSubM(cpu, ALU_OP_SUB, 1);
}

static void _addSub(Cpu *cpu, _U8 op, _U8 carry, _U8 copyAcu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				switch (cpu->ir & 0x7) {
					case 0x00: cpu->TMP = GET_B(cpu); break;
					case 0x01: cpu->TMP = GET_C(cpu); break;
					case 0x02: cpu->TMP = GET_D(cpu); break;
					case 0x03: cpu->TMP = GET_E(cpu); break;
					case 0x04: cpu->TMP = GET_H(cpu); break;
					case 0x05: cpu->TMP = GET_L(cpu); break;
					case 0x07: cpu->TMP = GET_A(cpu); break;
				}

				_alu_op(cpu, op, 2, copyAcu, carry, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_add(Cpu *cpu) {
	_addSub(cpu, ALU_OP_ADD, 0, 1);
}

static void _h_adc(Cpu *cpu) {
	_addSub(cpu, ALU_OP_ADD, 1, 1);
}

static void _h_sub(Cpu *cpu) {
	_addSub(cpu, ALU_OP_SUB, 0, 1);
}

static void _h_sbb(Cpu *cpu) {
	_addSub(cpu, ALU_OP_SUB, 1, 1);
}

static void _h_dad(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				// Save current flags
				SET_Z(cpu, GET_FLG(cpu));

				_cpu_next_cycle(cpu, CYCLE_TYPE_ALU_OPERATION, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 1) {
				switch (cpu->ir & 0x30) {
					case 0x00: cpu->ACT = GET_PAIR_LB(cpu->B);  break;
					case 0x10: cpu->ACT = GET_PAIR_LB(cpu->D);  break;
					case 0x20: cpu->ACT = GET_PAIR_LB(cpu->H);  break;
					case 0x30: cpu->ACT = GET_PAIR_LB(cpu->SP); break;
					default:
						break;
				}

			} else if (cpu->state == 2) {
				cpu->TMP = GET_L(cpu);

				_alu_op(cpu, ALU_OP_ADD, 1, 0, 0, 0);

			} else {
				SET_L(cpu, cpu->ACT);

				_cpu_next_cycle(cpu, CYCLE_TYPE_ALU_OPERATION, cpu->PC);
			}
			break;

		case 3:
			if (cpu->state == 1) {
				switch (cpu->ir & 0x30) {
					case 0x00: cpu->ACT = GET_PAIR_HB(cpu->B);  break;
					case 0x10: cpu->ACT = GET_PAIR_HB(cpu->D);  break;
					case 0x20: cpu->ACT = GET_PAIR_HB(cpu->H);  break;
					case 0x30: cpu->ACT = GET_PAIR_HB(cpu->SP); break;
					default:
						break;
				}
			} else if (cpu->state == 2) {
				cpu->TMP = GET_H(cpu);

				_alu_op(cpu, ALU_OP_ADD, 1, 0, 1, 0);

			} else {
				_U8 carry = FLAG_IS_SET(cpu, FLAG_CARRY);

				SET_H(cpu, cpu->ACT);

				// Restore flags
				SET_FLG(cpu, GET_Z(cpu));

				if (carry) {
					FLAG_SET(cpu, FLAG_CARRY);
				} else {
					FLAG_CLEAR(cpu, FLAG_CARRY);
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_daa(Cpu *cpu) {
	if (cpu->state == 4) {
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

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _addSubI(Cpu *cpu, _U8 operation, _U8 carry) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = GET_Z(cpu);

				_alu_op(cpu, operation, 2, 1, carry, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_adi(Cpu *cpu) {
	_addSubI(cpu, ALU_OP_ADD, 0);
}

static void _h_aci(Cpu *cpu) {
	_addSubI(cpu, ALU_OP_ADD, 1);
}

static void _h_sui(Cpu *cpu) {
	_addSubI(cpu, ALU_OP_SUB, 0);
}

static void _h_sbi(Cpu *cpu) {
	_addSubI(cpu, ALU_OP_SUB, 1);
}

static void _h_cpi(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = GET_Z(cpu);

				_alu_op(cpu, ALU_OP_SUB, 2, 0, 0, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_cmp(Cpu *cpu) {
	_addSub(cpu, ALU_OP_SUB, 0, 0);
}

static void _h_cmpM(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				cpu->ACT = GET_A(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ, GET_PAIR_W(cpu->H));
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->TMP = GET_Z(cpu);

				_alu_op(cpu, ALU_OP_SUB, 2, 0, 0, 0);

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}

static void _h_mvi(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				_U8 data = GET_Z(cpu);

				switch (cpu->ir & 0x38) {
					case 0x00: SET_B(cpu, data); break;
					case 0x08: SET_C(cpu, data); break;
					case 0x10: SET_D(cpu, data); break;
					case 0x18: SET_E(cpu, data); break;
					case 0x20: SET_H(cpu, data); break;
					case 0x28: SET_L(cpu, data); break;
					case 0x38: SET_A(cpu, data); break;
				}

				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;

		default:
			break;
	}
}

static void _h_mviM(Cpu *cpu) {
	switch (cpu->cycle) {
		case 1:
			if (cpu->state == 4) {
				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_READ_OPCODE, cpu->PC);
			}
			break;

		case 2:
			if (cpu->state == 3) {
				cpu->internalData = GET_Z(cpu);

				_cpu_next_cycle(cpu, CYCLE_TYPE_MEMORY_WRITE, GET_PAIR_W(cpu->H));
			}
			break;

		case 3:
			if (cpu->state == 3) {
				_cpu_next_instruction(cpu, cpu->PC);
			}
			break;
	}
}

static void _h_stc(Cpu *cpu) {
	if (cpu->state == 4) {
		FLAG_SET(cpu, FLAG_CARRY);

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_cmc(Cpu *cpu) {
	if (cpu->state == 4) {
		FLAG_TOGGLE(cpu, FLAG_CARRY);

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_cma(Cpu *cpu) {
	if (cpu->state == 4) {
		SET_A(cpu, ~GET_A(cpu));

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _rtX(Cpu *cpu, _U8 right, _U8 carry) {
	if (cpu->state == 4) {
		cpu->ACT = GET_A(cpu);

		_alu_op(cpu, right ? ALU_OP_ROT_R : ALU_OP_ROT_L, 2, 1, carry, 0);

		_cpu_next_instruction(cpu, cpu->PC);
	}
}

static void _h_rrc(Cpu *cpu) {
	_rtX(cpu, 1, 0);
}

static void _h_rar(Cpu *cpu) {
	_rtX(cpu, 1, 1);
}

static void _h_rlc(Cpu *cpu) {
	_rtX(cpu, 0, 0);
}

static void _h_ral(Cpu *cpu) {
	_rtX(cpu, 0, 1);
}

void cpu_init(Cpu *cpu) {
	SET_PAIR_W(cpu->PSW, 0);
	SET_PAIR_W(cpu->B,   0);
	SET_PAIR_W(cpu->D,   0);
	SET_PAIR_W(cpu->H,   0);
	SET_PAIR_W(cpu->SP,  0);

	_flagsCommon(cpu, 0);
	// second bit is always set to one
	FLAG_SET(cpu, 0x02);

	cpu->PC               = 0;
	cpu->ticks            = 0;
	cpu->readCycleCount   = 1;
	cpu->ALUOP            = ALU_OP_NONE;
	cpu->ALUDEL           = 0;
	cpu->ALUcarry         = 0;
	cpu->ALUpreserveCarry = 0;

	// PIO
	cpu->pins._WR = 1;

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

		DECLARE_OPCODE(0x32, _h_sta,  "STA a16");
		DECLARE_OPCODE(0x02, _h_stax, "STAX B");
		DECLARE_OPCODE(0x12, _h_stax, "STAX D");
		DECLARE_OPCODE(0x22, _h_shld, "SHLD a16");

		DECLARE_OPCODE(0x3a, _h_lda,  "LDA a16");
		DECLARE_OPCODE(0x0a, _h_ldax, "LDAX B");
		DECLARE_OPCODE(0x1a, _h_ldax, "LDAX D");
		DECLARE_OPCODE(0x2a, _h_lhld, "LHLD a16");

		DECLARE_OPCODE(0xc5, _h_push, "PUSH B");
		DECLARE_OPCODE(0xd5, _h_push, "PUSH D");
		DECLARE_OPCODE(0xe5, _h_push, "PUSH H");
		DECLARE_OPCODE(0xf5, _h_push, "PUSH PSW");

		DECLARE_OPCODE(0xc1, _h_pop, "POP B");
		DECLARE_OPCODE(0xd1, _h_pop, "POP D");
		DECLARE_OPCODE(0xe1, _h_pop, "POP H");
		DECLARE_OPCODE(0xf1, _h_pop, "POP PSW");

		DECLARE_OPCODE(0xf9, _h_sphl, "SPHL");
		DECLARE_OPCODE(0xe3, _h_xthl, "XTHL");
		DECLARE_OPCODE(0xe9, _h_pchl, "PCHL");

		DECLARE_OPCODE(0x03, _h_inx, "INX B");
		DECLARE_OPCODE(0x13, _h_inx, "INX D");
		DECLARE_OPCODE(0x23, _h_inx, "INX H");
		DECLARE_OPCODE(0x33, _h_inx, "INX SP");

		DECLARE_OPCODE(0x0b, _h_dcx, "DCX B");
		DECLARE_OPCODE(0x1b, _h_dcx, "DCX D");
		DECLARE_OPCODE(0x2b, _h_dcx, "DCX H");
		DECLARE_OPCODE(0x3b, _h_dcx, "DCX SP");

		DECLARE_OPCODE(0x04, _h_inr,  "INR B");
		DECLARE_OPCODE(0x14, _h_inr,  "INR D");
		DECLARE_OPCODE(0x24, _h_inr,  "INR H");
		DECLARE_OPCODE(0x34, _h_inrM, "INR M");
		DECLARE_OPCODE(0x0c, _h_inr,  "INR C");
		DECLARE_OPCODE(0x1c, _h_inr,  "INR E");
		DECLARE_OPCODE(0x2c, _h_inr,  "INR L");
		DECLARE_OPCODE(0x3c, _h_inr,  "INR A");

		DECLARE_OPCODE(0x05, _h_dcr,  "DCR B");
		DECLARE_OPCODE(0x15, _h_dcr,  "DCR D");
		DECLARE_OPCODE(0x25, _h_dcr,  "DCR H");
		DECLARE_OPCODE(0x35, _h_dcrM, "DCR M");
		DECLARE_OPCODE(0x0d, _h_dcr,  "DCR C");
		DECLARE_OPCODE(0x1d, _h_dcr,  "DCR E");
		DECLARE_OPCODE(0x2d, _h_dcr,  "DCR L");
		DECLARE_OPCODE(0x3d, _h_dcr,  "DCR A");

		DECLARE_OPCODE(0x46, _h_movrM, "MOV B,M");
		DECLARE_OPCODE(0x4e, _h_movrM, "MOV C,M");
		DECLARE_OPCODE(0x56, _h_movrM, "MOV D,M");
		DECLARE_OPCODE(0x5e, _h_movrM, "MOV E,M");
		DECLARE_OPCODE(0x66, _h_movrM, "MOV H,M");
		DECLARE_OPCODE(0x6e, _h_movrM, "MOV L,M");
		DECLARE_OPCODE(0x7e, _h_movrM, "MOV A,M");
		DECLARE_OPCODE(0x70, _h_movMr, "MOV M,B");
		DECLARE_OPCODE(0x71, _h_movMr, "MOV M,C");
		DECLARE_OPCODE(0x72, _h_movMr, "MOV M,D");
		DECLARE_OPCODE(0x73, _h_movMr, "MOV M,E");
		DECLARE_OPCODE(0x74, _h_movMr, "MOV M,H");
		DECLARE_OPCODE(0x75, _h_movMr, "MOV M,L");
		DECLARE_OPCODE(0x77, _h_movMr, "MOV M,A");

		DECLARE_OPCODE(0x40, _h_movrr, "MOV B,B");
		DECLARE_OPCODE(0x41, _h_movrr, "MOV B,C");
		DECLARE_OPCODE(0x42, _h_movrr, "MOV B,D");
		DECLARE_OPCODE(0x43, _h_movrr, "MOV B,E");
		DECLARE_OPCODE(0x44, _h_movrr, "MOV B,H");
		DECLARE_OPCODE(0x45, _h_movrr, "MOV B,L");
		DECLARE_OPCODE(0x47, _h_movrr, "MOV B,A");
		DECLARE_OPCODE(0x48, _h_movrr, "MOV C,B");
		DECLARE_OPCODE(0x49, _h_movrr, "MOV C,C");
		DECLARE_OPCODE(0x4a, _h_movrr, "MOV C,D");
		DECLARE_OPCODE(0x4b, _h_movrr, "MOV C,E");
		DECLARE_OPCODE(0x4c, _h_movrr, "MOV C,H");
		DECLARE_OPCODE(0x4d, _h_movrr, "MOV C,L");
		DECLARE_OPCODE(0x4f, _h_movrr, "MOV C,A");
		DECLARE_OPCODE(0x50, _h_movrr, "MOV D,B");
		DECLARE_OPCODE(0x51, _h_movrr, "MOV D,C");
		DECLARE_OPCODE(0x52, _h_movrr, "MOV D,D");
		DECLARE_OPCODE(0x53, _h_movrr, "MOV D,E");
		DECLARE_OPCODE(0x54, _h_movrr, "MOV D,H");
		DECLARE_OPCODE(0x55, _h_movrr, "MOV D,L");
		DECLARE_OPCODE(0x57, _h_movrr, "MOV D,A");
		DECLARE_OPCODE(0x58, _h_movrr, "MOV E,B");
		DECLARE_OPCODE(0x59, _h_movrr, "MOV E,C");
		DECLARE_OPCODE(0x5a, _h_movrr, "MOV E,D");
		DECLARE_OPCODE(0x5b, _h_movrr, "MOV E,E");
		DECLARE_OPCODE(0x5c, _h_movrr, "MOV E,H");
		DECLARE_OPCODE(0x5d, _h_movrr, "MOV E,L");
		DECLARE_OPCODE(0x5f, _h_movrr, "MOV E,A");
		DECLARE_OPCODE(0x60, _h_movrr, "MOV H,B");
		DECLARE_OPCODE(0x61, _h_movrr, "MOV H,C");
		DECLARE_OPCODE(0x62, _h_movrr, "MOV H,D");
		DECLARE_OPCODE(0x63, _h_movrr, "MOV H,E");
		DECLARE_OPCODE(0x64, _h_movrr, "MOV H,H");
		DECLARE_OPCODE(0x65, _h_movrr, "MOV H,L");
		DECLARE_OPCODE(0x67, _h_movrr, "MOV H,A");
		DECLARE_OPCODE(0x68, _h_movrr, "MOV L,B");
		DECLARE_OPCODE(0x69, _h_movrr, "MOV L,C");
		DECLARE_OPCODE(0x6a, _h_movrr, "MOV L,D");
		DECLARE_OPCODE(0x6b, _h_movrr, "MOV L,E");
		DECLARE_OPCODE(0x6c, _h_movrr, "MOV L,H");
		DECLARE_OPCODE(0x6d, _h_movrr, "MOV L,L");
		DECLARE_OPCODE(0x6f, _h_movrr, "MOV L,A");
		DECLARE_OPCODE(0x78, _h_movrr, "MOV A,B");
		DECLARE_OPCODE(0x79, _h_movrr, "MOV A,C");
		DECLARE_OPCODE(0x7a, _h_movrr, "MOV A,D");
		DECLARE_OPCODE(0x7b, _h_movrr, "MOV A,E");
		DECLARE_OPCODE(0x7c, _h_movrr, "MOV A,H");
		DECLARE_OPCODE(0x7d, _h_movrr, "MOV A,L");
		DECLARE_OPCODE(0x7f, _h_movrr, "MOV A,A");

		DECLARE_OPCODE(0x06, _h_mvi,   "MVI B,d8");
		DECLARE_OPCODE(0x16, _h_mvi,   "MVI D,d8");
		DECLARE_OPCODE(0x26, _h_mvi,   "MVI H,d8");
		DECLARE_OPCODE(0x36, _h_mviM,  "MVI M,d8");
		DECLARE_OPCODE(0x0e, _h_mvi,   "MVI C,d8");
		DECLARE_OPCODE(0x1e, _h_mvi,   "MVI E,d8");
		DECLARE_OPCODE(0x2e, _h_mvi,   "MVI L,d8");
		DECLARE_OPCODE(0x3e, _h_mvi,   "MVI A,d8");

		DECLARE_OPCODE(0xb0, _h_ora,  "ORA B");
		DECLARE_OPCODE(0xb1, _h_ora,  "ORA C");
		DECLARE_OPCODE(0xb2, _h_ora,  "ORA D");
		DECLARE_OPCODE(0xb3, _h_ora,  "ORA E");
		DECLARE_OPCODE(0xb4, _h_ora,  "ORA H");
		DECLARE_OPCODE(0xb5, _h_ora,  "ORA L");
		DECLARE_OPCODE(0xb6, _h_oraM, "ORA M");
		DECLARE_OPCODE(0xb7, _h_ora,  "ORA A");

		DECLARE_OPCODE(0xa0, _h_ana,  "ANA B");
		DECLARE_OPCODE(0xa1, _h_ana,  "ANA C");
		DECLARE_OPCODE(0xa2, _h_ana,  "ANA D");
		DECLARE_OPCODE(0xa3, _h_ana,  "ANA E");
		DECLARE_OPCODE(0xa4, _h_ana,  "ANA H");
		DECLARE_OPCODE(0xa5, _h_ana,  "ANA L");
		DECLARE_OPCODE(0xa6, _h_anaM, "ANA M");
		DECLARE_OPCODE(0xa7, _h_ana,  "ANA A");

		DECLARE_OPCODE(0xe6, _h_ani,  "ANI d8");
		DECLARE_OPCODE(0xf6, _h_ori,  "ORI d8");

		DECLARE_OPCODE(0xa8, _h_xra,  "XRA B");
		DECLARE_OPCODE(0xa9, _h_xra,  "XRA C");
		DECLARE_OPCODE(0xaa, _h_xra,  "XRA D");
		DECLARE_OPCODE(0xab, _h_xra,  "XRA E");
		DECLARE_OPCODE(0xac, _h_xra,  "XRA H");
		DECLARE_OPCODE(0xad, _h_xra,  "XRA L");
		DECLARE_OPCODE(0xae, _h_xraM, "XRA M");
		DECLARE_OPCODE(0xaf, _h_xra,  "XRA A");
		DECLARE_OPCODE(0xee, _h_xri,  "XRI d8");

//		DECLARE_OPCODE(0xd3, _h_out,  "OUT, d8");
//		DECLARE_OPCODE(0x76, _h_hlt,  "HLT");
		DECLARE_OPCODE(0xcd, _h_call, "CALL a16");
		DECLARE_OPCODE(0xeb, _h_xchg, "XCHG");

		DECLARE_OPCODE(0xc3, _h_jmp,  "JMP a16");
		DECLARE_OPCODE(0xca, _h_jcnd, "JZ  a16");
		DECLARE_OPCODE(0xd2, _h_jcnd, "JNC a16");
		DECLARE_OPCODE(0xea, _h_jcnd, "JPE a16");
		DECLARE_OPCODE(0xf2, _h_jcnd, "JP  a16");
		DECLARE_OPCODE(0xda, _h_jcnd, "JC  a16");
		DECLARE_OPCODE(0xfa, _h_jcnd, "JM  a16");
		DECLARE_OPCODE(0xc2, _h_jcnd, "JNZ a16");
		DECLARE_OPCODE(0xe2, _h_jcnd, "JPO a16");
		DECLARE_OPCODE(0xdc, _h_ccnd, "CC  a16");
		DECLARE_OPCODE(0xe4, _h_ccnd, "CPO a16");
		DECLARE_OPCODE(0xfc, _h_ccnd, "CM  a16");
		DECLARE_OPCODE(0xc4, _h_ccnd, "CNZ a16");
		DECLARE_OPCODE(0xd4, _h_ccnd, "CNC a16");
		DECLARE_OPCODE(0xec, _h_ccnd, "CPE a16");
		DECLARE_OPCODE(0xf4, _h_ccnd, "CP  a16");
		DECLARE_OPCODE(0xcc, _h_ccnd, "CZ  a16");

		DECLARE_OPCODE(0xe0, _h_retCond, "RPO");
		DECLARE_OPCODE(0xe8, _h_retCond, "RPE");
		DECLARE_OPCODE(0xf0, _h_retCond, "RP");
		DECLARE_OPCODE(0xf8, _h_retCond, "RM");
		DECLARE_OPCODE(0xd0, _h_retCond, "RNC");
		DECLARE_OPCODE(0xd8, _h_retCond, "RC");
		DECLARE_OPCODE(0xc0, _h_retCond, "RNZ");
		DECLARE_OPCODE(0xc8, _h_retCond, "RZ");

		DECLARE_OPCODE(0xde, _h_sbi,  "SBI");
		DECLARE_OPCODE(0xd6, _h_sui,  "SUI");
		DECLARE_OPCODE(0xce, _h_aci,  "ACI");
		DECLARE_OPCODE(0xfe, _h_cpi,  "CPI");

		DECLARE_OPCODE(0xb8, _h_cmp,  "CMP B");
		DECLARE_OPCODE(0xb9, _h_cmp,  "CMP C");
		DECLARE_OPCODE(0xba, _h_cmp,  "CMP D");
		DECLARE_OPCODE(0xbb, _h_cmp,  "CMP E");
		DECLARE_OPCODE(0xbc, _h_cmp,  "CMP H");
		DECLARE_OPCODE(0xbd, _h_cmp,  "CMP L");
		DECLARE_OPCODE(0xbe, _h_cmpM, "CMP M");
		DECLARE_OPCODE(0xbf, _h_cmp,  "CMP A");

		DECLARE_OPCODE(0xc6, _h_adi,  "ADI");

		DECLARE_OPCODE(0x80, _h_add,  "ADD B");
		DECLARE_OPCODE(0x81, _h_add,  "ADD C");
		DECLARE_OPCODE(0x82, _h_add,  "ADD D");
		DECLARE_OPCODE(0x83, _h_add,  "ADD E");
		DECLARE_OPCODE(0x84, _h_add,  "ADD H");
		DECLARE_OPCODE(0x85, _h_add,  "ADD L");
		DECLARE_OPCODE(0x86, _h_addM, "ADD M");
		DECLARE_OPCODE(0x87, _h_add,  "ADD A");

		DECLARE_OPCODE(0x88, _h_adc,  "ADC B");
		DECLARE_OPCODE(0x89, _h_adc,  "ADC C");
		DECLARE_OPCODE(0x8a, _h_adc,  "ADC D");
		DECLARE_OPCODE(0x8b, _h_adc,  "ADC E");
		DECLARE_OPCODE(0x8c, _h_adc,  "ADC H");
		DECLARE_OPCODE(0x8d, _h_adc,  "ADC L");
		DECLARE_OPCODE(0x8e, _h_adcM, "ADC M");
		DECLARE_OPCODE(0x8f, _h_adc,  "ADC A");

		DECLARE_OPCODE(0x09, _h_dad,  "DAD B");
		DECLARE_OPCODE(0x19, _h_dad,  "DAD D");
		DECLARE_OPCODE(0x29, _h_dad,  "DAD H");
		DECLARE_OPCODE(0x39, _h_dad,  "DAD SP");

		DECLARE_OPCODE(0x27, _h_daa,  "DAA");

		DECLARE_OPCODE(0x90, _h_sub,  "SUB B");
		DECLARE_OPCODE(0x91, _h_sub,  "SUB C");
		DECLARE_OPCODE(0x92, _h_sub,  "SUB D");
		DECLARE_OPCODE(0x93, _h_sub,  "SUB E");
		DECLARE_OPCODE(0x94, _h_sub,  "SUB H");
		DECLARE_OPCODE(0x95, _h_sub,  "SUB L");
		DECLARE_OPCODE(0x96, _h_subM, "SUB M");
		DECLARE_OPCODE(0x97, _h_sub,  "SUB A");

		DECLARE_OPCODE(0x98, _h_sbb,  "SBB B");
		DECLARE_OPCODE(0x99, _h_sbb,  "SBB C");
		DECLARE_OPCODE(0x9a, _h_sbb,  "SBB D");
		DECLARE_OPCODE(0x9b, _h_sbb,  "SBB E");
		DECLARE_OPCODE(0x9c, _h_sbb,  "SBB H");
		DECLARE_OPCODE(0x9d, _h_sbb,  "SBB L");
		DECLARE_OPCODE(0x9e, _h_sbbM, "SBB M");
		DECLARE_OPCODE(0x9f, _h_sbb,  "SBB A");

		DECLARE_OPCODE(0x37, _h_stc,  "STC");
		DECLARE_OPCODE(0x3f, _h_cmc,  "CMC");
		DECLARE_OPCODE(0x2f, _h_cma,  "CMA");

		DECLARE_OPCODE(0x07, _h_rlc,  "RLC");
		DECLARE_OPCODE(0x0f, _h_rrc,  "RRC");
		DECLARE_OPCODE(0x17, _h_ral,  "RAL");
		DECLARE_OPCODE(0x1f, _h_rar,  "RAR");
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

void cpu_phase(Cpu *cpu, _U8 p1) {
	if (p1) {
		// phase1
		switch (cpu->state) {
			case 1:
				{
					if (
						(cpu->cycleType != CYCLE_TYPE_MEMORY_READ) &&
						(cpu->cycleType != CYCLE_TYPE_STACK_READ)
					) {
						cpu->readCycleCount = 1;
					}
				}
				break;

			case 3:
				{
					if (
						(cpu->cycleType == CYCLE_TYPE_STACK_WRITE) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_WRITE)
					) {
						if (cpu->cycleType == CYCLE_TYPE_STACK_WRITE) {
							DEC_PAIR_W(cpu->SP);

							// Update address on internal bus
							cpu->internalAddress = GET_PAIR_W(cpu->SP);
						}

						cpu->pins.ADDRESS = cpu->internalAddress;
						cpu->pins.DATA    = cpu->internalData;

						cpu->pins._WR = 0;
					}
				}
				break;
		}

	} else {
		// phase2
		if (cpu->ALUOP != ALU_OP_NONE) {
			if (! --cpu->ALUDEL) {
				switch (cpu->ALUOP) {
					case ALU_OP_AND:
						cpu->ACT = cpu->ACT & cpu->TMP;
						_flagsLogic(cpu, cpu->ACT);
						break;

					case ALU_OP_OR:
						cpu->ACT = cpu->ACT | cpu->TMP;
						_flagsLogic(cpu, cpu->ACT);
						break;

					case ALU_OP_XOR:
						cpu->ACT = cpu->ACT ^ cpu->TMP;
						_flagsLogic(cpu, cpu->ACT);
						break;

					case ALU_OP_ADD:
						cpu->ACT = _flagsAdd(cpu, cpu->ACT, cpu->TMP, cpu->ALUcarry, cpu->ALUpreserveCarry);
						break;

					case ALU_OP_SUB:
						cpu->ACT = _flagsSub(cpu, cpu->ACT, cpu->TMP, cpu->ALUcarry, cpu->ALUpreserveCarry);
						break;

					case ALU_OP_ROT_R:
						{
							_U8 carry;
							_U8 setCarry;

							if (cpu->ALUcarry) {
								carry    = FLAG_IS_SET(cpu, FLAG_CARRY);
								setCarry = cpu->ACT & 0x01;

							} else {
								carry    = cpu->ACT & 0x01;
								setCarry = carry;
							}

							if (! cpu->ALUcarry) {
								cpu->ACT = ((cpu->ACT >> 1) | (carry << 7));
							}

							if (setCarry) {
								FLAG_SET(cpu, FLAG_CARRY);
							} else {
								FLAG_CLEAR(cpu, FLAG_CARRY);
							}

							if (cpu->ALUcarry) {
								cpu->ACT = ((GET_A(cpu) >> 1) | (carry << 7));
							}
						}
						break;

					case ALU_OP_ROT_L:
						{
							_U8 carry;
							_U8 setCarry;

							if (cpu->ALUcarry) {
								carry    = FLAG_IS_SET(cpu, FLAG_CARRY);
								setCarry = cpu->ACT & 0x80;

							} else {
								carry    = (cpu->ACT & 0x80) != 0;
								setCarry = carry;
							}

							if (! cpu->ALUcarry) {
								cpu->ACT = ((cpu->ACT << 1) | carry);
							}

							if (setCarry) {
								FLAG_SET(cpu, FLAG_CARRY);
							} else {
								FLAG_CLEAR(cpu, FLAG_CARRY);
							}

							if (cpu->ALUcarry) {
								cpu->ACT = ((GET_A(cpu) << 1) | carry);
							}
						}
						break;
				}

				if (cpu->ALUcopyAccu) {
					SET_A(cpu, cpu->ACT);
				}

				cpu->ALUOP            = ALU_OP_NONE;
				cpu->ALUcarry         = 0;
				cpu->ALUpreserveCarry = 0;
				cpu->ALUcopyAccu      = 0;
			}
		}

		switch (cpu->state) {
			case 1:
				{
					if (
						(cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ) ||
						(cpu->cycleType == CYCLE_TYPE_STACK_READ)
					) {
						cpu->pins.ADDRESS = cpu->internalAddress;
					}

					if (cpu->cycleType != CYCLE_TYPE_ALU_OPERATION) {
						cpu->pins.SYNC = 1;
						cpu->pins.DATA = _statusBitsPerCycle[cpu->cycleType];
					}
				}
				break;

			case 2:
				{
					if (
						(cpu->cycleType == CYCLE_TYPE_INSTRUCTION_FETCH) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ) ||
						(cpu->cycleType == CYCLE_TYPE_STACK_READ)
					) {
						cpu->pins.DBIN = 1;

						if (cpu->readOpcode) {
							cpu->PC = cpu->internalAddress + 1;
						}
					}

					if (cpu->cycleType != CYCLE_TYPE_ALU_OPERATION) {
						cpu->pins.SYNC = 0;
						cpu->pins.DATA = 0;
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

						DBG(("OPCODE: %s (ADDR: %04x)", _opcodesNames[cpu->pins.DATA], cpu->pins.ADDRESS));

						cpu->pins.DBIN = 0;

					} else if (
						(cpu->cycleType == CYCLE_TYPE_MEMORY_READ) ||
						(cpu->cycleType == CYCLE_TYPE_STACK_READ)
					) {
						// Increase reg offset fot memory read (cyclic)
						if (cpu->readCycleCount == 1) {
							SET_Z(cpu, cpu->pins.DATA);

							cpu->readCycleCount = 2;

						} else {
							SET_W(cpu, cpu->pins.DATA);

							cpu->readCycleCount = 1;
						}

						if (cpu->cycleType == CYCLE_TYPE_STACK_READ) {
							INC_PAIR_W(cpu->SP);
						}

						cpu->pins.DBIN = 0;

					} else if (
						(cpu->cycleType == CYCLE_TYPE_STACK_WRITE) ||
						(cpu->cycleType == CYCLE_TYPE_MEMORY_WRITE)
					) {
						cpu->pins._WR = 1;
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

	if (! p1) {
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
}

void cpu_state(Cpu *cpu) {
	cpu_phase(cpu, 1); // phase1
	cpu_phase(cpu, 0); // phase2
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

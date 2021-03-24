/*
 * MIT License
 *
 * Copyright (c) 2020 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <cstdlib>
#include <stddef.h>
#include <string>
#include "altair/Core/JitCore.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


#define INT_FLAG_TICKS  (1 << 0)
#define INT_FLAG_MEM_RD (1 << 1)
#define INT_FLAG_MEM_WR (1 << 2)
#define INT_FLAG_IO_RD  (1 << 3)
#define INT_FLAG_IO_WR  (1 << 4)
#define INT_FLAG_INTE   (1 << 5)


enum RegSingle {
	B = 0,
	C = 1,
	D = 2,
	E = 3,
	H = 4,
	L = 5,
	A = 7,

	PSW = 8
};

enum RegDouble {
	BC = 0,
	DE = 1,
	HL = 2,

	SP   = 3,
	PSWA = 3
};


void JitCore_onNativeInt(void *ctx) {
	altair::JitCore::Regs *regs = reinterpret_cast<altair::JitCore::Regs *>(ctx);

//	DBG(("%s(): CALL, regs: %p, self: %p, mask: %02x, address: %04x, data: %u (%02x)",
//		__func__, regs, regs->self, regs->intFlags, regs->intAddress, regs->intValue, regs->intValue
//	));

	if (regs->intFlags & INT_FLAG_TICKS) {
		regs->self->onTickInt(regs->intValue);

	} else if (regs->intFlags & INT_FLAG_MEM_WR) {
		regs->self->onMemoryWriteInt(regs->intAddress, regs->intValue);

	} else if (regs->intFlags & INT_FLAG_MEM_RD) {
		regs->self->onMemoryReadInt(regs->intAddress, regs->intValue);

	} else if (regs->intFlags & INT_FLAG_IO_WR) {
		regs->self->onIoWriteInt(regs->intAddress, regs->intValue);

	} else if (regs->intFlags & INT_FLAG_IO_RD) {
		regs->self->onIoReadInt(regs->intAddress, regs->intValue);

	} else if (regs->intFlags & INT_FLAG_INTE) {
		regs->self->onInte(regs->intValue);
	}
}


void altair::JitCore::onTickInt(uint8_t ticks) {
//	DBG(("%s(): CALL! ticks: %d", __func__, ticks));

	this->_pio.clk(ticks);

	this->_regs.intFF = (this->_pio.getInt() && this->_pio.getInte());
}


void altair::JitCore::onMemoryWriteInt(uint16_t address, uint8_t value) {
	this->_pio.memoryWrite(address, value);
}


void altair::JitCore::onMemoryReadInt(uint16_t address, uint8_t &value) {
	value = this->_pio.memoryRead(address);
}


void altair::JitCore::onIoWriteInt(uint8_t address, uint8_t value) {
	this->_pio.ioWrite(address, value);
}


void altair::JitCore::onIoReadInt(uint8_t address, uint8_t &value) {
	value = this->_pio.ioRead(address);
}


void altair::JitCore::onInte(uint8_t value) {
	this->_pio.setInte(value != 0);
}


altair::JitCore::JitCore(Pio &pio, uint16_t pc) : Core(), _pio(pio) {
	this->_regs.PC = pc;

	this->_regs.self       = this;
	this->_regs.intHandler = JitCore_onNativeInt;

	std::fill(std::begin(_opHandlers), std::begin(_opHandlers) + 0xff, nullptr);

	// MOV R, R
	this->_opAddDDDSSS(0, 1, A, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, A, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, B, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, C, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, D, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, E, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, H, A, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, B, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, C, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, D, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, E, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, H, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, L, _opMovRR);
	this->_opAddDDDSSS(0, 1, L, A, _opMovRR);
	// MOV R, M
	this->_opAddDDD(0, 1, B, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, C, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, D, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, E, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, H, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, L, 1, 1, 0, _opMovRM);
	this->_opAddDDD(0, 1, A, 1, 1, 0, _opMovRM);
	// MOV M, R
	this->_opAddSSS(0, 1, 1, 1, 0, B, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, C, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, D, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, E, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, H, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, L, _opMovMR);
	this->_opAddSSS(0, 1, 1, 1, 0, A, _opMovMR);
	// MVI R, I
	this->_opAddDDD(0, 0, B, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, C, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, D, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, E, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, H, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, L, 1, 1, 0, _opMviR);
	this->_opAddDDD(0, 0, A, 1, 1, 0, _opMviR);
	// MVI M, I
	this->_opAddBit(0, 0, 1, 1, 0, 1, 1, 0, _opMviM);
	// LXI
	this->_opAddRp(0, 0, BC, 0, 0, 0, 1, _opLxi);
	this->_opAddRp(0, 0, DE, 0, 0, 0, 1, _opLxi);
	this->_opAddRp(0, 0, HL, 0, 0, 0, 1, _opLxi);
	this->_opAddRp(0, 0, SP, 0, 0, 0, 1, _opLxi);
	// LDA
	this->_opAddBit(0, 0, 1, 1, 1, 0, 1, 0, _opLda);
	// STA
	this->_opAddBit(0, 0, 1, 1, 0, 0, 1, 0, _opSta);
	// LHLD
	this->_opAddBit(0, 0, 1, 0, 1, 0, 1, 0, _opLhld);
	// SHLD
	this->_opAddBit(0, 0, 1, 0, 0, 0, 1, 0, _opShld);
	// LDAX
	this->_opAddRp(0, 0, BC, 1, 0, 1, 0, _opLdax);
	this->_opAddRp(0, 0, DE, 1, 0, 1, 0, _opLdax);
	// STAX
	this->_opAddRp(0, 0, BC, 0, 0, 1, 0, _opStax);
	this->_opAddRp(0, 0, DE, 0, 0, 1, 0, _opStax);
	// XCHG
	this->_opAddBit(1, 1, 1, 0, 1, 0, 1, 1, _opXchg);

	// ADD R
	this->_opAddSSS(1, 0, 0, 0, 0, B, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, C, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, D, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, E, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, H, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, L, _opAddR);
	this->_opAddSSS(1, 0, 0, 0, 0, A, _opAddR);
	// ADD M
	this->_opAddBit(1, 0, 0, 0, 0, 1, 1, 0, _opAddM);


	// NOP
	this->_opAddBit(0, 0, 0, 0, 0, 0, 0, 0, _opNop);

	{

	}



	// INX
	this->_opAddRp(0, 0, BC, 0, 0, 1, 1, _opInx);
	this->_opAddRp(0, 0, DE, 0, 0, 1, 1, _opInx);
	this->_opAddRp(0, 0, HL, 0, 0, 1, 1, _opInx);
	this->_opAddRp(0, 0, SP, 0, 0, 1, 1, _opInx);

	// DCR
	this->_opAddDDD(0, 0, B, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, C, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, D, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, E, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, H, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, L, 1, 0, 1, _opDcr);
	this->_opAddDDD(0, 0, A, 1, 0, 1, _opDcr);

	// CMP
	this->_opAddSSS(1, 0, 1, 1, 1, B, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, C, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, D, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, E, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, H, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, L, _opCmp);
	this->_opAddSSS(1, 0, 1, 1, 1, A, _opCmp);

	// CPI
	this->_opAddBit(1, 1, 1, 1, 1, 1, 1, 0, _opCpi);

	// IN/OUT
	this->_opAddBit(1, 1, 0, 1, 0, 0, 1, 1, _opOut);
	this->_opAddBit(1, 1, 0, 1, 1, 0, 1, 1, _opIn);

	// EI/DI
	this->_opAddBit(1, 1, 1, 1, 1, 0, 1, 1, _opEid);
	this->_opAddBit(1, 1, 1, 1, 0, 0, 1, 1, _opEid);

	// ADI
	this->_opAddBit(1, 1, 0, 0, 0, 1, 1, 0, _opAdi);

	// SBBr
	this->_opAddSSS(1, 0, 0, 1, 1, B, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, C, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, D, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, E, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, H, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, L, _opSbbR);
	this->_opAddSSS(1, 0, 0, 1, 1, A, _opSbbR);

	// rotate accumulator
	this->_opAddBit(0, 0, 0, 0, 1, 1, 1, 1, _opRrc);

	// JMPX
	this->_opAddBit(1, 1, 0, 0, 0, 0, 1, 1, _opJmp); // jmp
	this->_opAddBit(1, 1, 0, 0, 0, 0, 1, 0, _opJmp); // jnz
	this->_opAddBit(1, 1, 0, 0, 1, 0, 1, 0, _opJmp); // jz
	this->_opAddBit(1, 1, 0, 1, 0, 0, 1, 0, _opJmp); // jnc
	this->_opAddBit(1, 1, 0, 1, 1, 0, 1, 0, _opJmp); // jc
	this->_opAddBit(1, 1, 1, 0, 0, 0, 1, 0, _opJmp); // jpo
	this->_opAddBit(1, 1, 1, 0, 1, 0, 1, 0, _opJmp); // jpe
	this->_opAddBit(1, 1, 1, 1, 0, 0, 1, 0, _opJmp); // jp
	this->_opAddBit(1, 1, 1, 1, 1, 0, 1, 0, _opJmp); // jm

	// CALLX
	this->_opAddBit(1, 1, 0, 0, 1, 1, 0, 1, _opCall); // call
	this->_opAddBit(1, 1, 0, 0, 0, 1, 0, 0, _opCall); // cnz
	this->_opAddBit(1, 1, 0, 0, 1, 1, 0, 0, _opCall); // cz
	this->_opAddBit(1, 1, 0, 1, 0, 1, 0, 0, _opCall); // cnc
	this->_opAddBit(1, 1, 0, 1, 1, 1, 0, 0, _opCall); // cc
	this->_opAddBit(1, 1, 1, 0, 0, 1, 0, 0, _opCall); // cpo
	this->_opAddBit(1, 1, 1, 0, 1, 1, 0, 0, _opCall); // cpe
	this->_opAddBit(1, 1, 1, 1, 0, 1, 0, 0, _opCall); // cp
	this->_opAddBit(1, 1, 1, 1, 1, 1, 0, 0, _opCall); // cm

	// RETX
	this->_opAddBit(1, 1, 0, 0, 1, 0, 0, 1, _opRet); // ret
	this->_opAddBit(1, 1, 0, 0, 0, 0, 0, 0, _opRet); // rnz
	this->_opAddBit(1, 1, 0, 0, 1, 0, 0, 0, _opRet); // rz
	this->_opAddBit(1, 1, 0, 1, 0, 0, 0, 0, _opRet); // rnc
	this->_opAddBit(1, 1, 0, 1, 1, 0, 0, 0, _opRet); // rc
	this->_opAddBit(1, 1, 1, 0, 0, 0, 0, 0, _opRet); // rpo
	this->_opAddBit(1, 1, 1, 0, 1, 0, 0, 0, _opRet); // rpe
	this->_opAddBit(1, 1, 1, 1, 0, 0, 0, 0, _opRet); // rp
	this->_opAddBit(1, 1, 1, 1, 1, 0, 0, 0, _opRet); // rm

	// PUSH/POP
	this->_opAddRp(1, 1, BC,   0, 1, 0, 1, _opPush);
	this->_opAddRp(1, 1, DE,   0, 1, 0, 1, _opPush);
	this->_opAddRp(1, 1, HL,   0, 1, 0, 1, _opPush);
	this->_opAddRp(1, 1, PSWA, 0, 1, 0, 1, _opPush);
	this->_opAddRp(1, 1, BC,   0, 0, 0, 1, _opPop);
	this->_opAddRp(1, 1, DE,   0, 0, 0, 1, _opPop);
	this->_opAddRp(1, 1, HL,   0, 0, 0, 1, _opPop);
	this->_opAddRp(1, 1, PSWA, 0, 0, 0, 1, _opPop);

	this->_opAddBit(0, 0, 1, 1, 0, 1, 1, 1, _opStc);
}


void altair::JitCore::turn() {
	this->execute(false);
}


void altair::JitCore::shutdown() {

}


altair::JitCore::ExecutionByteBuffer *altair::JitCore::compile(uint16_t pc, bool singleInstruction) {
	ExecutionByteBuffer *ret = new ExecutionByteBuffer();

	uint16_t currPc = pc;

	ret->begin();
	{
		uint8_t opcode;
		uint8_t ticks;
		bool    stop;
		uint8_t instructionSize;

		do {
			opcode = this->_pio.memoryRead(currPc);

			OpHandler handler = this->_opHandlers[opcode];
			if (handler == nullptr) {
				ERR(("%s(): opcode %02x is not implemented!", __func__, opcode));

				throw std::runtime_error("Opcode " + std::to_string(opcode) +  " is not supported!");
			}

			// Load flags
			//       [7][6][5][4][3][2][1][0]
			// 8080:  S  Z  0  AC 0  P  1  C
			// x86:   S  Z  0  AC 0  P  1  C
			// Load flags
			ret->
				// push rax
				append(0x50).
				// pushf
				append(0x9c).
				// pop    ax
				append(0x66).
				append(0x58).
				// and    ax,0xff2a
				append(0x66).
				append(0x25).
				append(0x2a).
				append(0xff).
				// or     al,BYTE PTR [rbp+<flags_off>]
				append(0x0a).
				append(0x45).
				append(offsetof (struct altair::JitCore::Regs, F)).
				// push   ax
				append(0x66).
				append(0x50).
				// popf
				append(0x9d).
				// pop rax
				append(0x58);

			instructionSize = handler(this, ret, opcode, currPc, ticks, stop);

			// Save flags
			ret->
				// push rax
				append(0x50).
				// pushf
				append(0x9c).
				// pop    ax
				append(0x66).
				append(0x58).
				// mov    BYTE PTR [rbp+<flags_off>],al
				append(0x88).
				append(0x45).
				append(offsetof (struct altair::JitCore::Regs, F)).
				// add    rsp,0x6
				append(0x48).
				append(0x83).
				append(0xc4).
				append(0x06).
				// pop rax
				append(0x58);

			// Increase PC
			if (! stop) {
				ret->
					// push rax
					append(0x50).

					// mov  ax,WORD PTR [rbp+offsetof(PC)]
					append(0x66).
					append(0x8b).
					append(0x45).
					append(offsetof(struct altair::JitCore::Regs, PC)).

					// add ax, isize
					append(0x66).
					append(0x83).
					append(0xc0).
					append(instructionSize).

					// mov  WORD PTR [rbp+offsetof(PC)],ax
					append(0x66).
					append(0x89).
					append(0x45).
					append(offsetof(struct altair::JitCore::Regs, PC)).

					// pop rax
					append(0x58);

				currPc += instructionSize;
			}

			this->addIntCodeCallTick(ret, ticks);

			// Interruption checking
			ret->
				// test   BYTE PTR [rbp+offsetof(intFF)], 0x1
				append(0xf6).
				append(0x45).
				append(offsetof(struct altair::JitCore::Regs, intFF)).
				append(0x01).
				// je <noint>
				append(0x74).
				append(0x01).
				// ret
				append(0xc3).
				// nop
				append(0x90);
		} while (! singleInstruction && ! stop);

		ret->append(0xc3); // retq
	}
	ret->end();

	DBG(("%s(): Compiled range between %04x - %04x", __func__, pc, currPc))

	return ret;
}


void altair::JitCore::addIntCodeCall(ExecutionByteBuffer *buffer, uint8_t flag) {
	buffer->
		// pushf
		append(0x9c).
		// push rax
		append(0x50).

		// mov al,flags
		append(0xb0).
		append(flag).

		// mov  BYTE PTR [rbp+offsetof(intFlags)],al
		append(0x88).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intFlags)).

		// mov rax,QWORD PTR [rbp+offsetof(intHandler)] ; load callback address
		append(0x48).
		append(0x8b).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intHandler)).

		// Store caller-save registers
		append(0x51).              // push rcx
		append(0x52).              // push rdx
		append(0x56).              // push rsi
		append(0x57).              // push rdi
		append(0x41).append(0x50). // push r8
		append(0x41).append(0x51). // push r9
		append(0x41).append(0x52). // push r10
		append(0x41).append(0x53). // push r11

		// mov rdi, rbp ; forward ctx as function parameters
		append(0x48).
		append(0x89).
		append(0xef).

		// call rax       ; call int handler
		append(0xff).
		append(0xd0).

		// Restore caller-save registers
		append(0x41).append(0x5b). // pop r11
		append(0x41).append(0x5a). // pop r10
		append(0x41).append(0x59). // pop r9
		append(0x41).append(0x58). // pop r8
		append(0x5f).              // pop rdi
		append(0x5e).              // pop rsi
		append(0x5a).              // pop rdx
		append(0x59).              // pop rcx

		// pop rax
		append(0x58).
		// popf
		append(0x9d);
}


void altair::JitCore::addIntCodeLoadIntAddrFromReg(ExecutionByteBuffer *b, uint8_t reg) {
	switch (reg) {
		case RegDouble::BC:
			// mov    WORD PTR [rbp+offsetof(intAddress)],bx
			b->append(0x66).append(0x89).append(0x5d).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::DE:
			// mov    WORD PTR [rbp+offsetof(intAddress)],cx
			b->append(0x66).append(0x89).append(0x4d).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::HL:
			// mov    WORD PTR [rbp+offsetof(intAddress)],dx
			b->append(0x66).append(0x89).append(0x55).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::SP:
			// mov    WORD PTR [rbp+offsetof(intAddress)],si
			b->append(0x66).append(0x89).append(0x75).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		default:
			ERR(("%s(): Not supported double register %u!", __func__, reg));

			throw std::runtime_error("Not supported double register!");
	}
}


void altair::JitCore::addIntCodeLoadIntAddrFromImm(ExecutionByteBuffer *buffer, uint16_t imm) {
	// mov WORD PTR [rbp+offsetof(intAddress)],imm
	buffer->
		append(0x66).
		append(0xc7).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intAddress)).
		append(imm & 0xff).
		append(imm >> 8);
}


void altair::JitCore::addIntCodeLoadIntValueFromReg(ExecutionByteBuffer *b, uint8_t reg) {
	if (reg == PSW) {
		// mov dil, PSW
		b->append(0x40).append(0x8a).append(0x7d).append(offsetof(struct altair::JitCore::Regs, F));
		// mov intValue, dil
		b->append(0x40).append(0x88).append(0x7d).append(offsetof(struct altair::JitCore::Regs, intValue));

	} else {
		b->append(0x88);

		switch(reg) {
			case RegSingle::B: b->append(0x7d); break;
			case RegSingle::C: b->append(0x5d); break;
			case RegSingle::D: b->append(0x6d); break;
			case RegSingle::E: b->append(0x4d); break;
			case RegSingle::H: b->append(0x75); break;
			case RegSingle::L: b->append(0x55); break;
			case RegSingle::A: b->append(0x45); break;

		default:
			ERR(("%s(): Not supported single register %u!", __func__, reg));

			throw std::runtime_error("Not supported single register!");
		}

		b->append(offsetof(struct altair::JitCore::Regs, intValue));
	}
}


void altair::JitCore::addIntCodeLoadIntValueFromImm(ExecutionByteBuffer *buffer, uint8_t imm) {
	buffer->
		// push rax
		append(0x50).

		// mov al,0xc
		append(0xb0).
		append(imm).

		// mov  BYTE PTR [rbp+offsetof(intValue)],al
		append(0x88).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intValue)).

		// pop rax
		append(0x58);
}


void altair::JitCore::addIntCodeLoadRegFromIntValue(ExecutionByteBuffer *buffer, uint8_t reg) {

}


void altair::JitCore::addIntCodeCallTick(ExecutionByteBuffer *buffer, uint16_t ticks) {
	// ticks == 0 indicates that number of ticks is runtime dependent. In that case
	// number of ticks spent, is put into dil register by opcode handler.
	if (ticks > 0) {
		this->addIntCodeLoadIntValueFromImm(buffer, ticks);

	} else {
		// mov intValue, dil
		buffer->append(0x40).append(0x88).append(0x7d).append(offsetof(struct altair::JitCore::Regs, intValue));
	}

	this->addIntCodeCall(buffer, INT_FLAG_TICKS);
}


void altair::JitCore::addIntCodeCallMemoryWrite(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_MEM_WR);
}


void altair::JitCore::addIntCodeCallMemoryRead(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_MEM_RD);
}


void altair::JitCore::addIntCodeCallIoWr(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_IO_WR);
}


void altair::JitCore::addIntCodeCallIoRd(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_IO_RD);
}


void altair::JitCore::nexti() {
	this->execute(true);
}


void altair::JitCore::execute(bool singleInstruction) {
	ExecutionByteBuffer *codeSegment = this->_compiledBlocks[this->_regs.PC];
	if (codeSegment == nullptr) {
		this->_compiledBlocks[this->_regs.PC] = this->compile(this->_regs.PC, singleInstruction);

		codeSegment = this->_compiledBlocks[this->_regs.PC];
	}

	/*
	 * Reg mapping:
	 * [P][A] -> [VT][AL] rax
	 * [B][C] -> [BH][BL] rbx
	 * [D][E] -> [CH][CL] rcx
	 * [H][L] -> [DH][DL] rdx
	 * [SP]   -> [SI]     rsi
	 *
	 * rdi - temporary register (used in push/pop/ret instructions)
	 */

	{
		Regs *regs = &this->_regs;

		regs->codeSegment = codeSegment->getPtr();

		__asm (
			"push rax                \n\t"
			"push rbx                \n\t"
			"push rcx                \n\t"
			"push rdx                \n\t"
			"push rdi                \n\t"
			"push rsi                \n\t"
			"push rsp                \n\t"
			"push rbp                \n\t"
			"pushf                   \n\t"

			"mov rbp, rax            \n\t"

			// Load 8080 regs
			"mov bh, [rbp + %[off_b]]  \n\t"
			"mov bl, [rbp + %[off_c]]  \n\t"
			"mov ch, [rbp + %[off_d]]  \n\t"
			"mov cl, [rbp + %[off_e]]  \n\t"
			"mov dh, [rbp + %[off_h]]  \n\t"
			"mov dl, [rbp + %[off_l]]  \n\t"
			"mov al, [rbp + %[off_a]]  \n\t"
			"mov si, [rbp + %[off_sp]] \n\t"

			"callq [rbp + %[off_cs]]   \n\t"

			// Store 8080 regs
			"mov [rbp + %[off_b]],  bh \n\t"
			"mov [rbp + %[off_c]],  bl \n\t"
			"mov [rbp + %[off_d]],  ch \n\t"
			"mov [rbp + %[off_e]],  cl \n\t"
			"mov [rbp + %[off_h]],  dh \n\t"
			"mov [rbp + %[off_l]],  dl \n\t"
			"mov [rbp + %[off_a]],  al \n\t"
			"mov [rbp + %[off_sp]], si \n\t"

			"popf                    \n\t"
			"pop rbp                 \n\t"
			"pop rsp                 \n\t"
			"pop rsi                 \n\t"
			"pop rdi                 \n\t"
			"pop rdx                 \n\t"
			"pop rcx                 \n\t"
			"pop rbx                 \n\t"
			"pop rax                 \n\t"
			:
			:
				"a" (regs),
				[off_b]  "i" (offsetof (struct altair::JitCore::Regs, B)),
				[off_c]  "i" (offsetof (struct altair::JitCore::Regs, C)),
				[off_d]  "i" (offsetof (struct altair::JitCore::Regs, D)),
				[off_e]  "i" (offsetof (struct altair::JitCore::Regs, E)),
				[off_h]  "i" (offsetof (struct altair::JitCore::Regs, H)),
				[off_l]  "i" (offsetof (struct altair::JitCore::Regs, L)),
				[off_a]  "i" (offsetof (struct altair::JitCore::Regs, A)),
				[off_f]  "i" (offsetof (struct altair::JitCore::Regs, F)),
				[off_sp] "i" (offsetof (struct altair::JitCore::Regs, SP)),
				[off_cs] "i" (offsetof (struct altair::JitCore::Regs, codeSegment))
			:
		);
	}

	// Handle interrupt
	if (this->_regs.intFF) {
		uint8_t ir = this->_pio.getData();

		DBG(("%s(): Interrupt! saving current PC (%04x), SP: %04x", __func__, this->_regs.PC, this->_regs.SP));

		if ((ir & 0xc7) != 0xc7) {
			throw std::runtime_error("Invalid instruction on interrupt! RST is the only one supported!");
		}

		// Disable interrupts
		this->_pio.setInte(false);

		// Put return address on stack
		this->_pio.memoryWrite(--this->_regs.SP, this->_regs.PC >> 8);
		this->_pio.memoryWrite(--this->_regs.SP, this->_regs.PC & 0xff);

		switch ((ir >> 3) & 0x07) {
			case 0: this->_regs.PC = 0x0000; break;
			case 1: this->_regs.PC = 0x0008; break;
			case 2: this->_regs.PC = 0x0010; break;
			case 3: this->_regs.PC = 0x0018; break;
			case 4: this->_regs.PC = 0x0020; break;
			case 5: this->_regs.PC = 0x0028; break;
			case 6: this->_regs.PC = 0x0030; break;
			case 7: this->_regs.PC = 0x0038; break;
		}

		DBG(("%s(): Interrupt handler PC: %04x", __func__, this->_regs.PC));

		this->_regs.intFF = 0;
	}
}


uint8_t  altair::JitCore::bR(BReg reg) const {
	switch (reg) {
		case BReg::A: return this->_regs.A;
		case BReg::B: return this->_regs.B;
		case BReg::C: return this->_regs.C;
		case BReg::D: return this->_regs.D;
		case BReg::E: return this->_regs.E;
		case BReg::H: return this->_regs.H;
		case BReg::L: return this->_regs.L;
		case BReg::F: return this->_regs.F;
	}

	ERR(("%s(): Not supported BReg value! (%02x)", __func__, (uint8_t)reg));

	throw std::runtime_error("Not supported BReg value!");
}


void altair::JitCore::bR(BReg r, uint8_t val) {

}


uint16_t altair::JitCore::wR(WReg reg) const {
	switch (reg) {
		case WReg::PC: return this->_regs.PC;
		case WReg::SP: return this->_regs.SP;
		case WReg::B:  return ((this->_regs.B << 8) | this->_regs.C);
		case WReg::D:  return ((this->_regs.D << 8) | this->_regs.E);
		case WReg::H:  return ((this->_regs.H << 8) | this->_regs.L);
	}

	ERR(("%s(): Not supported WReg value! (%02x)", __func__, (uint8_t)reg));

	throw std::runtime_error("Not supported WReg value!");
}


void altair::JitCore::wR(WReg reg, uint16_t val) {

}


uint8_t altair::JitCore::wRL(WReg reg) {
	switch (reg) {
		case WReg::B:  return this->_regs.C;
		case WReg::D:  return this->_regs.E;
		case WReg::H:  return this->_regs.L;
		case WReg::SP: return this->_regs.SP & 0xff;
		default:
			ERR(("%s(): Not supported WReg value! (%02x)", __func__, (uint8_t)reg));

			throw std::runtime_error("Not supported WReg value!");
	}
}


uint8_t altair::JitCore::wRH(WReg reg) {
	switch (reg) {
		case WReg::B:  return this->_regs.B;
		case WReg::D:  return this->_regs.D;
		case WReg::H:  return this->_regs.H;
		case WReg::SP: return this->_regs.SP >> 8;
		default:
			ERR(("%s(): Not supported WReg value! (%02x)", __func__, (uint8_t)reg));

			throw std::runtime_error("Not supported WReg value!");
	}
}


altair::Core::Alu *altair::JitCore::alu() {
	return this;
}


bool altair::JitCore::fZ()  const {
	return (this->_regs.F & (1 << 6)) != 0;
}


bool altair::JitCore::fCY() const {
	return (this->_regs.F & (1 << 0)) != 0;
}


bool altair::JitCore::fS()  const {
	return (this->_regs.F & (1 << 7)) != 0;
}


bool altair::JitCore::fP()  const {
	return (this->_regs.F & (1 << 2)) != 0;
}


bool altair::JitCore::fAC() const {
	return (this->_regs.F & (1 << 4)) != 0;
}


void altair::JitCore::_opAddBit(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler) {
	this->_opHandlers[
		(bit7 << 7) | (bit6 << 6) | (bit5 << 5) | (bit4 << 4) |
		(bit3 << 3) | (bit2 << 2) | (bit1 << 1) | (bit0 << 0)
	] = handler;
}


void altair::JitCore::_opAddSSS(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t src, OpHandler handler) {
	this->_opAddBit(bit7, bit6, bit5, bit4, bit3, (src & 4) >> 2, (src & 2) >> 1, src & 1, handler);
}


void altair::JitCore::_opAddDDD(uint8_t bit7, uint8_t bit6, uint8_t registers, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler) {
	this->_opAddBit(bit7, bit6, (registers & 4) >> 2, (registers & 2) >> 1, registers & 1, bit2, bit1, bit0, handler);
}


void altair::JitCore::_opAddDDDSSS(uint8_t bit7, uint8_t bit6, uint8_t dst, uint8_t src, OpHandler handler) {
	this->_opAddDDD(bit7, bit6, dst, (src & 4) >> 2, (src & 2) >> 1, src & 1, handler);
}


void altair::JitCore::_opAddRp(uint8_t bit7, uint8_t bit6, uint8_t rp, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler) {
	this->_opAddBit(bit7, bit6, (rp & 2) >> 1, (rp & 1), bit3, bit2,  bit1, bit0, handler);
}


static RegSingle _srcR(uint8_t reg) {
	return (RegSingle)(reg & 0x07);
}


static RegSingle _dstR(uint8_t reg) {
	return (RegSingle)((reg >> 3) & 0x07);
}


static RegDouble _rp(uint8_t reg) {
	return (RegDouble)((reg >> 4) & 0x03);
}


int altair::JitCore::_opNop(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	ticks = 4;

	return 1;
}


int altair::JitCore::_opMviR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	switch(_dstR(opcode)) {
		case RegSingle::B: buffer->append(0xb7); break; // mov bh, imm
		case RegSingle::C: buffer->append(0xb3); break; // mov bl, imm
		case RegSingle::D: buffer->append(0xb5); break; // mov ch, imm
		case RegSingle::E: buffer->append(0xb1); break; // mov cl, imm
		case RegSingle::H: buffer->append(0xb6); break; // mov dh, imm
		case RegSingle::L: buffer->append(0xb2); break; // mov dl, imm
		case RegSingle::A: buffer->append(0xb0); break; // mov al, imm

		default:
			ERR(("%s(): Not supported src reg! (%02x)", __func__, _dstR(opcode)));

			throw std::runtime_error("Not supported src reg! " + std::to_string(_dstR(opcode)));
	}

	buffer->append(core->_pio.memoryRead(pc + 1)); // imm

	ticks = 7;

	return 2;
}


int altair::JitCore::_opMviM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromReg (buffer, RegDouble::HL);
	core->addIntCodeLoadIntValueFromImm(buffer, core->_pio.memoryRead(pc + 1));
	core->addIntCodeCallMemoryWrite(buffer);

	ticks = 10;

	return 2;
}


int altair::JitCore::_opMovRR(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	b->append(0x88);

	switch (_dstR(opcode)) {
		case RegSingle::B:
			switch (_srcR(opcode)) {
				case RegSingle::B: break; // NOP
				case RegSingle::C: b->append(0xdf); break;
				case RegSingle::D: b->append(0xef); break;
				case RegSingle::E: b->append(0xcf); break;
				case RegSingle::H: b->append(0xf7); break;
				case RegSingle::L: b->append(0xd7); break;
				case RegSingle::A: b->append(0xc7); break;
			}
			break;

		case RegSingle::C:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xfb); break;
				case RegSingle::C: break; // NOP
				case RegSingle::D: b->append(0xeb); break;
				case RegSingle::E: b->append(0xcb); break;
				case RegSingle::H: b->append(0xf3); break;
				case RegSingle::L: b->append(0xd3); break;
				case RegSingle::A: b->append(0xc3); break;
			}
			break;

		case RegSingle::D:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xfd); break;
				case RegSingle::C: b->append(0xdd); break;
				case RegSingle::D: break; // NOP
				case RegSingle::E: b->append(0xcd); break;
				case RegSingle::H: b->append(0xf5); break;
				case RegSingle::L: b->append(0xd5); break;
				case RegSingle::A: b->append(0xc5); break;
			}
			break;

		case RegSingle::E:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xf9); break;
				case RegSingle::C: b->append(0xd9); break;
				case RegSingle::D: b->append(0xe9); break;
				case RegSingle::E: break; // NOP
				case RegSingle::H: b->append(0xf1); break;
				case RegSingle::L: b->append(0xd1); break;
				case RegSingle::A: b->append(0xc1); break;
			}
			break;

		case RegSingle::H:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xfe); break;
				case RegSingle::C: b->append(0xde); break;
				case RegSingle::D: b->append(0xee); break;
				case RegSingle::E: b->append(0xce); break;
				case RegSingle::H: break; // NOP
				case RegSingle::L: b->append(0xd6); break;
				case RegSingle::A: b->append(0xc6); break;
			}
			break;

		case RegSingle::L:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xfa); break;
				case RegSingle::C: b->append(0xda); break;
				case RegSingle::D: b->append(0xea); break;
				case RegSingle::E: b->append(0xca); break;
				case RegSingle::H: b->append(0xf2); break;
				case RegSingle::L: break; // NOP
				case RegSingle::A: b->append(0xc2); break;
			}
			break;

		case RegSingle::A:
			switch (_srcR(opcode)) {
				case RegSingle::B: b->append(0xf8); break;
				case RegSingle::C: b->append(0xd8); break;
				case RegSingle::D: b->append(0xe8); break;
				case RegSingle::E: b->append(0xc8); break;
				case RegSingle::H: b->append(0xf0); break;
				case RegSingle::L: b->append(0xd0); break;
				case RegSingle::A: break; // NOP
			}
			break;
	}

	ticks = 5;

	return 1;
}


int altair::JitCore::_opMovRM(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromReg(b, RegDouble::HL);
	core->addIntCodeCallMemoryRead(b);

	b->append(0x8a);

	switch (_dstR(opcode)) {
		case RegSingle::B: b->append(0x7d); break;
		case RegSingle::C: b->append(0x5d); break;
		case RegSingle::D: b->append(0x6d); break;
		case RegSingle::E: b->append(0x4d); break;
		case RegSingle::H: b->append(0x75); break;
		case RegSingle::L: b->append(0x55); break;
		case RegSingle::A: b->append(0x45); break;
	}

	b->append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 7;

	return 1;
}


int altair::JitCore::_opMovMR(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromReg (b, RegDouble::HL);
	core->addIntCodeLoadIntValueFromReg(b, _srcR(opcode));
	core->addIntCodeCallMemoryWrite(b);

	ticks = 7;

	return 1;
}


int altair::JitCore::_opLxi(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->append(0x66);

	switch (_rp(opcode)) {
		case RegDouble::BC: buffer->append(0xbb); break;
		case RegDouble::DE: buffer->append(0xb9); break;
		case RegDouble::HL: buffer->append(0xba); break;
		case RegDouble::SP: buffer->append(0xbe); break;
	}

	buffer->append(core->_pio.memoryRead(pc + 1));
	buffer->append(core->_pio.memoryRead(pc + 2));

	ticks = 10;

	return 3;
}


int altair::JitCore::_opLda(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromImm(buffer, core->_pio.memoryRead(pc + 1) | (core->_pio.memoryRead(pc + 2) << 8));
	core->addIntCodeCallMemoryRead(buffer);

	// mov al, intValue
	buffer->
		append(0x8a).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 13;

	return 3;
}


int altair::JitCore::_opSta(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromImm(buffer, core->_pio.memoryRead(pc + 1) | (core->_pio.memoryRead(pc + 2) << 8));
	core->addIntCodeLoadIntValueFromReg(buffer, A);
	core->addIntCodeCallMemoryWrite(buffer);

	ticks = 13;

	return 3;
}


int altair::JitCore::_opLhld (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	uint16_t addr = core->_pio.memoryRead(pc + 1) | (core->_pio.memoryRead(pc + 2) << 8);

	core->addIntCodeLoadIntAddrFromImm(buffer, addr);
	core->addIntCodeCallMemoryRead(buffer);

	buffer->
		append(0x8a).
		append(0x55).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	core->addIntCodeLoadIntAddrFromImm(buffer, addr + 1);
	core->addIntCodeCallMemoryRead(buffer);

	buffer->
		append(0x8a).
		append(0x75).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 16;

	return 3;
}


int altair::JitCore::_opShld (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	uint16_t addr = core->_pio.memoryRead(pc + 1) | (core->_pio.memoryRead(pc + 2) << 8);

	core->addIntCodeLoadIntAddrFromImm(buffer, addr);
	core->addIntCodeLoadIntValueFromReg(buffer, L);
	core->addIntCodeCallMemoryWrite(buffer);

	core->addIntCodeLoadIntAddrFromImm(buffer, addr + 1);
	core->addIntCodeLoadIntValueFromReg(buffer, H);
	core->addIntCodeCallMemoryWrite(buffer);

	ticks = 16;

	return 3;
}


int altair::JitCore::_opLdax (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	switch (_rp(opcode)) {
		case RegDouble::BC:
		case RegDouble::DE:
			core->addIntCodeLoadIntAddrFromReg(buffer, _rp(opcode));
			break;

		default:
			throw std::runtime_error("Not supported LDAX reg!");
	}

	core->addIntCodeCallMemoryRead(buffer);

	// mov al, intValue
	buffer->
		append(0x8a).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 7;

	return 1;
}


int altair::JitCore::_opStax(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	switch (_rp(opcode)) {
		case RegDouble::BC:
		case RegDouble::DE:
			core->addIntCodeLoadIntAddrFromReg(buffer, _rp(opcode));
			break;

		default:
			throw std::runtime_error("Not supported STAX reg!");
	}

	core->addIntCodeLoadIntValueFromReg(buffer, A);
	core->addIntCodeCallMemoryWrite(buffer);

	ticks = 7;

	return 1;
}


int altair::JitCore::_opXchg (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->
		// pushf
		append(0x9c).
		// xor    dh,ch
		append(0x30).append(0xee).
		// xor    ch,dh
		append(0x30).append(0xf5).
		// xor    dh,ch
		append(0x30).append(0xee).
		// xor    dl,cl
		append(0x30).append(0xca).
		// xor    cl,dl
		append(0x30).append(0xd1).
		// xor    dl,cl
		append(0x30).append(0xca).
		// popf
		append(0x9d);

	ticks = 4;

	return 1;
}


int altair::JitCore::_opAddR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->append(0x00);

	switch (_srcR(opcode)) {
		case B: buffer->append(0xf8); break;
		case C: buffer->append(0xd8); break;
		case D: buffer->append(0xe8); break;
		case E: buffer->append(0xc8); break;
		case H: buffer->append(0xf0); break;
		case L: buffer->append(0xd0); break;
		case A: buffer->append(0xc0); break;
	}

	ticks = 4;

	return 1;
}


int altair::JitCore::_opAddM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromReg(buffer, HL);
	core->addIntCodeCallMemoryRead(buffer);

	// add    al,BYTE PTR [rbp+intValue]
	buffer->
		append(0x02).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 7;

	return 1;
}


int altair::JitCore::_opAdi(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->
		append(0x04).
		append(core->_pio.memoryRead(pc + 1));

	ticks = 7;

	return 2;
}


int altair::JitCore::_opInx(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	// pushf
	b->append(0x9c);

	b->append(0x66).append(0xff);

	switch (_rp(opcode)) {
		case RegDouble::BC:
			b->append(0xc3);
			break;

		case RegDouble::DE:
			b->append(0xc1);
			break;

		case RegDouble::HL:
			b->append(0xc2);
			break;

		case RegDouble::SP:
			b->append(0xc6);
			break;

		default:
			break;
	}

	// popf
	b->append(0x9d);

	ticks = 5;

	return 1;
}


int altair::JitCore::_opOut(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromImm(buffer, core->_pio.memoryRead(pc + 1));
	core->addIntCodeLoadIntValueFromReg(buffer, RegSingle::A);
	core->addIntCodeCallIoWr(buffer);

	ticks = 10;

	return 2;
}


int altair::JitCore::_opIn(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntAddrFromImm(buffer, core->_pio.memoryRead(pc + 1));
	core->addIntCodeCallIoRd(buffer);

	buffer->
		append(0x8a).
		append(0x45).
		append(offsetof(struct altair::JitCore::Regs, intValue));

	ticks = 10;

	return 2;
}


int altair::JitCore::_opEid(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	core->addIntCodeLoadIntValueFromImm(buffer, (opcode & 0x08) != 0);
	core->addIntCodeCall(buffer, INT_FLAG_INTE);

	ticks = 4;

	return 1;
}


int altair::JitCore::_opPush(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	// pushf
	buffer->append(0x9c);

	// dec si
	buffer->
		append(0x66).
		append(0xff).
		append(0xce);
	core->addIntCodeLoadIntAddrFromReg(buffer, SP);

	switch (_rp(opcode)) {
		case BC:   core->addIntCodeLoadIntValueFromReg(buffer, B); break;
		case DE:   core->addIntCodeLoadIntValueFromReg(buffer, D); break;
		case HL:   core->addIntCodeLoadIntValueFromReg(buffer, H); break;
		case PSWA: core->addIntCodeLoadIntValueFromReg(buffer, A); break;
	}

	core->addIntCodeCallMemoryWrite(buffer);

	// dec si
	buffer->
		append(0x66).
		append(0xff).
		append(0xce);
	core->addIntCodeLoadIntAddrFromReg(buffer, SP);

	switch (_rp(opcode)) {
		case BC:   core->addIntCodeLoadIntValueFromReg(buffer, C);   break;
		case DE:   core->addIntCodeLoadIntValueFromReg(buffer, E);   break;
		case HL:   core->addIntCodeLoadIntValueFromReg(buffer, L);   break;
		case PSWA: core->addIntCodeLoadIntValueFromReg(buffer, PSW); break;
	}

	core->addIntCodeCallMemoryWrite(buffer);

	// popf
	buffer->append(0x9d);

	ticks = 11;

	return 1;
}


int altair::JitCore::_opPop(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	// pushf
	b->append(0x9c);

	{
		core->addIntCodeLoadIntAddrFromReg(b, SP);
		core->addIntCodeCallMemoryRead(b);

		if (_rp(opcode) == PSWA) {
			// Load flags register
			b->
				// mov    dil, intValue
				append(0x40).
				append(0x8a).
				append(0x7d).
				append(offsetof(struct altair::JitCore::Regs, intValue)).
				// and    BYTE PTR [rsp],0x2a
				append(0x80).
				append(0x24).
				append(0x24).
				append(0x2a).
				// or     BYTE PTR [rsp],dil
				append(0x40).
				append(0x08).
				append(0x3c).
				append(0x24);

		} else {
			b->append(0x8a);
			switch (_rp(opcode)) {
				case BC:   b->append(0x5d); break;
				case DE:   b->append(0x4d); break;
				case HL:   b->append(0x55); break;
				case PSWA:  break;
			}
			b->append(offsetof(struct altair::JitCore::Regs, intValue));
		}

		// inc SP
		b->
			append(0x66).
			append(0xff).
			append(0xc6);
	}

	{
		core->addIntCodeLoadIntAddrFromReg(b, SP);
		core->addIntCodeCallMemoryRead(b);

		b->append(0x8a);
		switch (_rp(opcode)) {
			case BC:   b->append(0x7d); break;
			case DE:   b->append(0x6d); break;
			case HL:   b->append(0x75); break;
			case PSWA: b->append(0x45); break;
		}
		b->append(offsetof(struct altair::JitCore::Regs, intValue));

		// inc SP
		b->
			append(0x66).
			append(0xff).
			append(0xc6);
	}

	// popf
	b->append(0x9d);

	ticks = 10;

	return 1;
}


int altair::JitCore::_opRet(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	ExecutionByteBuffer retCode;

	{
		core->addIntCodeLoadIntAddrFromReg(&retCode, SP);
		core->addIntCodeCallMemoryRead(&retCode);

		retCode.
			// mov    al,BYTE PTR [rbp+intValue]
			append(0x8a).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, intValue)).
			// inc    si
			append(0x66).
			append(0xff).
			append(0xc6);

		core->addIntCodeLoadIntAddrFromReg(&retCode, SP);
		core->addIntCodeCallMemoryRead(&retCode);

		retCode.
			// mov    ah,BYTE PTR [rbp+intValue]
			append(0x8a).
			append(0x65).
			append(offsetof(struct altair::JitCore::Regs, intValue)).
			// inc si
			append(0x66).
			append(0xff).
			append(0xc6).
			// mov    WORD PTR [rbp+PC],ax
			append(0x66).
			append(0x89).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC));
	}

	b->
		// pushf
		append(0x9c).
		// push rax
		append(0x50);

	if (opcode & 1) {
		// Add RET code
		b->append(retCode.getPtr(), retCode.getSize());

	} else {
		// mov    dil,11 ; ticks
		retCode.append(0x40).append(0xb7).append(11);

		// Skip code that sets PC+3
		retCode.
			append(0xeb).
			append(0x08);

		switch (_dstR(opcode)) {
			case 0: // jnz
				b->append(0x74).append(retCode.getSize()); // jz <false>
				break;

			case 1: // jz
				b->append(0x75).append(retCode.getSize()); // jnz <false>
				break;

			case 2: // jnc
				b->append(0x72).append(retCode.getSize()); // jc <false>
				break;

			case 3: // jc
				b->append(0x73).append(retCode.getSize()); // jnc <false>
				break;

			case 4: // jpo
				b->append(0x7a).append(retCode.getSize()); // jpe <false>
				break;

			case 5: // jpe
				b->append(0x7b).append(retCode.getSize()); // jpo <false>
				break;

			case 6: // jp
				b->append(0x78).append(retCode.getSize()); // jm <false>
				break;

			case 7: // jm
				b->append(0x79).append(retCode.getSize()); // jp <false>
				break;
		}

		// Add RET code
		b->append(retCode.getPtr(), retCode.getSize());

		b->
			// add    WORD PTR [rbp+<pc>],3
			append(0x66).
			append(0x83).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC)).
			append(1);

		// mov    dil,5 ; ticks
		b->
			append(0x40).
			append(0xb7).
			append(5);
	}

	b->
		// pop rax
		append(0x58).
		// popf
		append(0x9d);

	stop = true;

	if (opcode & 0x01) {
		ticks = 10;

	} else {
		ticks = 0;
	}

	return 1;
}


int altair::JitCore::_opCall(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	ExecutionByteBuffer callCode;

	pc += 3;

	{
		// dec si
		callCode.
			append(0x66).
			append(0xff).
			append(0xce);
		core->addIntCodeLoadIntAddrFromReg(&callCode, SP);
		core->addIntCodeLoadIntValueFromImm(&callCode, pc >> 8);
		core->addIntCodeCallMemoryWrite(&callCode);

		// dec si
		callCode.
			append(0x66).
			append(0xff).
			append(0xce);
		core->addIntCodeLoadIntAddrFromReg(&callCode, SP);
		core->addIntCodeLoadIntValueFromImm(&callCode, pc & 0xff);
		core->addIntCodeCallMemoryWrite(&callCode);

		{
			uint16_t dstPc = core->_pio.memoryRead(pc - 2) | (core->_pio.memoryRead(pc - 1) << 8);

			callCode.
				append(0x66).
				append(0xc7).
				append(0x45).
				append(offsetof(struct altair::JitCore::Regs, PC)).
				append(dstPc & 0xff).
				append(dstPc >> 8);
		}
	}

	// pushf
	b->append(0x9c);

	if (opcode & 1) {
		// Add RET code
		b->append(callCode.getPtr(), callCode.getSize());

	} else {
		// mov    dil,17 ; ticks
		callCode.append(0x40).append(0xb7).append(17);

		// Skip code that sets PC+3
		callCode.
			append(0xeb).
			append(0x08);

		switch (_dstR(opcode)) {
			case 0: // jnz
				b->append(0x74).append(callCode.getSize()); // jz <false>
				break;

			case 1: // jz
				b->append(0x75).append(callCode.getSize()); // jnz <false>
				break;

			case 2: // jnc
				b->append(0x72).append(callCode.getSize()); // jc <false>
				break;

			case 3: // jc
				b->append(0x73).append(callCode.getSize()); // jnc <false>
				break;

			case 4: // jpo
				b->append(0x7a).append(callCode.getSize()); // jpe <false>
				break;

			case 5: // jpe
				b->append(0x7b).append(callCode.getSize()); // jpo <false>
				break;

			case 6: // jp
				b->append(0x78).append(callCode.getSize()); // jm <false>
				break;

			case 7: // jm
				b->append(0x79).append(callCode.getSize()); // jp <false>
				break;
		}

		// Add CALL code
		b->append(callCode.getPtr(), callCode.getSize());

		b->
			// add    WORD PTR [rbp+<pc>],3
			append(0x66).
			append(0x83).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC)).
			append(3);

		// mov    dil,11 ; ticks
		b->
			append(0x40).
			append(0xb7).
			append(11);
	}

	// popf
	b->append(0x9d);

	stop = true;

	if (opcode & 0x01) {
		ticks = 17;

	} else {
		ticks = 0;
	}

	return 3;
}


int altair::JitCore::_opSbbR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->append(0x18);

	switch (_srcR(opcode)) {
		case B: buffer->append(0xf8); break;
		case C: buffer->append(0xd8); break;
		case D: buffer->append(0xe8); break;
		case E: buffer->append(0xc8); break;
		case H: buffer->append(0xf0); break;
		case L: buffer->append(0xd0); break;
		case A: buffer->append(0xc0); break;
	}

	ticks = 4;

	return 1;
}


int altair::JitCore::_opRrc(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {

	buffer->
		// pushf
		append(0x9c).

		// ror    al,1
		append(0xd0).
		append(0xc8).

		// test   al,0x80
		append(0xa8).
		append(0x80).

		// jne    +7 <isset>
		append(0x75).
		append(0x06).

		// and    BYTE PTR [rsp],0xfe
		append(0x80).
		append(0x24).
		append(0x24).
		append(0xfe).

		// jmp    +5 <done>
		append(0xeb).
		append(0x04).

		// <isset> or     BYTE PTR [rsp],0x01
		append(0x80).
		append(0x0c).
		append(0x24).
		append(0x01).

		// popf
		append(0x9d);

	ticks = 4;

	return 1;
}


int altair::JitCore::_opDcr(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	b->append(0xfe);

	switch (_dstR(opcode)) {
		case B: b->append(0xcf); break;
		case C: b->append(0xcb); break;
		case D: b->append(0xcd); break;
		case E: b->append(0xc9); break;
		case H: b->append(0xce); break;
		case L: b->append(0xca); break;
		case A: b->append(0xc8); break;
	}

	ticks = 5;

	return 1;
}


int altair::JitCore::_opCmp(JitCore *core, ExecutionByteBuffer *b, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	// push rax
	b->append(0x50);

	b->append(0x28); // sub
	switch (_srcR(opcode)) {
		case B: b->append(0xf8); break;
		case C: b->append(0xd8); break;
		case D: b->append(0xe8); break;
		case E: b->append(0xc8); break;
		case H: b->append(0xf0); break;
		case L: b->append(0xd0); break;
		case A: b->append(0xc0); break;
	}

	// pop rax
	b->append(0x58);

	ticks = 4;

	return 1;
}


int altair::JitCore::_opCpi(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	buffer->
		// push rax
		append(0x50).
		// sub al, imm
		append(0x2c).
		append(core->_pio.memoryRead(pc + 1)).
		// pop rax
		append(0x58);

	ticks = 7;

	return 2;
}


int altair::JitCore::_opStc(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	// STC
	buffer->append(0xf9);

	ticks = 4;

	return 1;
}


int altair::JitCore::_opJmp(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	uint16_t dstPc = core->_pio.memoryRead(pc + 1) | (core->_pio.memoryRead(pc + 2) << 8);

	if (opcode & 0x01) { // jmp
		buffer->
			// mov    WORD PTR [rbp+<pc>],new_PC
			append(0x66).
			append(0xc7).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC)).
			append(dstPc & 0xff).
			append(dstPc >> 8);

	} else {
		switch (_dstR(opcode)) {
			case 0: // jnz
				buffer->append(0x74).append(0x08); // jz <false>
				break;

			case 1: // jz
				buffer->append(0x75).append(0x08); // jnz <false>
				break;

			case 2: // jnc
				buffer->append(0x72).append(0x08); // jc <false>
				break;

			case 3: // jc
				buffer->append(0x73).append(0x08); // jnc <false>
				break;

			case 4: // jpo
				buffer->append(0x7a).append(0x08); // jpe <false>
				break;

			case 5: // jpe
				buffer->append(0x7b).append(0x08); // jpo <false>
				break;

			case 6: // jp
				buffer->append(0x78).append(0x08); // jm <false>
				break;

			case 7: // jm
				buffer->append(0x79).append(0x08); // jp <false>
				break;
		}

		buffer->
			// mov    WORD PTR [rbp+<pc>],new_PC
			append(0x66).
			append(0xc7).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC)).
			append(dstPc & 0xff).
			append(dstPc >> 8).

			// jmp <done>
			append(0xeb).
			append(0x07).

			// <false> pushf
			append(0x9c).

			// add    WORD PTR [rbp+<pc>],3
			append(0x66).
			append(0x83).
			append(0x45).
			append(offsetof(struct altair::JitCore::Regs, PC)).
			append(3).

			// popf
			append(0x9d).

			// <done> nop
			append(0x90);
	}

	stop = true;

	ticks = 10;

	return 3;
}

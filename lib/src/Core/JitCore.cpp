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


enum RegSingle {
	B = 0,
	C = 1,
	D = 2,
	E = 3,
	H = 4,
	L = 5,
	A = 7
};

enum RegDouble {
	BC = 0,
	DE = 1,
	HL = 2,
	SP = 3
};


void JitCore_onNativeInt(void *ctx) {
	altair::JitCore::Regs *regs = reinterpret_cast<altair::JitCore::Regs *>(ctx);

	DBG(("%s(): CALL, regs: %p, self: %p, mask: %02x, address: %04x, data: %u (%02x)",
		__func__, regs, regs->self, regs->intFlags, regs->intAddress, regs->intValue, regs->intValue
	));

	if (regs->intFlags & INT_FLAG_TICKS) {
		regs->self->onTickInt(regs->intValue);

	} else if (regs->intFlags & INT_FLAG_MEM_WR) {
		regs->self->onMemoryWriteInt(regs->intAddress, regs->intValue);

	} else if (regs->intFlags & INT_FLAG_MEM_RD) {
		regs->self->onMemoryReadInt(regs->intAddress, regs->intValue);
	}
}


void altair::JitCore::onTickInt(uint8_t ticks) {
	DBG(("%s(): CALL! ticks: %d", __func__, ticks));

	this->_pio.clk(ticks);
}


void altair::JitCore::onMemoryWriteInt(uint16_t address, uint8_t value) {
	this->_pio.memoryWrite(address, value);
}


void altair::JitCore::onMemoryReadInt(uint16_t address, uint8_t &value) {
	value = this->_pio.memoryRead(address);
}


altair::JitCore::JitCore(Pio &pio, uint16_t pc) : Core(), _pio(pio) {
	this->_regs.PC = pc;

	this->_regs.self       = this;
	this->_regs.intHandler = JitCore_onNativeInt;

	std::fill(std::begin(_opHandlers), std::begin(_opHandlers) + 0xff, nullptr);

	// MVI R, I
	// MVI M, I
	{
		this->_opAddDDD(0, 0, B, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, C, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, D, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, E, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, H, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, L, 1, 1, 0, _opMviR);
		this->_opAddDDD(0, 0, A, 1, 1, 0, _opMviR);

		this->_opAdd(0, 0, 1, 1, 0, 1, 1, 0, _opMviM);
	}

	// MOV R, R
	// MOV R, M
	// MOV M, R
	{
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

		this->_opAddDDD(0, 1, B, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, C, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, D, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, E, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, H, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, L, 1, 1, 0, _opMovRM);
		this->_opAddDDD(0, 1, A, 1, 1, 0, _opMovRM);
#if 0
		this->_opAddSSS(0, 1, 1, 1, 0, B, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, C, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, D, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, E, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, H, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, L, _opMovMR);
		this->_opAddSSS(0, 1, 1, 1, 0, A, _opMovMR);
#endif
	}
}


void altair::JitCore::turn() {
	this->execute(false);
}


void altair::JitCore::shutdown() {

}


altair::JitCore::ExecutionByteBuffer *altair::JitCore::compile(uint16_t pc, bool singleInstruction) {
	ExecutionByteBuffer *ret = new ExecutionByteBuffer();

	ret->begin();
	{
		uint8_t opcode;
		uint8_t ticks;
		bool    stop;
		uint8_t instructionSize;

		do {
			opcode = this->_pio.memoryRead(pc);

			OpHandler handler = this->_opHandlers[opcode];
			if (handler == nullptr) {
				ERR(("%s(): opcode %02x is not implemented!", __func__, opcode));

				throw std::runtime_error("Opcode " + std::to_string(opcode) +  " is not supported!");
			}

			instructionSize = handler(this, ret, opcode, pc, ticks, stop);

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

				pc += instructionSize;
			}

			// TODO: Call ticks callback there!
			this->addIntCodeCallTick(ret, ticks);
		} while (! singleInstruction && ! stop);

		ret->append(0xc3); // retq
	}
	ret->end();

	return ret;
}


void altair::JitCore::addIntCodeCall(ExecutionByteBuffer *buffer, uint8_t flag) {
	buffer->
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
		append(0x58);
}


void altair::JitCore::addIntCodeLoadIntAddrFromReg(ExecutionByteBuffer *buffer, uint8_t reg) {
	switch (reg) {
		case RegDouble::BC:
			// mov    WORD PTR [rbp+offsetof(intAddress)],bx
			buffer->append(0x66).append(0x89).append(0x5d).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::DE:
			// mov    WORD PTR [rbp+offsetof(intAddress)],cx
			buffer->append(0x66).append(0x89).append(0x4d).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::HL:
			// mov    WORD PTR [rbp+offsetof(intAddress)],dx
			buffer->append(0x66).append(0x89).append(0x55).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		case RegDouble::SP:
			// mov    WORD PTR [rbp+offsetof(intAddress)],si
			buffer->append(0x66).append(0x89).append(0x75).append(offsetof(struct altair::JitCore::Regs, intAddress));
			break;

		default:
			ERR(("%s(): Not supported double register %u!", __func__, reg));

			throw std::runtime_error("Not supported double register!");
	}
}


void altair::JitCore::addIntCodeLoadIntValueFromReg(ExecutionByteBuffer *buffer, uint8_t reg) {
	ERR(("TODO: Implement!"));
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
	this->addIntCodeLoadIntValueFromImm(buffer, ticks);
	this->addIntCodeCall(buffer, INT_FLAG_TICKS);
}


void altair::JitCore::addIntCodeCallMemoryWrite(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_MEM_WR);
}


void altair::JitCore::addIntCodeCallMemoryRead(ExecutionByteBuffer *buffer) {
	this->addIntCodeCall(buffer, INT_FLAG_MEM_RD);
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

			"callq [rbp + %[off_cs]]  \n\t"

			// Store 8080 regs
			"mov [rbp + %[off_b]],  bh \n\t"
			"mov [rbp + %[off_c]],  bl \n\t"
			"mov [rbp + %[off_d]],  ch \n\t"
			"mov [rbp + %[off_e]],  cl \n\t"
			"mov [rbp + %[off_h]],  dh \n\t"
			"mov [rbp + %[off_l]],  dl \n\t"
			"mov [rbp + %[off_a]],  al \n\t"
			"mov [rbp + %[off_sp]], si \n\t"

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
				[off_sp] "i" (offsetof (struct altair::JitCore::Regs, SP)),
				[off_cs] "i" (offsetof (struct altair::JitCore::Regs, codeSegment))
			:
		);
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
	}

	ERR(("%s(): Not supported WReg value! (%02x)", __func__, (uint8_t)reg));

	throw std::runtime_error("Not supported WReg value!");
}


void altair::JitCore::wR(WReg reg, uint16_t val) {

}


uint8_t altair::JitCore::wRL(WReg reg) {

}


uint8_t altair::JitCore::wRH(WReg reg) {

}


altair::Core::Alu *altair::JitCore::alu() const {
	return NULL;
}


void altair::JitCore::_opAdd(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler) {
	this->_opHandlers[
		(bit7 << 7) | (bit6 << 6) | (bit5 << 5) | (bit4 << 4) |
		(bit3 << 3) | (bit2 << 2) | (bit1 << 1) | (bit0 << 0)
	] = handler;
}


void altair::JitCore::_opAddSSS(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t src, OpHandler handler) {
	this->_opAdd(bit7, bit6, bit5, bit4, bit3, (src & 4) >> 2, (src & 2) >> 1, src & 1, handler);
}


void altair::JitCore::_opAddDDD(uint8_t bit7, uint8_t bit6, uint8_t registers, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler) {
	this->_opAdd(bit7, bit6, (registers & 4) >> 2, (registers & 2) >> 1, registers & 1, bit2, bit1, bit0, handler);
}


void altair::JitCore::_opAddDDDSSS(uint8_t bit7, uint8_t bit6, uint8_t dst, uint8_t src, OpHandler handler) {
	this->_opAddDDD(bit7, bit6, dst, (src & 4) >> 2, (src & 2) >> 1, src & 1, handler);
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


int altair::JitCore::_opMovMR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop) {
	ticks = 7;

	return 1;
}

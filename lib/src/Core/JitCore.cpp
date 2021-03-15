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
#include "altair/Core/JitCore.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


/*
 * Reg mapping:
 * [P][A] -> [VT][AL]
 * [B][C] -> [BH][BL]
 * [D][E] -> [CH][CL]
 * [H][L] -> [DH][DL]
 * [ SP ] -> [ESI]
 */

void _onTick(void *ctx, uint8_t ticks) {
	altair::JitCore::Regs *core = reinterpret_cast<altair::JitCore::Regs *>(ctx);

	DBG(("%s(): CALL, regs: %p, ticks: %u, regs: %p", __func__, ctx, ticks, core));

	core->self->onTick(ticks);
}
//
//static void _onTick(void *ctx, uint8_t ticks) {
//
//}


void altair::JitCore::onTick(uint8_t ticks) {
	if (ticks == 128) {
		std::abort();
	}
	DBG(("%s(): CALL for ticks: %u, regs: %p", __func__, ticks, &this->_regs));
}


altair::JitCore::JitCore(Pio &pio, uint16_t pc) : Core(), _pio(pio) {
	this->_regs.PC = pc;

	this->_regs.self = this;
}


void altair::JitCore::turn() {
	ExecutionByteBuffer *codeSegment = this->_compiledBlocks[this->_regs.PC];
	if (codeSegment == nullptr) {
		this->_compiledBlocks[this->_regs.PC] = this->compile(this->_regs.PC, false);

		codeSegment = this->_compiledBlocks[this->_regs.PC];
	}

#if 0
	"push r8                 \n\t"
	"mov r8, rax             \n\t"

	"mov al, [r8 + %[off_a]] \n\t"


	"push rax      \n\t"
	"push rbx      \n\t"
	"push rcx      \n\t"
	"push rdx      \n\t"
	"push rbp      \n\t"
	"push rsi      \n\t"
	"push r8       \n\t"

	"callq %[code] \n\t"

	"pop r8  \n\t"
	"pop rsi \n\t"
	"pop rbp \n\t"
	"pop rdx \n\t"
	"pop rcx \n\t"
	"pop rbx \n\t"
	"pop rax \n\t"

	"inc al                  \n\t"
	"inc al                  \n\t"

	"mov [r8 + %[off_a]], al \n\t"

	"pop r8                  \n\t"

#endif

	{
		Regs *regs = &this->_regs;

		regs->codeSegment = codeSegment->getPtr();

		__asm (
			"push r8                 \n\t"
			"push r9                 \n\t"
			"push rax                \n\t"

			"mov r8, rax             \n\t"

			// Load 8080 regs
			"mov al, [r8 + %[off_a]]  \n\t"

			"callq [r8 + %[off_cs]]  \n\t"

			// Store 8080 regs
			"mov [r8 + %[off_a]], al \n\t"

			"pop rax                 \n\t"
			"pop r9                  \n\t"
			"pop r8                  \n\t"
			:
			:
				"a" (regs),
				[off_a]  "i" (offsetof (struct altair::JitCore::Regs, A)),
				[off_cs] "i" (offsetof (struct altair::JitCore::Regs, codeSegment))
			:
		);
	}

#if 0

	Regs *regs = &this->_regs;

	this->_pio.setWr(false);

	this->compile(regs->PC, false);

	this->_regs.A = 1;

	this->_pio.clk();
#if 0
	r	any GPR
	a	eax, ax, al
	b	ebx, bx, bl
	c	ecx, cx, cl
	d	edx, dx, dl
	S	esi, si
	D	edi, di
#endif

	/*
	 * R8 = ctx
	 */
#if 1

#endif
#endif
}


void altair::JitCore::shutdown() {

}


altair::JitCore::ExecutionByteBuffer *altair::JitCore::compile(uint16_t pc, bool singleInstruction) {
	ExecutionByteBuffer *ret = new ExecutionByteBuffer();

	uint8_t opcode = this->_pio.memoryRead(pc);

	ERR(("%02x %08x", opcode, pc));

	ret->begin();
	{
		ret->appendByte(0xc3); // retq
	}
	ret->end();

	return ret;
}


void altair::JitCore::nexti() {

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

	throw std::runtime_error("Not supported BReg value!");
}


void altair::JitCore::bR(BReg r, uint8_t val) {

}


uint16_t altair::JitCore::wR(WReg reg) const {

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

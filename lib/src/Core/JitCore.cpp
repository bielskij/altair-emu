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


altair::JitCore::JitCore(Pio &pio, uint16_t pc) : _pio(pio) {
	this->_regs.PC = pc;
}


int altair::JitCore::turn() {
	Regs *regs = &this->_regs;

	this->_regs.A = 1;

//	this->_pio.clk();
#if 0
	r	any GPR
	a	eax, ax, al
	b	ebx, bx, bl
	c	ecx, cx, cl
	d	edx, dx, dl
	S	esi, si
	D	edi, di
#endif
	__asm (
		"push r8                 \n\t"
		"mov r8, rax             \n\t"

		"mov al, [r8 + %[off_a]] \n\t"

		"inc al                  \n\t"
		"inc al                  \n\t"

		"mov [r8 + %[off_a]], al \n\t"

		"pop r8                  \n\t"
		:
		:
			"a"(regs),
			[off_a] "i" (offsetof (struct altair::JitCore::Regs, A))
		:
	);

	ERR(("VAL: %u", this->_regs.A));
}


void altair::JitCore::shutdown() {

}

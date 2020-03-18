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

#include "cunit.h"

#include "test/Core.hpp"
#include "Core/Pio.hpp"


CUNIT_TEST(core_instruction, sbbr_clk) {
	test::Pio  pio({
		0x9d
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, sbbr_regs) {
	// mvi b,1
	// mvi c,2
	// mvi d,3
	// mvi e,4
	// mvi h,5
	// mvi l,6
	// stc
	// mvi a,16
	// sbb a
	// stc
	// mvi a,16
	// sbb b
	// stc
	// mvi a,16
	// sbb c
	// stc
	// mvi a,16
	// sbb d
	// stc
	// mvi a,16
	// sbb e
	// stc
	// mvi a,16
	// sbb h
	// stc
	// mvi a,16
	// sbb l
	test::Pio  pio({
		0x06, 0x01, 0x0E, 0x02, 0x16, 0x03, 0x1E, 0x04,
		0x26, 0x05, 0x2E, 0x06, 0x37, 0x3E, 0x10, 0x9F,
		0x37, 0x3E, 0x10, 0x98, 0x37, 0x3E, 0x10, 0x99,
		0x37, 0x3E, 0x10, 0x9A, 0x37, 0x3E, 0x10, 0x9B,
		0x37, 0x3E, 0x10, 0x9C, 0x37, 0x3E, 0x10, 0x9D
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_TRUE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xe);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x0d);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x0c);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x0b);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x0a);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x09);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}

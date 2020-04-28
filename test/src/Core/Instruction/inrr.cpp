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


CUNIT_TEST(core_instruction, inrr_clk) {
	test::Pio  pio({
		0x2c
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 5);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, inrr_regs) {
	// mvi a,0xff
	// mvi b,0xff
	// mvi c,0xff
	// mvi d,0xff
	// mvi e,0xff
	// mvi h,0xff
	// mvi l,0xff
	// inr a
	// inr b
	// inr c
	// inr d
	// inr e
	// inr h
	// inr l
	test::Pio  pio({
		0x3E, 0xFF, 0x06, 0xFF, 0x0E, 0xFF, 0x16, 0xFF,
		0x1E, 0xFF, 0x26, 0xFF, 0x2E, 0xFF, 0x3C, 0x04,
		0x0C, 0x14, 0x1C, 0x24, 0x2C,
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}

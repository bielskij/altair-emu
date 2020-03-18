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


CUNIT_TEST(core_instruction, dcrr_clk) {
	test::Pio  pio({
		0x2d
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 5);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, dcrr_regs) {
	// mvi a,0x00
	// mvi b,0x00
	// mvi c,0x00
	// mvi d,0x00
	// mvi e,0x00
	// mvi h,0x00
	// mvi l,0x00
	// dcr a
	// dcr b
	// dcr c
	// dcr d
	// dcr e
	// dcr h
	// dcr l
	test::Pio  pio({
		0x3E, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x16, 0x00,
		0x1E, 0x00, 0x26, 0x00, 0x2E, 0x00, 0x3D, 0x05,
		0x0D, 0x15, 0x1D, 0x25, 0x2D,
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0xff);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0xff);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}

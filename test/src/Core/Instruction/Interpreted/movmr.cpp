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


CUNIT_TEST(core_instruction, movmr_clk) {
	test::Pio  pio({
		0x77
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 7);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, movmr) {
	// mvi a,1
	// mvi b,2
	// mvi c,3
	// mvi d,4
	// mvi e,5
	// mvi h,0
	// mvi l,21
	// mov m,a
	// mov m,b
	// mov m,c
	// mov m,d
	// mov m,e
	// mov m,h
	// mov m,l
	test::Pio  pio({
		0x3E, 0x01, 0x06, 0x02, 0x0E, 0x03, 0x16, 0x04,
		0x1E, 0x05, 0x26, 0x00, 0x2E, 0x15, 0x77, 0x70,
		0x71, 0x72, 0x73, 0x74, 0x75, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x15);
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x02);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x03);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x04);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x05);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.mem(0x15), 0x15);

	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x15);
}

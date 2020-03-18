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


CUNIT_TEST(core_instruction, dad_clk) {
	test::Pio  pio({
		0x09
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, dad_reg) {
	// lxi h,0x0100
	// lxi b,0x0001
	// lxi d,0x0001
	// lxi sp,0x0001
	// dad h
	// dad b
	// dad d
	// dad sp
	test::Pio  pio({
		0x21, 0x00, 0x01, 0x01, 0x01, 0x00, 0x11, 0x01,
		0x00, 0x31, 0x01, 0x00, 0x29, 0x09, 0x19, 0x39
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0200);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0201);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0202);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0203);
}


CUNIT_TEST(core_instruction, dad_cy_set) {
	// lxi h,0x0100
	// lxi b,0xff10
	// dad b
	test::Pio  pio({
		0x21, 0x00, 0x01, 0x01, 0x10, 0xFF, 0x09
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();

	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0010);
	CUNIT_ASSERT_TRUE(core.alu()->fCY());
}


CUNIT_TEST(core_instruction, dad_cy_not_set) {
	// lxi h,0x01ff
	// lxi b,0x0001
	// dad b
	test::Pio  pio({
		0x21, 0xff, 0x01, 0x01, 0x01, 0x00, 0x09
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x0200);
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}

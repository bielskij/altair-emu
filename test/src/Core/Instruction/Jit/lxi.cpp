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


CUNIT_TEST(core_instruction_jit, lxi_clk) {
	test::Pio  pio({
		0x01, 0x20, 0x10
	});

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 3);
}


CUNIT_TEST(core_instruction_jit, lxi) {
	/// lxi b,0x1020
	/// lxi d,0x1020
	/// lxi h,0x1020
	/// lxi sp,0x1020
	test::Pio  pio({ 0x01, 0x20, 0x10, 0x11, 0x20, 0x10, 0x21, 0x20, 0x10, 0x31, 0x20, 0x10 });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::B),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D),  0x0000);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H),  0x0000);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0000);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::B),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H),  0x0000);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0000);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::B),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0000);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::B),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H),  0x1020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x1020);
}



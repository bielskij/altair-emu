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


CUNIT_TEST(core_instruction_jit, pushrc_clk) {
	test::Pio  pio({
		0x31, 0x08, 0x00, 0xc5, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	{
		uint16_t pc = core.wR(test::Core::WReg::PC);

		core.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 21);
		CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC) - pc, 1);
	}
}


CUNIT_TEST(core_instruction_jit, push_b) {
	// lxi sp,0x0010
	// lxi b,0x1234
	// push b
	test::Pio  pio({
		0x31, 0x10, 0x00, 0x01, 0x34, 0x12, 0xc5, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000e);
	CUNIT_ASSERT_EQ(pio.mem(0x000e), 0x34);
	CUNIT_ASSERT_EQ(pio.mem(0x000f), 0x12);
}


CUNIT_TEST(core_instruction_jit, push_d) {
	// lxi sp,0x0010
	// lxi d,0x1234
	// push d
	test::Pio  pio({
		0x31, 0x10, 0x00, 0x11, 0x34, 0x12, 0xd5, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000e);
	CUNIT_ASSERT_EQ(pio.mem(0x000e), 0x34);
	CUNIT_ASSERT_EQ(pio.mem(0x000f), 0x12);
}


CUNIT_TEST(core_instruction_jit, push_h) {
	// lxi sp,0x0010
	// lxi h,0x1234
	// push h
	test::Pio  pio({
		0x31, 0x10, 0x00, 0x21, 0x34, 0x12, 0xe5, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000e);
	CUNIT_ASSERT_EQ(pio.mem(0x000e), 0x34);
	CUNIT_ASSERT_EQ(pio.mem(0x000f), 0x12);
}

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


CUNIT_TEST(core_instruction_jit, poprc_clk) {
	test::Pio  pio({
		0x31, 0x06, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 20);
}


CUNIT_TEST(core_instruction_jit, pop_b) {
	// lxi sp,0x0006
	// pop b
	test::Pio  pio({
		0x31, 0x06, 0x00, 0xc1, 0x00, 0x00, 0x34, 0x12
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0008);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::B), 0x1234);
}


CUNIT_TEST(core_instruction_jit, pop_d) {
	// lxi sp,0x0006
	// pop d
	test::Pio  pio({
		0x31, 0x06, 0x00, 0xd1, 0x00, 0x00, 0x34, 0x12
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0008);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D), 0x1234);
}


CUNIT_TEST(core_instruction_jit, pop_h) {
	// lxi sp,0x0006
	// pop h
	test::Pio  pio({
		0x31, 0x06, 0x00, 0xe1, 0x00, 0x00, 0x34, 0x12
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0008);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::H), 0x1234);
}


CUNIT_TEST(core_instruction_jit, push_pop) {
	// lxi sp,0x000f
	// lxi h, 0x1234
	// push h
	// pop d
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0x21, 0x34, 0x12, 0xE5, 0xD1,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio, true);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();

	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000f);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::D), 0x1234);
}

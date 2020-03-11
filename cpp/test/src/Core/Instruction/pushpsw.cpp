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


CUNIT_TEST(core_instruction, pushpsw_clk) {
	test::Pio  pio({
		0x31, 0x08, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00,
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 21);
}


CUNIT_TEST(core_instruction, pushpsw_reg) {
	// lxi sp,0x0010
	// mvi a,0xa5
	// stc
	// push psw
	test::Pio  pio({
		0x31, 0x10, 0x00, 0x3e, 0xa5, 0x37, 0xf5, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000e);
	CUNIT_ASSERT_EQ(pio.mem(0x000e), 0x03);
	CUNIT_ASSERT_EQ(pio.mem(0x000f), 0xa5);
}

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


CUNIT_TEST(core_instruction, xchg_clk) {
	test::Pio  pio({
		0xEB
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
}


CUNIT_TEST(core_instruction, xchg) {
	// mvi h,1
	// mvi l,2
	// mvi d,3
	// mvi e,4
	// xchg
	test::Pio  pio({
		0x26, 0x01, 0x2E, 0x02, 0x16, 0x03, 0x1E, 0x04, 0xEB
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 1);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 2);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 3);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 4);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 3);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 4);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 1);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 2);
}

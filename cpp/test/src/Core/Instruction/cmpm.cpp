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


CUNIT_TEST(core_instruction, cmpm_clk) {
	test::Pio  pio({
		0xbe
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 7);
}


CUNIT_TEST(core_instruction, cmpm_regs) {
	// mvi l,0x07
	// mvi a,0xa5
	// cmp m
	test::Pio  pio({
		0x2E, 0x07, 0x3E, 0xA5, 0xbe, 0x00, 0x00, 0xa5
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xa5);
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_TRUE(core.alu()->fZ());
}

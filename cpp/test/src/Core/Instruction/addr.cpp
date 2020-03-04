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


CUNIT_TEST(core_instruction, addr_clk) {
	test::Pio  pio({
		0x85
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
}


CUNIT_TEST(core_instruction, addr_regs) {
	// mvi b,1
	// mvi c,2
	// mvi d,3
	// mvi e,4
	// mvi h,5
	// mvi l,6
	// mvi a,1
	// add a
	// mvi a,1
	// add b
	// mvi a,1
	// add c
	// mvi a,1
	// add d
	// mvi a,1
	// add e
	// mvi a,1
	// add h
	// mvi a,1
	// add l
	test::Pio  pio({
		0x06, 0x01, 0x0E, 0x02, 0x16, 0x03, 0x1E, 0x04,
		0x26, 0x05, 0x2E, 0x06, 0x3E, 0x01, 0x87, 0x3E,
		0x01, 0x80, 0x3E, 0x01, 0x81, 0x3E, 0x01, 0x82,
		0x3E, 0x01, 0x83, 0x3E, 0x01, 0x84, 0x3E, 0x01,
		0x85
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x02);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x02);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x03);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x04);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x05);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x06);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x07);
}

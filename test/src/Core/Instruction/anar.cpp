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


CUNIT_TEST(core_instruction, anar_clk) {
	test::Pio  pio({
		0xa5
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, anar_regs) {
	// mvi a,0xff
	// mvi b,0xfe
	// mvi c,0xfd
	// mvi d,0xfc
	// mvi e,0xfb
	// mvi h,0xfa
	// mvi l,0xf9
	// ana a
	// ana b
	// ana c
	// ana d
	// ana e
	// ana h
	// ana l
	test::Pio  pio({
		0x3E, 0xFF, 0x06, 0xFE, 0x0E, 0xFD, 0x16, 0xFC,
		0x1E, 0xFB, 0x26, 0xFA, 0x2E, 0xF9, 0xA7, 0xA0,
		0xA1, 0xA2, 0xA3, 0xA4, 0xA5,
		0x00
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
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xff);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xfe);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xfc);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xfc);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xf8);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xf8);

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xf8);
}

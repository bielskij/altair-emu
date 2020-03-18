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


CUNIT_TEST(core_instruction, movrm_clk) {
	test::Pio  pio({
		0x7E
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 7);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction, movrm) {
	// mov a,m
	// mov b,m
	// mov c,m
	// mov d,m
	// mov e,m
	// mov h,m
	// mov l,m
	test::Pio  pio({ 0x7E, 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E });

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x7e);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);
}

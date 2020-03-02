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


CUNIT_TEST(core_instruction, mvir) {
	// mov a,1
	// mov b,2
	// mov c,3
	// mov d,4
	// mov e,5
	// mov h,6
	// mov l,7
	test::Pio  pio({ 0x3E, 0x01, 0x06, 0x02, 0x0E, 0x03, 0x16, 0x04, 0x1E, 0x05, 0x26, 0x06, 0x2E, 0x07 });

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x06);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x02);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x03);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x04);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x06);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x07);
}



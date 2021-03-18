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


CUNIT_TEST(core_instruction_jit, movrr_clk) {
	test::Pio  pio({
		0x47
	});

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 5);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
}


CUNIT_TEST(core_instruction_jit, movrr_x_a) {
	// mov a,1
	// mov b,a
	// mov c,a
	// mov d,a
	// mov e,a
	// mov h,a
	// mov l,a
	test::Pio  pio({ 0x3E, 0x01, 0x47, 0x4F, 0x57, 0x5F, 0x67, 0x6F });

	test::Core core(pio, true);

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
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_b) {
	// mov b,1
	// mov a,b
	// mov c,b
	// mov d,b
	// mov e,b
	// mov h,b
	// mov l,b
	test::Pio  pio({ 0x06, 0x01, 0x78, 0x48, 0x50, 0x58, 0x60, 0x68 });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_c) {
	// mov c,1
	// mov a,c
	// mov b,c
	// mov d,c
	// mov e,c
	// mov h,c
	// mov l,c
	test::Pio  pio({ 0x0E, 0x01, 0x79, 0x41, 0x51, 0x59, 0x61, 0x69 });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_d) {
	// mov d,1
	// mov a,d
	// mov c,d
	// mov d,d
	// mov e,d
	// mov h,d
	// mov l,d
	test::Pio  pio({ 0x16, 0x01, 0x7A, 0x42, 0x4A, 0x5A, 0x62, 0x6A });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_e) {
	// mov e,1
	// mov a,e
	// mov b,e
	// mov c,e
	// mov d,e
	// mov h,e
	// mov l,e
	test::Pio  pio({ 0x1E, 0x01, 0x7B, 0x43, 0x4B, 0x53, 0x63, 0x6B });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_h) {
	// mov h,1
	// mov a,h
	// mov b,h
	// mov c,h
	// mov d,h
	// mov e,h
	// mov l,h
	test::Pio  pio({ 0x26, 0x01, 0x7C, 0x44, 0x4C, 0x54, 0x5C, 0x6C });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x00);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}


CUNIT_TEST(core_instruction_jit, movrr_x_l) {
	// mov l,1
	// mov a,l
	// mov b,l
	// mov c,l
	// mov d,l
	// mov e,l
	// mov h,l
	test::Pio  pio({ 0x2E, 0x01, 0x7D, 0x45, 0x4D, 0x55, 0x5D, 0x65 });

	test::Core core(pio, true);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x00);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x01);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x01);
}

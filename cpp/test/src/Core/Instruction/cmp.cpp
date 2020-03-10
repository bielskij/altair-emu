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


CUNIT_TEST(core_instruction, cmp_clk) {
	test::Pio  pio({
		0xbd
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
}


CUNIT_TEST(core_instruction, cmp_regs) {
	// mvi a,0x10
	// mvi b,0x10
	// mvi c,0x20
	// mvi d,0x05
	// mvi e,0x10
	// mvi h,0x20
	// mvi l,0x05
	// cmp a
	// cmp b
	// cmp c
	// cmp d
	// cmp e
	// cmp h
	// cmp l
	test::Pio  pio({
		0x3E, 0x10, 0x06, 0x10, 0x0E, 0x20, 0x16, 0x05,
		0x1E, 0x10, 0x26, 0x20, 0x2E, 0x05, 0xBF, 0xB8,
		0xB9, 0xBA, 0xBB, 0xBC, 0xBD,
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
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_TRUE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_TRUE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_TRUE(core.alu()->fS());
	CUNIT_ASSERT_FALSE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_TRUE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_FALSE(core.alu()->fZ());
	CUNIT_ASSERT_FALSE(core.alu()->fAC());
	CUNIT_ASSERT_FALSE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_TRUE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_TRUE(core.alu()->fS());
	CUNIT_ASSERT_FALSE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_TRUE(core.alu()->fP());
	CUNIT_ASSERT_TRUE(core.alu()->fCY());

	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::B), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::C), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::D), 0x05);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::E), 0x10);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::H), 0x20);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::L), 0x05);
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_FALSE(core.alu()->fZ());
	CUNIT_ASSERT_FALSE(core.alu()->fAC());
	CUNIT_ASSERT_FALSE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}


CUNIT_TEST(core_instruction, cmp_carry) {
	// ; cmp = 1 - 0
	// mvi a,1
	// cmp b
	test::Pio  pio({
		0x3E, 0x01, 0xb8
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.tick();
	core.tick();
	CUNIT_ASSERT_FALSE(core.alu()->fS());
	CUNIT_ASSERT_FALSE(core.alu()->fZ());
	CUNIT_ASSERT_TRUE(core.alu()->fAC());
	CUNIT_ASSERT_FALSE(core.alu()->fP());
	CUNIT_ASSERT_FALSE(core.alu()->fCY());
}

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
#include "test/Context.hpp"
#include "test/asm8080.hpp"

#include "Core/Pio.hpp"


using namespace asm8080;


CUNIT_TEST(core_instruction, daa_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().daa().toBin());

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 4);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 1);
	}
}


CUNIT_TEST(core_instruction, daa_val_flags) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 0x79).
			mvi(B, 0x35).
			add(B).
			daa().

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ   (c.bR(test::Core::BReg::A), 0x14);
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
		CUNIT_ASSERT_TRUE (c.alu()->fCY());
	}
}


CUNIT_TEST(core_instruction, daa_val) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 0x55).
			daa().

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ   (c.bR(test::Core::BReg::A), 0x55);
		CUNIT_ASSERT_FALSE(c.alu()->fAC());
		CUNIT_ASSERT_FALSE(c.alu()->fCY());
	}
}


CUNIT_TEST(core_instruction, daa_val_2) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 0x88).
			add(A).
			daa().

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ  (c.bR(test::Core::BReg::A), 0x76);
		CUNIT_ASSERT_TRUE(c.alu()->fCY());
		CUNIT_ASSERT_TRUE(c.alu()->fAC());
	}
}

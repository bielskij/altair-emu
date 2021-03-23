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


CUNIT_TEST(core_instruction, adi_clk) {
	test::Pio pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(
			Compiler()
				.adi(Imm8(0x10))
				.toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 7);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 2);
	}
}


CUNIT_TEST(core_instruction, adi_regs) {
	test::Pio pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		// adi 0x10
		pio.setProgram(
			Compiler()
				.adi(Imm8(0x10))
				.toBin()
		);

		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x10);
	}
}


CUNIT_TEST(core_instruction, adi_cy_p_ac) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(
			Compiler()
				.mvi(A, Imm8(0xae))
				.adi(Imm8(0x74))

				.toBin()
		);

		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
		CUNIT_ASSERT_FALSE(c.alu()->fS());
		CUNIT_ASSERT_FALSE(c.alu()->fAC());

		c.nextInstruction();
		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x22);
		CUNIT_ASSERT_TRUE (c.alu()->fCY());
		CUNIT_ASSERT_TRUE (c.alu()->fP());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
		CUNIT_ASSERT_FALSE(c.alu()->fS());
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
	}
}


CUNIT_TEST(core_instruction, adi_ac_s) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(
			Compiler()
				.mvi(A, Imm8(0x2e))
				.adi(Imm8(0x74))

				.toBin()
		);

		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
		CUNIT_ASSERT_FALSE(c.alu()->fS());
		CUNIT_ASSERT_FALSE(c.alu()->fAC());

		c.nextInstruction();
		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0xa2);
		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
		CUNIT_ASSERT_TRUE (c.alu()->fS());
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
	}
}


CUNIT_TEST(core_instruction, adi_cy_p_z_ac) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(
			Compiler()
				.mvi(A, Imm8(0xa7))
				.adi(Imm8(0x59))

				.toBin()
		);

		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
		CUNIT_ASSERT_FALSE(c.alu()->fS());
		CUNIT_ASSERT_FALSE(c.alu()->fAC());

		c.nextInstruction();
		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0);
		CUNIT_ASSERT_TRUE (c.alu()->fCY());
		CUNIT_ASSERT_TRUE (c.alu()->fP());
		CUNIT_ASSERT_TRUE (c.alu()->fZ());
		CUNIT_ASSERT_FALSE(c.alu()->fS());
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
	}
}

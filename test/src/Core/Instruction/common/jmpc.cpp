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


CUNIT_TEST(core_instruction, jmpc_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			jc(0x20).
			jz(0x20).
			jm(0x20).
			jpe(0x20).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 10);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 3);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 20);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 6);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 30);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 9);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 40);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 12);
	}
}


CUNIT_TEST(core_instruction, jmpc_c) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			jc(0x20).
			jnc(0x20).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 3);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x20);
	}
}


CUNIT_TEST(core_instruction, jmpc_n) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			jz(0x20).
			jnz(0x20).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 3);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x20);
	}
}


CUNIT_TEST(core_instruction, jmpc_s) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			jm(0x20).
			jp(0x20).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 3);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x20);
	}
}


CUNIT_TEST(core_instruction, jmpc_p) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			jpe(0x20).
			jpo(0x20).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 3);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x20);
	}
}

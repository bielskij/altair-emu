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


CUNIT_TEST(core_instruction, callc_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			cc(0x07).
			cnc(0x07).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 11);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 28);
	}
}


CUNIT_TEST(core_instruction, callc_c) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			lxi(SP, 0x000f).
			cc(0x1234).
			cnc(0x1234).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x0006);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000f);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x1234);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000d);
	}
}


CUNIT_TEST(core_instruction, callc_n) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			lxi(SP, 0x000f).
			cz(0x1234).
			cnz(0x1234).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x0006);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000f);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x1234);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000d);
	}
}


CUNIT_TEST(core_instruction, callc_s) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			lxi(SP, 0x000f).
			cm(0x1234).
			cp(0x1234).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x0006);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000f);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x1234);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000d);
	}
}


CUNIT_TEST(core_instruction, callc_p) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			lxi(SP, 0x000f).
			cpe(0x1234).
			cpo(0x1234).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x0006);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000f);
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 0x1234);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::SP), 0x000d);
	}
}

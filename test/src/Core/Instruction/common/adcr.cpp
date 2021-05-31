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


CUNIT_TEST(core_instruction, adcr_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			adc(A).

			toBin()
		);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 4);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 1);
	}
}


CUNIT_TEST(core_instruction, adcr_regs) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(B, 1).
			mvi(C, 2).
			mvi(D, 3).
			mvi(E, 4).
			mvi(H, 5).
			mvi(L, 6).

			stc().
			mvi(A, 1).
			adc(A).

			stc().
			mvi(A, 1).
			adc(B).

			stc().
			mvi(A, 1).
			adc(C).

			stc().
			mvi(A, 1).
			adc(D).

			stc().
			mvi(A, 1).
			adc(E).

			stc().
			mvi(A, 1).
			adc(H).

			stc().
			mvi(A, 1).
			adc(L).

			toBin()
		);

		core.nextInstruction();
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
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x03);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x03);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x04);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x05);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x06);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x07);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());

		core.nextInstruction();
		core.nextInstruction();
		core.nextInstruction();
		core.tick();
		core.tick();
		CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0x08);
		CUNIT_ASSERT_FALSE(core.alu()->fCY());
	}
}


CUNIT_TEST(core_instruction, adcr_flags) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 0x7f).
			mvi(B, 0x01).
			adc(B).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x80);
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_TRUE (c.alu()->fS());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
	}
}


CUNIT_TEST(core_instruction, adcr_flags_2) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 0x3d).
			mvi(B, 0x42).
			stc().
			adc(B).

			toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.tick();
		c.tick();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x80);
		CUNIT_ASSERT_TRUE (c.alu()->fAC());
		CUNIT_ASSERT_FALSE(c.alu()->fCY());
		CUNIT_ASSERT_FALSE(c.alu()->fP());
		CUNIT_ASSERT_TRUE (c.alu()->fS());
		CUNIT_ASSERT_FALSE(c.alu()->fZ());
	}
}
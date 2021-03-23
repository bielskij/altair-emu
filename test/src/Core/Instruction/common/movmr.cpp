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


CUNIT_TEST(core_instruction, movmr_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().mov(M, A).toBin());

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 7);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 1);
	}
}


CUNIT_TEST(core_instruction, movmr) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().
			mvi(A, 1).
			mvi(B, 2).
			mvi(C, 3).
			mvi(D, 4).
			mvi(E, 5).
			mvi(H, 0).
			mvi(L, 0x80).

			mov(M, A).
			mov(M, B).
			mov(M, C).
			mov(M, D).
			mov(M, E).
			mov(M, H).
			mov(M, L).

			mov(M, A).toBin()
		);

		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		c.nextInstruction();
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x01);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::B), 0x02);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::C), 0x03);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::D), 0x04);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::E), 0x05);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::H), 0x00);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::L), 0x80);
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x00);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x01);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x02);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x03);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x04);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x05);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x00);

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.mem(0x80), 0x80);

		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::A), 0x01);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::B), 0x02);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::C), 0x03);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::D), 0x04);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::E), 0x05);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::H), 0x00);
		CUNIT_ASSERT_EQ(c.bR(test::Core::BReg::L), 0x80);
	}
}

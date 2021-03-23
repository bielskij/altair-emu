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


CUNIT_TEST(core_instruction, movrr_clk) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &core : cores) {
		test::Core c(core.get());

		pio.setProgram(Compiler().mov(A, B).toBin());

		c.nextInstruction();
		CUNIT_ASSERT_EQ(pio.clkCount, 5);
		CUNIT_ASSERT_EQ(c.wR(test::Core::WReg::PC), 1);
	}
}


CUNIT_TEST(core_instruction, movrr_x_a) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(A, 1).
			mov(B, A).
			mov(C, A).
			mov(D, A).
			mov(E, A).
			mov(H, A).
			mov(L, A).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_b) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(B, 1).
			mov(A, B).
			mov(C, B).
			mov(D, B).
			mov(E, B).
			mov(H, B).
			mov(L, B).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_c) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(C, 1).
			mov(A, C).
			mov(B, C).
			mov(D, C).
			mov(E, C).
			mov(H, C).
			mov(L, C).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_d) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(D, 1).
			mov(A, D).
			mov(B, D).
			mov(C, D).
			mov(E, D).
			mov(H, D).
			mov(L, D).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_e) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(E, 1).
			mov(A, E).
			mov(B, E).
			mov(C, E).
			mov(D, E).
			mov(H, E).
			mov(L, E).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_h) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(H, 1).
			mov(A, H).
			mov(B, H).
			mov(C, H).
			mov(D, H).
			mov(E, H).
			mov(L, H).

			toBin()
		);

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
}


CUNIT_TEST(core_instruction, movrr_x_l) {
	test::Pio  pio;

	auto cores = getCores(pio);

	for (auto &_core : cores) {
		test::Core core(_core.get());

		pio.setProgram(Compiler().
			mvi(L, 1).
			mov(A, L).
			mov(B, L).
			mov(C, L).
			mov(D, L).
			mov(E, L).
			mov(H, L).

			toBin()
		);

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
}

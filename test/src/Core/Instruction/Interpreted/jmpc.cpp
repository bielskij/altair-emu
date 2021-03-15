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


CUNIT_TEST(core_instruction, jmpc_clk) {
	test::Pio  pio({
		0xda, 0x07, 0x00, 0xd2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 3);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 20);
}


CUNIT_TEST(core_instruction, jmpc_c) {
	// jc 0x0007
	// jnc 0x0007
	test::Pio  pio({
		0xda, 0x07, 0x00, 0xd2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0003);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0007);
}


CUNIT_TEST(core_instruction, jmpc_n) {
	// jz 0x0007
	// jnz 0x0007
	test::Pio  pio({
		0xca, 0x07, 0x00, 0xc2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0003);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0007);
}


CUNIT_TEST(core_instruction, jmpc_s) {
	// jm 0x0007
	// jp 0x0007
	test::Pio  pio({
		0xfa, 0x07, 0x00, 0xf2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0003);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0007);
}


CUNIT_TEST(core_instruction, jmpc_p) {
	// jpe 0x0007
	// jpo 0x0007
	test::Pio  pio({
		0xea, 0x07, 0x00, 0xe2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0003);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0007);
}

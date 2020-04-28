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


CUNIT_TEST(core_instruction, rst_clk) {
	test::Pio  pio({
		0x31, 0x08, 0x00, 0xcf, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 21);
}

CUNIT_TEST(core_instruction, rst_0) {
	// nop
	// lxi sp,0x0008
	// rst 0
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xc7, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0000);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}


CUNIT_TEST(core_instruction, rst_1) {
	// nop
	// lxi sp,0x0008
	// rst 1
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xcf, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0008);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_2) {
	// nop
	// lxi sp,0x0008
	// rst 2
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xd7, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0010);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_3) {
	// nop
	// lxi sp,0x0008
	// rst 3
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xdf, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0018);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_4) {
	// nop
	// lxi sp,0x0008
	// rst 0
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xe7, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0020);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_5) {
	// nop
	// lxi sp,0x0008
	// rst 5
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xef, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0028);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_6) {
	// nop
	// lxi sp,0x0008
	// rst 6
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xf7, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0030);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

CUNIT_TEST(core_instruction, rst_7) {
	// nop
	// lxi sp,0x0008
	// rst 7
	test::Pio  pio({
		0x00, 0x31, 0x08, 0x00, 0xff, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0038);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0006);
	CUNIT_ASSERT_EQ(pio.mem(0x0006), 0x05);
	CUNIT_ASSERT_EQ(pio.mem(0x0007), 0x00);
}

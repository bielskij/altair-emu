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


CUNIT_TEST(core_instruction, callc_clk) {
	test::Pio  pio({
		0xda, 0x07, 0x00, 0xd2, 0x07, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 5);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(pio.clkCount, 16);
}


CUNIT_TEST(core_instruction, callc_c) {
	// lxi sp,0x000f
	// cc 0x1234
	// cnc 0x1234
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0xdc, 0x34, 0x12, 0xd4, 0x34,
		0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0006);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000f);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x1234);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000d);
}


CUNIT_TEST(core_instruction, callc_n) {
	// lxi sp,0x000f
	// cz 0x1234
	// cnz 0x1234
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0xcc, 0x34, 0x12, 0xc4, 0x34,
		0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0006);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000f);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x1234);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000d);
}


CUNIT_TEST(core_instruction, callc_s) {
	// lxi sp,0x000f
	// cm 0x1234
	// cp 0x1234
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0xfc, 0x34, 0x12, 0xf4, 0x34,
		0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0006);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000f);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x1234);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000d);
}


CUNIT_TEST(core_instruction, callc_p) {
	// lxi sp,0x000f
	// cpe 0x1234
	// cpo 0x1234
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0xec, 0x34, 0x12, 0xe4, 0x34,
		0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	core.nextInstruction();
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x0006);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000f);
	core.nextInstruction();
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0x1234);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000d);
}

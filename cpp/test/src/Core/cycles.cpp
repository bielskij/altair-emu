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
#include "Pio.hpp"


CUNIT_TEST(core_cycles, fetch) {
	test::Pio  pio({ 0x5f }); // MOV r,r
	test::Core core(pio);

	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0000);
	CUNIT_ASSERT_EQ(pio.data,    0xA0); // Fetch cycle
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 1);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 0);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_TRUE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 2);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 3);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::IR), 0x5f);
}


CUNIT_TEST(core_cycles, read_memory) {
	test::Pio  pio({ 0x3e, 0xa5 }); // MVI A,0xa5
	test::Core core(pio);

	// fetch cycle
	core.nextCycle();
	CUNIT_ASSERT_EQ(pio.clkCount, 4);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);

	// read memory cycle
	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0001);
	CUNIT_ASSERT_EQ(pio.data,    0x80);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 5);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_TRUE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 6);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 2);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 7);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::A), 0xa5);
}

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


CUNIT_TEST(core_cycles, write_memory) {
	test::Pio  pio({ 0x77 }); // MOV m,a
	test::Core core(pio);

	// fetch cycle
	core.nextCycle();

	// write memory cycle
	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0000);
	CUNIT_ASSERT_EQ(pio.data,    0x02);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 1);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_TRUE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.getData(), 0);
}


CUNIT_TEST(core_cycles, write_stack) {
	// lxi sp,0x000f
	// call 0x1234
	test::Pio  pio({
		0x31, 0x0f, 0x00, 0xcd, 0x34, 0x12, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	});

	test::Core core(pio);

	// lxi
	core.nextInstruction();

	// call
	core.nextCycle(); // Fetch
	core.nextCycle(); // Memory read
	core.nextCycle(); // Memory read

	// Stack write
	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x000e);
	CUNIT_ASSERT_EQ(pio.data,    0x06);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 22);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 23);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_TRUE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 24);
	CUNIT_ASSERT_EQ(pio.getData(), 0);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x000d);
}


CUNIT_TEST(core_cycles, read_stack) {
	// lxi sp,0x000f
	// ret
	test::Pio pio({
		0x31, 0x0f, 0x00, 0xc9
	});

	test::Core core(pio);

	// lxi
	core.nextInstruction();

	core.nextCycle(); // Fetch

	// stack read
	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x000f);
	CUNIT_ASSERT_EQ(pio.data,    0x84);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 15);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 4);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_TRUE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 16);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 4);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 17);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::SP), 0x0010);
}


CUNIT_TEST(core_cycles, input_read) {
	test::Pio pio({
		0xdb
	});

	test::Core core(pio);

	core.nextCycle(); // fetch
	core.nextCycle(); // memory read

	// input read
	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0000);
	CUNIT_ASSERT_EQ(pio.data,    0x40);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 8);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_TRUE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 9);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
}


CUNIT_TEST(core_cycles, output_write) {
	test::Pio pio({
		0xd3
	});

	test::Core core(pio);

	core.nextCycle(); // fetch
	core.nextCycle(); // memory read

	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0000);
	CUNIT_ASSERT_EQ(pio.data,    0x12);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 8);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 9);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_TRUE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
	CUNIT_ASSERT_EQ(pio.getData(), 0);
}


CUNIT_TEST(core_cycles, interrupt) {
	test::Pio pio({
		0xfb, 0x00
	});

	test::Core core(pio);

	CUNIT_ASSERT_FALSE(pio.inte);
	core.nextInstruction();
	CUNIT_ASSERT_TRUE(pio.inte);

	core.tick();
	pio.setInt(true);
	core.nextInstruction();
	pio.setInt(false);
	CUNIT_ASSERT_EQ(pio.clkCount, 8);
	CUNIT_ASSERT_TRUE(pio.inte);

	core.tick();
	CUNIT_ASSERT_EQ(pio.address, 0x0002);
	CUNIT_ASSERT_EQ(pio.data,    0x21);
	CUNIT_ASSERT_TRUE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_FALSE(pio.inte);
	CUNIT_ASSERT_EQ(pio.clkCount, 9);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 2);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_TRUE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_FALSE(pio.inte);
	CUNIT_ASSERT_EQ(pio.clkCount, 10);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 2);

	// RST instruction
	pio.setData(0xff);

	core.tick();
	CUNIT_ASSERT_FALSE(pio.sync);
	CUNIT_ASSERT_FALSE(pio.dbin);
	CUNIT_ASSERT_FALSE(pio.wr);
	CUNIT_ASSERT_FALSE(pio.wait);
	CUNIT_ASSERT_FALSE(pio.inte);
	CUNIT_ASSERT_EQ(pio.clkCount, 11);
	CUNIT_ASSERT_EQ(core.wR(test::Core::WReg::PC), 2);
	CUNIT_ASSERT_EQ(core.bR(test::Core::BReg::IR), 0xff);
}

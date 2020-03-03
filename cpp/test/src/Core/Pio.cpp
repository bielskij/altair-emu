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

#include "Pio.hpp"

#define DEBUG_LEVEL 1
#include "common/debug.h"

bool test::Pio::getInt() {
	DBG(("CALL"));
	return false;
}

bool test::Pio::getHold() {
	DBG(("CALL"));
	return false;
}


bool test::Pio::getReady() {
	DBG(("CALL"));
	return true;
}


bool test::Pio::getReset() {
	DBG(("CALL"));
	return false;
}


void test::Pio::setWr(bool active) {
	DBG(("CALL %d", active));

	this->wr = active;
}


void test::Pio::setDbin(bool active) {
	DBG(("CALL"));

	this->dbin = active;
}


void test::Pio::setInte(bool active) {
	DBG(("CALL"));
}


void test::Pio::setHoldAck(bool active) {
	DBG(("CALL"));
}


void test::Pio::setWait(bool active) {
	DBG(("CALL"));

	this->wait = active;
}


void test::Pio::setSync(bool active) {
	DBG(("CALL"));

	this->sync = active;
}


uint8_t test::Pio::getData() {
	DBG(("CALL"));

	return this->data;
}


void test::Pio::setData(uint8_t val) {
	DBG(("CALL"));

	this->data = val;
}


void test::Pio::setAddress(uint16_t val) {
	DBG(("CALL"));

	this->address = val;
}


void test::Pio::clk() {
	DBG(("CALL"));

	if (this->dbin) {
		this->data = this->program[this->address];
	}

	if (this->wr) {
		this->program[this->address] = this->data;
	}

	this->clkCount++;
}


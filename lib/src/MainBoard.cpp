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

#include "altair/MainBoard.hpp"


altair::MainBoard::MainBoard() {
	this->_a  = 0;
	this->_di = 0;
	this->_do = 0;
	this->_vi = 0;

	this->_mwrt   = false;

	this->_sm1    = false;
	this->_sout   = false;
	this->_sinp   = false;
	this->_sinta  = false;
	this->_swo    = false;
	this->_sstack = false;
	this->_smemr  = false;
	this->_shlta  = false;

	this->_pdbin  = false;
	this->_psync  = false;
	this->_pwr    = false;
	this->_pint   = false;
	this->_pinte  = false;
}

void altair::MainBoard::clk() {
	// Apply pull ups
	this->din(0xff);
	this->vi(0xff);

	for (auto c : _connectors) {
		c->onClk();
	}
}

void altair::MainBoard::onClk() {
	// Nothing to do
}

uint8_t altair::MainBoard::vi() const {
	return this->_vi;
}

void altair::MainBoard::vi(uint8_t val) {
	this->_vi = val;
}

uint16_t altair::MainBoard::a() const {
	return this->_a;
}

void altair::MainBoard::a(uint16_t address) {
	this->_a = address;
}

uint8_t altair::MainBoard::dout() const {
	return this->_do;
}

void altair::MainBoard::dout(uint8_t data) {
	this->_do = data;
}

uint8_t altair::MainBoard::din() const {
	return this->_di;
}

void altair::MainBoard::din(uint8_t data) {
	this->_di = data;
}

bool altair::MainBoard::mwrt() const {
	return this->_mwrt;
}

void altair::MainBoard::mwrt(bool val) {
	this->_mwrt = val;
}

bool altair::MainBoard::sm1() const {
	return this->_sm1;
}

void altair::MainBoard::sm1(bool val) {
	this->_sm1 = val;
}

bool altair::MainBoard::sout() const {
	return this->_sout;
}

void altair::MainBoard::sout(bool val) {
	this->_sout = val;
}

bool altair::MainBoard::sinp() const {
	return this->_sinp;
}

void altair::MainBoard::sinp(bool val) {
	this->_sinp = val;
}

bool altair::MainBoard::sinta() const {
	return this->_sinta;
}

void altair::MainBoard::sinta(bool val) {
	this->_sinta = val;
}

bool altair::MainBoard::swo() const {
	return this->_swo;
}

void altair::MainBoard::swo(bool val) {
	this->_swo = val;
}

bool altair::MainBoard::sstack() const {
	return this->_sstack;
}

void altair::MainBoard::sstack(bool val) {
	this->_sstack = val;
}

bool altair::MainBoard::smemr() const {
	return this->_smemr;
}

void altair::MainBoard::smemr(bool val) {
	this->_smemr = val;
}

bool altair::MainBoard::shlta() const {
	return this->_shlta;
}

void altair::MainBoard::shlta(bool val) {
	this->_shlta = val;
}

bool altair::MainBoard::pdbin() const {
	return this->_pdbin;
}

void altair::MainBoard::pdbin(bool val) {
	this->_pdbin = val;
}

bool altair::MainBoard::pwr() const {
	return this->_pwr;
}

void altair::MainBoard::pwr(bool val) {
	this->_pwr = val;
}

bool altair::MainBoard::psync() const {
	return this->_psync;
}

void altair::MainBoard::psync(bool val) {
	this->_psync = val;
}

bool altair::MainBoard::pint() const {
	return this->_pint;
}

void altair::MainBoard::pint(bool val) {
	this->_pint = val;
}

bool altair::MainBoard::pinte() const {
	return this->_pinte;
}
void altair::MainBoard::pinte(bool val) {
	this->_pinte = val;
}


bool altair::MainBoard::memoryRead(uint16_t address, uint8_t &value) {
	for (auto c : _connectors) {
		if (c->onMemoryRead(address, value)) {
			return true;
		}
	}

	return false;
}


void altair::MainBoard::memoryWrite(uint16_t address, uint8_t value) {
	for (auto c : _connectors) {
		c->onMemoryWrite(address, value);
	}
}


bool altair::MainBoard::ioIn(uint8_t number, uint8_t &value) {
	for (auto c : _connectors) {
		if (c->onIn(number, value)) {
			return true;
		}
	}

	return false;
}


void altair::MainBoard::ioOut(uint8_t number, uint8_t data) {
	for (auto c : _connectors) {
		c->onOut(number, data);
	}
}


void altair::MainBoard::clk(uint8_t ticks) {
	for (auto c : _connectors) {
		c->onClk(ticks);
	}
}

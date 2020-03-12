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

#include "altair/Card/Cpu.hpp"

bool altair::card::Cpu::PioImpl::getReset() {
#warning "TODO implement"
	return false;
}

bool altair::card::Cpu::PioImpl::getHold() {
#warning "TODO implement"
	return false;
}

bool altair::card::Cpu::PioImpl::getInt() {
#warning "TODO implement"
	return false;
}

bool altair::card::Cpu::PioImpl::getReady() {
#warning "TODO implement"
	return true;
}

void altair::card::Cpu::PioImpl::setInte(bool active) {

}

void altair::card::Cpu::PioImpl::setDbin(bool active) {
	this->conn->pdbin(active);
}

void altair::card::Cpu::PioImpl::setWr(bool active) {
	this->conn->pwr(active);
}

void altair::card::Cpu::PioImpl::setSync(bool active) {

}

void altair::card::Cpu::PioImpl::setHoldAck(bool active) {

}

void altair::card::Cpu::PioImpl::setWait(bool active) {

}

uint8_t altair::card::Cpu::PioImpl::getData() {
	return this->conn->din();
}

void altair::card::Cpu::PioImpl::setData(uint8_t val) {
	this->conn->dout(val);
}

void altair::card::Cpu::PioImpl::setAddress(uint16_t val) {
	this->conn->a(val);
}

void altair::card::Cpu::PioImpl::clk() {
	// Decode control byte
	if (this->conn->psync()) {
		uint8_t val = conn->dout();

		conn->sinta (val & 0x01);
		conn->swo   (val & 0x02);
		conn->sstack(val & 0x04);
		conn->shlta (val & 0x08);
		conn->sout  (val & 0x10);
		conn->sm1   (val & 0x20);
		conn->sinp  (val & 0x40);
		conn->smemr (val & 0x80);
	}

	// Memory write signal
	conn->mwrt(conn->pwr() && ! conn->sout());

	conn->clk();
}

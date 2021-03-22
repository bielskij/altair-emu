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

#include <chrono>
#include <thread>

#include "altair/Card/Cpu.hpp"

#define DEBUG_LEVEL 2
#include "common/debug.h"


void altair::card::Cpu::ClkSource::loop() {
	auto lastCheckClk = std::chrono::steady_clock::now();
	int  ticksNumber  = 0;

	while (! this->_stop) {
		while (! this->_stop && (this->_ticksTurn < 1000)) {
			this->_cpu->turn();
		}

		ticksNumber += this->_ticksTurn;
		this->_ticksTurn = 0;

		if (! this->_stop) {
			auto ticksTime   = ticksNumber * this->_tickNanoseconds;
			auto elaspedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
				std::chrono::steady_clock::now() - lastCheckClk
			).count();

			if (ticksTime - elaspedTime > 30000000) {
				std::this_thread::sleep_for(
					std::chrono::nanoseconds(ticksTime - elaspedTime)
				);

				lastCheckClk = std::chrono::steady_clock::now();
				ticksNumber  = 0;
			}
		}
	}
}


void altair::card::Cpu::ClkSource::tick() {
	this->_ticksTurn++;
}


void altair::card::Cpu::ClkSource::tick(int ticks) {
	this->_ticksTurn += ticks;
}


bool altair::card::Cpu::PioImpl::getReset() {
#warning "TODO implement"
	return false;
}

bool altair::card::Cpu::PioImpl::getHold() {
#warning "TODO implement"
	return false;
}

bool altair::card::Cpu::PioImpl::getInt() {
	return this->_conn->pint();
}

bool altair::card::Cpu::PioImpl::getInte() {
	return this->_conn->pinte();
}

bool altair::card::Cpu::PioImpl::getReady() {
#warning "TODO implement"
	return true;
}

void altair::card::Cpu::PioImpl::setInte(bool active) {
	this->_conn->pinte(active);
}

void altair::card::Cpu::PioImpl::setInt(bool active) {
	this->_conn->pint(active);
}

void altair::card::Cpu::PioImpl::setDbin(bool active) {
	this->_conn->pdbin(active);
}

void altair::card::Cpu::PioImpl::setWr(bool active) {
	this->_conn->pwr(active);
}

void altair::card::Cpu::PioImpl::setSync(bool active) {
	this->_conn->psync(active);
}

void altair::card::Cpu::PioImpl::setHoldAck(bool active) {

}

void altair::card::Cpu::PioImpl::setWait(bool active) {

}

uint8_t altair::card::Cpu::PioImpl::getData() {
	return this->_conn->din();
}

void altair::card::Cpu::PioImpl::setData(uint8_t val) {
	this->_conn->dout(val);
}

void altair::card::Cpu::PioImpl::setAddress(uint16_t val) {
	this->_conn->a(val);
}

void altair::card::Cpu::PioImpl::clk() {
	// equivalent to pull up
	this->_conn->pint(false);

	// Decode control byte
	if (this->_conn->psync()) {
		uint8_t val = _conn->dout();

		_conn->sinta ((val & 0x01) != 0);
		_conn->swo   ((val & 0x02) != 0);
		_conn->sstack((val & 0x04) != 0);
		_conn->shlta ((val & 0x08) != 0);
		_conn->sout  ((val & 0x10) != 0);
		_conn->sm1   ((val & 0x20) != 0);
		_conn->sinp  ((val & 0x40) != 0);
		_conn->smemr ((val & 0x80) != 0);
	}

	_conn->clk();

	this->_clkSource->tick();
}


uint8_t altair::card::Cpu::PioImpl::memoryRead(uint16_t address) {
	uint8_t value = 0;

	this->_conn->memoryRead(address, value);

	return value;
}


void altair::card::Cpu::PioImpl::memoryWrite(uint16_t address, uint8_t value) {
	this->_conn->memoryWrite(address, value);
}


uint8_t altair::card::Cpu::PioImpl::ioRead(uint8_t number) {
	uint8_t value = 0;

	this->_conn->ioIn(number, value);

	return value;
}


void altair::card::Cpu::PioImpl::ioWrite(uint8_t number, uint8_t data) {
	this->_conn->ioOut(number, data);
}


void altair::card::Cpu::PioImpl::clk(uint8_t ticks) {
	// equivalent to pull up
	this->_conn->pint(false);

	this->_conn->clk(ticks);

	this->_clkSource->tick(ticks);
}

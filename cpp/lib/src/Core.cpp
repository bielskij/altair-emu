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
#include <stdexcept>

#include "altair/Cpu.hpp"
#include "Core/MachineCycle/Fetch.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


altair::Core::Core(Pio &pio) : _pio(pio) {
	this->_fetchCycle = new MachineCycleFetch(this);
	this->_decoder    = new InstructionDecoder(this);

	this->_i     = nullptr;
	this->_cycle = nullptr;
	this->_state = 0;

	for (auto &r : this->_bregs) {
		r = 0;
	}

	for (auto &r : this->_wregs) {
		r = 0;
	}
}


altair::Core::~Core() {
	delete this->_fetchCycle;
}


void altair::Core::nexti() {
	// No instruction - fetch new one
	if (this->_i == nullptr) {
		// Fetch next instruction
		this->_fetchCycle->t1();
		this->_pio.clk();

		this->_fetchCycle->t2();
		this->_pio.clk();

		this->_fetchCycle->t3();
		this->_pio.clk();

		this->_i = this->_decoder->decode(this->bR(BReg::IR));
		if (this->_i == nullptr) {
			ERR(("Not supported opcode: %02x", this->bR(BReg::IR)));

			throw std::runtime_error("Not supported opcode!");
		}

		this->_i->reset();

		MachineCycle *cycle;
		while ((cycle = this->_i->nextCycle()) != nullptr) {
			if (! cycle->t1()) {
				continue;
			}

			if (! cycle->t2()) {
				continue;
			}

			if (! cycle->t3()) {
				continue;
			}

			if (! cycle->t4()) {
				continue;
			}

			cycle->t5();
		}
	}
}


void altair::Core::tick() {
	// No instruction there. Do fetch
	if (this->_i == nullptr) {
		switch (this->_state) {
			case 0:
				{
					this->_fetchCycle->t1();
					this->_pio.clk();
				}
				break;

			case 1:
				{
					this->_fetchCycle->t2();
					this->_pio.clk();
				}
				break;

			case 2:
				{
					this->_fetchCycle->t3();
					// No cycle there

					this->_i = this->_decoder->decode(this->bR(BReg::IR));
					if (this->_i == nullptr) {
						ERR(("Not supported opcode: %02x", this->bR(BReg::IR)));

						throw std::runtime_error("Not supported opcode!");
					}

					this->_i->reset();

					this->_cycle = this->_i->nextCycle();
				}
				break;

			default:
				break;
		}

		this->_state++;
	}

	if (this->_i != nullptr) {
		bool nextCycle = false;

		switch (this->_state) {
			case 0:
				nextCycle = ! this->_fetchCycle->t1();
				break;

			case 1:
				nextCycle = ! this->_fetchCycle->t2();
				break;

			case 2:
				nextCycle = ! this->_fetchCycle->t3();
				break;

			case 3:
				nextCycle = ! this->_fetchCycle->t4();
				break;

			case 4:
				nextCycle = ! this->_fetchCycle->t5();
				break;
		}

		this->_pio.clk();

		this->_state++;
		if (nextCycle) {
			this->_state = 0;

			this->_cycle = this->_i->nextCycle();
			if (this->_cycle == nullptr) {
				this->_i = nullptr;
			}
		}
	}
}

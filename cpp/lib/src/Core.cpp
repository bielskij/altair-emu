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
#include "Core/MachineCycle/Interrupt.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


altair::Core::Core(Pio &pio) : _pio(pio) {
	this->_fetchCycle     = new MachineCycleFetch(this);
	this->_interruptCycle = new MachineCycleInterrupt(this);
	this->_decoder        = new InstructionDecoder(this);
	this->_alu            = new Alu(this);

	this->_i       = nullptr;
	this->_cycle   = nullptr;
	this->_state   = 0;
	this->_inteFF  = false;
	this->_intePin = false;

	for (auto &r : this->_bregs) {
		r = 0;
	}

	// Initialize constant value in flag register
	this->bR(BReg::F, 0x02);
}


altair::Core::~Core() {
	delete this->_fetchCycle;
	delete this->_decoder;
	delete this->_alu;
}


void altair::Core::nextInstruction() {
	if (this->_i == nullptr) {
		this->nextCycle();
	}

	while (this->_i != nullptr) {
		this->nextCycle();
	}
}


void altair::Core::nextCycle() {
	MachineCycle *cycle = this->_cycle;

	while (cycle == nullptr || (cycle == this->_cycle)) {
		this->tick();

		if (cycle == nullptr && this->_cycle != nullptr && this->_cycle != this->_fetchCycle) {
			cycle = this->_cycle;
		}
	}
}


void altair::Core::tick() {
	// WR is set in the last state of write cycle - require to be cleared on the next cycle/state.
	this->_pio.setWr(false);

	this->_alu->clk();

	// No instruction there. Do fetch
	if (this->_cycle == nullptr){
		this->_cycle = this->inteFF() ? this->_interruptCycle : this->_fetchCycle;
	}
	{
		bool nextCycle = false;

		switch (this->_state) {
			case 0:
				nextCycle = ! this->_cycle->t1();
				break;

			case 1:
				nextCycle = ! this->_cycle->t2();
				break;

			case 2:
				nextCycle = ! this->_cycle->t3();

				if (this->_cycle == this->_fetchCycle) {
					this->_i = this->_decoder->decode(this->bR(BReg::IR));
					if (this->_i == nullptr) {
						ERR(("Not supported opcode: %02x", this->bR(BReg::IR)));

						throw std::runtime_error("Not supported opcode!");
					}

					this->_i->reset();

					this->_cycle = this->_i->nextCycle();
				}
				break;

			case 3:
				nextCycle = ! this->_cycle->t4();
				break;

			case 4:
				nextCycle = ! this->_cycle->t5();
				break;
		}

		this->_pio.clk();

		if (! nextCycle) {
			this->_state++;

		} else {
			this->_state = 0;

			this->_cycle = this->_i->nextCycle();
			if (this->_cycle == nullptr) {
				this->_i     = nullptr;

				this->inteFF(this->pio().getInt() && this->intePin());
			}
		}
	}
}

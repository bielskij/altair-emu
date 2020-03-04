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
#include "altair/Core.hpp"


void altair::Core::Alu::reset() {
	this->operation = Op::IDLE;
	this->clkCount  = 0;

	// Fake requests to set initial state
	this->fCY(0);
	this->fZ(1);
	this->fS(0);
	this->fP(0);
	this->fAC(0);
}


void altair::Core::Alu::clk() {
	if (this->operation != Op::IDLE) {
		this->clkCount++;

		switch(this->operation) {
			case Op::ADD:
				if (this->clkCount == 2) {
					uint16_t valCy = (uint16_t) core->bR(Core::BReg::ACT) + (uint16_t) core->bR(Core::BReg::TMP);
					uint8_t  valAc = (core->bR(Core::BReg::ACT) & 0x0f) + (core->bR(Core::BReg::TMP) + 0x0f);

					if (this->operationCarry) {
						valCy += this->fCY();
						valAc += this->fCY();
					}

					this->fZ (valCy);
					this->fS (valCy);
					this->fP (valCy);
					this->fCY(valCy);
					this->fAC(valAc);

					core->bR(Core::BReg::A, valCy);
				}
				break;
		}
	}
}


void altair::Core::Alu::op(Act actSrc, Op operation, bool includeCarry) {
	switch (actSrc) {
		case Act::A:
			core->bR(Core::BReg::ACT, core->bR(Core::BReg::A));
			break;

		default:
			throw std::runtime_error("Unsupported ACT source!");
	}

	this->operation      = operation;
	this->operationCarry = includeCarry;
	this->clkCount       = 0;
}

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
	this->operation     = Op::IDLE;
	this->clkCount      = 0;
	this->clkDelay      = 0;
	this->includeCarry  = false;
	this->updateCarry   = true;

	// Fake requests to set initial state
	clear(CY);
	clear(Z);
	clear(S);
	clear(P);
	clear(AC);
}


void altair::Core::Alu::clk() {
	if (this->operation != Op::IDLE) {
		this->clkCount++;

		switch(this->operation) {
			case Op::ADD:
				if (this->clkCount == this->clkDelay) {
					uint16_t valCy = (uint16_t) core->bR(Core::BReg::ACT) + (uint16_t) core->bR(Core::BReg::TMP);
					uint8_t  valAc = (core->bR(Core::BReg::ACT) & 0x0f) + (core->bR(Core::BReg::TMP) + 0x0f);

					if (this->includeCarry) {
						valCy += this->fCY();
						valAc += this->fCY();
					}

					this->fZ (valCy);
					this->fS (valCy);
					this->fP (valCy);
					this->fAC(valAc);
					if (this->updateCarry) {
						this->fCY(valCy);
					}

					core->bR(this->dstReg, valCy);
				}
				break;

			case Op::SUB:
				if (this->clkCount == this->clkDelay) {
					uint16_t valCy = (uint16_t) core->bR(Core::BReg::ACT) - (uint16_t) core->bR(Core::BReg::TMP);
					uint8_t  valAc = (core->bR(Core::BReg::ACT) & 0x0f) - (core->bR(Core::BReg::TMP) + 0x0f);

					if (this->includeCarry) {
						valCy -= this->fCY();
						valAc -= this->fCY();
					}

					this->fZ (valCy);
					this->fS (valCy);
					this->fP (valCy);
					this->fCY(valCy);
					this->fAC(valAc);

					if (this->updateCarry) {
						this->fCY(valCy);
					}

					core->bR(this->dstReg, valCy);
				}
				break;
		}
	}
}


void altair::Core::Alu::op(Act actSrc, Core::BReg dstReg, Op operation, bool includeCarry, bool updateCarry, uint8_t clkDelay) {
	switch (actSrc) {
		case Act::A:
			core->bR(Core::BReg::ACT, core->bR(Core::BReg::A));
			break;

		case Act::C_1:
			core->bR(Core::BReg::ACT, 1);
			break;

		default:
			throw std::runtime_error("Unsupported ACT source!");
	}

	this->operation     = operation;
	this->includeCarry  = includeCarry;
	this->updateCarry   = updateCarry;
	this->clkCount      = 0;
	this->clkDelay      = clkDelay;
	this->dstReg        = dstReg;
}

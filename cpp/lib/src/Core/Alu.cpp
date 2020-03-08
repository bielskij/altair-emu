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
	this->updateFlags   = 0;

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

		if (this->clkCount >= this->clkDelay) {
			uint8_t  valTmp = core->bR(Core::BReg::TMP);
			uint16_t valCy  = core->bR(Core::BReg::ACT);
			uint8_t  valAc  = valCy & 0x0f;

			uint8_t carry = 0;

			if (this->includeCarry) {
				carry = this->fCY();
			}

			switch(this->operation) {
				case Op::ADD:
					valCy +=  ((uint16_t)valTmp + carry);
					valAc += (((uint16_t)valTmp & 0x0f) + carry);
					break;

				case Op::SUB:
					valCy -=  ((uint16_t)valTmp + carry);
					valAc -= (((uint16_t)valTmp & 0x0f) + carry);
					break;

				case Op::AD:
					{
						if (((valCy & 0x0f) > 0x09) || this->fAC()) {
							valCy += 0x06;
							valAc += 0x06;
						}

						if (((valCy & 0xf0) > 0x90) || this->fCY()) {
							valCy += 0x60;
						}
					}
					break;

				case Op::AND:
					{
						// 8085 always set AC flag in case of AND operation!
						if (((valTmp & 0x08) | (valCy & 0x08)) != 0) {
							valAc = 0xff;
						}
						valCy &= valTmp;
					}
					break;

				case Op::XOR:
					{
						valCy ^= valTmp;
						valAc  = 0;
					}
					break;

				case Op::OR:
					{
						valCy |= valTmp;
						valAc  = 0;
					}
					break;
			}

			if (this->updateFlags & Z) {
				this->fZ (valCy);
			}

			if (this->updateFlags & S) {
				this->fS (valCy);
			}

			if (this->updateFlags & P) {
				this->fP (valCy);
			}

			if (this->updateFlags & AC) {
				this->fAC(valAc);
			}

			if (this->updateFlags & CY) {
				this->fCY(valCy);
			}

			core->bR(this->dstReg, valCy);

			this->operation = Op::IDLE;
		}
	}
}


void altair::Core::Alu::op(Core::BReg actSrc, Core::BReg dstReg, Op operation, bool includeCarry, uint8_t updateFlags, uint8_t clkDelay) {
	this->op(core->bR(actSrc), dstReg, operation, includeCarry, updateFlags, clkDelay);
}


void altair::Core::Alu::op(uint8_t actVal, Core::BReg dstReg, Op operation, bool includeCarry, uint8_t updateFlags, uint8_t clkDelay) {
	core->bR(Core::BReg::ACT, actVal);

	this->operation     = operation;
	this->includeCarry  = includeCarry;
	this->updateFlags   = updateFlags;
	this->clkCount      = 0;
	this->clkDelay      = clkDelay;
	this->dstReg        = dstReg;

	if (this->clkDelay == 0) {
		this->clk();
	}
}

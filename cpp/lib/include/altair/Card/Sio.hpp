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
#ifndef ALTAIR_CARD_SIO_HPP_
#define ALTAIR_CARD_SIO_HPP_

#include <cstdint>

#include "altair/utils/Terminal.hpp"

namespace altair {
	namespace card {
		class Sio : public Card, protected SimpleConnector {
			public:
				enum BaudRate {
					B110,
					B150,
					B300,
					B600,
					B1200,
					B2400,
					B4800,
					B9600,
					B19200
				};

				enum IntLevel {
					VI0, // Vectored interrupt (88-VI)
					VI1,
					VI2,
					VI3,
					VI4,
					VI5,
					VI6,
					VI7,

					SL, // Single level interrupt (PINT)
				};

			public:
				// Only even value available
				Sio(uint8_t address, BaudRate baudrate, IntLevel inLvl = SL, IntLevel outLvl = SL) {
					if (address & 0x01) {
						throw std::invalid_argument("Only even address is allowed!");
					}

					this->_address   = address;
					this->_baudrate  = baudrate;
					this->_intInLvl  = inLvl;
					this->_intOutLvl = outLvl;

					this->_intIn   = false;
					this->_intOut  = false;
				}

				card::Connector *getConnector() {
					return this;
				}

			protected:
				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) override {
					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) override {
				}

				bool onIn(uint8_t number, uint8_t &val) override {
					if (this->_address == number) {
						val = 0x81;

						if (this->_terminal.canWrite()) {
							val &= ~0x80;
						}

						if (this->_terminal.canRead()) {
							val &= ~0x01;
						}

						return true;

					} else if ((this->_address + 1) == number) {
						val = this->_terminal.readByte();

						return true;
					}

					return false;
				}

				void onOut(uint8_t number, uint8_t data) override {
					if (this->_address == number) {
						if (data & 0x01) {
							this->_intIn = true;
						}

						if (data & 0x02) {
							this->_intOut = true;
						}

					} else if ((this->_address + 1) == number) {
						this->_terminal.writeByte(data);
					}
				}

				void onClk() override {
					SimpleConnector::onClk();

					if (this->_intIn) {
						bool canRead = this->_terminal.canRead();

						if (this->_intInLvl == SL) {
							this->pint(canRead);

						} else {
							uint8_t ivVal = this->vi();

							if (canRead) {
								ivVal |= (1 << this->_intInLvl);
							} else {
								ivVal &= ~(1 << this->_intInLvl);
							}

							this->vi(ivVal);
						}
					}

					if (this->_intOut) {
						bool canWrite = this->_terminal.canWrite();

						if (this->_intOutLvl == SL) {
							this->pint(canWrite);

						} else {
							uint8_t ivVal = this->vi();

							if (canWrite) {
								ivVal |= (1 << this->_intOutLvl);
							} else {
								ivVal &= ~(1 << this->_intOutLvl);
							}

							this->vi(ivVal);
						}
					}
				}

			private:
				altair::utils::Terminal _terminal;

				uint8_t  _address;
				BaudRate _baudrate;

				bool     _intIn;
				IntLevel _intInLvl;
				bool     _intOut;
				IntLevel _intOutLvl;
		};
	}
}

#endif /* ALTAIR_CARD_SIO_HPP_ */

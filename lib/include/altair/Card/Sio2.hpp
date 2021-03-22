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
#ifndef ALTAIR_CARD_SIO2_HPP_
#define ALTAIR_CARD_SIO2_HPP_

#include <cstdint>

#include "altair/Card/common.hpp"
#include "altair/utils/Terminal.hpp"

namespace altair {
	namespace card {
		class Sio2 : public Card, protected Connector {
			protected:
				class SingleSio {
					public:
						SingleSio(uint32_t baud, IntLevel intLevel) {
							this->_baudrate = baud;
							this->_intLvl   = intLevel;
							this->_intIn    = false;
							this->_intOut   = false;
						}

						uint8_t getStatus() {
							uint8_t ret = 0;

							{
								bool canRead, canWrite;

								this->_terminal.canReadWrite(canRead, canWrite);

								if (canRead) {
									ret |= 1;
								}

								if (canWrite) {
									ret |= 2;
								}
							}

							return ret;
						}

						uint8_t getData() {
							return this->_terminal.readByte();
						}

						void setConfig(uint8_t val) {
							this->_intIn  = (val & 0x80) != 0;
							this->_intOut = (val & 0x60) == 0x20;
						}

						void putData(uint8_t val) {
							this->_terminal.writeByte(val);
						}

						bool hasInt(IntLevel &level, bool &hi) {
							bool ret = this->_intIn || this->_intOut;

							// interrupt enabled
							if (ret) {
								bool canRead, canWrite;

								this->_terminal.canReadWrite(canRead, canWrite);

								hi = false;

								if (this->_intIn) {
									hi = canRead;
								}

								if (this->_intOut) {
									hi |= canWrite;
								}
							}

							level = this->_intLvl;

							return ret;
						}

					private:
						bool     _intIn;
						bool     _intOut;
						IntLevel _intLvl;
						uint32_t _baudrate;

						altair::utils::Terminal _terminal;
				};

			public:
				// address - has to be multiple of 4
				// baudrate - 110, 150, 300, 1200, 1800, 2400, 4800, 9600
				Sio2(uint8_t address, uint32_t baudrate0, uint32_t baudrate1, IntLevel lvl0 = IntLevel::SL, IntLevel lvl1 = IntLevel::SL) {
					if (address & 0x03) {
						throw std::invalid_argument("Only even address is allowed!");
					}

					this->_address = address;

					this->_terms[0] = new SingleSio(baudrate0, lvl0);
					this->_terms[1] = new SingleSio(baudrate1, lvl1);
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
					if (this->_address <= number) {
						SingleSio *term = this->_terms[0];

						number -= this->_address;

						if (number > 1) {
							term = this->_terms[1];

							number -= 2;
						}

						if (number < 2) {
							if (number == 1) {
								val = term->getData();

							} else {
								val = term->getStatus();
							}

							return true;
						}
					}

					return false;
				}

				void onOut(uint8_t number, uint8_t data) override {
					if (this->_address <= number) {
						SingleSio *term = this->_terms[0];

						number -= this->_address;

						if (number > 1) {
							term = this->_terms[1];

							number -= 2;
						}

						if (number < 2) {
							if (number == 1) {
								term->putData(data);
							} else {
								term->setConfig(data);
							}
						}
					}
				}

				void _onClk() {
					bool    pint;
					uint8_t vi;

					Connector::onClk();

					pint = this->pint();
					vi   = this->vi();

					for (int i = 0; i < 2; i++) {
						IntLevel lvl;
						bool     hi;

						if (this->_terms[i]->hasInt(lvl, hi)) {
							if (lvl == IntLevel::SL) {
								pint |= hi;

							} else {
								if (hi) {
									vi |= (1 << lvl);
								} else {
									vi &= ~(1 << lvl);
								}
							}
						}
					}

					this->pint(pint);
					this->vi(vi);
				}

				void onClk(uint8_t ticks) override {
					this->_onClk();
				}

				void onClk() override {
					this->_onClk();
				}

			private:
				uint16_t   _address;
				SingleSio *_terms[2];
		};
	}
}

#endif /* ALTAIR_CARD_SIO2_HPP_ */

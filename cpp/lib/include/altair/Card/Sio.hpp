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

			public:
				// Only even value available
				Sio(uint8_t address, BaudRate baudrate) {
					if (address & 0x01) {
						throw std::invalid_argument("Only even address is allowed!");
					}

					this->_address  = address;
					this->_baudrate = baudrate;

					this->_intIn   = false;
					this->_intOut  = false;
				}

				card::Connector *getConnector() {
					return this;
				}

			protected:
				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) {
					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) {
				}

				bool onIn(uint8_t number, uint8_t &val) {
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

				void onOut(uint8_t number, uint8_t data) {
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

			private:
				altair::utils::Terminal _terminal;

				uint8_t  _address;
				BaudRate _baudrate;

				bool _intIn;
				bool _intOut;
		};
	}
}

#endif /* ALTAIR_CARD_SIO_HPP_ */

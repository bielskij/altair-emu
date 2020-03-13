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
#ifndef ALTAIR_CARD_SIMPLECONNECTOR_HPP_
#define ALTAIR_CARD_SIMPLECONNECTOR_HPP_

namespace altair {
	namespace card {
		class SimpleConnector : public Connector {
			public:
				SimpleConnector() : Connector() {
				}

				virtual void onClk() {
					if (this->pdbin()) {
						if (this->smemr()) {
							uint8_t val;

							if (this->onMemoryRead(this->a(), val)) {
								this->din(val);
							}

						} else if (this->sinp()) {
							this->din(this->onIn(this->a()));
						}

					} else if (this->pwr()) {
						if (this->sout()) {
							this->onOut(this->a(), this->dout());

						} else {
							this->onMemoryWrite(this->a(), this->dout());
						}
					}
				}

				virtual bool    onMemoryRead(uint16_t address, uint8_t &val) = 0;
				virtual void    onMemoryWrite(uint16_t address, uint8_t data) = 0;
				virtual uint8_t onIn(uint8_t number) = 0;
				virtual void    onOut(uint8_t number, uint8_t data) = 0;
		};
	}
}



#endif /* ALTAIR_CARD_SIMPLECONNECTOR_HPP_ */

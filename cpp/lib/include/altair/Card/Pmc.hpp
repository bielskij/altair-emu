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
#ifndef ALTAIR_CARD_88_PMC_HPP_
#define ALTAIR_CARD_88_PMC_HPP_

#include <algorithm>

#include "altair/MainBoard.hpp"
#include "altair/Card/SimpleConnector.hpp"
#include "altair/utils/ImageLoader.hpp"

namespace altair {
	namespace card {
		/*!
		 * 88-PMC 2KB PROM card
		 */
		class Pmc : public Card, protected SimpleConnector {
			public:
				/*
				 * bank - 0 - 31
				 * waitStates - 0 - 3 extra machine wait states while accessing
				 */
				Pmc(uint8_t bank, uint8_t waitStates) {
					this->bankAddr = bank;

					std::fill(std::begin(prom), std::end(prom), 0xff);
				}

				void load(utils::ImageLoader *loader) {
					uint16_t address;
					uint8_t  data;

					loader->rewind();

					while (loader->nextByte(address, data)) {
						if (! validBank(address)) {
							throw std::invalid_argument("The image does not fit in this bank address bounds!");

						} else {
							this->prom[promOffset(address)] = data;
						}
					}
				}

				card::Connector *getConnector() override {
					return this;
				}

			protected:
				inline bool validBank(uint16_t address) {
					return (this->bankAddr == (address >> 11));
				}

				inline uint16_t promOffset(uint16_t address) {
					return address & 0x07ff;
				}

				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) {
					if (validBank(address)) {
						val = this->prom[promOffset(address)];

						return true;
					}

					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) {
				}

				bool onIn(uint8_t number, uint8_t &val) {
					return false;
				}

				void onOut(uint8_t number, uint8_t data) {
				}

			private:
				uint8_t bankAddr;

				uint8_t prom[2 * 1024];
		};
	}
}

#endif /* ALTAIR_CARD_88_PMC_HPP_ */

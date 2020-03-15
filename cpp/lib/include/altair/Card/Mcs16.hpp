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
#ifndef ALTAIR_CARD_88_16MCS_HPP_
#define ALTAIR_CARD_88_16MCS_HPP_

#include <algorithm>

#include "altair/MainBoard.hpp"
#include "altair/Card/SimpleConnector.hpp"

namespace altair {
	namespace card {
		class Mcs16 : public Card, protected SimpleConnector {
			public:
				// Equivalent of original switch on the module board, defines address offset of the card memory.
				// a:     0 - 16383
				// b: 16384 - 32767
				// c: 32768 - 49151
				// d: 49152 - 65535
				enum Sw1 {
					COOO, //     0 - 16383
					OCOO, // 16384 - 32767
					OOCO, // 32768 - 49151
					OOOC  // 49152 - 65535
				};

			public:
				Mcs16(Sw1 switchMode) {
					switch (switchMode) {
						case Sw1::COOO: this->bankAddr = 0x0000; break;
						case Sw1::OCOO: this->bankAddr = 0x0100; break;
						case Sw1::OOCO: this->bankAddr = 0x1000; break;
						case Sw1::OOOC: this->bankAddr = 0x1100; break;
						default:
							throw std::invalid_argument("Invalid switch mode");
					}

					std::fill(std::begin(memory), std::end(memory), 0x00);
				}

				card::Connector *getConnector() override {
					return this;
				}

			protected:
				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) {
					uint16_t bank = address & 0xc000;

					if (bank == this->bankAddr) {
						uint16_t addr = address & 0x3fff;

						val = this->memory[addr];

						return true;
					}

					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) {
					uint16_t bank = address & 0xc000;

					if (bank == this->bankAddr) {
						uint16_t addr = address & 0x3fff;

						this->memory[addr] = data;
					}
				}

				bool onIn(uint8_t number, uint8_t &val) {
					return false;
				}

				void onOut(uint8_t number, uint8_t data) {
				}

			private:
				uint16_t bankAddr;

				int8_t memory[16 * 1024];
		};
	}
}

#endif /* ALTAIR_CARD_88_16MCS_HPP_ */

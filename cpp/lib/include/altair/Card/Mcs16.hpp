/*
 * 88-16mcs.hpp
 *
 *  Created on: 13.03.2020
 *      Author: jarko
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

				uint8_t onIn(uint8_t number) {
					return 0;
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

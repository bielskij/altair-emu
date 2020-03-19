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
#ifndef ALTAIR_CARD_88_DCDD_HPP_
#define ALTAIR_CARD_88_DCDD_HPP_

#include <algorithm>

#include "altair/MainBoard.hpp"
#include "altair/Card/SimpleConnector.hpp"
#include "altair/utils/ImageLoader.hpp"

namespace altair {
	namespace card {
		/*!
		 * 88-DCDD 8" floppy disk drive
		 */
		class Dcdd : public Card, protected SimpleConnector {
			private:
				constexpr uint32_t TRACK_COUNT   = 77;
				constexpr uint32_t TRACK_SECTORS = 32;
				constexpr uint32_t SECTOR_SIZE   = 137;

				constexpr uint8_t STATUS_ENWD   = 0x01; // Enter new write data - indicates write circuit is ready for new data
				constexpr uint8_t STATUS_MH     = 0x02; // Move Head - indicates head movement allowed when true (step in/out)
				constexpr uint8_t STATUS_HS     = 0x04; // True 40ms after head loaded or step command
				constexpr uint8_t STATUS_INTE   = 0x20; // Indicates interrupt enabled
				constexpr uint8_t STATUS_TRACK0 = 0x40;
				constexpr uint8_t STATUS_NRDA   = 0x80; // New read data available

				constexpr uint8_t CTRL_STEP_IN  = 0x01;
				constexpr uint8_t CTRL_STEP_OUT = 0x02;
				constexpr uint8_t CTRL_HL       = 0x04; // loads head onto disk
				constexpr uint8_t CTRL_HUL      = 0x08; // removes head from disk surface
				constexpr uint8_t CTRL_IE       = 0x10; // Interrupt enable
				constexpr uint8_t CTRL_ID       = 0x20; // Interrupt disable
				constexpr uint8_t CTRL_HCS      = 0x40; // Head current switch
				constexpr uint8_t CTRL_WE       = 0x80; // Write enable

			public:
				Dcdd() {
				}

				card::Connector *getConnector() override {
					return this;
				}

			protected:
				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) {
					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) {
				}

				bool onIn(uint8_t number, uint8_t &val);
				void onOut(uint8_t number, uint8_t data);

			private:
		};
	}
}

#endif /* ALTAIR_CARD_88_DCDD_HPP_ */

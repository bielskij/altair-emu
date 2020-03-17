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
#ifndef ALTAIR_MAINBOARD_HPP_
#define ALTAIR_MAINBOARD_HPP_

#include <cstdint>
#include <vector>
#include <algorithm>

#include "altair/Card.hpp"
#include "altair/Card/Connector.hpp"

namespace altair {
	class CardConnector;

	class MainBoard : protected altair::card::Connector {
		public:
			MainBoard();
			virtual ~MainBoard() {
			}

			void addCard(Card *card) {
				this->_cards.push_back(card);

				this->_connectors.push_back(card->getConnector());
				this->_connectors.back()->connect(this);
			}

			void removeCard(Card *card) {
				this->_connectors.erase(
					std::remove(
						this->_connectors.begin(), this->_connectors.end(), card->getConnector()
					), this->_connectors.end()
				);

				this->_cards.erase(
					std::remove(
						this->_cards.begin(), this->_cards.end(), card
					), this->_cards.end()
				);
			}

			// ---------- Connector methods
			void clk() override;
			void onClk() override;
			uint8_t vi() const override;
			void vi(uint8_t val) override;

			uint16_t a() const override;
			void a(uint16_t address) override;
			uint8_t dout() const override;
			void dout(uint8_t data) override;
			uint8_t din() const override;
			void din(uint8_t data) override;

			virtual bool mwrt() const override;
			virtual void mwrt(bool val) override;

			bool sm1() const override;
			void sm1(bool val) override;
			bool sout() const override;
			void sout(bool val) override;
			bool sinp() const override;
			void sinp(bool val) override;
			bool sinta() const override;
			void sinta(bool val) override;
			bool swo() const override;
			void swo(bool val) override;
			bool sstack() const override;
			void sstack(bool val) override;
			bool smemr() const override;
			void smemr(bool val) override;
			bool shlta() const override;
			void shlta(bool val) override;

			bool pdbin() const override;
			void pdbin(bool val) override;
			bool pwr() const override;
			void pwr(bool val) override;
			bool psync() const override;
			void psync(bool val) override;

		private:
			std::vector<Card *>            _cards;
			std::vector<card::Connector *> _connectors;

			// vectored interrupt
			uint8_t  _vi;
			uint16_t _a;
			uint8_t  _di;
			uint8_t  _do;

			// CPU control flags
			bool _sm1, _sout, _sinp, _sinta, _swo, _sstack, _smemr, _shlta;

			// CPU control pins
			bool _pdbin, _pwr, _psync;

			// Other bus pins
			bool _mwrt;
	};
}


#endif /* ALTAIR_MAINBOARD_HPP_ */

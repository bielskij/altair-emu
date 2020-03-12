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
#ifndef ALTAIR_CARD_CPU_HPP_
#define ALTAIR_CARD_CPU_HPP_

#include "altair/MainBoard.hpp"
#include "altair/Cpu.hpp"

namespace altair {
	namespace card {
		class Cpu : public Card {
			protected:
				class ClkSource {
					public:
						ClkSource(altair::Cpu *cpu) {
							this->cpu = cpu;
						}

					private:
						altair::Cpu *cpu;
				};

				class PioImpl : public altair::Cpu::Pio {
					public:
						PioImpl(Connector *conn) {
							this->conn = conn;
						}

					public:
						bool getReset() override;
						bool getHold() override;
						bool getInt() override;
						bool getReady() override;

						void setInte(bool active) override;
						void setDbin(bool active) override;
						void setWr(bool active) override;
						void setSync(bool active) override;
						void setHoldAck(bool active) override;
						void setWait(bool active) override;

						uint8_t getData() override;
						void    setData(uint8_t val) override;

						void setAddress(uint16_t val) override;

						void clk() override;

					private:
						Connector *conn;
				};

			public:
				Cpu() {
					this->_pio = new PioImpl(&this->_connector);
					this->_cpu = new altair::Cpu(*this->_pio);
					this->_clk = new ClkSource(this->_cpu);
				}

				card::Connector *getConnector() override {
					return &this->_connector;
				}

			private:
				PioImpl     *_pio;
				altair::Cpu *_cpu;
				ClkSource   *_clk;
				Connector    _connector;
		};
	}
}


#endif /* ALTAIR_CARD_CPU_HPP_ */

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

#include "altair/Config.hpp"
#include "altair/MainBoard.hpp"
#include "altair/Core/InterpretedCore.hpp"
#include "altair/Core/JitCore.hpp"

namespace altair {
	namespace card {
		class Cpu : public Card {
			public:
				class ClkSource {
					public:
						ClkSource(altair::Core *cpu) {
							this->_cpu             = cpu;
							this->_stop            = false;
							this->_frequency       = Config::getClkFrequency();
							this->_tickNanoseconds = 1000000000 / this->_frequency;
							this->_ticksTurn       = 0;
						}

						virtual ~ClkSource() {
						}

						void loop();

						void stop() {
							this->_stop = true;
						}

						void tick();

					private:
						altair::Core *_cpu;

						bool _stop;
						int  _frequency;
						int  _tickNanoseconds;
						int  _ticksTurn;
				};

			protected:
				class PioImpl : public altair::InterpretedCore::Pio {
					public:
						PioImpl(Connector *conn) {
							this->_conn      = conn;
							this->_clkSource = nullptr;
						}

					public:
						bool getReset() override;
						bool getHold() override;
						bool getInt() override;
						bool getInte() override;
						bool getReady() override;

						void setInt(bool active) override;
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

					public:
						void setClkSource(ClkSource *clk) {
							this->_clkSource = clk;
						}

					private:
						Connector *_conn;
						ClkSource *_clkSource;
				};

			public:
				Cpu(uint16_t startPc, bool useJitCore) {
					this->_pio = new PioImpl(&this->_connector);
					if (useJitCore) {
						this->_cpu = new altair::JitCore(*this->_pio, startPc);
					} else {
						this->_cpu = new altair::InterpretedCore(*this->_pio, startPc);
					}
					this->_clk = new ClkSource(this->_cpu);

					this->_pio->setClkSource(this->_clk);
				}

				inline card::Connector *getConnector() override {
					return &this->_connector;
				}

				inline ClkSource *getClock() const {
					return this->_clk;
				}

			private:
				PioImpl      *_pio;
				altair::Core *_cpu;
				ClkSource    *_clk;
				Connector     _connector;
		};
	}
}


#endif /* ALTAIR_CARD_CPU_HPP_ */

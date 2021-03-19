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
#ifndef TESTCORE_HPP_
#define TESTCORE_HPP_

#include "altair/Core/InterpretedCore.hpp"
#include "altair/Core/JitCore.hpp"

namespace test {
	class Core : public altair::Core {
		public:
			Core(altair::InterpretedCore::Pio &pio, bool jit = false) {
				if (jit) {
					this->_core = new altair::JitCore(pio, 0);

				} else {
					this->_core = new altair::InterpretedCore(pio, 0);
				}
			}

			void turn() {
				this->_core->turn();
			}

			void nexti() {
				this->_core->nexti();
			}

			void shutdown() {
				this->_core->shutdown();
			}

			uint8_t bR(BReg reg) const {
				return this->_core->bR(reg);
			}

			void bR(BReg r, uint8_t val) {
				this->_core->bR(r, val);
			}

			uint16_t wR(WReg reg) const {
				return this->_core->wR(reg);
			}

			void wR(WReg reg, uint16_t val) {
				this->_core->wR(reg, val);
			}

			uint8_t wRL(WReg reg) {
				return this->_core->wRL(reg);
			}

			uint8_t wRH(WReg reg) {
				return this->_core->wRH(reg);
			}

			void nextInstruction() {
				this->_core->nexti();
			}

			void nextCycle() {
				altair::InterpretedCore *c = dynamic_cast<altair::InterpretedCore *>(this->_core);
				if (c == nullptr) {
					throw std::runtime_error("JIT core does not support tick method!");
				}

				c->nextCycle();
			}

			void tick() {
				altair::InterpretedCore *c = dynamic_cast<altair::InterpretedCore *>(this->_core);
				if (c == nullptr) {
					throw std::runtime_error("JIT core does not support tick method!");
				}

				c->tick();
			}

			altair::InterpretedCore::Alu *alu() {
				return this->_core->alu();
			}

		private:
			altair::Core *_core;
	};
}

#endif /* TESTCORE_HPP_ */

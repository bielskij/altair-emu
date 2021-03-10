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
#ifndef CORE_INTERPRETEDCORE_MACHINECYCLE_INTERRUPT_HPP_
#define CORE_INTERPRETEDCORE_MACHINECYCLE_INTERRUPT_HPP_

#include "altair/Core.hpp"

namespace altair {
	class MachineCycleInterrupt : public InterpretedCore::MachineCycle {
		public:
			MachineCycleInterrupt(InterpretedCore *core) : InterpretedCore::MachineCycle(core, true, false, false, false, false, true, false, false) {
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				pio.setAddress(this->core()->wR(InterpretedCore::WReg::PC));
				pio.setData(this->getStatus());
				pio.setSync(true);

				// Disable interrupts
				pio.setInte(false);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = this->core()->pio();

				pio.setSync(false);
				pio.setDbin(true);

				// Clear internal interrupt flag
				core()->intFF(false);

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				this->core()->bR(InterpretedCore::BReg::IR, pio.getData());
				pio.setDbin(false);

				core()->bR(InterpretedCore::BReg::W, 0);

				return true;
			}

			bool t4() override {
				return true;
			}

			bool t5() override {
				core()->wR(InterpretedCore::WReg::SP, core()->wR(InterpretedCore::WReg::SP) - 1);

				return false;
			}
	};
}

#endif /* CORE_INTERPRETEDCORE_MACHINECYCLE_INTERRUPT_HPP_ */

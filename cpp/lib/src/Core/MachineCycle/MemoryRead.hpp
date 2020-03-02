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
#ifndef CORE_MACHINECYCLE_MEMORY_READ_HPP_
#define CORE_MACHINECYCLE_MEMORY_READ_HPP_

#include "altair/Core.hpp"

namespace altair {
	class MachineCycleMemoryRead : public Core::MachineCycle {
		public:
			/*!
			 * readFromPc true - address from PC, false - address from HL register pair
			 */
			MachineCycleMemoryRead(Core *core, bool readFromPc) : Core::MachineCycle(core, false, false, false, false, false, false, false, true) {
				this->readFromPc = readFromPc;
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				if (this->readFromPc) {
					pio.setAddress(this->core()->wR(Core::WReg::PC));
				} else {
					pio.setAddress(this->core()->wR(Core::WReg::H));
				}

				pio.setData(this->getStatus());
				pio.setSync(true);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = this->core()->pio();

				pio.setSync(false);
				pio.setDbin(true);

				if (this->readFromPc) {
					this->core()->wR(Core::WReg::PC, this->core()->wR(Core::WReg::PC) + 1);
				}

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				pio.setDbin(false);

				return false;
			}

		private:
			bool readFromPc;
	};
}

#endif /* CORE_MACHINECYCLE_MEMORY_READ_HPP_ */

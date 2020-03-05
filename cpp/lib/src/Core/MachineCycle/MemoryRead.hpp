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
			MachineCycleMemoryRead(Core *core, Core::WReg addr, Core::BReg dest, bool addressInc) : Core::MachineCycle(core, false, false, false, false, false, false, false, true) {
				this->address    = addr;
				this->dstReg     = dest;
				this->addressInc = addressInc;
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				if (this->address == Core::WReg::RP) {
					this->address = rp();
				}

				pio.setAddress(core()->wR(this->address));

				pio.setData(this->getStatus());
				pio.setSync(true);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = core()->pio();

				pio.setSync(false);
				pio.setDbin(true);

				if (this->addressInc) {
					core()->wR(this->address, core()->wR(this->address) + 1);
				}

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				if (this->dstReg == Core::BReg::RP_H) {
					core()->wRH(rp(), pio.getData());

				} else if (this->dstReg == Core::BReg::RP_L) {
					core()->wRL(rp(), pio.getData());

				} else {
					if (this->dstReg == Core::BReg::SSS) {
						this->dstReg = sss();

					} else if (this->dstReg == Core::BReg::DDD) {
						this->dstReg = ddd();
					}

					core()->bR(this->dstReg, pio.getData());
				}

				pio.setDbin(false);

				return false;
			}

		private:
			Core::WReg address;
			Core::BReg dstReg;
			bool       addressInc;
	};
}

#endif /* CORE_MACHINECYCLE_MEMORY_READ_HPP_ */

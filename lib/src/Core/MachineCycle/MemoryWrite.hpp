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
#ifndef CORE_MACHINECYCLE_MEMORY_WRITE_HPP_
#define CORE_MACHINECYCLE_MEMORY_WRITE_HPP_

#include <stdexcept>

#include "altair/Core.hpp"

namespace altair {
	class MachineCycleMemoryWrite : public Core::MachineCycle {
		public:
			MachineCycleMemoryWrite(Core *core, Core::WReg addr, Core::BReg src, bool addressInc) : Core::MachineCycle(core, false, true, false, false, false, false, false, false) {
				this->address    = addr;
				this->srcReg     = src;
				this->addressInc = addressInc;
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				if (this->address == Core::WReg::RP) {
					pio.setAddress(core()->wR(rp()));

				} else {
					pio.setAddress(core()->wR(this->address));
				}

				pio.setData(this->getStatus());
				pio.setSync(true);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = this->core()->pio();

				pio.setSync(false);

				if (this->addressInc) {
					if (this->address == Core::WReg::RP) {
						this->core()->wR(rp(), this->core()->wR(rp()) + 1);

					} else {
						this->core()->wR(this->address, this->core()->wR(this->address) + 1);
					}
				}

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				if (this->srcReg == Core::BReg::RP_H) {
					pio.setData(core()->wRH(rp()));

				} else if (this->srcReg == Core::BReg::RP_L) {
					pio.setData(core()->wRL(rp()));

				} else {
					if (this->srcReg == Core::BReg::SSS) {
						pio.setData(core()->bR(sss()));

					} else if (this->srcReg == Core::BReg::DDD) {
						pio.setData(core()->bR(ddd()));

					} else {
						pio.setData(core()->bR(this->srcReg));
					}
				}

				pio.setWr(true);

				return false;
			}

		private:
			Core::WReg address;
			Core::BReg srcReg;
			bool       addressInc;
	};
}

#endif /* CORE_MACHINECYCLE_MEMORY_WRITE_HPP_ */

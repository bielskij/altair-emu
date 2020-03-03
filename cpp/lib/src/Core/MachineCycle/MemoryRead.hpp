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
			enum Address {
				PC_INC,
				HL,
				WZ,
				WZ_INC
			};

		public:
			/*!
			 * readFromPc true - address from PC, false - address from HL register pair
			 */
			MachineCycleMemoryRead(Core *core, Address addr) : Core::MachineCycle(core, false, false, false, false, false, false, false, true) {
				this->address = addr;
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				switch (this->address) {
					case Address::PC_INC:
						pio.setAddress(this->core()->wR(Core::WReg::PC));
						break;

					case Address::HL:
						pio.setAddress(this->core()->wR(Core::WReg::H));
						break;

					case Address::WZ:
					case Address::WZ_INC:
						pio.setAddress(this->core()->wR(Core::WReg::W));
						break;
				}

				pio.setData(this->getStatus());
				pio.setSync(true);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = this->core()->pio();

				pio.setSync(false);
				pio.setDbin(true);

				if (this->address == Address::PC_INC) {
					this->core()->wR(Core::WReg::PC, this->core()->wR(Core::WReg::PC) + 1);

				} else if (this->address == Address::WZ_INC) {
					this->core()->wR(Core::WReg::W, this->core()->wR(Core::WReg::W) + 1);
				}

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				pio.setDbin(false);

				return false;
			}

		private:
			Address address;
	};
}

#endif /* CORE_MACHINECYCLE_MEMORY_READ_HPP_ */

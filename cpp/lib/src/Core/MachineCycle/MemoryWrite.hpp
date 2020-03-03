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
			enum Address {
				HL,
				WZ,
				WZ_INC,
				B,
				D
			};

		public:
			MachineCycleMemoryWrite(Core *core) : Core::MachineCycle(core, false, true, false, false, false, false, false, false) {
				this->addr = Address::HL;
			}

			MachineCycleMemoryWrite(Core *core, Address address) : Core::MachineCycle(core, false, true, false, false, false, false, false, false) {
				this->addr = address;
			}

			bool t1() override {
				Core::Pio &pio = this->core()->pio();

				switch (this->addr) {
					case Address::HL:
						pio.setAddress(this->core()->wR(Core::WReg::H));
						break;

					case Address::WZ:
					case Address::WZ_INC:
						pio.setAddress(this->core()->wR(Core::WReg::W));
						break;

					case Address::B:
						pio.setAddress(this->core()->wR(Core::WReg::B));
						break;

					case Address::D:
						pio.setAddress(this->core()->wR(Core::WReg::D));
						break;

					default:
						throw std::runtime_error("Invalid addressing!");
				}

				pio.setData(this->getStatus());
				pio.setSync(true);

				return true;
			}

			bool t2() override {
				Core::Pio &pio = this->core()->pio();

				pio.setSync(false);

				if (this->addr == Address::WZ_INC) {
					this->core()->wR(Core::WReg::W, this->core()->wR(Core::WReg::W) + 1);
				}

				return true;
			}

			bool t3() override {
				Core::Pio &pio = this->core()->pio();

				pio.setWr(true);

				return false;
			}

		protected:
			void setAddress(Address addr) {
				this->addr = addr;
			}

		private:
			Address addr;
	};
}

#endif /* CORE_MACHINECYCLE_MEMORY_WRITE_HPP_ */

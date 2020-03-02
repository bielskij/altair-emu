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
#ifndef CORE_INSTRUCTION_MOV_HPP_
#define CORE_INSTRUCTION_MOV_HPP_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionMovRR : public Core::Instruction {
		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core) : MachineCycleFetch(core) {
					}

					bool t3() override {
						return true;
					}

					bool t4() override {
						// SSS -> TMP
						this->core()->bR(
							Core::BReg::TMP,
							this->core()->bR(Utils::binToBreg(Utils::getBits(0, 3)))
						);

						return true;
					}

					bool t5() override {
						// TMP -> DDD
						this->core()->bR(
							Utils::binToBreg(Utils::getBits(3, 3)),
							this->core()->bR(Core::BReg::TMP)
						);

						return false;
					}
			};

		public:
			InstructionMovRR(Core *core) : Instruction(core) {
				this->addCycle(new Fetch(core));
			}
	};
}

#endif /* CORE_INSTRUCTION_MOV_HPP_ */
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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_DCX_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_DCX_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"
#include "Core/InterpretedCore/MachineCycle/MemoryRead.hpp"

namespace altair {
	class InstructionDcx : public InterpretedCore::Instruction {
		private:
			class Fetch : public MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						MachineCycleFetch::t4();

						return true;
					}

					bool t5() override {
						core()->wR(rp(), core()->wR(rp()) - 1);

						return false;
					}
			};

		public:
			InstructionDcx(InterpretedCore *core) : Instruction(core) {
				this->addCycle(new Fetch(core));

				this->addCodeRP(0x0b, InterpretedCore::WReg::COUNT);
			}

			std::string toAsm() const override {
				return "dcx " + Utils::rpToString(rp(core()));
			}
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_DCX_H_ */

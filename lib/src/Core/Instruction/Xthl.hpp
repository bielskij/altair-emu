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
#ifndef CORE_INSTRUCTION_XTHL_H_
#define CORE_INSTRUCTION_XTHL_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/StackRead.hpp"
#include "Core/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionXthl: public Core::Instruction {
		private:
			class StackWrite : public MachineCycleStackWrite {
				public:
					StackWrite(Core *core) : MachineCycleStackWrite(core, Core::BReg::L, false) {
					}

					bool t3() override {
						MachineCycleStackWrite::t3();

						return true;
					}

					bool t4() override {
						MachineCycleStackWrite::t4();

						return true;
					}

					bool t5() override {
						core()->wR(Core::WReg::H, core()->wR(Core::WReg::W));

						return false;
					}
			};

		public:
			InstructionXthl(Core *core) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new MachineCycleStackRead(core, Core::BReg::Z, true));
				this->addCycle(new MachineCycleStackRead(core, Core::BReg::W, false));
				this->addCycle(new MachineCycleStackWrite(core, Core::BReg::H, true));
				this->addCycle(new StackWrite(core));

				this->addCode(0xe3);
			}

			std::string toAsm() const override {
				return "xthl";
			}
	};
}

#endif /* CORE_INSTRUCTION_XTHL_H_ */

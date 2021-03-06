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
#ifndef CORE_INSTRUCTION_RST_H_
#define CORE_INSTRUCTION_RST_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Interrupt.hpp"
#include "Core/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionRst : public Core::Instruction {
		private:
			class StackWrite : public altair::MachineCycleStackWrite {
				public:
					StackWrite(Core *core) : MachineCycleStackWrite(core, Core::BReg::PCL, false) {
					}

					bool t3() override {
						bool ret = MachineCycleStackWrite::t3();

						core()->bR(Core::BReg::Z, core()->bR(Core::BReg::IR) & 0x38);
						core()->wR(Core::WReg::PC, core()->wR(Core::WReg::W));

						return ret;
					}
			};

		public:
			InstructionRst(Core *core) : Instruction(core) {
				this->addCycle(new MachineCycleInterrupt(core));
				this->addCycle(new MachineCycleStackWrite(core, Core::BReg::PCH, true));
				this->addCycle(new StackWrite(core));

				this->addCodeNNN(0xc7);
			}

			std::string toAsm() const override {
				return "rst " + common::Utils::uint8ToString(nnn(core()));
			}
	};
}

#endif /* CORE_INSTRUCTION_RST_H_ */

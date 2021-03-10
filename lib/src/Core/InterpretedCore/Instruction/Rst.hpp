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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_RST_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_RST_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Interrupt.hpp"
#include "Core/InterpretedCore/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionRst : public InterpretedCore::Instruction {
		private:
			class StackWrite : public altair::MachineCycleStackWrite {
				public:
					StackWrite(InterpretedCore *core) : MachineCycleStackWrite(core, InterpretedCore::BReg::PCL, false) {
					}

					bool t3() override {
						bool ret = MachineCycleStackWrite::t3();

						core()->bR(InterpretedCore::BReg::Z, core()->bR(InterpretedCore::BReg::IR) & 0x38);
						core()->wR(InterpretedCore::WReg::PC, core()->wR(InterpretedCore::WReg::W));

						return ret;
					}
			};

		public:
			InstructionRst(InterpretedCore *core) : Instruction(core) {
				this->addCycle(new MachineCycleInterrupt(core));
				this->addCycle(new MachineCycleStackWrite(core, InterpretedCore::BReg::PCH, true));
				this->addCycle(new StackWrite(core));

				this->addCodeNNN(0xc7);
			}

			std::string toAsm() const override {
				return "rst " + common::Utils::uint8ToString(nnn(core()));
			}
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_RST_H_ */

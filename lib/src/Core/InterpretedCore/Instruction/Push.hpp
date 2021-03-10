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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_PUSH_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_PUSH_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"
#include "Core/InterpretedCore/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionPush : public InterpretedCore::Instruction {
		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						this->MachineCycleFetch::t4();

						return true;
					}

					bool t5() override {
						core()->wR(InterpretedCore::WReg::SP, core()->wR(InterpretedCore::WReg::SP) - 1);

						return false;
					}
			};

		public:
			InstructionPush(InterpretedCore *core, bool pushRp) : Instruction(core) {
				this->_pushRp = pushRp;

				this->addCycle(new Fetch(core));

				if (pushRp) {
					this->addCycle(new MachineCycleStackWrite(core, InterpretedCore::BReg::RP_H, true));
					this->addCycle(new MachineCycleStackWrite(core, InterpretedCore::BReg::RP_L, false));

					// SP reg cannot be specified
					this->addCodeRP(0xc5, InterpretedCore::WReg::B);
					this->addCodeRP(0xc5, InterpretedCore::WReg::D);
					this->addCodeRP(0xc5, InterpretedCore::WReg::H);

				} else {
					this->addCycle(new MachineCycleStackWrite(core, InterpretedCore::BReg::A, true));
					this->addCycle(new MachineCycleStackWrite(core, InterpretedCore::BReg::F, false));

					this->addCode(0xf5);
				}
			}

			std::string toAsm() const override {
				if (this->_pushRp) {
					return "push " + Utils::wregToString(rp(core()));
				} else {
					return "push psw";
				}
			}

		private:
			bool _pushRp;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_PUSH_H_ */

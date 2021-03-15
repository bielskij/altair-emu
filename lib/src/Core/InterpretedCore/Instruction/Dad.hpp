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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_DAD_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_DAD_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionDad : public InterpretedCore::Instruction {
		private:
			class InternalCycle : public InterpretedCore::MachineCycle {
				public:
					InternalCycle(InterpretedCore *core, bool lowByte) : MachineCycle(core, false, false, false, false, false, false, false, false) {
						this->lowByte = lowByte;
					}

					bool t1() override {
						if (this->lowByte) {
							core()->bR(InterpretedCore::BReg::TMP, core()->bR(InterpretedCore::BReg::L));

							alu()->op(InterpretedCore::wRegL(rp()), InterpretedCore::BReg::L, InterpretedCore::AluImpl::Op::ADD, false, InterpretedCore::AluImpl::CY, 2);

						} else {
							core()->bR(InterpretedCore::BReg::TMP, core()->bR(InterpretedCore::BReg::H));

							alu()->op(InterpretedCore::wRegH(rp()), InterpretedCore::BReg::H, InterpretedCore::AluImpl::Op::ADD, true, InterpretedCore::AluImpl::CY, 2);
						}

						return true;
					}

					bool t2() override {
						return true;
					}

					bool t3() override {
						return false;
					}

				private:
					bool lowByte;
			};

		public:
			InstructionDad(InterpretedCore *core) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new InternalCycle(core, true));
				this->addCycle(new InternalCycle(core, false));

				this->addCodeRP(0x09, InterpretedCore::WReg::COUNT);
			}

			std::string toAsm() const override {
				return "dad " + Utils::rpToString(rp(core()));
			}
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_DAD_H_ */

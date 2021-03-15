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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_INR_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_INR_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"
#include "Core/InterpretedCore/MachineCycle/MemoryRead.hpp"

namespace altair {
	class InstructionInr : public InterpretedCore::Instruction {
		public:
			enum Mode {
				R,
				M
			};

		private:
			class Fetch : public MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(ddd()));

						alu()->op(1, ddd(), InterpretedCore::AluImpl::Op::ADD, false,
							InterpretedCore::AluImpl::Z | InterpretedCore::AluImpl::S | InterpretedCore::AluImpl::P | InterpretedCore::AluImpl::AC,
							1
						);

						return true;
					}

					bool t5() override {
						return false;
					}
			};

			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(InterpretedCore *core) : MachineCycleMemoryRead(core, InterpretedCore::WReg::H, InterpretedCore::BReg::TMP, false) {
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						alu()->op(1, InterpretedCore::BReg::ACT, InterpretedCore::AluImpl::Op::ADD, false,
							InterpretedCore::AluImpl::Z | InterpretedCore::AluImpl::S | InterpretedCore::AluImpl::P | InterpretedCore::AluImpl::AC,
							1
						);

						return ret;
					}
			};

		public:
			InstructionInr(InterpretedCore *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch(mode) {
					case Mode::R:
						{
							this->addCycle(new Fetch(core));

							this->addCodeDDD(0x04, InterpretedCore::BReg::COUNT);
						}
						break;

					case Mode::M:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core));
							this->addCycle(new MachineCycleMemoryWrite(core, InterpretedCore::WReg::H, InterpretedCore::BReg::ACT, false));

							this->addCode(0x34);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret = "inr ";

				switch (this->_mode) {
					case Mode::R:
						ret += Utils::bregToString(ddd(core()));
						break;

					case Mode::M:
						ret += "M";
						break;

					default:
						throw std::invalid_argument("Not supported INR opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_INR_H_ */

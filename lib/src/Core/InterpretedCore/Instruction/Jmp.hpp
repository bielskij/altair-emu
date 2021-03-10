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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_JMP_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_JMP_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionJmp : public InterpretedCore::Instruction {
		public:
			enum Mode {
				UNCONDITIONED,
				CONDITIONED
			};

		private:
			class Read : public altair::MachineCycleMemoryRead {
				public:
					Read(InterpretedCore *core) : MachineCycleMemoryRead(core, InterpretedCore::WReg::PC, InterpretedCore::BReg::W, true) {
						this->conditionTrue = true;
					}

					bool t3() override {
						bool ret = MachineCycleMemoryRead::t3();

						if (this->conditionTrue) {
							core()->wR(InterpretedCore::WReg::PC, core()->wR(InterpretedCore::WReg::W));
						}

						return ret;
					}

				public:
					bool conditionTrue;
			};

			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core, Read *readCycle) : MachineCycleFetch(core) {
						this->_readCycle = readCycle;
					}

					bool t4() override {
						this->MachineCycleFetch::t4();

						this->_readCycle->conditionTrue = core()->alu()->checkCondition(ccc());

						return false;
					}

				private:
					Read *_readCycle;
			};

		public:
			InstructionJmp(InterpretedCore *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::UNCONDITIONED:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MachineCycleMemoryRead(core, InterpretedCore::WReg::PC, InterpretedCore::BReg::Z, true));
							this->addCycle(new Read(core));

							this->addCode(0xc3);
						}
						break;

					case Mode::CONDITIONED:
						{
							Read *readCycle = new Read(core);

							this->addCycle(new Fetch(core, readCycle));
							this->addCycle(new MachineCycleMemoryRead(core, InterpretedCore::WReg::PC, InterpretedCore::BReg::Z, true));
							this->addCycle(readCycle);

							this->addCodeCCC(0xc2, InterpretedCore::Cond::COUNT);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret;

				switch (this->_mode) {
					case Mode::CONDITIONED:
						ret = "j" + Utils::condToString(ccc(core())) + " " + common::Utils::uint16ToString(core()->wR(InterpretedCore::WReg::W), true);
						break;

					case Mode::UNCONDITIONED:
						ret = "jmp " + common::Utils::uint16ToString(core()->wR(InterpretedCore::WReg::W), true);
						break;

					default:
						throw std::invalid_argument("Not supported JMP opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_ADD_H_ */

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
#ifndef CORE_INSTRUCTION_CALL_H_
#define CORE_INSTRUCTION_CALL_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionCall : public Core::Instruction {
		public:
			enum Mode {
				UNCONDITIONED,
				CONDITIONED
			};

		private:
			class Read : public altair::MachineCycleMemoryRead {
				public:
					Read(Core *core) : MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::W, true) {
						this->conditionTrue = true;
					}

					bool t3() override {
						bool ret = MachineCycleMemoryRead::t3();

						if (! this->conditionTrue) {
							parent()->finish();
						}

						return ret;
					}

				public:
					bool conditionTrue;
			};

			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core, Read *readCycle, Mode mode) : MachineCycleFetch(core) {
						this->_readCycle = readCycle;
						this->_mode      = mode;
					}

					bool t4() override {
						this->MachineCycleFetch::t4();

						return true;
					}

					bool t5() override {
						bool decreaseStack = true;

						if (this->_mode == Mode::CONDITIONED) {
							this->_readCycle->conditionTrue = core()->alu()->checkCondition(ccc());
							if (! this->_readCycle->conditionTrue) {
								decreaseStack = false;
							}
						}

						if (decreaseStack) {
							core()->wR(Core::WReg::SP, core()->wR(Core::WReg::SP) - 1);
						}

						return false;
					}

				private:
					Read *_readCycle;
					Mode  _mode;
			};

			class StackWrite : public altair::MachineCycleStackWrite {
				public:
					StackWrite(Core *core) : MachineCycleStackWrite(core, Core::BReg::PCL, false) {
					}

					bool t3() override {
						bool ret = MachineCycleStackWrite::t3();

						core()->wR(Core::WReg::PC, core()->wR(Core::WReg::W));

						return ret;
					}
			};

		public:
			InstructionCall(Core *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::UNCONDITIONED:
						{
							this->addCycle(new Fetch(core, nullptr, mode));
							this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::Z, true));
							this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::W, true));
							this->addCycle(new MachineCycleStackWrite(core, Core::BReg::PCH, true));
							this->addCycle(new StackWrite(core));

							this->addCode(0xcd);
						}
						break;

					case Mode::CONDITIONED:
						{
							Read *readCycle = new Read(core);

							this->addCycle(new Fetch(core, readCycle, mode));
							this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::Z, true));
							this->addCycle(readCycle);
							this->addCycle(new MachineCycleStackWrite(core, Core::BReg::PCH, true));
							this->addCycle(new StackWrite(core));

							this->addCodeCCC(0xc4, Core::Cond::COUNT);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret;

				switch (this->_mode) {
					case Mode::CONDITIONED:
						ret = "j" + Utils::condToString(ccc(core())) + " " + common::Utils::uint16ToString(core()->wR(Core::WReg::W), true);
						break;

					case Mode::UNCONDITIONED:
						ret = "call " + common::Utils::uint16ToString(core()->wR(Core::WReg::W), true);
						break;

					default:
						throw std::invalid_argument("Not supported CALL opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INSTRUCTION_CALL_H_ */

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
#ifndef CORE_INSTRUCTION_RET_H_
#define CORE_INSTRUCTION_RET_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/StackRead.hpp"

namespace altair {
	class InstructionRet : public Core::Instruction {
		public:
			enum Mode {
				UNCONDITIONED,
				CONDITIONED
			};

		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core, Mode mode) : MachineCycleFetch(core) {
						this->mode = mode;
					}

					bool t4() override {
						bool ret = this->MachineCycleFetch::t4();

						if (this->mode == Mode::CONDITIONED) {
							if (! core()->alu()->checkCondition(ccc())) {
								parent()->finish();
							}
						}

						return mode == Mode::CONDITIONED;
					}

				private:
					Mode mode;
			};

			class StackRead : public altair::MachineCycleStackRead {
				public:
					StackRead(Core *core) : MachineCycleStackRead(core, Core::BReg::W, true) {
					}

					bool t3() override {
						bool ret = MachineCycleStackRead::t3();

						core()->wR(Core::WReg::PC, core()->wR(Core::WReg::W));

						return ret;
					}
			};

		public:
			InstructionRet(Core *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::UNCONDITIONED:
						{
							this->addCycle(new Fetch(core, mode));
							this->addCycle(new MachineCycleStackRead(core, Core::BReg::Z, true));
							this->addCycle(new StackRead(core));

							this->addCode(0xc9);
						}
						break;

					case Mode::CONDITIONED:
						{
							this->addCycle(new Fetch(core, mode));
							this->addCycle(new MachineCycleStackRead(core, Core::BReg::Z, true));
							this->addCycle(new StackRead(core));

							this->addCodeCCC(0xc0, Core::Cond::COUNT);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret;

				switch (this->_mode) {
					case Mode::CONDITIONED:
						ret = "r" + Utils::condToString(ccc(core())) + " " + common::Utils::uint16ToString(core()->wR(Core::WReg::W), true);
						break;

					case Mode::UNCONDITIONED:
						ret = "ret " + common::Utils::uint16ToString(core()->wR(Core::WReg::W), true);
						break;

					default:
						throw std::invalid_argument("Not supported RET opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INSTRUCTION_CALL_H_ */

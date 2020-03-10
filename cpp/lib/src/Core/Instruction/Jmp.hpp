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
#ifndef CORE_INSTRUCTION_JMP_H_
#define CORE_INSTRUCTION_JMP_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionJmp : public Core::Instruction {
		public:
			enum Mode {
				UNCONDITIONED,
				CONDITIONED
			};

		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						this->MachineCycleFetch::t4();

						return true;
					}

					bool t5() override {
						if (! core()->alu()->checkCondition(ccc())) {
							// Skip address reading
							core()->wR(Core::WReg::PC, core()->wR(Core::WReg::PC) + 2);

							parent()->finish();
						}

						return false;
					}
			};

			class Read : public altair::MachineCycleMemoryRead {
				public:
					Read(Core *core) : MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::W, true) {
					}

					bool t3() override {
						bool ret = MachineCycleMemoryRead::t3();

						core()->wR(Core::WReg::PC, core()->wR(Core::WReg::W));

						return ret;
					}
			};

		public:
			InstructionJmp(Core *core, Mode mode) : Instruction(core) {
				switch (mode) {
					case Mode::UNCONDITIONED:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::Z, true));
							this->addCycle(new Read(core));

							this->addCode(0xc3);
						}
						break;

					case Mode::CONDITIONED:
						{
							this->addCycle(new Fetch(core));
							this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::Z, true));
							this->addCycle(new Read(core));

							this->addCodeCCC(0xc2, Core::Cond::COUNT);
						}
						break;
				}
			}
	};
}

#endif /* CORE_INSTRUCTION_ADD_H_ */

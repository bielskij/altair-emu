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
#ifndef CORE_INSTRUCTION_DCRM_H_
#define CORE_INSTRUCTION_DCRM_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/MemoryRead.hpp"

namespace altair {
	class InstructionDcrM : public Core::Instruction {
		private:
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core) : MachineCycleMemoryRead(core, Core::WReg::H, Core::BReg::TMP, false) {
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						core()->alu()->op(0xff, Core::BReg::ACT, Core::Alu::Op::ADD, false,
							Core::Alu::Z | Core::Alu::S | Core::Alu::P | Core::Alu::AC,
							1
						);

						return ret;
					}
			};

		public:
			InstructionDcrM(Core *core) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new MemoryRead(core));
				this->addCycle(new MachineCycleMemoryWrite(core, Core::WReg::H, Core::BReg::ACT, false));
			}
	};
}

#endif /* CORE_INSTRUCTION_DCRM_H_ */
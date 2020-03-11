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
#ifndef CORE_INSTRUCTION_POP_H_
#define CORE_INSTRUCTION_POP_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/StackWrite.hpp"

namespace altair {
	class InstructionPop : public Core::Instruction {
		public:
			InstructionPop(Core *core, bool popRp) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));

				if (popRp) {
					this->addCycle(new MachineCycleStackRead(core, Core::BReg::RP_L, true));
					this->addCycle(new MachineCycleStackRead(core, Core::BReg::RP_H, true));

					// SP reg cannot be specified
					this->addCodeRP(0xc1, Core::WReg::B);
					this->addCodeRP(0xc1, Core::WReg::D);
					this->addCodeRP(0xc1, Core::WReg::H);

				} else {
					this->addCycle(new MachineCycleStackRead(core, Core::BReg::F, true));
					this->addCycle(new MachineCycleStackRead(core, Core::BReg::A, true));

					this->addCode(0xf1);
				}
			}
	};
}

#endif /* CORE_INSTRUCTION_POP_H_ */

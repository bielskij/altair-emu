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
#ifndef CORE_INSTRUCTION_LHLD_H_
#define CORE_INSTRUCTION_LHLD_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/MemoryRead.hpp"

namespace altair {
	class InstructionLhld : public Core::Instruction {
		private:
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core, MachineCycleMemoryRead::Address addr, Core::BReg dest) : MachineCycleMemoryRead(core, addr) {
						this->destination = dest;
					}

					bool t3() override {
						core()->bR(this->destination, core()->pio().getData());

						return this->MachineCycleMemoryRead::t3();
					}

				private:
					Core::BReg destination;
			};

		public:
			InstructionLhld(Core *core) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new MemoryRead(core, MachineCycleMemoryRead::Address::PC_INC, Core::BReg::Z));
				this->addCycle(new MemoryRead(core, MachineCycleMemoryRead::Address::PC_INC, Core::BReg::W));
				this->addCycle(new MemoryRead(core, MachineCycleMemoryRead::Address::WZ_INC, Core::BReg::L));
				this->addCycle(new MemoryRead(core, MachineCycleMemoryRead::Address::WZ,     Core::BReg::H));
			}
	};
}

#endif /* CORE_INSTRUCTION_LHLD_H_ */

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
#ifndef CORE_INSTRUCTION_LXI_H_
#define CORE_INSTRUCTION_LXI_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/MemoryRead.hpp"
#include "Core/MachineCycle/MemoryWrite.hpp"

namespace altair {
	class InstructionLxi : public Core::Instruction {
		private:
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core, bool lowbyte) : MachineCycleMemoryRead(core, MachineCycleMemoryRead::Address::PC_INC) {
						this->lowByte = lowbyte;
					}

					bool t3() override {
						if (this->lowByte) {
							core()->wRL(rp(), core()->pio().getData());

						} else {
							core()->wRH(rp(), core()->pio().getData());
						}

						return this->MachineCycleMemoryRead::t3();
					}

				private:
					bool lowByte;
			};

		public:
			InstructionLxi(Core *core) : Instruction(core) {
				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new MemoryRead(core, true));
				this->addCycle(new MemoryRead(core, false));
			}
	};
}

#endif /* CORE_INSTRUCTION_LXI_H_ */

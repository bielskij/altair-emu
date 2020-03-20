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
#ifndef CORE_INSTRUCTION_IO_H_
#define CORE_INSTRUCTION_IO_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/MemoryRead.hpp"
#include "Core/MachineCycle/InputRead.hpp"
#include "Core/MachineCycle/OutputWrite.hpp"

namespace altair {
	class InstructionIo : public Core::Instruction {
		private:
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core) : MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::Z, true) {
					}

					bool t3() override {
						bool ret = MachineCycleMemoryRead::t3();

						core()->bR(Core::BReg::W, core()->bR(Core::BReg::Z));

						return ret;
					}
			};

		public:
			InstructionIo(Core *core, bool inOperation) : Instruction(core) {
				this->_inOperation = inOperation;

				this->addCycle(new MachineCycleFetch(core));
				this->addCycle(new MemoryRead(core));

				if (inOperation) {
					this->addCycle(new MachineCycleInputRead(core));

					this->addCode(0xdb);

				} else {
					this->addCycle(new MachineCycleOutputWrite(core));

					this->addCode(0xd3);
				}
			}

			std::string toAsm() const override {
				return (this->_inOperation ? "in " : "out ") + common::Utils::uint8ToString(core()->bR(Core::BReg::Z));
			}

		private:
			bool _inOperation;
	};
}

#endif /* CORE_INSTRUCTION_IO_H_ */

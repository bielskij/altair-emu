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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_IO_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_IO_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"
#include "Core/InterpretedCore/MachineCycle/MemoryRead.hpp"
#include "Core/InterpretedCore/MachineCycle/InputRead.hpp"
#include "Core/InterpretedCore/MachineCycle/OutputWrite.hpp"

namespace altair {
	class InstructionIo : public InterpretedCore::Instruction {
		private:
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(InterpretedCore *core) : MachineCycleMemoryRead(core, InterpretedCore::WReg::PC, InterpretedCore::BReg::Z, true) {
					}

					bool t3() override {
						bool ret = MachineCycleMemoryRead::t3();

						core()->bR(InterpretedCore::BReg::W, core()->bR(InterpretedCore::BReg::Z));

						return ret;
					}
			};

		public:
			InstructionIo(InterpretedCore *core, bool inOperation) : Instruction(core) {
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
				return (this->_inOperation ? "in " : "out ") + common::Utils::uint8ToString(core()->bR(InterpretedCore::BReg::Z));
			}

		private:
			bool _inOperation;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_IO_H_ */

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
#ifndef CORE_INSTRUCTION_MVI_H_
#define CORE_INSTRUCTION_MVI_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"
#include "Core/MachineCycle/MemoryRead.hpp"
#include "Core/MachineCycle/MemoryWrite.hpp"

namespace altair {
	class InstructionMvi : public Core::Instruction {
		public:
			enum Mode {
				R,
				M
			};

		public:
			InstructionMvi(Core *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				if (mode == Mode::R) {
					this->addCycle(new MachineCycleFetch(core));
					this->addCycle(new MachineCycleMemoryRead(core, Core::WReg::PC, Core::BReg::DDD, true));

					this->addCodeDDD(0x06, Core::BReg::COUNT);

				} else {
					this->addCycle(new MachineCycleFetch(core));
					this->addCycle(new MachineCycleMemoryRead (core, Core::WReg::PC, Core::BReg::TMP, true));
					this->addCycle(new MachineCycleMemoryWrite(core, Core::WReg::H,  Core::BReg::TMP, false));

					this->addCode(0x36);
				}
			}

			std::string toAsm() const override {
				std::string ret = "mvi ";

				switch (this->_mode) {
					case Mode::R:
						ret += (
							altair::Utils::bregToString(ddd(core())) +
							"," + common::Utils::uint8ToString((core()->bR(ddd(core()))))
						);
						break;

					case Mode::M:
						ret += ("M," + common::Utils::uint8ToString(core()->bR(Core::BReg::TMP)));
						break;

					default:
						throw std::invalid_argument("Not supported MOV opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INSTRUCTION_MVI_H_ */

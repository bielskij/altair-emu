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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_DAA_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_DAA_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionDaa : public InterpretedCore::Instruction {
		private:
			class Fetch : public MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::A, InterpretedCore::AluImpl::Op::AD, false,
							InterpretedCore::AluImpl::Z | InterpretedCore::AluImpl::S | InterpretedCore::AluImpl::P | InterpretedCore::AluImpl::CY | InterpretedCore::AluImpl::AC,
							0
						);

						return MachineCycleFetch::t4();
					}
			};

		public:
			InstructionDaa(InterpretedCore *core) : Instruction(core) {
				this->addCycle(new Fetch(core));

				this->addCode(0x27);
			}

			std::string toAsm() const override {
				return "daa";
			}
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_DAA_H_ */

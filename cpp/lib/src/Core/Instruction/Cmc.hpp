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
#ifndef CORE_INSTRUCTION_CMC_H_
#define CORE_INSTRUCTION_CMC_H_

#include "altair/Core.hpp"
#include "altair/Utils.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionCmc: public Core::Instruction {
		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						if (core()->alu()->fCY()) {
							core()->alu()->clear(Core::Alu::CY);

						} else {
							core()->alu()->set(Core::Alu::CY);
						}

						return false;
					}
			};

		public:
			InstructionCmc(Core *core) : Instruction(core) {
				this->addCycle(new Fetch(core));

				this->addCode(0x3f);
			}
	};
}

#endif /* CORE_INSTRUCTION_CMC_H_ */

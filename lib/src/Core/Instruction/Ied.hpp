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
#ifndef CORE_INSTRUCTION_IED_H_
#define CORE_INSTRUCTION_IED_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionIed: public Core::Instruction {
		private:
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core, bool enable) : MachineCycleFetch(core) {
						this->enable = enable;
					}

					bool t4() override {
						core()->pio().setInte(this->enable);

						return false;
					}

				private:
					bool enable;
			};

		public:
			InstructionIed(Core *core, bool enable) : Instruction(core) {
				this->_iee = enable;

				this->addCycle(new Fetch(core, enable));

				this->addCode(enable ? 0xfb : 0xf3);
			}

			std::string toAsm() const override {
				return this->_iee ? "ei" : "di";
			}

		private:
			bool _iee;
	};
}

#endif /* CORE_INSTRUCTION_IED_H_ */

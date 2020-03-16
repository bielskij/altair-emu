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
#ifndef CORE_INSTRUCTION_ADD_H_
#define CORE_INSTRUCTION_ADD_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionAdd : public Core::Instruction {
		public:
			enum Mode {
				R,
				M,
				I
			};

		private:
			// ----------- R
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(Core *core, bool withCarry) : MachineCycleFetch(core) {
						this->withCarry = withCarry;
					}

					bool t4() override {
						core()->bR(Core::BReg::TMP, core()->bR(sss()));

						core()->alu()->op(Core::BReg::A, Core::BReg::A, Core::Alu::Op::ADD, this->withCarry,
							Core::Alu::Z | Core::Alu::S | Core::Alu::P | Core::Alu::CY | Core::Alu::AC,
							2
						);

						return false;
					}

				private:
					bool withCarry;
			};

			// ----------- M, I
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core, Core::WReg srcReg, bool withCarry) : MachineCycleMemoryRead(core, srcReg, Core::BReg::TMP, false) {
						this->withCarry = withCarry;
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						core()->alu()->op(Core::BReg::A, Core::BReg::A, Core::Alu::Op::ADD, this->withCarry,
							Core::Alu::Z | Core::Alu::S | Core::Alu::P | Core::Alu::CY | Core::Alu::AC,
							2
						);

						return ret;
					}

				private:
					bool withCarry;
			};

		public:
			InstructionAdd(Core *core, Mode mode, bool withCarry) : Instruction(core) {
				switch (mode) {
					case Mode::R:
						{
							this->addCycle(new Fetch(core, withCarry));

							this->addCodeSSS(withCarry ? 0x88 : 0x80, Core::BReg::COUNT);
						}
						break;

					case Mode::M:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, Core::WReg::H, withCarry));

							this->addCode(withCarry ? 0x8e : 0x86);
						}
						break;

					case Mode::I:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, Core::WReg::PC, withCarry));

							this->addCode(withCarry ? 0xce : 0xc6);
						}
						break;
				}
			}
	};
}

#endif /* CORE_INSTRUCTION_ADD_H_ */

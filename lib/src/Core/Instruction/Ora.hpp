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
#ifndef CORE_INSTRUCTION_ORA_H_
#define CORE_INSTRUCTION_ORA_H_

#include "altair/Core.hpp"
#include "Core/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionOra : public Core::Instruction {
		public:
			enum Mode {
				R,
				M,
				I
			};

		private:
			class FetchR : public altair::MachineCycleFetch {
				public:
					FetchR(Core *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						core()->bR(Core::BReg::TMP, core()->bR(sss()));

						core()->alu()->op(Core::BReg::A, Core::BReg::A, Core::Alu::Op::OR, false,
							Core::Alu::Z | Core::Alu::S | Core::Alu::P | Core::Alu::CY | Core::Alu::AC,
							2
						);

						return false;
					}
			};

			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(Core *core, bool mem) : MachineCycleMemoryRead(core, mem ? Core::WReg::H : Core::WReg::PC, Core::BReg::TMP, ! mem) {
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						core()->alu()->op(Core::BReg::A, Core::BReg::A, Core::Alu::Op::OR, false,
							Core::Alu::Z | Core::Alu::S | Core::Alu::P | Core::Alu::CY | Core::Alu::AC,
							2
						);

						return ret;
					}
			};

		public:
			InstructionOra(Core *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::R:
						{
							this->addCycle(new FetchR(core));

							this->addCodeSSS(0xb0, Core::BReg::COUNT);
						}
						break;

					case Mode::M:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, true));

							this->addCode(0xb6);
						}
						break;

					case Mode::I:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, false));

							this->addCode(0xf6);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret = (this->_mode == Mode::I) ? "ori " : "ora ";

				switch (this->_mode) {
					case Mode::R:
						ret += altair::Utils::bregToString(sss(core()));
						break;

					case Mode::M:
						ret += "M";
						break;

					case Mode::I:
						ret += common::Utils::uint8ToString(core()->bR(Core::BReg::TMP));
						break;

					default:
						throw std::invalid_argument("Not supported ORA opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}


#endif /* CORE_INSTRUCTION_ORA_H_ */

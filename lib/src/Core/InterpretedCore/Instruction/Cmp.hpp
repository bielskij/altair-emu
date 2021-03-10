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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_CMP_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_CMP_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionCmp : public InterpretedCore::Instruction {
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
					Fetch(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(sss()));

						core()->alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::COUNT, InterpretedCore::Alu::Op::SUB, false,
							InterpretedCore::Alu::Z | InterpretedCore::Alu::S | InterpretedCore::Alu::P | InterpretedCore::Alu::CY | InterpretedCore::Alu::AC,
							2
						);

						return false;
					}
			};

			// ----------- M, I
			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(InterpretedCore *core, InterpretedCore::WReg srcReg, bool inc) : MachineCycleMemoryRead(core, srcReg, InterpretedCore::BReg::TMP, inc) {
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						core()->alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::COUNT, InterpretedCore::Alu::Op::SUB, false,
							InterpretedCore::Alu::Z | InterpretedCore::Alu::S | InterpretedCore::Alu::P | InterpretedCore::Alu::CY | InterpretedCore::Alu::AC,
							2
						);

						return ret;
					}
			};

		public:
			InstructionCmp(InterpretedCore *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::R:
						{
							this->addCycle(new Fetch(core));

							this->addCodeSSS(0xb8, InterpretedCore::BReg::COUNT);
						}
						break;

					case Mode::M:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, InterpretedCore::WReg::H, false));

							this->addCode(0xbe);
						}
						break;

					case Mode::I:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, InterpretedCore::WReg::PC, true));

							this->addCode(0xfe);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret = (this->_mode == Mode::I) ? "cpi " : "cmp ";

				switch (this->_mode) {
					case Mode::R:
						ret += Utils::bregToString(sss(core()));
						break;

					case Mode::M:
						ret += "M";
						break;

					case Mode::I:
						ret += common::Utils::uint8ToString(core()->bR(InterpretedCore::BReg::TMP));
						break;

					default:
						throw std::invalid_argument("Not supported CMP opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_CMP_H_ */

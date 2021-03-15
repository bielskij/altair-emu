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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_ANA_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_ANA_H_

#include "altair/Core.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionAna : public InterpretedCore::Instruction {
		public:
			enum Mode {
				R,
				M,
				I
			};

		private:
			class FetchR : public altair::MachineCycleFetch {
				public:
					FetchR(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(sss()));

						alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::A, InterpretedCore::AluImpl::Op::AND, false,
								InterpretedCore::AluImpl::Z | InterpretedCore::AluImpl::S | InterpretedCore::AluImpl::P | InterpretedCore::AluImpl::CY | InterpretedCore::AluImpl::AC,
							2
						);

						return false;
					}
			};

			class MemoryRead : public MachineCycleMemoryRead {
				public:
					MemoryRead(InterpretedCore *core, bool mem) : MachineCycleMemoryRead(core, mem ? InterpretedCore::WReg::H : InterpretedCore::WReg::PC, InterpretedCore::BReg::TMP, ! mem) {
					}

					bool t3() override {
						bool ret = this->MachineCycleMemoryRead::t3();

						alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::A, InterpretedCore::AluImpl::Op::AND, false,
								InterpretedCore::AluImpl::Z | InterpretedCore::AluImpl::S | InterpretedCore::AluImpl::P | InterpretedCore::AluImpl::CY | InterpretedCore::AluImpl::AC,
							2
						);

						return ret;
					}
			};

		public:
			InstructionAna(InterpretedCore *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::R:
						{
							this->addCycle(new FetchR(core));

							this->addCodeSSS(0xa0, InterpretedCore::BReg::COUNT);
						}
						break;

					case Mode::M:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, true));

							this->addCode(0xa6);
						}
						break;

					case Mode::I:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MemoryRead(core, false));

							this->addCode(0xe6);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret = "ana ";

				switch (this->_mode) {
					case Mode::R:
						ret += altair::Utils::bregToString(sss(core()));
						break;

					case Mode::M:
						ret += "M";
						break;

					case Mode::I:
						ret = "ani " + common::Utils::uint8ToString(core()->bR(InterpretedCore::BReg::TMP));
						break;

					default:
						throw std::invalid_argument("Not supported ANA opcode!");
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_ANA_H_ */

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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_MOV_HPP_
#define CORE_INTERPRETEDCORE_INSTRUCTION_MOV_HPP_

#include "common/Utils.hpp"

#include "altair/Utils.hpp"
#include "altair/Core/InterpretedCore.hpp"

#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"
#include "Core/InterpretedCore/MachineCycle/MemoryRead.hpp"
#include "Core/InterpretedCore/MachineCycle/MemoryWrite.hpp"

namespace altair {
	class InstructionMov : public InterpretedCore::Instruction {
		public:
			enum Mode {
				RR,
				MR,
				RM
			};

		private:
			class FetchRR : public altair::MachineCycleFetch {
				public:
					FetchRR(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						// SSS -> TMP
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(sss()));

						return true;
					}

					bool t5() override {
						// TMP -> DDD
						core()->bR(ddd(), core()->bR(InterpretedCore::BReg::TMP));

						return false;
					}
			};

			class FetchMr : public altair::MachineCycleFetch {
				public:
					FetchMr(InterpretedCore *core) : MachineCycleFetch(core) {
					}

					bool t4() override {
						// SSS -> TMP
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(sss()));

						return false;
					}
			};

		public:
			InstructionMov(InterpretedCore *core, Mode mode) : Instruction(core) {
				this->_mode = mode;

				switch (mode) {
					case Mode::RR:
						{
							this->addCycle(new FetchRR(core));

							this->addCodeDDDSSS(0x40, InterpretedCore::BReg::COUNT, InterpretedCore::BReg::COUNT);
						}
						break;

					case Mode::RM:
						{
							this->addCycle(new MachineCycleFetch(core));
							this->addCycle(new MachineCycleMemoryRead(core, InterpretedCore::WReg::H, InterpretedCore::BReg::DDD, false));

							this->addCodeDDD(0x46, InterpretedCore::BReg::COUNT);
						}
						break;

					case Mode::MR:
						{
							this->addCycle(new FetchMr(core));
							this->addCycle(new MachineCycleMemoryWrite(core, InterpretedCore::WReg::H,  InterpretedCore::BReg::TMP, false));

							this->addCodeSSS(0x70, InterpretedCore::BReg::COUNT);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret = "mov ";

				{
					uint8_t ir = core()->bR(InterpretedCore::BReg::IR);

					switch (this->_mode) {
						case Mode::RR:
							ret += (
								altair::Utils::bregToString(Instruction::ddd(core())) +
								"," +
								altair::Utils::bregToString(Instruction::sss(core()))
							);
							break;

						case Mode::RM:
							ret += (altair::Utils::bregToString(Instruction::ddd(core())) + ",M");
							break;

						case Mode::MR:
							ret += ("M," + altair::Utils::bregToString(Instruction::sss(core())));
							break;

						default:
							throw std::invalid_argument("Not supported MOV opcode!");
					}
				}

				return ret;
			}

		private:
			Mode _mode;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_MOV_HPP_ */

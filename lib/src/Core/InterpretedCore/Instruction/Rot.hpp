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
#ifndef CORE_INTERPRETEDCORE_INSTRUCTION_ROT_H_
#define CORE_INTERPRETEDCORE_INSTRUCTION_ROT_H_

#include "altair/Core/InterpretedCore.hpp"
#include "Core/InterpretedCore/MachineCycle/Fetch.hpp"

namespace altair {
	class InstructionRot : public InterpretedCore::Instruction {
		public:
			enum Mode {
				RIGHT,
				LEFT,
			};

		private:
			// ----------- R
			class Fetch : public altair::MachineCycleFetch {
				public:
					Fetch(InterpretedCore *core, InterpretedCore::Alu::Op op, bool withCarry) : MachineCycleFetch(core) {
						this->withCarry = withCarry;
						this->operation = op;
					}

					bool t4() override {
						core()->bR(InterpretedCore::BReg::TMP, core()->bR(sss()));

						core()->alu()->op(InterpretedCore::BReg::A, InterpretedCore::BReg::A, this->operation, this->withCarry,
							InterpretedCore::Alu::CY, 2
						);

						return false;
					}

				private:
					InterpretedCore::Alu::Op operation;
					bool          withCarry;
			};

		public:
			InstructionRot(InterpretedCore *core, Mode mode, bool withCarry) : Instruction(core) {
				this->_mode  = mode;
				this->_carry = withCarry;

				switch (mode) {
					case Mode::RIGHT:
						{
							// RRC, RAR (carry)
							this->addCycle(new Fetch(core, InterpretedCore::Alu::Op::ROT_R, withCarry));

							this->addCode(withCarry ? 0x1f : 0x0f);
						}
						break;

					case Mode::LEFT:
						{
							// RLC, RAL (carry)
							this->addCycle(new Fetch(core, InterpretedCore::Alu::Op::ROT_L, withCarry));

							this->addCode(withCarry ? 0x17 : 0x07);
						}
						break;
				}
			}

			std::string toAsm() const override {
				std::string ret;

				switch (this->_mode) {
					case Mode::RIGHT:
						ret = this->_carry ? "rrc" : "rar";
						break;

					case Mode::LEFT:
						ret = this->_carry ? "rlc" : "ral";
						break;
				}

				return ret;
			}

		private:
			Mode _mode;
			bool _carry;
	};
}

#endif /* CORE_INTERPRETEDCORE_INSTRUCTION_ROT_H_ */

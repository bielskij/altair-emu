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
#ifndef ALTAIR_CORE_HPP_
#define ALTAIR_CORE_HPP_

#include <cstdint>


namespace altair {
	class Core {
		public:
			// IMPORTANT: !! Do not change reg order !!
			enum class BReg : uint8_t {
				A, B, C, D, E, H, L, IR, TMP, COUNT
			};

			// IMPORTANT: !! Do not change reg order !!
			// IR - instruction register
			// PC - program counter
			enum class WReg {
				B, D, H, SP, PC, COUNT
			};

			// IMPORTANT: !! Do not change reg order !!
			enum Cond {
				NOT_ZERO,
				ZERO,
				NO_CARRY,
				CARRY,
				PARITY_ODD,
				PARITY_EVEN,
				PLUS,
				MINUS
			};

			class Pio {
				// true - active, false - not
				public:
					virtual ~Pio() {};

					virtual bool getReset() = 0;
					virtual bool getHold()  = 0;
					virtual bool getInt()   = 0;
					virtual bool getReady() = 0;

					virtual void setInte(bool active)    = 0;
					virtual void setDbin(bool active)    = 0;
					virtual void setWr(bool active)      = 0;
					virtual void setSync(bool active)    = 0;
					virtual void setHoldAck(bool active) = 0;
					virtual void setWait(bool active)    = 0;


					virtual uint8_t getData()            = 0;
					virtual void    setData(uint8_t val) = 0;

					virtual void setAddress(uint16_t val) = 0;

					// It is clock output from CPU - pin available in 8085
					virtual void clk() = 0;
			};

			class Alu {
				private:
					// Flags
					bool fCarry;
					bool fZero;
					bool fSign;
					bool fParity;
					bool fAuxCarry;

					uint8_t _ac;  // Accumulator
					uint8_t _act; // Temporary accumulator
					uint8_t _tmp; // Temporary register

				public:
					Alu() {
						this->fCarry    = false;
						this->fZero     = false;
						this->fSign     = false;
						this->fParity   = false;
						this->fAuxCarry = false;

						this->_ac  = 0;
						this->_act = 0;
						this->_tmp = 0;
					}

					virtual ~Alu() {
					}

					inline void reset() {
						this->fCarry    = false;
						this->fZero     = false;
						this->fSign     = false;
						this->fParity   = false;
						this->fAuxCarry = false;

						this->ac(0);
						this->act(0);
						this->tmp(0);

						this->setCommon(0);
					}

					inline uint8_t ac() const {
						return this->_ac;
					}

					inline void ac(uint8_t val) {
						this->_ac = val;
					}

					inline uint8_t act() const {
						return this->_act;
					}

					inline void act(uint8_t val) {
						this->_act = val;
					}

					inline uint8_t tmp() const {
						return this->_tmp;
					}

					inline void tmp(uint8_t val) {
						this->_tmp = val;
					}

					static inline bool getParity(uint8_t val) {
						val ^= (val >> 4);
						val ^= (val >> 2);
						val ^= (val >> 1);

						return (val) & 1;
					}

					inline void setCommon(uint8_t val) {
						this->fSign   = (val & 0x80) != 0;
						this->fZero   = (val == 0);
						this->fParity = getParity(val) == 0;
					}
			};

			class MachineCycle {
				public:
					MachineCycle(Core *cpu, bool inta, bool wo, bool stack, bool hlta, bool out, bool m1, bool inp, bool memr);
					virtual ~MachineCycle();

					virtual bool t1();
					virtual bool t2();
					virtual bool t3();
					virtual bool t4();
					virtual bool t5();

					virtual uint8_t getStatus();

					inline Core *core() {
						return this->_core;
					}

				private:
					Core   *_core;
					uint8_t _status;
			};

			class Instruction {
				public:
					virtual ~Instruction();

					MachineCycle *nextCycle() {
						if (this->_cycleIdx == this->_cyclesCount - 1) {
							return nullptr;

						} else {
							return this->_cycles[this->_cycleIdx++];
						}
					}

					inline void reset() {
						this->_cycleIdx = 0;
					}

				protected:
					Instruction(Core *core);
					void addCycle(MachineCycle *cycle);

					inline Core *core() {
						return this->_core;
					}

				private:
					uint8_t       _cycleIdx;
					MachineCycle *_cycles[5];
					uint8_t       _cyclesCount;
					Core         *_core;
			};

			class InstructionDecoder {
				public:
					InstructionDecoder(Core *core);
					virtual ~InstructionDecoder();

					Instruction *decode(uint8_t binaryInstruction);

				private:
					Instruction *_opCodes[255];
			};

		private:
			// General purpose registers + temporary internals
			uint8_t  _bregs[static_cast<uint8_t>(BReg::COUNT) - 1];
			uint16_t _wregs[static_cast<uint8_t>(WReg::COUNT)];

			Instruction  *_i;
			MachineCycle *_cycle;

			Alu                _alu;
			InstructionDecoder*_decoder;
			Pio               &_pio;
			MachineCycle      *_fetchCycle;
			uint8_t            _state; // Current state
		public:
			Core(Pio &pio);
			virtual ~Core();

			void nexti();
			void tick();

			inline uint8_t bR(BReg reg) const {
				if (reg == BReg::A) {
					return this->_alu.ac();
				}

				return this->_bregs[static_cast<uint8_t>(reg) - 1];
			}

			inline void bR(BReg r, uint8_t val) {
				if (r == BReg::A) {
					this->_alu.ac(val);

				} else {
					this->_bregs[static_cast<uint8_t>(r) - 1] = val;
				}
			}

			inline uint16_t wR(WReg reg) const {
				return this->_wregs[static_cast<uint8_t>(reg)];
			}

			inline void wR(WReg reg, uint16_t val) {
				this->_wregs[static_cast<uint8_t>(reg)] = val;
			}

			inline Pio &pio() const {
				return this->_pio;
			}

		protected:
			Core() = delete;
			Core(const Core &core) = delete;
	};
}

#endif /* ALTAIR_CORE_HPP_ */

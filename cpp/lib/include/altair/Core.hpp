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
#include <stdexcept>
#include <type_traits>

namespace altair {
	class Core {
		public:
			// IR  - instruction register
			// A   - accumulator (ALU)
			// ACT - temporary accumulator (ALU)
			// TMP - temporary (ALU)
			enum class BReg : uint8_t {
				A, F, B, C, D, E, H, L, W, Z, IR, TMP, ACT, COUNT
			};

			// IMPORTANT: !! Do not change reg order !!
			// PC - Program counter
			// SP - Stack pointer
			// W  - temporary
			enum class WReg : uint8_t {
				SP, PC,
				B, D, H, W, // Virtual registers, wrappers on byte reg pairs
				COUNT
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
				public:
					static constexpr uint8_t Z  = 1 << 6;
					static constexpr uint8_t CY = 1 << 0;
					static constexpr uint8_t S  = 1 << 7;
					static constexpr uint8_t P  = 1 << 2;
					static constexpr uint8_t AC = 1 << 4;

				public:
					enum class Act {
						A
					};

					enum class Op {
						IDLE,

						ADD
					};

				public:
					Alu(Core *core) {
						this->core = core;

						this->reset();
					}

					virtual ~Alu() {
					}

					inline void set(uint8_t flag) {
						core->bR(Core::BReg::F, core->bR(Core::BReg::F) | flag);
					}

					inline void clear(uint8_t flag) {
						core->bR(Core::BReg::F, core->bR(Core::BReg::F) & ~flag);
					}

					inline bool fZ() const {
						return (core->bR(Core::BReg::F) & Z) != 0;
					}

					inline void fZ(uint8_t val) {
						if (val == 0) {
							set(Z);
						} else {
							clear(Z);
						}
					}

					inline bool fCY() const {
						return (core->bR(Core::BReg::F) & CY) != 0;
					}

					inline void fCY(uint16_t val) {
						if ((val & 0xff00) != 0) {
							set(CY);
						} else {
							clear(CY);
						}
					}

					inline bool fS() const {
						return (core->bR(Core::BReg::F) & S) != 0;
					}

					inline void fS(uint8_t val) {
						if ((val & 0x80) != 0) {
							set(S);
						} else {
							clear(S);
						}
					}

					inline bool fP() const {
						return (core->bR(Core::BReg::F) & P) != 0;
					}

					inline void fP(uint8_t val) {
						if (getParityOdd(val)) {
							clear(P);
						} else {
							set(P);
						}
					}

					inline bool fAC() const {
						return (core->bR(Core::BReg::F) & AC) != 0;
					}

					inline void fAC(uint8_t val) {
						if ((val & 0xf0) != 0) {
							set(AC);
						} else {
							clear(AC);
						}
					}

					void reset();
					void op(Act actSrc, Op operation, bool includeCarry);
					void clk();

				private:
					static inline bool getParityOdd(uint8_t val) {
						val ^= (val >> 4);
						val ^= (val >> 2);
						val ^= (val >> 1);

						return (val) & 1;
					}

				private:
					Core   *core;
					Op      operation;
					bool    operationCarry;
					uint8_t clkCount;
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

					virtual uint8_t getStatus() const;

					inline Core *core() const {
						return this->_core;
					}

				protected:
					inline Core::BReg sss() const {
						return binToBreg(this->_core->bR(Core::BReg::IR) & 0x07);
					}

					inline Core::BReg ddd() const {
						return binToBreg((this->_core->bR(Core::BReg::IR) & 0x38) >> 3);
					}

					inline Core::WReg rp() const {
						return binToWreg((this->_core->bR(Core::BReg::IR) & 0x30) >> 4);
					}

					inline Core::BReg ccc() const {
						return binToBreg((this->_core->bR(Core::BReg::IR) & 0x38) >> 3);
					}

				private:
					static inline Core::BReg binToBreg(uint8_t val) {
						switch (val) {
							case 0x00: return BReg::B;
							case 0x01: return BReg::C;
							case 0x02: return BReg::D;
							case 0x03: return BReg::E;
							case 0x04: return BReg::H;
							case 0x05: return BReg::L;
							case 0x07: return BReg::A;
							default:
								throw std::runtime_error("Invalid code!");
						}
					}

					static inline Core::WReg binToWreg(uint8_t val) {
						switch (val) {
							case 0x00: return WReg::B;
							case 0x01: return WReg::D;
							case 0x02: return WReg::H;
							case 0x03: return WReg::SP;
							default:
								throw std::runtime_error("Invalid code!");
						}
					}

				private:
					Core   *_core;
					uint8_t _status;
			};

			class Instruction {
				public:
					virtual ~Instruction();

					MachineCycle *nextCycle() {
						if (this->_cycleIdx == this->_cyclesCount) {
							return nullptr;
						}

						return this->_cycles[this->_cycleIdx++];
					}

					inline void reset() {
						this->_cycleIdx = 0;
					}

				protected:
					Instruction(Core *core);
					void addCycle(MachineCycle *cycle);

					inline Core *core() const {
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
			uint8_t  _bregs[static_cast<uint8_t>(BReg::COUNT)];
			uint16_t _wregs[static_cast<uint8_t>(WReg::COUNT) - 4];

			Instruction  *_i;
			MachineCycle *_cycle;

			Alu               *_alu;
			InstructionDecoder*_decoder;
			Pio               &_pio;
			MachineCycle      *_fetchCycle;
			uint8_t            _state; // Current state

		public:
			Core(Pio &pio);
			virtual ~Core();

			void tick();
			void nextCycle();
			void nextInstruction();

			inline uint8_t bR(BReg reg) const {
				return this->_bregs[static_cast<uint8_t>(reg)];
			}

			inline void bR(BReg r, uint8_t val) {
				this->_bregs[static_cast<uint8_t>(r)] = val;
			}

			inline uint16_t wR(WReg reg) const {
				switch (reg) {
					case WReg::B:
						return ((uint16_t) this->bR(BReg::B) << 8) | this->bR(BReg::C);

					case WReg::D:
						return ((uint16_t) this->bR(BReg::D) << 8) | this->bR(BReg::E);

					case WReg::H:
						return ((uint16_t) this->bR(BReg::H) << 8) | this->bR(BReg::L);

					case WReg::W:
						return ((uint16_t) this->bR(BReg::W) << 8) | this->bR(BReg::Z);

					default:
						return this->_wregs[static_cast<uint8_t>(reg)];
				}
			}

			inline void wR(WReg reg, uint16_t val) {
				switch (reg) {
					case WReg::B:
						this->bR(BReg::B, val >> 8);
						this->bR(BReg::C, val);
						break;

					case WReg::D:
						this->bR(BReg::D, val >> 8);
						this->bR(BReg::E, val);
						break;

					case WReg::H:
						this->bR(BReg::H, val >> 8);
						this->bR(BReg::L, val);
						break;

					case WReg::W:
						this->bR(BReg::W, val >> 8);
						this->bR(BReg::Z, val);
						break;

					default:
						this->_wregs[static_cast<uint8_t>(reg)] = val;
				}
			}

			inline uint8_t wRL(WReg reg) {
				switch (reg) {
					case WReg::B: return this->bR(BReg::C);
					case WReg::D: return this->bR(BReg::E);
					case WReg::H: return this->bR(BReg::L);
					case WReg::W: return this->bR(BReg::Z);
					default:
						return this->_wregs[static_cast<uint8_t>(reg)];
				}
			}

			inline void wRL(WReg reg, uint8_t val) {
				switch (reg) {
					case WReg::B: this->bR(BReg::C, val); break;
					case WReg::D: this->bR(BReg::E, val); break;
					case WReg::H: this->bR(BReg::L, val); break;
					case WReg::W: this->bR(BReg::Z, val); break;
					default:
						{
							uint16_t &r = this->_wregs[static_cast<uint8_t>(reg)];

							r = (r & 0xff00) | val;
						}
				}
			}

			inline uint8_t wRH(WReg reg) {
				switch (reg) {
					case WReg::B: return this->bR(BReg::B);
					case WReg::D: return this->bR(BReg::D);
					case WReg::H: return this->bR(BReg::H);
					case WReg::W: return this->bR(BReg::W);
					default:
						return this->_wregs[static_cast<uint8_t>(reg)] >> 8;
				}
			}

			inline void wRH(WReg reg, uint8_t val) {
				switch (reg) {
					case WReg::B: this->bR(BReg::B, val); break;
					case WReg::D: this->bR(BReg::D, val); break;
					case WReg::H: this->bR(BReg::H, val); break;
					case WReg::W: this->bR(BReg::W, val); break;
					default:
						{
							uint16_t &r = this->_wregs[static_cast<uint8_t>(reg)];

							r = (r & 0x00ff) | ((uint16_t) val << 8);
						}
				}
			}

			inline Pio &pio() const {
				return this->_pio;
			}

			inline Alu *alu() const {
				return this->_alu;
			}

		protected:
			Core() = delete;
			Core(const Core &core) = delete;
	};
}

#endif /* ALTAIR_CORE_HPP_ */

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
#include <set>
#include <vector>

namespace altair {
	class Core {
		public:
			// IR  - instruction register
			// A   - accumulator (ALU)
			// ACT - temporary accumulator (ALU)
			// TMP - temporary (ALU)
			enum class BReg : uint8_t {
				A, F, B, C, D, E, H, L, W, Z, IR, TMP, ACT, SPL, SPH, PCL, PCH, COUNT,

				// Special register declaration - used in MachineCycle to
				// determine dynamic register
				DDD, SSS, RP_H, RP_L
			};

			// IMPORTANT: !! Do not change reg order !!
			// PC - Program counter
			// SP - Stack pointer
			// W  - temporary
			enum class WReg : uint8_t {
				PC, SP, B, D, H, W, // Virtual registers, wrappers on byte reg pairs
				COUNT,

				// Special register declaration - used in MachineCycle to
				// determine dynamic register
				RP
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
				MINUS,

				COUNT
			};

			class Pio {
				// true - active, false - not
				public:
					virtual ~Pio() {};

					virtual bool getReset() = 0;
					virtual bool getHold()  = 0;
					virtual bool getInt()   = 0;
					virtual bool getInte()  = 0;
					virtual bool getReady() = 0;

					virtual void setInt(bool active)     = 0;
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
					enum class Op {
						IDLE,

						ADD,
						SUB,
						AD, // Adjust decimal

						AND,
						XOR,
						OR,

						ROT_R,
						ROT_L
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
						if ((val & 0x0100) != 0) {
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
					void op(Core::BReg actSrc, Core::BReg dstReg, Op operation, bool includeCarry, uint8_t updateFlags, uint8_t clkDelay);
					void op(uint8_t actVal, Core::BReg dstReg, Op operation, bool includeCarry, uint8_t updateFlags, uint8_t clkDelay);
					void clk();
					bool checkCondition(Core::Cond condition);

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
					bool    includeCarry;
					uint8_t updateFlags;
					uint8_t clkCount;
					uint8_t clkDelay;

					Core::BReg dstReg;
			};

			class Instruction;
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

					inline void setParent(Instruction *i) {
						this->_parent = i;
					}

					inline Instruction *parent() const {
						return this->_parent;
					}

				protected:
					inline Core::BReg sss() const {
						return Instruction::sss(this->_core);
					}

					inline Core::BReg ddd() const {
						return Instruction::ddd(this->_core);
					}

					inline Core::WReg rp() const {
						return Instruction::rp(this->_core);
					}

					inline Core::Cond ccc() const {
						return Instruction::ccc(this->_core);
					}

				private:
					Core        *_core;
					uint8_t      _status;
					Instruction *_parent;
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

					// Finishes current instruction even if there are more cycles available
					// Used in conditional instructions
					inline void finish() {
						this->_cycleIdx = this->_cyclesCount;
					}

					const std::set<uint8_t> &getOpcodes() const {
						return this->_opcodes;
					}

					virtual std::string toAsm() const = 0;

					static inline Core::BReg sss(Core *core) {
						return InstructionDecoder::binToBreg(core->bR(Core::BReg::IR) & 0x07);
					}

					static inline Core::BReg ddd(Core *core) {
						return InstructionDecoder::binToBreg((core->bR(Core::BReg::IR) & 0x38) >> 3);
					}

					static inline Core::WReg rp(Core *core) {
						return InstructionDecoder::binToWreg((core->bR(Core::BReg::IR) & 0x30) >> 4);
					}

					static inline Core::Cond ccc(Core *core) {
						return InstructionDecoder::binToCond((core->bR(Core::BReg::IR) & 0x38) >> 3);
					}

					static inline Core::Cond nnn(Core *core) {
						return InstructionDecoder::binToCond((core->bR(Core::BReg::IR) & 0x38) >> 3);
					}

				protected:
					Instruction(Core *core);
					void addCycle(MachineCycle *cycle);

					inline Core *core() const {
						return this->_core;
					}

					inline void addCode(uint8_t code) {
						this->_opcodes.insert(code);
					}

					inline void addCodeNNN(uint8_t baseCode) {
						for (int i = 0; i < 8; i++) {
							this->_opcodes.insert(baseCode | (i << 3));
						}
					}

					inline void addCodeCCC(uint8_t baseCode, Core::Cond cond) {
						if (cond == Cond::COUNT) {
							for (auto &c : _allConds) {
								this->_opcodes.insert(baseCode | (InstructionDecoder::condToBin(c) << 3));
							}

						} else {
							this->_opcodes.insert(baseCode | (InstructionDecoder::condToBin(cond) << 3));
						}
					}

					inline void addCodeSSS(uint8_t baseCode, Core::BReg sss) {
						if (sss == Core::BReg::COUNT) {
							this->addAllBreg(baseCode, 0);

						} else {
							this->addBreg(baseCode, sss, 0);
						}
					}

					inline void addCodeDDD(uint8_t baseCode, Core::BReg ddd) {
						if (ddd == Core::BReg::COUNT) {
							this->addAllBreg(baseCode, 3);

						} else {
							this->addBreg(baseCode, ddd, 3);
						}
					}

					inline void addCodeDDDSSS(uint8_t baseCode, Core::BReg ddd, Core::BReg sss) {
						if (ddd == Core::BReg::COUNT) {
							for (auto &d : _allBregs) {
								if (sss == Core::BReg::COUNT) {
									for (auto &s : _allBregs) {
										addBreg(baseCode, d, s);
									}

								} else {
									addBreg(baseCode, d, sss);
								}
							}

						} else if (sss == Core::BReg::COUNT) {
							for (auto &s : _allBregs) {
								addBreg(baseCode, ddd, s);
							}

						} else {
							addBreg(baseCode, ddd, sss);
						}
					}

					inline void addCodeRP(uint8_t baseCode, Core::WReg rp) {
						if (rp == Core::WReg::COUNT) {
							this->addAllWreg(baseCode);

						} else {
							this->addWreg(baseCode, rp);
						}
					}

				private:
					inline void addBreg(uint8_t baseCode, Core::BReg ddd, Core::BReg sss) {
						this->_opcodes.insert(baseCode | (InstructionDecoder::bregToBin(ddd) << 3) | InstructionDecoder::bregToBin(sss));
					}

					inline void addBreg(uint8_t baseCode, Core::BReg reg, uint8_t offset) {
						this->_opcodes.insert(baseCode | (InstructionDecoder::bregToBin(reg) << offset));
					}

					inline void addAllBreg(uint8_t baseCode, uint8_t offset) {
						for (auto &b : _allBregs) {
							addBreg(baseCode, b, offset);
						}
					}

					inline void addWreg(uint8_t baseCode, Core::WReg reg) {
						this->_opcodes.insert(baseCode | (InstructionDecoder::wregToBin(reg) << 4));
					}

					inline void addAllWreg(uint8_t baseCode) {
						addWreg(baseCode, Core::WReg::B);
						addWreg(baseCode, Core::WReg::D);
						addWreg(baseCode, Core::WReg::H);
						addWreg(baseCode, Core::WReg::SP);
					}

				private:
					uint8_t       _cycleIdx;
					MachineCycle *_cycles[5];
					uint8_t       _cyclesCount;
					Core         *_core;

					std::set<uint8_t> _opcodes;
					static const std::set<Core::BReg> _allBregs;
					static const std::set<Core::Cond> _allConds;
			};

			class InstructionDecoder {
				public:
					InstructionDecoder(Core *core);
					virtual ~InstructionDecoder();

					Instruction *decode(uint8_t binaryInstruction);

				public:
					static inline Core::Cond binToCond(uint8_t val) {
						switch (val) {
							case 0: return Core::Cond::NOT_ZERO;
							case 1: return Core::Cond::ZERO;
							case 2: return Core::Cond::NO_CARRY;
							case 3: return Core::Cond::CARRY;
							case 4: return Core::Cond::PARITY_ODD;
							case 5: return Core::Cond::PARITY_EVEN;
							case 6: return Core::Cond::PLUS;
							case 7: return Core::Cond::MINUS;
							default:
								throw std::runtime_error("Invalid code!");
						}
					}

					static inline uint8_t condToBin(Cond cond) {
						switch (cond) {
							case Cond::NOT_ZERO:    return 0;
							case Cond::ZERO:        return 1;
							case Cond::NO_CARRY:    return 2;
							case Cond::CARRY:       return 3;
							case Cond::PARITY_ODD:  return 4;
							case Cond::PARITY_EVEN: return 5;
							case Cond::PLUS:        return 6;
							case Cond::MINUS:       return 7;
							default:
								throw std::runtime_error("Invalid condition!");
						}

						return 0xff;
					}

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

					static inline uint8_t bregToBin(Core::BReg reg) {
						switch (reg) {
							case Core::BReg::B: return 0x00;
							case Core::BReg::C: return 0x01;
							case Core::BReg::D: return 0x02;
							case Core::BReg::E: return 0x03;
							case Core::BReg::H: return 0x04;
							case Core::BReg::L: return 0x05;
							case Core::BReg::A: return 0x07;
							default:
								throw std::runtime_error("Invalid BReg!");
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

					static inline uint8_t wregToBin(Core::WReg reg) {
						switch (reg) {
							case Core::WReg::B:  return 0x00;
							case Core::WReg::D:  return 0x01;
							case Core::WReg::H:  return 0x02;
							case Core::WReg::SP: return 0x03;
							default:
								throw std::runtime_error("Invalid WReg!");
						}
					}

				private:
					Instruction *_instructionLut[256];
					std::vector<Instruction *> _instructions;
			};

		private:
			// General purpose registers + temporary internals
			uint8_t _bregs[static_cast<uint8_t>(BReg::COUNT)];

			Instruction  *_i;
			uint16_t      _iPC;
			MachineCycle *_cycle;

			Alu               *_alu;
			InstructionDecoder*_decoder;
			Pio               &_pio;
			MachineCycle      *_fetchCycle;
			MachineCycle      *_interruptCycle;
			uint8_t            _state; // Current state

			bool _intFF;

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

					case WReg::SP:
						return ((uint16_t) this->bR(BReg::SPH) << 8) | this->bR(BReg::SPL);

					case WReg::PC:
						return ((uint16_t) this->bR(BReg::PCH) << 8) | this->bR(BReg::PCL);

					default:
						throw std::runtime_error("Not supported register pair!");
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

					case WReg::SP:
						this->bR(BReg::SPH, val >> 8);
						this->bR(BReg::SPL, val);
						break;

					case WReg::PC:
						this->bR(BReg::PCH, val >> 8);
						this->bR(BReg::PCL, val);
						break;

					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			static inline BReg wRegL(WReg reg) {
				switch (reg) {
					case WReg::B:  return BReg::C;
					case WReg::D:  return BReg::E;
					case WReg::H:  return BReg::L;
					case WReg::W:  return BReg::Z;
					case WReg::SP: return BReg::SPL;
					case WReg::PC: return BReg::PCL;
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			inline uint8_t wRL(WReg reg) {
				switch (reg) {
					case WReg::B:  return this->bR(BReg::C);
					case WReg::D:  return this->bR(BReg::E);
					case WReg::H:  return this->bR(BReg::L);
					case WReg::W:  return this->bR(BReg::Z);
					case WReg::SP: return this->bR(BReg::SPL);
					case WReg::PC: return this->bR(BReg::PCL);
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			inline void wRL(WReg reg, uint8_t val) {
				switch (reg) {
					case WReg::B:  this->bR(BReg::C,   val); break;
					case WReg::D:  this->bR(BReg::E,   val); break;
					case WReg::H:  this->bR(BReg::L,   val); break;
					case WReg::W:  this->bR(BReg::Z,   val); break;
					case WReg::SP: this->bR(BReg::SPL, val); break;
					case WReg::PC: this->bR(BReg::PCL, val); break;
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			static inline BReg wRegH(WReg reg) {
				switch (reg) {
					case WReg::B:  return BReg::B;
					case WReg::D:  return BReg::D;
					case WReg::H:  return BReg::H;
					case WReg::W:  return BReg::W;
					case WReg::SP: return BReg::SPH;
					case WReg::PC: return BReg::PCH;
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			inline uint8_t wRH(WReg reg) {
				switch (reg) {
					case WReg::B:  return this->bR(BReg::B);
					case WReg::D:  return this->bR(BReg::D);
					case WReg::H:  return this->bR(BReg::H);
					case WReg::W:  return this->bR(BReg::W);
					case WReg::SP: return this->bR(BReg::SPH);
					case WReg::PC: return this->bR(BReg::PCH);
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			inline void wRH(WReg reg, uint8_t val) {
				switch (reg) {
					case WReg::B:  this->bR(BReg::B,   val); break;
					case WReg::D:  this->bR(BReg::D,   val); break;
					case WReg::H:  this->bR(BReg::H,   val); break;
					case WReg::W:  this->bR(BReg::W,   val); break;
					case WReg::SP: this->bR(BReg::SPH, val); break;
					case WReg::PC: this->bR(BReg::PCH, val); break;
					default:
						throw std::runtime_error("Not supported register pair!");
				}
			}

			inline Pio &pio() const {
				return this->_pio;
			}

			inline Alu *alu() const {
				return this->_alu;
			}

			inline bool intFF() const {
				return this->_intFF;
			}

			inline void intFF(bool enable) {
				this->_intFF = enable;
			}

		protected:
			Core() = delete;
			Core(const Core &core) = delete;
	};
}

#endif /* ALTAIR_CORE_HPP_ */

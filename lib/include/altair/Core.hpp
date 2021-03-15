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

		public:
			class Alu {
				public:
					Alu() {}
					virtual ~Alu() {}

					virtual bool fZ()  const = 0;
					virtual bool fCY() const = 0;
					virtual bool fS()  const = 0;
					virtual bool fP()  const = 0;
					virtual bool fAC() const = 0;
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

					/**!
					 * Simplified API - designated for state/cycle-less Core implementations
					 */
					virtual uint8_t memoryRead(uint16_t address) = 0;
					virtual void    memoryWrite(uint16_t address, uint8_t value) = 0;
					virtual uint8_t ioRead(uint8_t number) = 0;
					virtual void    ioWrite(uint8_t number, uint8_t data) = 0;
					virtual void    clk(uint8_t ticks) = 0;
			};

		public:
			virtual ~Core() {}

			/*!
			 * Executes 'some' instructions. There is no limitation of running instruction number.
			 */
			virtual void turn() = 0;

			virtual void nexti() = 0;

			/*!
			 * Terminates CPU execution. This method forces turn() execution to interrupt.
			 */
			virtual void shutdown() = 0;

			virtual uint8_t  bR(BReg reg) const         = 0;
			virtual void     bR(BReg r, uint8_t val)    = 0;
			virtual uint16_t wR(WReg reg) const         = 0;
			virtual void     wR(WReg reg, uint16_t val) = 0;

			virtual uint8_t wRL(WReg reg) = 0;
			virtual uint8_t wRH(WReg reg) = 0;

			virtual altair::Core::Alu *alu() const = 0;

		protected:
			Core() {}
			Core(const Core &);
	};
}

#endif /* ALTAIR_CORE_HPP_ */

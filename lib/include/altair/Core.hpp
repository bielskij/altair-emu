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

		public:
			virtual ~Core() {}

			/*!
			 * Executes 'some' instructions. There is no limitation of running instruction number
			 *
			 * @return number of clock cycles.
			 */
			virtual int turn() = 0;

			/*!
			 * Terminates CPU execution. This method forces turn() execution to interrupt.
			 */
			virtual void shutdown() = 0;

		protected:
			Core() {}
			Core(const Core &);
	};
}

#endif /* ALTAIR_CORE_HPP_ */

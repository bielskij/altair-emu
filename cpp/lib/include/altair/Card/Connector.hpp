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
#ifndef ALTAIR_CARD_SOCKET_HPP_
#define ALTAIR_CARD_SOCKET_HPP_

#include <cstdint>

namespace altair {
	namespace card {
		class Connector {
			public:
				Connector() {
					this->other = nullptr;
				}

				virtual ~Connector() {
				}

				inline void connect(Connector *other) {
					this->other = other;
				}

			public:
				// clk to bus
				virtual void clk() {
					this->other->clk();
				}

				// clk from bus
				virtual void onClk() {
				}

				virtual uint8_t vi() const      { return other->vi(); }
				virtual void    vi(uint8_t val) { other->vi(val); }

				virtual uint16_t a() const           { return other->a(); }
				virtual void     a(uint16_t address) { other->a(address); }

				// CPU to external
				virtual uint8_t dout() const       { return other->dout(); }
				virtual void    dout(uint8_t data) { other->dout(data); }

				// external to CPU
				virtual uint8_t din() const       { return other->din(); }
				virtual void    din(uint8_t data) { other->din(data); }

				virtual bool mwrt() const   { return other->mwrt(); }
				virtual void mwrt(bool val) { other->mwrt(val); }

				// Status output signal that indicates that the porcessor is in the fetch
				// cycle for the first byte of an instruction
				virtual bool sm1() const   { return other->sm1(); }
				virtual void sm1(bool val) { other->sm1(val); }

				// Status output signal which indicates that the address bus contains the
				// address of an output device and data bus will contain the output data
				// when PWR is active
				virtual bool sout() const   { return other->sout(); }
				virtual void sout(bool val) { other->sout(val); }

				// Status output signal which indicates that the address bus contains the
				// address of an input device and the input data should be placed on the
				// data bus when PDBIN is active
				virtual bool sinp() const   { return other->sinp(); }
				virtual void sinp(bool val) { other->sinp(val); }

				// Status output signal to acknowledge signal for INTERRUPT request
				virtual bool sinta() const   { return other->sinta(); }
				virtual void sinta(bool val) { other->sinta(val); }

				// Status output signal indicates that the operation int the current machine
				// cycle will be a WRITE memory or output function
				virtual bool swo() const   { return other->swo(); }
				virtual void swo(bool val) { other->swo(val); }

				// Status output signal indicates that the address bus holds the pushdown
				// stack address form the stack pointer.
				virtual bool sstack() const   { return other->sstack(); }
				virtual void sstack(bool val) { other->sstack(val); }

				// Status output signal which indicates that the data bus will be used
				// for memory read data
				virtual bool smemr() const   { return other->smemr(); }
				virtual void smemr(bool val) { other->smemr(val); }

				// Status output signal which acknowledges a HALT instruction
				virtual bool shlta() const   { return other->shlta(); }
				virtual void shlta(bool val) { other->shlta(val); }

				// Processor command/control output signal indicates to external circuits
				// that the data bus is in the input mode
				virtual bool pdbin() const   { return other->pdbin(); }
				virtual void pdbin(bool val) { other->pdbin(val); }

				// Processor command/control output used for memory write or I/O output control:
				// data on the data bus is stable while PWR is active
				virtual bool pwr() const   { return other->pwr(); }
				virtual void pwr(bool val) { other->pwr(val); }

				// Processor command/control output provides a signal to indicate the beginning
				// of each machine cycle
				virtual bool psync() const   { return other->psync(); }
				virtual void psync(bool val) { other->psync(val); }

			private:
				Connector *other;
		};
	}
}



#endif /* ALTAIR_CARD_SOCKET_HPP_ */

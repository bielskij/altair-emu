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

#ifndef ALTAIR_CORE_JITCORE_HPP_
#define ALTAIR_CORE_JITCORE_HPP_

#include <map>

#include "altair/Core.hpp"

extern void _onTick(void *ctx, uint8_t ticks);

namespace altair {
	class JitCore : public altair::Core {
		private:
			struct Regs {
				uint8_t A, B, C, D, E, H, L;
				uint16_t PC, SP;

				JitCore *self;

				void *codeSegment;

				Regs() {
					this->A = 0;
					this->B = 0;
					this->C = 0;
					this->D = 0;
					this->E = 0;
					this->H = 0;
					this->L = 0;

					this->PC = 0;
					this->SP = 0;

					this->self        = nullptr;
					this->codeSegment = nullptr;
				}
			};

			class ExecutionByteBuffer {
				public:
					typedef void (*FunctionPtr)();

				public:
					ExecutionByteBuffer();
					ExecutionByteBuffer(size_t initialSize);
					~ExecutionByteBuffer();

					ExecutionByteBuffer &begin();
					ExecutionByteBuffer &appendByte(uint8_t byte);
					ExecutionByteBuffer &end();

					FunctionPtr getFunction();
					void       *getPtr();

				private:
					uint8_t *_buffer;
					size_t   _bufferSize;
					size_t   _bufferWritten;

					size_t _pageSize;
			};

		public:
			JitCore(Pio &pio, uint16_t pc);

			void turn() override;
			void shutdown() override;
			void nexti() override;

			uint8_t  bR(BReg reg) const         override;
			void     bR(BReg r, uint8_t val)    override;
			uint16_t wR(WReg reg) const         override;
			void     wR(WReg reg, uint16_t val) override;
			uint8_t  wRL(WReg reg)              override;
			uint8_t  wRH(WReg reg)              override;

			Alu *alu() const override;

		private:
			void onTick(uint8_t ticks);

			ExecutionByteBuffer *compile(uint16_t pc, bool singleInstruction);

			friend void ::_onTick(void *ctx, uint8_t ticks);

		private:
			JitCore();

		private:
			Regs _regs;
			Pio &_pio;

			std::map<uint16_t, ExecutionByteBuffer *> _compiledBlocks;
	};
}

#endif /* ALTAIR_CORE_JITCORE_HPP_ */

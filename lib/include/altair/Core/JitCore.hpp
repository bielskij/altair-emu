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

extern void JitCore_onNativeInt(void *ctx);

namespace altair {
	class JitCore : public altair::Core, protected altair::Core::Alu {
		private:
			struct Regs {
				uint8_t F, A, B, C, D, E, H, L;
				uint16_t PC, SP;

				uint8_t intFF;

				JitCore *self;

				void *codeSegment;

				// Interrupt callback flags
				uint8_t  intFlags;
				void   (*intHandler)(void *ctx);
				uint16_t intAddress;
				uint8_t  intValue;

				Regs() {
					this->F = 0;
					this->A = 0;
					this->B = 0;
					this->C = 0;
					this->D = 0;
					this->E = 0;
					this->H = 0;
					this->L = 0;

					this->PC = 0;
					this->SP = 0;

					this->intFF = 0;

					this->self        = nullptr;
					this->codeSegment = nullptr;
					this->intHandler  = nullptr;
					this->intFlags    = 0;
					this->intAddress  = 0;
					this->intValue    = 0;
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
					ExecutionByteBuffer &append(uint8_t byte);
					ExecutionByteBuffer &append(void *data, size_t dataSize);
					ExecutionByteBuffer &end();

					FunctionPtr getFunction();
					void       *getPtr();
					size_t      getSize();

				private:
					uint8_t *_buffer;
					size_t   _bufferSize;
					size_t   _bufferWritten;

					size_t _pageSize;
			};

			// returns instruction size in bytes
			typedef int (*OpHandler)(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

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

			Core::Alu *alu() override;

		protected:
			bool fZ()  const override;
			bool fCY() const override;
			bool fS()  const override;
			bool fP()  const override;
			bool fAC() const override;

		private:
			void execute(bool singleInstruction);

			void onTickInt(uint8_t ticks);
			void onMemoryWriteInt(uint16_t address, uint8_t value);
			void onMemoryReadInt(uint16_t address, uint8_t &value);
			void onIoWriteInt(uint8_t address, uint8_t value);
			void onIoReadInt(uint8_t address, uint8_t &value);
			void onInte(uint8_t value);

			ExecutionByteBuffer *compile(uint16_t pc, bool singleInstruction);

			void addIntCodeLoadIntAddrFromReg(ExecutionByteBuffer *buffer, uint8_t reg);
			void addIntCodeLoadIntAddrFromImm(ExecutionByteBuffer *buffer, uint16_t imm);
			void addIntCodeLoadIntValueFromReg(ExecutionByteBuffer *buffer, uint8_t reg);
			void addIntCodeLoadIntValueFromImm(ExecutionByteBuffer *buffer, uint8_t imm);
			void addIntCodeLoadRegFromIntValue(ExecutionByteBuffer *buffer, uint8_t reg);

			void addIntCodeCall(ExecutionByteBuffer *buffer, uint8_t flag);
			void addIntCodeCallTick(ExecutionByteBuffer *buffer, uint16_t ticks);
			void addIntCodeCallMemoryWrite(ExecutionByteBuffer *buffer);
			void addIntCodeCallMemoryRead(ExecutionByteBuffer *buffer);
			void addIntCodeCallIoWr(ExecutionByteBuffer *buffer);
			void addIntCodeCallIoRd(ExecutionByteBuffer *buffer);


			friend void ::JitCore_onNativeInt(void *ctx);

		private:
			// Operands implementation

			// Data transfer group
			static int _opMovRR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opMovRM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opMovMR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opMviR (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opMviM (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opLxi  (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opLda  (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSta  (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opLhld (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opShld (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opLdax (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opStax (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opXchg (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

			// Arithmetic group
			static int _opAddR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAddM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAdi (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAdcR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAdcM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAci (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSubR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSubM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSui (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSbbR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSbbM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSbi (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opInrR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opInrM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opDcrR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opDcrM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opInx (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opDcx (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opDad (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
//			static int _opDaa (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

			// Logical group
			static int _opAnaR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAnaM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opAni (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opXraR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opXraM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opXri (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opOraR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opOraM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opOri (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opCmpR(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opCmpM(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opCpi (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opRot (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop, bool left);
			static int _opRotC (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop, bool left);
			static int _opRlc (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opRrc (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opRal (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opRar (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opCma (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
//			static int _opCmc (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opStc (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

			// Branch group
			static int _opCall(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opRet (JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opPchl(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

			// Stack, I/O
			static int _opPush(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opPop(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opXthl(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opSphl(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opIn(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opOut(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opEid(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);;
//			static int _opHlt(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);
			static int _opNop(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);



			static int _opJmp(JitCore *core, ExecutionByteBuffer *buffer, uint8_t opcode, uint16_t pc, uint8_t &ticks, bool &stop);

			void _opAddBit(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler);
			void _opAddSSS(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t src, OpHandler handler);
			void _opAddDDD(uint8_t bit7, uint8_t bit6, uint8_t dst, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler);
			void _opAddDDDSSS(uint8_t bit7, uint8_t bit6, uint8_t dst, uint8_t src, OpHandler handler);
			void _opAddRp(uint8_t bit7, uint8_t bit6, uint8_t rp, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0, OpHandler handler);


		private:
			JitCore();

		private:
			Regs _regs;
			Pio &_pio;

			std::map<uint16_t, ExecutionByteBuffer *> _compiledBlocks;

			OpHandler _opHandlers[0xff];
	};
}

#endif /* ALTAIR_CORE_JITCORE_HPP_ */

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
#include "altair/Core.hpp"

#include "Core/Instruction/MovRR.hpp"
#include "Core/Instruction/MovRM.hpp"
#include "Core/Instruction/MovMR.hpp"
#include "Core/Instruction/Sphl.hpp"
#include "Core/Instruction/MviR.hpp"
#include "Core/Instruction/MviM.hpp"
#include "Core/Instruction/Lxi.hpp"
#include "Core/Instruction/Lda.hpp"
#include "Core/Instruction/Sta.hpp"
#include "Core/Instruction/Lhld.hpp"
#include "Core/Instruction/Shld.hpp"
#include "Core/Instruction/Ldax.hpp"
#include "Core/Instruction/Stax.hpp"
#include "Core/Instruction/Xchg.hpp"
#include "Core/Instruction/AddR.hpp"
#include "Core/Instruction/AddM.hpp"

#include "Core/Instruction/Stc.hpp"


altair::Core::InstructionDecoder::InstructionDecoder(Core *core) {
	for (auto &opcode : this->_opCodes) {
		opcode = nullptr;
	}

	this->_opCodes[0x01] = new InstructionLxi(core);
	this->_opCodes[0x02] = new InstructionStax(core);
	this->_opCodes[0x06] = new InstructionMviR(core);
	this->_opCodes[0x0a] = new InstructionLdax(core);
	this->_opCodes[0x0e] = new InstructionMviR(core);
	this->_opCodes[0x11] = new InstructionLxi(core);
	this->_opCodes[0x12] = new InstructionStax(core);
	this->_opCodes[0x16] = new InstructionMviR(core);
	this->_opCodes[0x1a] = new InstructionLdax(core);
	this->_opCodes[0x1e] = new InstructionMviR(core);
	this->_opCodes[0x21] = new InstructionLxi(core);
	this->_opCodes[0x22] = new InstructionShld(core);
	this->_opCodes[0x26] = new InstructionMviR(core);
	this->_opCodes[0x2a] = new InstructionLhld(core);
	this->_opCodes[0x2e] = new InstructionMviR(core);
	this->_opCodes[0x31] = new InstructionLxi(core);
	this->_opCodes[0x32] = new InstructionSta(core);
	this->_opCodes[0x36] = new InstructionMviM(core);
	this->_opCodes[0x37] = new InstructionStc(core);
	this->_opCodes[0x3a] = new InstructionLda(core);
	this->_opCodes[0x3e] = new InstructionMviR(core);
	this->_opCodes[0x40] = new InstructionMovRR(core);
	this->_opCodes[0x41] = new InstructionMovRR(core);
	this->_opCodes[0x42] = new InstructionMovRR(core);
	this->_opCodes[0x43] = new InstructionMovRR(core);
	this->_opCodes[0x44] = new InstructionMovRR(core);
	this->_opCodes[0x45] = new InstructionMovRR(core);
	this->_opCodes[0x46] = new InstructionMovRM(core);
	this->_opCodes[0x47] = new InstructionMovRR(core);
	this->_opCodes[0x48] = new InstructionMovRR(core);
	this->_opCodes[0x49] = new InstructionMovRR(core);
	this->_opCodes[0x4a] = new InstructionMovRR(core);
	this->_opCodes[0x4b] = new InstructionMovRR(core);
	this->_opCodes[0x4c] = new InstructionMovRR(core);
	this->_opCodes[0x4d] = new InstructionMovRR(core);
	this->_opCodes[0x4e] = new InstructionMovRM(core);
	this->_opCodes[0x4f] = new InstructionMovRR(core);
	this->_opCodes[0x50] = new InstructionMovRR(core);
	this->_opCodes[0x51] = new InstructionMovRR(core);
	this->_opCodes[0x52] = new InstructionMovRR(core);
	this->_opCodes[0x53] = new InstructionMovRR(core);
	this->_opCodes[0x54] = new InstructionMovRR(core);
	this->_opCodes[0x55] = new InstructionMovRR(core);
	this->_opCodes[0x56] = new InstructionMovRM(core);
	this->_opCodes[0x57] = new InstructionMovRR(core);
	this->_opCodes[0x58] = new InstructionMovRR(core);
	this->_opCodes[0x59] = new InstructionMovRR(core);
	this->_opCodes[0x5a] = new InstructionMovRR(core);
	this->_opCodes[0x5b] = new InstructionMovRR(core);
	this->_opCodes[0x5c] = new InstructionMovRR(core);
	this->_opCodes[0x5d] = new InstructionMovRR(core);
	this->_opCodes[0x5e] = new InstructionMovRM(core);
	this->_opCodes[0x5f] = new InstructionMovRR(core);
	this->_opCodes[0x60] = new InstructionMovRR(core);
	this->_opCodes[0x61] = new InstructionMovRR(core);
	this->_opCodes[0x62] = new InstructionMovRR(core);
	this->_opCodes[0x63] = new InstructionMovRR(core);
	this->_opCodes[0x64] = new InstructionMovRR(core);
	this->_opCodes[0x65] = new InstructionMovRR(core);
	this->_opCodes[0x66] = new InstructionMovRM(core);
	this->_opCodes[0x67] = new InstructionMovRR(core);
	this->_opCodes[0x68] = new InstructionMovRR(core);
	this->_opCodes[0x69] = new InstructionMovRR(core);
	this->_opCodes[0x6a] = new InstructionMovRR(core);
	this->_opCodes[0x6b] = new InstructionMovRR(core);
	this->_opCodes[0x6c] = new InstructionMovRR(core);
	this->_opCodes[0x6d] = new InstructionMovRR(core);
	this->_opCodes[0x6e] = new InstructionMovRM(core);
	this->_opCodes[0x6f] = new InstructionMovRR(core);
	this->_opCodes[0x70] = new InstructionMovMR(core);
	this->_opCodes[0x71] = new InstructionMovMR(core);
	this->_opCodes[0x72] = new InstructionMovMR(core);
	this->_opCodes[0x73] = new InstructionMovMR(core);
	this->_opCodes[0x74] = new InstructionMovMR(core);
	this->_opCodes[0x75] = new InstructionMovMR(core);
	this->_opCodes[0x77] = new InstructionMovMR(core);
	this->_opCodes[0x78] = new InstructionMovRR(core);
	this->_opCodes[0x79] = new InstructionMovRR(core);
	this->_opCodes[0x7a] = new InstructionMovRR(core);
	this->_opCodes[0x7b] = new InstructionMovRR(core);
	this->_opCodes[0x7c] = new InstructionMovRR(core);
	this->_opCodes[0x7d] = new InstructionMovRR(core);
	this->_opCodes[0x7e] = new InstructionMovRM(core);
	this->_opCodes[0x7f] = new InstructionMovRR(core);
	this->_opCodes[0x80] = new InstructionAddR(core, false);
	this->_opCodes[0x81] = new InstructionAddR(core, false);
	this->_opCodes[0x82] = new InstructionAddR(core, false);
	this->_opCodes[0x83] = new InstructionAddR(core, false);
	this->_opCodes[0x84] = new InstructionAddR(core, false);
	this->_opCodes[0x85] = new InstructionAddR(core, false);
	this->_opCodes[0x86] = new InstructionAddM(core, false);
	this->_opCodes[0x87] = new InstructionAddR(core, false);
	this->_opCodes[0x88] = new InstructionAddR(core, true);
	this->_opCodes[0x89] = new InstructionAddR(core, true);
	this->_opCodes[0x8a] = new InstructionAddR(core, true);
	this->_opCodes[0x8b] = new InstructionAddR(core, true);
	this->_opCodes[0x8c] = new InstructionAddR(core, true);
	this->_opCodes[0x8d] = new InstructionAddR(core, true);
	this->_opCodes[0x8e] = new InstructionAddM(core, true);
	this->_opCodes[0x8f] = new InstructionAddR(core, true);
	this->_opCodes[0xeb] = new InstructionXchg(core);
	this->_opCodes[0xf9] = new InstructionSphl(core);
}


altair::Core::InstructionDecoder::~InstructionDecoder() {
	for (auto &opcode : this->_opCodes) {
		if (opcode != nullptr) {
			delete opcode;
		}

		opcode = nullptr;
	}
}


altair::Core::Instruction *altair::Core::InstructionDecoder::decode(uint8_t binaryInstruction) {
	return this->_opCodes[binaryInstruction];
}

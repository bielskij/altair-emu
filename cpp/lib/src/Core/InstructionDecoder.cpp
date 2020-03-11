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

#include "Core/Instruction/Mov.hpp"
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
#include "Core/Instruction/Add.hpp"
#include "Core/Instruction/Sub.hpp"
#include "Core/Instruction/Inr.hpp"
#include "Core/Instruction/Dcr.hpp"
#include "Core/Instruction/Inx.hpp"
#include "Core/Instruction/Dcx.hpp"
#include "Core/Instruction/Dad.hpp"
#include "Core/Instruction/Daa.hpp"
#include "Core/Instruction/Add.hpp"
#include "Core/Instruction/Ana.hpp"
#include "Core/Instruction/Dcr.hpp"
#include "Core/Instruction/Inr.hpp"
#include "Core/Instruction/Mov.hpp"
#include "Core/Instruction/Sub.hpp"
#include "Core/Instruction/Xra.hpp"
#include "Core/Instruction/Ora.hpp"
#include "Core/Instruction/Cmp.hpp"
#include "Core/Instruction/Rot.hpp"
#include "Core/Instruction/Cma.hpp"
#include "Core/Instruction/Cmc.hpp"
#include "Core/Instruction/Stc.hpp"
#include "Core/Instruction/Jmp.hpp"
#include "Core/Instruction/Call.hpp"
#include "Core/Instruction/Ret.hpp"
#include "Core/Instruction/Rst.hpp"
#include "Core/Instruction/Pchl.hpp"
#include "Core/Instruction/Push.hpp"
#include "Core/Instruction/Pop.hpp"
#include "Core/Instruction/Xthl.hpp"
#include "Core/Instruction/Nop.hpp"


altair::Core::InstructionDecoder::InstructionDecoder(Core *core) {
	this->_instructions.push_back(new InstructionAna(core, InstructionAna::R));
	this->_instructions.push_back(new InstructionAna(core, InstructionAna::M));
	this->_instructions.push_back(new InstructionAna(core, InstructionAna::I));

	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::R, true));
	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::R, false));
	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::M, true));
	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::M, false));
	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::I, true));
	this->_instructions.push_back(new InstructionAdd(core, InstructionAdd::I, false));

	this->_instructions.push_back(new InstructionSub(core, InstructionSub::R, true));
	this->_instructions.push_back(new InstructionSub(core, InstructionSub::R, false));
	this->_instructions.push_back(new InstructionSub(core, InstructionSub::M, true));
	this->_instructions.push_back(new InstructionSub(core, InstructionSub::M, false));
	this->_instructions.push_back(new InstructionSub(core, InstructionSub::I, true));
	this->_instructions.push_back(new InstructionSub(core, InstructionSub::I, false));

	this->_instructions.push_back(new InstructionMov(core, InstructionMov::RR));
	this->_instructions.push_back(new InstructionMov(core, InstructionMov::MR));
	this->_instructions.push_back(new InstructionMov(core, InstructionMov::RM));

	this->_instructions.push_back(new InstructionInr(core, InstructionInr::R));
	this->_instructions.push_back(new InstructionInr(core, InstructionInr::M));

	this->_instructions.push_back(new InstructionDcr(core, InstructionDcr::R));
	this->_instructions.push_back(new InstructionDcr(core, InstructionDcr::M));

	this->_instructions.push_back(new InstructionXra(core, InstructionXra::R));
	this->_instructions.push_back(new InstructionXra(core, InstructionXra::M));
	this->_instructions.push_back(new InstructionXra(core, InstructionXra::I));

	this->_instructions.push_back(new InstructionOra(core, InstructionOra::R));
	this->_instructions.push_back(new InstructionOra(core, InstructionOra::M));
	this->_instructions.push_back(new InstructionOra(core, InstructionOra::I));

	this->_instructions.push_back(new InstructionCmp(core, InstructionCmp::R));
	this->_instructions.push_back(new InstructionCmp(core, InstructionCmp::M));
	this->_instructions.push_back(new InstructionCmp(core, InstructionCmp::I));

	this->_instructions.push_back(new InstructionRot(core, InstructionRot::RIGHT, false));
	this->_instructions.push_back(new InstructionRot(core, InstructionRot::RIGHT, true));
	this->_instructions.push_back(new InstructionRot(core, InstructionRot::LEFT,  false));
	this->_instructions.push_back(new InstructionRot(core, InstructionRot::LEFT,  true));

	this->_instructions.push_back(new InstructionCma(core));
	this->_instructions.push_back(new InstructionCmc(core));

	this->_instructions.push_back(new InstructionJmp(core, InstructionJmp::Mode::UNCONDITIONED));
	this->_instructions.push_back(new InstructionJmp(core, InstructionJmp::Mode::CONDITIONED));

	this->_instructions.push_back(new InstructionCall(core, InstructionCall::Mode::UNCONDITIONED));
	this->_instructions.push_back(new InstructionCall(core, InstructionCall::Mode::CONDITIONED));

	this->_instructions.push_back(new InstructionRet(core, InstructionRet::Mode::UNCONDITIONED));
	this->_instructions.push_back(new InstructionRet(core, InstructionRet::Mode::CONDITIONED));

	this->_instructions.push_back(new InstructionRst(core));
	this->_instructions.push_back(new InstructionPchl(core));

	this->_instructions.push_back(new InstructionPush(core, true));
	this->_instructions.push_back(new InstructionPush(core, false));

	this->_instructions.push_back(new InstructionPop(core, true));
	this->_instructions.push_back(new InstructionPop(core, false));

	this->_instructions.push_back(new InstructionXthl(core));

	this->_instructions.push_back(new InstructionNop(core));

	for (auto &opcode : this->_instructionLut) {
		opcode = nullptr;
	}

	this->_instructionLut[0x01] = new InstructionLxi(core);
	this->_instructionLut[0x02] = new InstructionStax(core);
	this->_instructionLut[0x03] = new InstructionInx(core);
	this->_instructionLut[0x06] = new InstructionMviR(core);
	this->_instructionLut[0x09] = new InstructionDad(core);
	this->_instructionLut[0x0a] = new InstructionLdax(core);
	this->_instructionLut[0x0b] = new InstructionDcx(core);
	this->_instructionLut[0x0e] = new InstructionMviR(core);
	this->_instructionLut[0x11] = new InstructionLxi(core);
	this->_instructionLut[0x12] = new InstructionStax(core);
	this->_instructionLut[0x13] = new InstructionInx(core);
	this->_instructionLut[0x16] = new InstructionMviR(core);
	this->_instructionLut[0x19] = new InstructionDad(core);
	this->_instructionLut[0x1a] = new InstructionLdax(core);
	this->_instructionLut[0x1b] = new InstructionDcx(core);
	this->_instructionLut[0x1e] = new InstructionMviR(core);
	this->_instructionLut[0x21] = new InstructionLxi(core);
	this->_instructionLut[0x22] = new InstructionShld(core);
	this->_instructionLut[0x23] = new InstructionInx(core);
	this->_instructionLut[0x26] = new InstructionMviR(core);
	this->_instructionLut[0x27] = new InstructionDaa(core);
	this->_instructionLut[0x29] = new InstructionDad(core);
	this->_instructionLut[0x2a] = new InstructionLhld(core);
	this->_instructionLut[0x2b] = new InstructionDcx(core);
	this->_instructionLut[0x2e] = new InstructionMviR(core);
	this->_instructionLut[0x31] = new InstructionLxi(core);
	this->_instructionLut[0x32] = new InstructionSta(core);
	this->_instructionLut[0x33] = new InstructionInx(core);
	this->_instructionLut[0x36] = new InstructionMviM(core);
	this->_instructionLut[0x37] = new InstructionStc(core);
	this->_instructionLut[0x39] = new InstructionDad(core);
	this->_instructionLut[0x3a] = new InstructionLda(core);
	this->_instructionLut[0x3b] = new InstructionDcx(core);
	this->_instructionLut[0x3e] = new InstructionMviR(core);
	this->_instructionLut[0xeb] = new InstructionXchg(core);
	this->_instructionLut[0xf9] = new InstructionSphl(core);

	for (auto i : this->_instructions) {
		for (auto opcode : i->getOpcodes()) {
			if (this->_instructionLut[opcode] != nullptr) {
				throw std::runtime_error("Duplicated opcode handler!");
			}

			this->_instructionLut[opcode] = i;
		}
	}
}


altair::Core::InstructionDecoder::~InstructionDecoder() {
	for (auto i : this->_instructions) {
		for (auto opcode : i->getOpcodes()) {
			this->_instructionLut[opcode] = nullptr;
		}

		delete i;
	}

	this->_instructions.clear();

	for (auto &opcode : this->_instructionLut) {
		if (opcode != nullptr) {
			delete opcode;
		}

		opcode = nullptr;
	}
}


altair::Core::Instruction *altair::Core::InstructionDecoder::decode(uint8_t binaryInstruction) {
	return this->_instructionLut[binaryInstruction];
}

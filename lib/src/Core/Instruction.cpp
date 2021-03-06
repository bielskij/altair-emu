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


const std::set<altair::Core::BReg> altair::Core::Instruction::_allBregs = {
	Core::BReg::B, Core::BReg::C, Core::BReg::D, Core::BReg::E,
	Core::BReg::H, Core::BReg::L, Core::BReg::A
};

const std::set<altair::Core::Cond> altair::Core::Instruction::_allConds = {
	Core::Cond::NOT_ZERO, Core::Cond::ZERO, Core::Cond::NO_CARRY,
	Core::Cond::CARRY, Core::Cond::PARITY_ODD, Core::Cond::PARITY_EVEN,
	Core::Cond::PLUS, Core::Cond::MINUS,
};


altair::Core::Instruction::Instruction(Core *core) {
	this->_core        = core;
	this->_cycleIdx    = 0;
	this->_cyclesCount = 0;
}


altair::Core::Instruction::~Instruction() {

}


void altair::Core::Instruction::addCycle(MachineCycle *cycle) {
	this->_cycles[this->_cyclesCount++] = cycle;

	cycle->setParent(this);
}

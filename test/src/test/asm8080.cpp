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

#include "test/asm8080.hpp"


const asm8080::Reg8 asm8080::B(0);
const asm8080::Reg8 asm8080::C(1);
const asm8080::Reg8 asm8080::D(2);
const asm8080::Reg8 asm8080::E(3);
const asm8080::Reg8 asm8080::H(4);
const asm8080::Reg8 asm8080::L(5);
const asm8080::Reg8 asm8080::M(6);
const asm8080::Reg8 asm8080::A(7);

//const asm8080::Reg8 F;
//
//const asm8080::Reg16 BC;
//const asm8080::Reg16 DE;
//const asm8080::Reg16 HL;
//const asm8080::Reg16 APSW;
//const asm8080::Reg16 SP;
//const asm8080::Reg16 PC;


asm8080::Imm8::Imm8(uint8_t val) {
	this->_val = val;
}


uint8_t asm8080::Imm8::val() const {
	return this->_val;
}


asm8080::Reg8::Reg8(uint8_t val) {
	this->_val = val;
}


uint8_t asm8080::Reg8::getVal(bool isSrc) const {
	return isSrc ? this->_val : (this->_val << 3);
}


asm8080::Compiler::Compiler() {
	this->_addr = 0;

	this->_memory.resize(64 * 1024, 0x00);
}


asm8080::Compiler::~Compiler() {

}


asm8080::Compiler &asm8080::Compiler::adi(const Imm8 &imm8) {
	this->_memory[this->_addr++] = 0xc6;
	this->_memory[this->_addr++] = imm8.val();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::mvi(const Reg8 &reg, const Imm8 &imm) {
	this->_memory[this->_addr++] = 0x06 | reg.getVal(false);
	this->_memory[this->_addr++] = imm.val();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::org(uint16_t address) {
	this->_addr = address;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::label(const std::string &name) {
	return *this;
}


asm8080::Compiler &asm8080::Compiler::db(const Imm8 &value) {
	this->_memory[this->_addr++] = value.val();

	return *this;
}


std::vector<uint8_t> asm8080::Compiler::toBin() {
	return this->_memory;
}

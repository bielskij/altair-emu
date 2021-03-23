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


const asm8080::Reg8 asm8080::B = asm8080::Reg8::fromRaw(0);
const asm8080::Reg8 asm8080::C = asm8080::Reg8::fromRaw(1);
const asm8080::Reg8 asm8080::D = asm8080::Reg8::fromRaw(2);
const asm8080::Reg8 asm8080::E = asm8080::Reg8::fromRaw(3);
const asm8080::Reg8 asm8080::H = asm8080::Reg8::fromRaw(4);
const asm8080::Reg8 asm8080::L = asm8080::Reg8::fromRaw(5);
const asm8080::Reg8 asm8080::M = asm8080::Reg8::fromRaw(6);
const asm8080::Reg8 asm8080::A = asm8080::Reg8::fromRaw(7);

//const asm8080::Reg8 F;
//
const asm8080::Reg16 asm8080::BC   = asm8080::Reg16::fromRaw(0);
const asm8080::Reg16 asm8080::DE   = asm8080::Reg16::fromRaw(1);
const asm8080::Reg16 asm8080::HL   = asm8080::Reg16::fromRaw(2);
const asm8080::Reg16 asm8080::PSWA = asm8080::Reg16::fromRaw(3);
const asm8080::Reg16 asm8080::SP   = asm8080::Reg16::fromRaw(3);
//const asm8080::Reg16 PC;


asm8080::Imm8::Imm8(uint8_t val) {
	this->_val = val;
}


uint8_t asm8080::Imm8::val() const {
	return this->_val;
}


asm8080::Imm16::Imm16(uint16_t val) {
	this->_val = val;
}


uint8_t asm8080::Imm16::valH() const {
	return this->_val >> 8;
}


uint8_t asm8080::Imm16::valL() const {
	return this->_val & 0xff;
}


asm8080::Reg8::Reg8(uint8_t val) {
	this->_val = val;
}


uint8_t asm8080::Reg8::getVal(bool isSrc) const {
	return isSrc ? this->_val : (this->_val << 3);
}


asm8080::Reg8 asm8080::Reg8::fromRaw(uint8_t val) {
	return Reg8(val);
}


asm8080::Reg16::Reg16(uint8_t val) {
	this->_val = val;
}


uint8_t asm8080::Reg16::getVal() const {
	return this->_val << 4;
}


asm8080::Reg16 asm8080::Reg16::fromRaw(uint8_t val) {
	return Reg16(val);
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


asm8080::Compiler &asm8080::Compiler::lxi(const Reg16 &reg, const Imm16 &imm) {
	this->_memory[this->_addr++] = 0x01 | reg.getVal();
	this->_memory[this->_addr++] = imm.valL();
	this->_memory[this->_addr++] = imm.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::mov(const Reg8 &dst, const Reg8 &src) {
	this->_memory[this->_addr++] = 0x40 | dst.getVal(false) | src.getVal(true);

	return *this;
}


asm8080::Compiler &asm8080::Compiler::push(const Reg16 &reg) {
	this->_memory[this->_addr++] = 0xc5 | reg.getVal();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::pop(const Reg16 &reg) {
	this->_memory[this->_addr++] = 0xc1 | reg.getVal();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::stc() {
	this->_memory[this->_addr++] = 0x37;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::rrc() {
	this->_memory[this->_addr++] = 0x0f;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::call(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xcd;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::ret() {
	this->_memory[this->_addr++] = 0xc9;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::in(const Imm8 &port) {
	this->_memory[this->_addr++] = 0xdb;
	this->_memory[this->_addr++] = port.val();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::out(const Imm8 &port) {
	this->_memory[this->_addr++] = 0xd3;
	this->_memory[this->_addr++] = port.val();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::ei() {
	this->_memory[this->_addr++] = 0xfb;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::di() {
	this->_memory[this->_addr++] = 0xf3;

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jmp(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xc3;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jc(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xda;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jnc(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xd2;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jz(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xca;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jnz(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xc2;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jp(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xf2;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jm(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xfa;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jpe(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xea;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

	return *this;
}


asm8080::Compiler &asm8080::Compiler::jpo(const Imm16 &address) {
	this->_memory[this->_addr++] = 0xe2;
	this->_memory[this->_addr++] = address.valL();
	this->_memory[this->_addr++] = address.valH();

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

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

#ifndef ASM8080_HPP_
#define ASM8080_HPP_

#include <vector>
#include <string>
#include <cstdint>

namespace asm8080 {
	class Operand {
		public:
			virtual ~Operand() {}
	};

	class Reg8 : public Operand {
		public:
			uint8_t getVal(bool isSrc) const;

			static Reg8 fromRaw(uint8_t val);

		private:
			Reg8(uint8_t val);

		private:
			uint8_t _val;
	};

	class Reg16 : public Operand {
		public:
			uint8_t getVal() const;

			static Reg16 fromRaw(uint8_t val);

		private:
			Reg16(uint8_t val);

		private:
			uint8_t _val;
	};

	class Imm8 : public Operand {
		public:
			Imm8(uint8_t val);
			uint8_t val() const;

		private:
			uint8_t _val;

	};

	class Imm16 : public Operand {
		public:
			Imm16(uint16_t val);
			uint8_t valH() const;
			uint8_t valL() const;

		private:
			uint16_t _val;
	};

	class Compiler {
		public:


		public:


		public:
			Compiler();
			virtual ~Compiler();

		public:
			Compiler &adi(const Imm8 &imm);
			Compiler &mvi(const Reg8 &reg, const Imm8 &imm);
			Compiler &lxi(const Reg16 &reg, const Imm16 &imm);

			Compiler &mov(const Reg8 &dst, const Reg8 &src);

			Compiler &push(const Reg16 &reg);
			Compiler &pop(const Reg16 &reg);

			Compiler &stc();
			Compiler &rrc();

			Compiler &call(const Imm16 &address);
			Compiler &ret();

			Compiler &jmp(const Imm16 &address);
			Compiler &jc(const Imm16 &address);
			Compiler &jnc(const Imm16 &address);
			Compiler &jz(const Imm16 &address);
			Compiler &jnz(const Imm16 &address);
			Compiler &jp(const Imm16 &address);
			Compiler &jm(const Imm16 &address);
			Compiler &jpe(const Imm16 &address);
			Compiler &jpo(const Imm16 &address);

			Compiler &in(const Imm8 &port);
			Compiler &out(const Imm8 &port);

			Compiler &ei();
			Compiler &di();

			Compiler &org(uint16_t address);
			Compiler &label(const std::string &name);
			Compiler &db(const Imm8 &value);

			std::vector<uint8_t> toBin();

		private:
			std::vector<uint8_t> _memory;
			size_t               _addr;
	};

	extern const Reg8 B;
	extern const Reg8 C;
	extern const Reg8 D;
	extern const Reg8 E;
	extern const Reg8 H;
	extern const Reg8 L;
	extern const Reg8 A;

	extern const Reg8 M;
	extern const Reg8 F;

	extern const Reg16 BC;
	extern const Reg16 DE;
	extern const Reg16 HL;
	extern const Reg16 PSWA;
	extern const Reg16 SP;
	extern const Reg16 PC;
}



#endif /* ASM8080_HPP_ */

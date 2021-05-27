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
			Compiler &mov(const Reg8 &dst, const Reg8 &src);
			Compiler &mvi(const Reg8 &reg, const Imm8 &imm);
			Compiler &lxi(const Reg16 &reg, const Imm16 &imm);
			Compiler &lda(const Imm16 &imm);
			Compiler &sta(const Imm16 &imm);
			Compiler &lhld(const Imm16 &imm);
			Compiler &shld(const Imm16 &imm);
			Compiler &ldax(const Reg16 &reg);
			Compiler &stax(const Reg16 &reg);
			Compiler &xchg();

			Compiler &add(const Reg8 &src);
			Compiler &addM();
			Compiler &adi(const Imm8 &imm);
			Compiler &adc(const Reg8 &src);
			Compiler &adcM();
			Compiler &aci(const Imm8 &imm);
			Compiler &sub(const Reg8 &src);
			Compiler &subM();
			Compiler &sui(const Imm8 &imm);
			Compiler &sbb(const Reg8 &src);
			Compiler &sbbM();
			Compiler &sbi(const Imm8 &imm);
			Compiler &inr(const Reg8 &reg);
			Compiler &inrM();
			Compiler &dcr(const Reg8 &reg);
			Compiler &dcrM();
			Compiler &inx(const Reg16 &reg);
			Compiler &dcx(const Reg16 &reg);
			Compiler &dad(const Reg16 &reg);
			Compiler &daa();
			Compiler &ana(const Reg8 &src);
			Compiler &anaM();
			Compiler &ani(const Imm8 &imm);
			Compiler &xra(const Reg8 &src);
			Compiler &xraM();
			Compiler &xri(const Imm8 &imm);
			Compiler &ora(const Reg8 &src);
			Compiler &oraM();
			Compiler &ori(const Imm8 &imm);
			Compiler &cmp(const Reg8 &src);
			Compiler &cmpM();
			Compiler &rlc();
			Compiler &rrc();
			Compiler &ral();
			Compiler &rar();
			Compiler &cma();
			Compiler &stc();
			Compiler &cmc();


			Compiler &call(const Imm16 &address);
			Compiler &cc(const Imm16 &address);
			Compiler &cnc(const Imm16 &address);
			Compiler &cz(const Imm16 &address);
			Compiler &cnz(const Imm16 &address);
			Compiler &cp(const Imm16 &address);
			Compiler &cm(const Imm16 &address);
			Compiler &cpe(const Imm16 &address);
			Compiler &cpo(const Imm16 &address);
			Compiler &ret();
			Compiler &rc();
			Compiler &rnc();
			Compiler &rz();
			Compiler &rnz();
			Compiler &rp();
			Compiler &rm();
			Compiler &rpe();
			Compiler &rpo();
			Compiler &pchl();


			Compiler &push(const Reg16 &reg);
			Compiler &pop(const Reg16 &reg);
			Compiler &xthl();
			Compiler &sphl();
			Compiler &in(const Imm8 &port);
			Compiler &out(const Imm8 &port);
			Compiler &ei();
			Compiler &di();

			Compiler &cpi(const Imm8 &imm);
			Compiler &jmp(const Imm16 &address);
			Compiler &jc(const Imm16 &address);
			Compiler &jnc(const Imm16 &address);
			Compiler &jz(const Imm16 &address);
			Compiler &jnz(const Imm16 &address);
			Compiler &jp(const Imm16 &address);
			Compiler &jm(const Imm16 &address);
			Compiler &jpe(const Imm16 &address);
			Compiler &jpo(const Imm16 &address);

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

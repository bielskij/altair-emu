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
#ifndef ALTAIR_UTILS_HPP_
#define ALTAIR_UTILS_HPP_

#include <limits.h>
#include <stdlib.h>

#include <string>
#include <algorithm>

#include "altair/Core.hpp"

namespace altair {
	class Utils {
		public:
			static std::string bregToString(InterpretedCore::BReg breg) {
				std::string ret;

				switch (breg) {
					case InterpretedCore::BReg::A: return "a";
					case InterpretedCore::BReg::B: return "b";
					case InterpretedCore::BReg::C: return "c";
					case InterpretedCore::BReg::D: return "d";
					case InterpretedCore::BReg::E: return "e";
					case InterpretedCore::BReg::H: return "h";
					case InterpretedCore::BReg::L: return "l";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown breg.");
			}

			static std::string wregToString(InterpretedCore::WReg wreg) {
				std::string ret;

				switch (wreg) {
					case InterpretedCore::WReg::SP: return "sp";
					case InterpretedCore::WReg::B:  return "b";
					case InterpretedCore::WReg::D:  return "d";
					case InterpretedCore::WReg::H:  return "h";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown wreg.");
			}

			static std::string condToString(InterpretedCore::Cond cond) {
				std::string ret;

				switch (cond) {
					case InterpretedCore::Cond::NOT_ZERO:    return "nz";
					case InterpretedCore::Cond::ZERO:        return "z";
					case InterpretedCore::Cond::NO_CARRY:    return "nc";
					case InterpretedCore::Cond::CARRY:       return "c";
					case InterpretedCore::Cond::PARITY_ODD:  return "po";
					case InterpretedCore::Cond::PARITY_EVEN: return "pe";
					case InterpretedCore::Cond::PLUS:        return "p";
					case InterpretedCore::Cond::MINUS:       return "m";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown condition.");
			}

			static std::string rpToString(InterpretedCore::WReg reg) {
				std::string ret;

				switch(reg) {
					case InterpretedCore::WReg::B:  return "b";
					case InterpretedCore::WReg::D:  return "d";
					case InterpretedCore::WReg::H:  return "h";
					case InterpretedCore::WReg::SP: return "sp";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown rp.");
			}
	};
}

#endif /* ALTAIR_UTILS_HPP_ */

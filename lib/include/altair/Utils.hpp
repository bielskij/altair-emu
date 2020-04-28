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

#include <cstring>
#include <algorithm>

#include "altair/Core.hpp"

namespace altair {
	class Utils {
		public:
			static std::string bregToString(Core::BReg breg) {
				std::string ret;

				switch (breg) {
					case Core::BReg::A: return "a";
					case Core::BReg::B: return "b";
					case Core::BReg::C: return "c";
					case Core::BReg::D: return "d";
					case Core::BReg::E: return "e";
					case Core::BReg::H: return "h";
					case Core::BReg::L: return "l";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown breg.");
			}

			static std::string wregToString(Core::WReg wreg) {
				std::string ret;

				switch (wreg) {
					case Core::WReg::SP: return "sp";
					case Core::WReg::B:  return "b";
					case Core::WReg::D:  return "d";
					case Core::WReg::H:  return "h";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown wreg.");
			}

			static std::string condToString(Core::Cond cond) {
				std::string ret;

				switch (cond) {
					case Core::Cond::NOT_ZERO:    return "nz";
					case Core::Cond::ZERO:        return "z";
					case Core::Cond::NO_CARRY:    return "nc";
					case Core::Cond::CARRY:       return "c";
					case Core::Cond::PARITY_ODD:  return "po";
					case Core::Cond::PARITY_EVEN: return "pe";
					case Core::Cond::PLUS:        return "p";
					case Core::Cond::MINUS:       return "m";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown condition.");
			}

			static std::string rpToString(Core::WReg reg) {
				std::string ret;

				switch(reg) {
					case Core::WReg::B:  return "b";
					case Core::WReg::D:  return "d";
					case Core::WReg::H:  return "h";
					case Core::WReg::SP: return "sp";
				}

				throw std::invalid_argument("Cannot convert to string: Unknown rp.");
			}
	};
}

#endif /* ALTAIR_UTILS_HPP_ */

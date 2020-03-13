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

#include <algorithm>

#include "altair/Core.hpp"

namespace altair {
	class Utils {
		public:
			static inline std::string bregToString(Core::BReg reg) {
				switch (reg) {
					case Core::BReg::A: return "a";
					case Core::BReg::B: return "b";
					case Core::BReg::C: return "c";
					case Core::BReg::D: return "d";
					case Core::BReg::E: return "e";
					case Core::BReg::H: return "h";
					case Core::BReg::L: return "l";
					default: return "?";
				}
			}

			static uint8_t hexToBin(const char hex[2]) {
				uint8_t ret = 0;
				uint8_t i = 0;

				while (i != 2) {
					char x = hex[i];
					uint8_t shift = 4 * (1 - i);

					if (x >= '0' && x <= '9') {
						ret |= ((x - '0') << shift);

					} else if (x >= 'A' && x <= 'F') {
						ret |= ((x - 'A' + 10) << shift);

					} else {
						ret |= ((x - 'a' + 10) << shift);
					}

					i++;
				}

				return ret;
			}

			static inline std::string toLower(const std::string &str) {
				std::string ret;

				if (str.empty()) {
					return ret;
				}

				std::transform(str.begin(), str.end(), ret.begin(), ::tolower);

				return ret;
			}

			static std::string trimRight(const std::string &text, const std::string &trim) {
				std::string ret = text;

				size_t found = ret.find_last_not_of(trim);

				if (found != std::string::npos) {
					ret.erase(found + 1);
				} else {
					ret.clear();
				}

				return ret;
			}

			static std::string trimLeft(const std::string &text, const std::string &trim) {
				size_t strBegin = text.find_first_not_of(trim);
				if (strBegin == std::string::npos) {
					return ""; // no content

				} else {
					return text.substr(strBegin);
				}
			}

			static std::string trim(const std::string &text, const std::string &trim) {
				return trimLeft(trimRight(text, trim), trim);
			}
	};
}

#endif /* ALTAIR_UTILS_HPP_ */

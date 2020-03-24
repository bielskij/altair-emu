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
#ifndef COMMON_UTILS_HPP_
#define COMMON_UTILS_HPP_

#include <limits.h>
#include <stdlib.h>

#include <cstring>
#include <cstdarg>
#include <algorithm>

#include "altair/Core.hpp"

namespace common {
	class Utils {
		public:
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
					return str;
				}

				ret.resize(str.size());

				std::transform(str.begin(), str.end(), ret.begin(), ::tolower);

				return ret;
			}

			static std::string trimRight(const std::string &text, char trim) {
				std::string ret = text;

				size_t found = ret.find_last_not_of(trim);

				if (found != std::string::npos) {
					ret.erase(found + 1);
				} else {
					ret.clear();
				}

				return ret;
			}

			static std::string trimLeft(const std::string &text, char trim) {
				size_t strBegin = text.find_first_not_of(trim);
				if (strBegin == std::string::npos) {
					return ""; // no content

				} else {
					return text.substr(strBegin);
				}
			}

			static std::string trim(const std::string &text, const std::string &trim) {
				std::string ret = text;

				for (auto c : trim) {
					ret = trimLeft(trimRight(ret, c), c);
				}

				return ret;
			}

			static std::string getFileExtension(const std::string &path) {
				std::string extension;

				if (path.empty()) {
					return path;
				}

				{
					size_t idx = path.find_last_of('.');
					if (idx != std::string::npos) {
						extension = path.substr(idx);
					}
				}

				if (! extension.empty()) {
					extension = toLower(extension);
				}

				return extension;
			}

			static uint32_t toUint32(const std::string &str) {
				return strtoul(str.c_str(), nullptr, 0);
			}

			static uint16_t toUint16(const std::string &str) {
				return toUint32(str);
			}

			static uint8_t toUint8(const std::string &str) {
				return toUint32(str);
			}

			static std::string getRealPath(const std::string &path) {
				std::string ret;

				{
					char resolved[PATH_MAX];

					realpath(path.c_str(), resolved);

					ret = resolved;
				}

				return ret;
			}

			static std::string dirname(const std::string &path) {
				if(path == "") {
					return std::string("");
				}

				size_t found = path.find_last_of("/\\");
				if(found == std::string::npos) {
					return std::string(".");

				} else if(found == 0) {
					return std::string(path.begin(),path.begin() + 1);

				} else if(found == path.length() - 1) {
					std::string redo = std::string(path.begin(), path.end() - 1);

					return dirname(redo);
				}

				return std::string(path.begin(), path.begin() + found);
			}

			static std::string sprintf(const char *format, va_list &va) {
				const uint32_t bufferSize = 1024;
				char           buffer[bufferSize];

				vsnprintf(buffer, bufferSize, format, va);

				return std::string(buffer);
			}

			static std::string sprintf(const char *format, ...) {
				va_list ap;

				va_start(ap, format);
				std::string ret = sprintf(format, ap);
				va_end(ap);

				return ret;
			}

			static std::string uint8ToString(uint8_t val, bool hex = false) {
				std::string ret;

				if (hex) {
					ret.push_back(_hexCharsSmall[val >> 4]);
					ret.push_back(_hexCharsSmall[val & 0x0f]);

				} else {
					ret = Utils::sprintf("%hhu", val);
				}

				return ret;
			}

			static std::string uint16ToString(uint8_t val, bool hex = false) {
				std::string ret;

				if (hex) {
					ret += uint8ToString(val >> 8, true);
					ret += uint8ToString(val, true);

				} else {
					ret = Utils::sprintf("%hu", val);
				}

				return ret;
			}

		private:
			static constexpr char *_hexCharsBig   = "0123456789ABCDEF";
			static constexpr char *_hexCharsSmall = "0123456789abcdef";
	};
}

#endif /* COMMON_UTILS_HPP_ */

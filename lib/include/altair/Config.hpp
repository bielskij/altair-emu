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
#ifndef ALTAIR_CONFIG_HPP_
#define ALTAIR_CONFIG_HPP_

#include <cstdint>

namespace altair {
	class Config {
		public:
			static uint32_t getClkFrequency() {
				return _clkFrequency;
			}

			static void setClkFrequency(uint32_t freq) {
				_clkFrequency = freq;
			}

			static uint32_t getClkMultiplier() {
				return _clkMultiplier;
			}

			static void setClkMultiplier(uint32_t multiplier) {
				_clkMultiplier = multiplier;
			}

		private:
			Config() = delete;
			Config(const Config &) = delete;

		private:
			static uint32_t _clkFrequency;
			static uint32_t _clkMultiplier;
	};
}

#endif /* ALTAIR_CONFIG_HPP_ */

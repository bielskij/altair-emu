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

#ifndef CORE_PIO_HPP_
#define CORE_PIO_HPP_

#include <vector>
#include <stdexcept>
#include "test/Core.hpp"

namespace test {
	class Pio : public test::Core::Pio {
		public:
			uint16_t address;
			uint8_t  data;
			uint32_t clkCount;

			bool sync;
			bool dbin;
			bool wr;
			bool wait;
			bool intr;
			bool inte;

			std::vector<uint8_t> program;

		public:
			Pio(const std::vector<uint8_t> &program) : program(program) {
				this->clkCount = 0;
				this->address  = 0;
				this->data     = 0;
				this->sync     = false;
				this->dbin     = false;
				this->wr       = false;
				this->wait     = false;
				this->intr     = false;
				this->inte     = false;
			}

			bool getInt() override;
			bool getHold() override;
			bool getReady() override;
			bool getReset() override;
			void setInt(bool active) override;
			void setWr(bool active) override;
			void setDbin(bool active) override;
			void setInte(bool active) override;
			void setHoldAck(bool active) override;
			void setWait(bool active) override;
			void setSync(bool active) override;
			uint8_t getData() override;
			void setData(uint8_t val) override;
			void setAddress(uint16_t val) override;
			void clk() override;

		public:
			uint8_t mem(uint16_t addr) {
				if (addr >= this->program.size()) {
					throw std::runtime_error("Read outside memory!");
				}
				return this->program[addr];
			}
	};
}

#endif /* CORE_PIO_HPP_ */

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
#ifndef ALTAIR_UTILS_TERMINAL_HPP_
#define ALTAIR_UTILS_TERMINAL_HPP_

#include <thread>
#include <deque>
#include <cstdint>

namespace altair {
	namespace utils {
		class Terminal {
			public:
				Terminal();
				virtual ~Terminal();

				void canReadWrite(bool &read, bool &write);

				uint8_t readByte();
				void    writeByte(uint8_t val);

			private:
				static void threadRoutine(Terminal *term);

			private:
				int         ptyMasterFd;
				std::string ptyPath;
				pid_t       xtermPid;

				std::deque<uint8_t> fifo;
				std::thread         fifoThread;
				bool                fifoThreadRunning;
		};
	}
}

#endif /* ALTAIR_UTILS_TERMINAL_HPP_ */

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

#include <cstdint>
#include <string>
#include <stdexcept>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "altair/utils/Terminal.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


altair::utils::Terminal::Terminal() {
	bool fail = true;

	this->ptyMasterFd = -1;
	this->xtermPid    = 0;

	do {
		this->ptyMasterFd = posix_openpt(O_RDWR | O_NOCTTY);
		if (this->ptyMasterFd < 0) {
			ERR(("Unable to open PTY!"));

			break;
		}

		{
			char path[FILENAME_MAX];

			if (ptsname_r(this->ptyMasterFd, path, FILENAME_MAX) != 0) {
				ERR(("Unable to get PTY path!"));

				break;
			}

			this->ptyPath = path;
		}

		if (grantpt(this->ptyMasterFd) != 0) {
			ERR(("Unable to grant access to the slave pseudoterminal!"));

			break;
		}

		if (unlockpt(this->ptyMasterFd) != 0) {
			ERR(("Unable to unlock master/slave pair!"));

			break;
		}

		DBG(("PTY path: %s, mFd: %d", this->ptyPath.c_str(), this->ptyMasterFd));

		this->xtermPid = fork();
		if (this->xtermPid < 0) {
			ERR(("Fork failed! (%d)", this->xtermPid));

			break;

		} else if (this->xtermPid > 0) {
			char c;

			// One interesting thing is that xterm writes its window ID to
			// the pty master after successful initialization. This is something
			// you'll need to deal with. It appears as a line of input on
			// the tty before the actual user input begins.
			while (::read(this->ptyMasterFd, &c, 1) == 1 && c != '\n');

		// child process
		} else {
			int slaveFd;

			// Close master fd
			close(this->ptyMasterFd);

			slaveFd = open(this->ptyPath.c_str(), O_RDWR | O_NOCTTY);
			if (slaveFd >= 0) {
				char argLine[FILENAME_MAX];

				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);

				dup2(slaveFd, STDIN_FILENO);
				dup2(slaveFd, STDOUT_FILENO);
				dup2(slaveFd, STDERR_FILENO);

				snprintf(argLine, FILENAME_MAX, "-S%s/%d", this->ptyPath.c_str(), slaveFd);

				// Apply terminal parameters
				{
					struct termios termOps;

					if (tcgetattr(slaveFd, &termOps) < 0) {
						ERR(("Unable to get terminal windows size!"));

						break;
					}

					// settings taken from simh emulator
					termOps.c_lflag = termOps.c_lflag & ~(ECHO | ICANON);
					termOps.c_oflag = termOps.c_oflag & ~OPOST;
					termOps.c_iflag = termOps.c_iflag & ~ICRNL;

					termOps.c_cc[VMIN]     = 1;
					termOps.c_cc[VTIME]    = 0;

					termOps.c_cc[VQUIT]    = 0;
					termOps.c_cc[VERASE]   = 0;
					termOps.c_cc[VKILL]    = 0;
					termOps.c_cc[VEOF]     = 0;
					termOps.c_cc[VEOL]     = 0;
					termOps.c_cc[VSTART]   = 0;
					termOps.c_cc[VSUSP]    = 0;
					termOps.c_cc[VSTOP]    = 0;
					termOps.c_cc[VREPRINT] = 0;
					termOps.c_cc[VDISCARD] = 0;
					termOps.c_cc[VWERASE]  = 0;
					termOps.c_cc[VLNEXT]   = 0;

					if (tcsetattr(slaveFd, TCSANOW, &termOps) != 0) {
						ERR(("Unable to set terminal window size!"));

						break;
					}
				}

				// Set terminal size
				{
					struct winsize ws;

					ws.ws_col = 80;
					ws.ws_row = 24;

					ws.ws_xpixel = 0;
					ws.ws_ypixel = 0;

					if (ioctl(slaveFd, TIOCSWINSZ, &ws) < 0) {
						ERR(("Unable to set terminal size!"));

						break;
					}
				}

				execlp("xterm", "xterm", "-bc", "-ti", "vt100", "-fa", "Monospace", "-fs", "14", argLine, (char *) 0);

				exit(1);
			}
		}

		fail = false;
	} while (0);

	if (fail) {
		if (this->ptyMasterFd >= 0) {
			close(this->ptyMasterFd);
		}
	}
}


altair::utils::Terminal::~Terminal() {
	if (this->xtermPid != 0) {
		int xtermExitStatus;

		kill(this->xtermPid, SIGTERM);

		waitpid(this->xtermPid, &xtermExitStatus, 0);

		this->xtermPid = 0;
	}

	if (this->ptyMasterFd >= 0) {
		close(this->ptyMasterFd);

		this->ptyMasterFd = -1;
	}
}


void altair::utils::Terminal::canReadWrite(bool &read, bool &write) {
	read  = false;
	write = false;

	if (this->ptyMasterFd >= 0) {
		Terminal::select(this->ptyMasterFd, read, write);
	}
}


uint8_t altair::utils::Terminal::readByte() {
	uint8_t ret;

	if (::read(this->ptyMasterFd, &ret, 1) != 1) {
		throw std::runtime_error("read() cannot read!");
	}

	return ret;
}


void altair::utils::Terminal::writeByte(uint8_t val) {
	if (::write(this->ptyMasterFd, &val, 1) != 1) {
		throw std::runtime_error("write() cannot write!");
	}
}


void altair::utils::Terminal::select(int fd, bool &canRead, bool &canWrite) {
	canRead  = false;
	canWrite = false;

	fd_set readSet;
	fd_set writeSet;

	struct timeval timeout;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);

	FD_SET(fd, &readSet);
	FD_SET(fd, &writeSet);

	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;

	if (::select(fd + 1, &readSet, &writeSet, NULL, &timeout)) {
		if (FD_ISSET(fd, &readSet)) {
			canRead = true;
		}

		if (FD_ISSET(fd, &writeSet)) {
			canWrite = true;
		}
	}
}

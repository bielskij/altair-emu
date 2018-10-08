/*******************************************************************************
 * Copyright (C) 2018 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/select.h>

#include "serial.h"

#define DEBUG_LEVEL 2
#include "common/debug.h"


#define IGNORE_RET(__f, __args) (void) __f __args;


#define SELECT_READ  (1 << 0)
#define SELECT_WRITE (1 << 1)


static _U8 _select(int fd) {
	_U8 ret = 0;

	fd_set readSet;
	fd_set writeSet;

	struct timeval timeout;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);

	FD_SET(fd, &readSet);
	FD_SET(fd, &writeSet);

	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;

	if (select(fd + 1, &readSet, &writeSet, NULL, &timeout)) {
		if (FD_ISSET(fd, &readSet)) {
			ret |= SELECT_READ;
		}

		if (FD_ISSET(fd, &writeSet)) {
			ret |= SELECT_WRITE;
		}
	}

	return ret;
}



_U8 serial_open(SerialContext *ctx) {
	_U8 ret = 1;

	ctx->ptyMasterFd = -1;
	ctx->ptyPath[0]  = '\0';
	ctx->xtermPid    = 0;

	do {
		ctx->ptyMasterFd = posix_openpt(O_RDWR | O_NOCTTY);
		if (ctx->ptyMasterFd < 0) {
			ERR(("Unable to open PTY!"));

			ret = 0;
			break;
		}

		if (ptsname_r(ctx->ptyMasterFd, ctx->ptyPath, FILENAME_MAX) != 0) {
			ERR(("Unable to get PTY path!"));

			ret = 0;
			break;
		}

		if (grantpt(ctx->ptyMasterFd) != 0) {
			ERR(("Unable to grant access to the slave pseudoterminal!"));

			ret = 0;
			break;
		}

		if (unlockpt(ctx->ptyMasterFd) != 0) {
			ERR(("Unable to unlock master/slave pair!"));

			ret = 0;
			break;
		}

		DBG(("PTY path: %s, mFd: %d", ctx->ptyPath, ctx->ptyMasterFd));

		ctx->xtermPid = fork();
		if (ctx->xtermPid < 0) {
			ERR(("Fork failed! (%d)", ctx->xtermPid));

			ret = 0;
			break;

		} else if (ctx->xtermPid > 0) {
			char c;

			// One interesting thing is that xterm writes its window ID to
			// the pty master after successful initialization. This is something
			// you'll need to deal with. It appears as a line of input on
			// the tty before the actual user input begins.
			while (read(ctx->ptyMasterFd, &c, 1) == 1 && c != '\n');

		// child process
		} else {
			int slaveFd;

			// Close master fd
			close(ctx->ptyMasterFd);

			slaveFd = open(ctx->ptyPath, O_RDWR | O_NOCTTY);
			if (slaveFd >= 0) {
				char argLine[FILENAME_MAX];

				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);

				dup2(slaveFd, STDIN_FILENO);
				dup2(slaveFd, STDOUT_FILENO);
				dup2(slaveFd, STDERR_FILENO);

				snprintf(argLine, FILENAME_MAX, "-S%s/%d", ctx->ptyPath, slaveFd);

				// Apply terminal parameters
				{
					struct termios termOps;

					if (tcgetattr(slaveFd, &termOps) < 0) {
						ERR(("Unable to get terminal windows size!"));

						ret = 0;
						break;
					}

					termOps.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
					termOps.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
					termOps.c_cflag &= ~(CSIZE|PARENB);
					termOps.c_cflag |= CS8;
					termOps.c_oflag &= ~(OPOST);

					termOps.c_cc[VMIN]  = 1;
					termOps.c_cc[VTIME] = 0;

					if (tcsetattr(slaveFd, TCSANOW, &termOps) != 0) {
						ERR(("Unable to set terminal window size!"));

						ret = 0;
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

						ret = 0;
						break;
					}
				}

				execlp("xterm", "xterm", argLine, (char *) 0);

				exit(1);
			}
		}
	} while (0);

	return ret;
}


void serial_close(SerialContext *ctx) {
	if (ctx->xtermPid != 0) {
		int xtermExitStatus;

		kill(ctx->xtermPid, SIGTERM);

		waitpid(ctx->xtermPid, &xtermExitStatus, 0);

		ctx->xtermPid = 0;
	}

	if (ctx->ptyMasterFd >= 0) {
		close(ctx->ptyMasterFd);

		ctx->ptyMasterFd = -1;
	}
}


_U8 serial_canRead(SerialContext *ctx) {
	_U8 ret = 0;

	if (ctx->ptyMasterFd >= 0) {
		ret = (_select(ctx->ptyMasterFd) & SELECT_READ) != 0;
	}

	return ret;
}


_U8 serial_canWrite(SerialContext *ctx) {
	_U8 ret = 0;

	if (ctx->ptyMasterFd >= 0) {
		ret = (_select(ctx->ptyMasterFd) & SELECT_WRITE) != 0;
	}

	return ret;
}


_U8 serial_readByte(SerialContext *ctx) {
	_U8 ret = 0;

	if (read(ctx->ptyMasterFd, &ret, 1) != 1) {
		ERR(("read returned error!"));
	}

	return ret;
}


void serial_writeByte(SerialContext *ctx, _U8 data) {
	if (write(ctx->ptyMasterFd, &data, 1) != 1) {
		ERR(("write returned error!"));
	}
}

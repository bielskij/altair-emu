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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "arch/module.h"

#include "emulator/altair/module/88-2sio.h"

#define DEBUG_LEVEL 0
#include "common/debug.h"


#define SELECT_READ  (1 << 0)
#define SELECT_WRITE (1 << 1)

typedef struct _PtyContext {
	int  ptyMasterFd;
	int  ptySlaveFd;
	char ptyPath[FILENAME_MAX];
} PtyContext;


typedef struct _Sio882Data {
	PtyContext *ports[2];
} Sio882Data;


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


int _ptyOpen(PtyContext *ctx) {
	int ret = 1;

	do {
		int ttyFd;

		struct winsize wsize;

		memset(&wsize, 0, sizeof(wsize));
		// VT100
		wsize.ws_col = 24;
		wsize.ws_row = 80;

		ctx->ptyMasterFd = posix_openpt(O_RDWR | O_NOCTTY);

		if (ctx->ptyMasterFd < 0) {
			ERR(("Unable to open PTY!"));

			ret = 0;
			break;
		}

		ptsname_r(ctx->ptyMasterFd, ctx->ptyPath, FILENAME_MAX);

		grantpt(ctx->ptyMasterFd);

		unlockpt(ctx->ptyMasterFd);

		DBG(("PTY path: %s", ctx->ptyPath));

		int childPid = fork();
		if (childPid < 0) {
			ERR(("Fork failed!"));

			ret = 0;
			break;

		} else if (childPid == 0) {
			int slaveFd;

			close(ctx->ptyMasterFd);

			slaveFd = open(ctx->ptyPath, O_RDWR | O_NOCTTY);

			char argLine[64];

			snprintf(argLine, sizeof(argLine), "-S%s/%d", ctx->ptyPath, slaveFd);

			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			dup2(slaveFd, STDIN_FILENO);
			dup2(slaveFd, STDOUT_FILENO);
			dup2(slaveFd, STDERR_FILENO);
#if 1
			{
				struct termios termOps;

				tcgetattr(slaveFd, &termOps);

				termOps.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
				termOps.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
				termOps.c_cflag &= ~(CSIZE|PARENB);
				termOps.c_cflag |= CS8;
				termOps.c_oflag &= ~(OPOST);

				termOps.c_cc[VMIN] = 1;
				termOps.c_cc[VTIME] = 0;

				tcsetattr(slaveFd, TCSANOW, &termOps);
			}
#endif
			execlp("xterm", "xterm", argLine, (char *) 0);
			exit(1);
		} else {
			char c;

			// One interesting thing is that xterm writes its window ID to
			// the pty master after successful initialization. This is something
			// you'll need to deal with. It appears as a line of input on
			// the tty before the actual user input begins.
			while (read(ctx->ptyMasterFd, &c, 1) == 1 && c != '\n');
		}
	} while (0);

	return ret;
}


static void _882SioStatusRegCallback(_U8 port, Altair882SioStatusRegister *reg, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	LOG(("SIO STATUS[%02x]", port));

	_U8 selectRet = _select(ctx->ports[port]->ptyMasterFd);

	reg->RDRF = (selectRet & SELECT_READ)  != 0;
	reg->TDRE = (selectRet & SELECT_WRITE) != 0;
}


static void _882SioControlRegCallback(_U8 port, Altair882SioControlRegister *reg, void *privateData) {
	LOG(("SIO CONTROL[%02x]", port));
}


static _U8  _882SioInputDataCallback (_U8 port, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	_U8 ret;

	if (read(ctx->ports[port]->ptyMasterFd, &ret, 1) <= 0) {
		ret = 0;
	}

	LOG(("SIO INPUT[%02x] <- %02x (%c)", port, ret, ret));

	return ret;
}


static void _882SioOutputDataCallback(_U8 port, _U8 data, void *privateData) {
	Sio882Data *ctx = (Sio882Data *) privateData;

	LOG(("SIO OUTPUT[%02x] -> %02x (%c)", port, data, data));

	write(ctx->ports[port]->ptyMasterFd, &data, 1);
}


AltairModule *arch_create_module_882Sio(_U8 port) {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair882SioParameters *parameters = malloc(sizeof(Altair882SioParameters));

		parameters->address            = port * 4;
		parameters->controlRegCallback = _882SioControlRegCallback;
		parameters->inputDataCallback  = _882SioInputDataCallback;
		parameters->outputDataCallback = _882SioOutputDataCallback;
		parameters->statusRegCallback  = _882SioStatusRegCallback;

		{
			Sio882Data *sioData = malloc(sizeof(Sio882Data));

			for (int i = 0; i < 2; i++) {
				sioData->ports[i] = malloc(sizeof(PtyContext));
				if (! _ptyOpen(sioData->ports[i])) {
					sioData->ports[i] = NULL;
				}
			}

			parameters->callbackData = sioData;
		}

		altair_module_882sio_init(ret, parameters);
	}

	return ret;
}

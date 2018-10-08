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
#include <time.h>

#include "common/utils/ihex.h"

#include "module/internal.h"
#include "arch/arch.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static struct timespec _start;


void _timespecDiff(struct timespec *start, struct timespec *stop, struct timespec *result) {
	if ((stop->tv_nsec - start->tv_nsec) < 0) {
		result->tv_sec  = stop->tv_sec - start->tv_sec - 1;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;

	} else {
		result->tv_sec  = stop->tv_sec - start->tv_sec;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec;
	}
}


static _U8 _loadHex(char *path, _U8 *memory, _U16 memorySize) {
	_U8 ret = 0;

	{
		FILE *fd = fopen(path, "rb");
		if (fd) {
			char *line = NULL;
			size_t n;

			while (getline(&line, &n, fd) > 0) {
				ihex_line_to_binary(line, memory, memorySize);

				free(line);
				line = NULL;
			}

			fclose(fd);
		}
	}

	return ret;
}


void arch_initialize(void) {
	_start.tv_sec  = 0;
	_start.tv_nsec = 0;
}


void arch_timer_start() {
	clock_gettime(CLOCK_MONOTONIC, &_start);
}


_U32 arch_timer_stop() {
	struct timespec stop;
	struct timespec diffTime;

	clock_gettime(CLOCK_MONOTONIC, &stop);

	_timespecDiff(&_start, &stop, &diffTime);

	return (diffTime.tv_sec * 1000000) + (diffTime.tv_nsec / 1000);
}


void arch_waitUs(_U32 us) {
	struct timespec spec;

	spec.tv_sec  = us / 1000000;
	spec.tv_nsec = (us % 1000000) * 1000;

	nanosleep(&spec, NULL);
}


void arch_loadHex(char *path) {
	_U8 *memory;
	_U32 memorySize;

	internal_sram_getMemory(&memory, &memorySize);

	_loadHex(path, memory, memorySize);
}


#if 0


static void _dumpMemory(_U8 *mem, _U32 memSize) {
	_U32 i = 0;
	_U8 j;

	const _U8 lineLength = 32;

	for (i = 0; i < memSize; i += lineLength) {
		printf("%04x:", i);

		for (j = 0; j < lineLength; j++) {
			printf(" %02x", mem[i + j]);
		}

		printf("   ");

		for (j = 0; j < lineLength; j++) {
			char c = mem[i + j];

			if (! isalnum(c)) {
				c = '.';
			}

			printf("%c", c);
		}

		printf("\n");
	}
}
#endif

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

#include "common/utils/ihex.h"

#include "module/internal.h"
#include "arch/arch.h"


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

}


void arch_loadHex(char *path) {
	_U8 *memory;
	_U32 memorySize;

	internal_sram_getMemory(&memory, &memorySize);

	_loadHex(path, memory, memorySize);
}

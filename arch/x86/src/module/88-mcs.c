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

#include <stdlib.h>

#include "arch/module.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


AltairModule *arch_create_module_88mcs() {
	AltairModule *ret = malloc(sizeof(AltairModule));

	{
		Altair88McsParameters *params = malloc(sizeof(Altair88McsParameters));

		altair_module_88mcs_init(ret, params);
	}

	return ret;
}

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

#ifndef EMULATOR_ALTAIR_MODULE_88_MCS_H_
#define EMULATOR_ALTAIR_MODULE_88_MCS_H_

#include "emulator/altair/module.h"


typedef struct _Altair88McsParameters {
	_U32 reserved;
} Altair88McsParameters;


void altair_module_88mcs_init(AltairModule *module, Altair88McsParameters *params);

#endif /* EMULATOR_ALTAIR_MODULE_88_MCS_H_ */

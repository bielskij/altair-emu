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

#ifndef ARCH_MODULE_H_
#define ARCH_MODULE_H_

#include "emulator/altair/module/88-2sio.h"
#include "emulator/altair/module/88-16mcs.h"
#include "emulator/altair/module/88-pmc.h"
#include "emulator/altair/module/88-mcs.h"


void arch_initialize(void);

AltairModule *arch_create_module_882Sio(_U8 port);

AltairModule *arch_create_module_88Sio(_U8 port);

AltairModule *arch_create_module_8816Mcs(_U8 bank);

AltairModule *arch_create_module_88Pmc(_U8 addressPattern);

AltairModule *arch_create_module_88mcs();

#endif /* ARCH_MODULE_H_ */

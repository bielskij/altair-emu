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

#ifndef ARCH_ARCH_H_
#define ARCH_ARCH_H_

#include "common/types.h"


void arch_initialize(void);

// Starts internal timer (us resolution)
void arch_timer_start();
// Return time in us since start
_U32 arch_timer_stop();

void arch_waitUs(_U32 us);

void arch_loadHex(char *path);

#endif /* ARCH_ARCH_H_ */

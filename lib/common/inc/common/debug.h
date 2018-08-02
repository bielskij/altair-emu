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

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#include <stdio.h>

#define DEBUG_LEVEL_ERR 0
#define DEBUG_LEVEL_LOG 1
#define DEBUG_LEVEL_DBG 2
#define DEBUG_LEVEL_TRC 3

#if !defined(DEBUG_LEVEL)
#define DEBUG_LEVEL DEBUG_LEVEL_ERR
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_TRC
#define TRC(x) { printf("[TRC %s:%d]: ", __FUNCTION__, __LINE__); printf x; printf("\n"); }
#else
#define TRC(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DBG
#define DBG(x) { printf("[DBG %s:%d]: ", __FUNCTION__, __LINE__); printf x; printf("\n"); }
#else
#define DBG(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_LOG
#define LOG(x) { printf("[LOG %s:%d]: ", __FUNCTION__, __LINE__); printf x; printf("\n"); }
#else
#define LOG(x) do { } while (0);
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERR
#define ERR(x) { printf("[ERR %s:%d]: ", __FUNCTION__, __LINE__); printf x; printf("\n"); }
#else
#define ERR(x) do { } while (0);
#endif

#endif /* COMMON_DEBUG_H_ */

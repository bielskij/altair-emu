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

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#if !defined(TRUE)
#define TRUE (1 == 1)
#endif

#if !defined(FALSE)
#define FALSE (!TRUE)
#endif

#if !defined(NULL)
#define NULL ((void *)0)
#endif

// TODO: arch
typedef unsigned char  _U8;
typedef unsigned short _U16;
typedef unsigned int   _U32;

#endif /* COMMON_TYPES_H_ */

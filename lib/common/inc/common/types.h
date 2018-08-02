/*
 * types.h
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

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

/*
 * debug.h
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

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

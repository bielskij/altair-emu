/*
 * MIT License
 *
 * Copyright (c) 2020 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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


namespace dbg {
	void dump(void *mem, size_t memSize);
}

#endif /* COMMON_DEBUG_H_ */

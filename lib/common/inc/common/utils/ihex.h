/*
 * ihex.h
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

#ifndef UTILS_IHEX_H_
#define UTILS_IHEX_H_

#include "common/types.h"

// Returns 0 on success
_U8 ihex_line_to_binary(char *ihex, void *buffer, _U32 bufferSize);

#endif /* UTILS_IHEX_H_ */

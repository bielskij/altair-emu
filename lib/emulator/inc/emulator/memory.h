/*
 * memory.h
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

#ifndef EMULATOR_INC_MEMORY_H_
#define EMULATOR_INC_MEMORY_H_

#include "common/types.h"

typedef struct _Memory *Memory;

extern _U8  memory_read_byte(Memory memory, _U16 address);
extern void memory_write_byte(Memory memory, _U16 address, _U8 data);

#endif /* EMULATOR_INC_MEMORY_H_ */

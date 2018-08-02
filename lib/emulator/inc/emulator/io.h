/*
 * io.h
 *
 *  Created on: 27.07.2018
 *      Author: jarko
 */

#ifndef EMULATOR_INC_IO_H_
#define EMULATOR_INC_IO_H_

#include "common/types.h"

typedef struct _IO *IO;

extern _U8  IO_read_byte(IO io, _U8 address);
extern void IO_write_byte(IO io, _U8 address, _U8 data);


#endif /* EMULATOR_INC_IO_H_ */

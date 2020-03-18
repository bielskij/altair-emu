#include <cstdint>
#include <string>

#include "common/debug.h"


void dbg::dump(void *mem, size_t memSize) {
	uint32_t i = 0;
	uint8_t j;

	const uint8_t lineLength = 32;

	for (i = 0; i < memSize; i += lineLength) {
		printf("%04x:", i);

		for (j = 0; j < lineLength; j++) {
			printf(" %02x", reinterpret_cast<uint8_t *>(mem)[i + j]);
		}

		printf("   ");

		for (j = 0; j < lineLength; j++) {
			char c = reinterpret_cast<uint8_t *>(mem)[i + j];

			if (! ::isalnum(c)) {
				c = '.';
			}

			printf("%c", c);
		}

		printf("\n");
	}
}

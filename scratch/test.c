#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// gcc -O0 -o test ./test.c
// objdump -h -S ./test > ./test.lss
// objdump -h -S ./test > ./test.lss

void nexti(void *data, uint8_t ticks) {

}


uint8_t readByte(void *data, uint16_t address) {
	return 123;
}

void runCode(uint16_t *regs, void (*tickCallback)(void *data), void *data) {
	uint16_t first  = regs[0];
	uint16_t second = regs[1];

	tickCallback(data);
}


int main(int argc, char *argv[]) {
//	int data;

//	uint16_t regs[3];

//	runCode(regs, callback, &data);

	nexti(NULL, 2);

	readByte(NULL, 0x88);
}

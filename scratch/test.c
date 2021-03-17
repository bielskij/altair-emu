#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

// gcc -O0 -o test ./test.c
// objdump -h -S ./test > ./test.lss
// objdump -h -S ./test > ./test.lss

typedef struct _T {
	uint32_t junk;
	uint16_t PC;
} T;

void nexti(void *data, uint8_t ticks) {
//	__asm (
//		"mov al, 32 \n\t"
//		"mov bh, 32 \n\t"
//		"mov bl, 32 \n\t"
//		"mov ch, 32 \n\t"
//		"mov cl, 32 \n\t"
//		"mov dh, 32 \n\t"
//		"mov dl, 32 \n\t"
//
//		"mov bh, [rbx] \n\t"
//	);

	// PC = PC + 2
	{
		T t;

		t.PC = 2;

		__asm (
			"push rbp                  \n\t"
			"mov rbp, rax              \n\t"
			"push rax                  \n\t"
			"mov ax, [rbp + %[off_pc]] \n\t"
			"add ax, 2                 \n\t"
			"mov [rbp + %[off_pc]], ax \n\t"
			"pop rax                   \n\t"
			"pop rbp                   \n\t"
			:
			:
				"a" (&t),
				[off_pc] "i" (offsetof (struct _T, PC))
			:
		);

		printf("PC: %u\n", t.PC);
	}
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

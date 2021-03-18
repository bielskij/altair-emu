#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

// gcc -O0 -o test ./test.c
// objdump -h -S ./test > ./test.lss
// objdump -h -S ./test > ./test.lss

typedef struct _Ctx {
	uint8_t flag;
} Ctx;


void _onInt(void *ctx) {

}


typedef struct _T {
	uint32_t junk;

	uint8_t A, B, C, D, E, H, L;
	uint16_t PC, SP;

	void *codeSegment;

	uint8_t   intMask;
	uint16_t  intAddress;
	uint8_t   intValue;
	void    (*intHandler)(void *);
} T;

void nexti(void *data, uint8_t ticks) {
	// execute int callback
	{
		__asm(
			// set address from M reg
			"mov [rbp + %[off_address]], bx \n\t"
			"mov [rbp + %[off_address]], cx \n\t"
			"mov [rbp + %[off_address]], dx \n\t"
			"mov [rbp + %[off_address]], si \n\t"
			:
			:
				[off_address]  "i" (offsetof (struct _T, intAddress))
			:
		);

//		[off_address]  "i" (offsetof (struct _T, intAddress)),
//		[off_value]    "i" (offsetof (struct _T, intValue)),

		__asm(
			"push rax                       \n\t"

			"mov al, 2                      \n\t"
			"mov [rbp + %[off_flags]], al   \n\t"

			"mov  rax, [rbp + %[off_hndlr]] \n\t"

			"push rcx                       \n\t"
			"push rdx                       \n\t"
			"push rsi                       \n\t"
			"push rdi                       \n\t"
			"push r8                        \n\t"
			"push r9                        \n\t"
			"push r10                       \n\t"
			"push r11                       \n\t"

			"mov  rdi, rbp                  \n\t"
			"call rax                       \n\t"

			"pop  r11                       \n\t"
			"pop  r10                       \n\t"
			"pop  r9                        \n\t"
			"pop  r8                        \n\t"
			"pop  rdi                       \n\t"
			"pop  rsi                       \n\t"
			"pop  rdx                       \n\t"
			"pop  rcx                       \n\t"

			"pop  rax                       \n\t"
			:
			:
				[off_flags]    "i" (offsetof (struct _T, intMask)),
				[off_hndlr]    "i" (offsetof (struct _T, intHandler))
			:
		);
	}

	__asm(
		"push rdi      \n\t"
		"mov  rdi, rbp \n\t"
		"pop rdi       \n\t"
	);

	{
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
	}

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

	runCode(NULL, _onInt, NULL);

	nexti(NULL, 2);

	readByte(NULL, 0x88);
}

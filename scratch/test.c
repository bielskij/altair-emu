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


void _io() {
	__asm(
		"mov WORD PTR [rbp + %[off_address]], 0x1234 \n\t"
		:
		:
			[off_address]  "i" (offsetof (struct _T, intAddress))
		:
	);
}

void _movrr() {
	__asm(
		"mov bh,bh  \t\n"
		"mov bh,bl  \t\n"
		"mov bh,ch  \t\n"
		"mov bh,cl  \t\n"
		"mov bh,dh  \t\n"
		"mov bh,dl  \t\n"
		"mov bh,al  \t\n"

		"mov bl,bh  \t\n"
		"mov bl,bl  \t\n"
		"mov bl,ch  \t\n"
		"mov bl,cl  \t\n"
		"mov bl,dh  \t\n"
		"mov bl,dl  \t\n"
		"mov bl,al  \t\n"

		"mov ch,bh  \t\n"
		"mov ch,bl  \t\n"
		"mov ch,ch  \t\n"
		"mov ch,cl  \t\n"
		"mov ch,dh  \t\n"
		"mov ch,dl  \t\n"
		"mov ch,al  \t\n"

		"mov cl,bh  \t\n"
		"mov cl,bl  \t\n"
		"mov cl,ch  \t\n"
		"mov cl,cl  \t\n"
		"mov cl,dh  \t\n"
		"mov cl,dl  \t\n"
		"mov cl,al  \t\n"

		"mov dh,bh  \t\n"
		"mov dh,bl  \t\n"
		"mov dh,ch  \t\n"
		"mov dh,cl  \t\n"
		"mov dh,dh  \t\n"
		"mov dh,dl  \t\n"
		"mov dh,al  \t\n"

		"mov dl,bh  \t\n"
		"mov dl,bl  \t\n"
		"mov dl,ch  \t\n"
		"mov dl,cl  \t\n"
		"mov dl,dh  \t\n"
		"mov dl,dl  \t\n"
		"mov dl,al  \t\n"

		"mov al,bh  \t\n"
		"mov al,bl  \t\n"
		"mov al,ch  \t\n"
		"mov al,cl  \t\n"
		"mov al,dh  \t\n"
		"mov al,dl  \t\n"
		"mov al,al  \t\n"
	);
}


void movRM() {
	__asm (
		// set address from M reg
		"mov bh, [rbp + %[off_value]] \n\t"
		"mov bl, [rbp + %[off_value]] \n\t"
		"mov ch, [rbp + %[off_value]] \n\t"
		"mov cl, [rbp + %[off_value]] \n\t"
		"mov dh, [rbp + %[off_value]] \n\t"
		"mov dl, [rbp + %[off_value]] \n\t"
		"mov al, [rbp + %[off_value]] \n\t"
		:
		:
			[off_value]  "i" (offsetof (struct _T, intValue))
		:
	);
}

void movMR() {
	__asm (
		// set address from M reg
		"mov [rbp + %[off_value]], bh \n\t"
		"mov [rbp + %[off_value]], bl \n\t"
		"mov [rbp + %[off_value]], ch \n\t"
		"mov [rbp + %[off_value]], cl \n\t"
		"mov [rbp + %[off_value]], dh \n\t"
		"mov [rbp + %[off_value]], dl \n\t"
		"mov [rbp + %[off_value]], al \n\t"
		:
		:
			[off_value]  "i" (offsetof (struct _T, intValue))
		:
	);
}


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

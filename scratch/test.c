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

	uint8_t F, A, B, C, D, E, H, L;
	uint16_t PC, SP;

	uint8_t intFF;

	void *codeSegment;

	uint8_t   intMask;
	uint16_t  intAddress;
	uint8_t   intValue;
	void    (*intHandler)(void *);
} T;


void _anaR() {
	__asm(
		"and al, bh \t\n"
		"and al, bl \t\n"
		"and al, ch \t\n"
		"and al, cl \t\n"
		"and al, dh \t\n"
		"and al, dl \t\n"
		"and al, al \t\n"
	);
}


void _anaM() {
	__asm(
		"and al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _ani() {
	__asm(
		"and al, 0x13 \t\n"
	);
}


void _xraR() {
	__asm(
		"xor al, bh \t\n"
		"xor al, bl \t\n"
		"xor al, ch \t\n"
		"xor al, cl \t\n"
		"xor al, dh \t\n"
		"xor al, dl \t\n"
		"xor al, al \t\n"
	);
}


void _xraM() {
	__asm(
		"xor al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _xri() {
	__asm(
		"xor al, 0x13 \t\n"
	);
}

void _oraR() {
	__asm(
		"or al, bh \t\n"
		"or al, bl \t\n"
		"or al, ch \t\n"
		"or al, cl \t\n"
		"or al, dh \t\n"
		"or al, dl \t\n"
		"or al, al \t\n"
	);
}


void _oraM() {
	__asm(
		"or al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _ori() {
	__asm(
		"or al, 0x13 \t\n"
	);
}


void _subR() {
	__asm(
		"sub al, bh \t\n"
		"sub al, bl \t\n"
		"sub al, ch \t\n"
		"sub al, cl \t\n"
		"sub al, dh \t\n"
		"sub al, dl \t\n"
		"sub al, al \t\n"
	);
}

void _subM() {
	__asm(
		"sub al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _sui() {
	__asm(
		"sub al, 0x13 \t\n"
	);
}


void _sbbR() {
	__asm(
		"sbb al, bh \t\n"
		"sbb al, bl \t\n"
		"sbb al, ch \t\n"
		"sbb al, cl \t\n"
		"sbb al, dh \t\n"
		"sbb al, dl \t\n"
		"sbb al, al \t\n"
	);
}

void _sbbM() {
	__asm(
		"sbb al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _sbi() {
	__asm(
		"sbb al, 0x13 \t\n"
	);
}


void _adcM() {
	__asm(
		"adc al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _addM() {
	__asm(
		"add al, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}

void _addR() {
	__asm(
		"add al, bh \t\n"
		"add al, bl \t\n"
		"add al, ch \t\n"
		"add al, cl \t\n"
		"add al, dh \t\n"
		"add al, dl \t\n"
		"add al, al \t\n"
	);
}


void _adcR() {
	__asm(
		"adc al, bh \t\n"
		"adc al, bl \t\n"
		"adc al, ch \t\n"
		"adc al, cl \t\n"
		"adc al, dh \t\n"
		"adc al, dl \t\n"
		"adc al, al \t\n"
	);
}


void _xchg() {
	__asm(
		"pushf      \t\n"
		"xor dh, ch \t\n"
		"xor ch, dh \t\n"
		"xor dh, ch \t\n"
		"xor dl, cl \t\n"
		"xor cl, dl \t\n"
		"xor dl, cl \t\n"
		"popf       \t\n"
	);
}

void _inrM() {
	__asm(
		"inc BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}

void _dcrM() {
	__asm(
		"dec BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}


void _lhld() {
	__asm(
		"mov dh, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov dl, BYTE PTR [rbp + %[off_value]] \t\n"
		:
		:
			[off_value] "i" (offsetof (struct _T, intValue))
		:
	);
}

void _cpi() {
	__asm(
		"push rax \t\n"
		"sub al, 12 \t\n"
		"pop rax \t\n"
	);
}

void _cmp() {
	__asm(
		"push rax \t\n"
		"sub al, bh \t\n"
		"sub al, bl \t\n"
		"sub al, ch \t\n"
		"sub al, cl \t\n"
		"sub al, dh \t\n"
		"sub al, dl \t\n"
		"sub al, al \t\n"
		"pop rax \t\n"
	);
}


void _sbb() {
	__asm(
		"sbb al, bh \t\n"
		"sbb al, bl \t\n"
		"sbb al, ch \t\n"
		"sbb al, cl \t\n"
		"sbb al, dh \t\n"
		"sbb al, dl \t\n"
		"sbb al, al \t\n"
	);
}

void _incR() {
	__asm(
		"inc bh \t\n"
		"inc bl \t\n"
		"inc ch \t\n"
		"inc cl \t\n"
		"inc dh \t\n"
		"inc dl \t\n"
		"inc al \t\n"
		:
		:
		:
	);
}

void _dcr() {
	__asm(
		"dec bh \t\n"
		"dec bl \t\n"
		"dec ch \t\n"
		"dec cl \t\n"
		"dec dh \t\n"
		"dec dl \t\n"
		"dec al \t\n"
		:
		:
		:
	);
}


void _stc() {
	__asm("stc");
}

void _inx() {
	__asm(
		"inc bx \t\n"
		"inc cx \t\n"
		"inc dx \t\n"
		"inc si \t\n"
	);
}


void _dcx() {
	__asm(
		"dec bx \t\n"
		"dec cx \t\n"
		"dec dx \t\n"
		"dec si \t\n"
	);
}


void _dad() {
	__asm(
		"pushf \t\n"

		"add dx, bx \t\n"
		"add dx, cx \t\n"
		"add dx, dx \t\n"
		"add dx, si \t\n"

		"pushf         \t\n"
		"mov dil, 0x01 \t\n"
		"and dil, byte ptr [rsp]  \t\n"
		"popf   \t\n"
		"and byte ptr [rsp], 0xfe \t\n"
		"or  byte ptr [rsp], dil   \t\n"

		"popf  \t\n"
	);
}


void _ret() {
	__asm(
			"pushfq         \t\n"
			"push rax \t\n"

			"mov al, BYTE PTR [rbp + %[off_value]] \t\n"
			"inc si  \t\n"

			"mov ah, BYTE PTR [rbp + %[off_value]] \t\n"
			"inc si  \t\n"

			"mov WORD PTR [rbp + %[off_pc]] , ax \t\n"

			"pop  rax \t\n"
			"popfq                               \t\n"

			"mov dil, 10 \t\n"

			:
			:
				[off_value]  "i" (offsetof (struct _T, intValue)),
				[off_pc]  "i" (offsetof (struct _T, PC))
			:
		);
}


void _pushpop() {
	__asm(
		"mov bh, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov bl, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov ch, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov cl, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov dh, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov dl, BYTE PTR [rbp + %[off_value]] \t\n"
		"mov al, BYTE PTR [rbp + %[off_value]] \t\n"

		"dec si \t\n"
		"inc si \t\n"

		"pushf                         \n\t"
		"mov dil, [rbp + %[off_value]] \n\t"
		"and byte ptr [rsp], 0xd5      \n\t"
		"or  [rsp], dil                \n\t"
		"popf                          \n\t"

		"mov [rbp + %[off_f]], dil \n\t"

		"mov dil, [rbp + %[off_f]] \n\t"
		"mov [rbp + %[off_value]], dil \n\t"
		:
		:
			[off_value]  "i" (offsetof (struct _T, intValue)),
			[off_f]      "i" (offsetof (struct _T, F))
		:
	);
}


void _lxi() {
	__asm(
		"mov bx, 0x1234 \t\n"
		"mov cx, 0x1234 \t\n"
		"mov dx, 0x1234 \t\n"
		"mov si, 0x1234 \t\n"
	);
}

void _checkint() {
	__asm(
		"test BYTE PTR [rbp + %[off_int]], 1 \n\t"
		"jz noint \n\t"
		"retq     \n\t"
		"noint:   \n\t"
		"nop      \n\t"
		:
		:
			[off_int]  "i" (offsetof (struct _T, intFF))
		:
	);
}


void _io() {
	__asm(
		"mov WORD PTR [rbp + %[off_address]], 0x1234 \n\t"
		"mov al, BYTE PTR [rbp + %[off_address]] \n\t"
		:
		:
			[off_address]  "i" (offsetof (struct _T, intAddress))
		:
	);
}


void _saveFlags() {
	__asm(
		// Save flags
		"pushfq                    \n\t"
		"pop ax                    \n\t"
		"and al, 0xd5              \n\t"
		"mov [rbp + %[off_f]], al  \n\t"
		"add rsp, 6                \n\t"
		:
		:
			[off_f]  "i" (offsetof (struct _T, F))
		:
	);
}


void _loadFlags() {
	__asm(
		"pushfq                    \n\t"
		"pop ax                    \n\t"
		"and ax, 0xff2a            \n\t"
		"or  al, [rbp + %[off_f]]  \n\t"
		"push ax                   \n\t"
		"popfq                     \n\t"
		:
		:
			[off_f]  "i" (offsetof (struct _T, F))
		:
	);
}


void _rrc() {
	__asm(
		"pushfq                     \t\n"
		"ror  al, 1                 \t\n"
		"test al, 0x80              \t\n"
		"jnz isset                  \t\n"
		"  and byte ptr [rsp], 0xFE \t\n"
		"  jmp done                 \t\n"
		"isset:                     \t\n"
		"  or byte ptr [rsp], 0x01  \t\n"
		"done:                      \t\n"
		"popfq                      \t\n"
	);
}

void _rrl() {
	__asm(
		"pushfq                     \t\n"
		"rol  al, 1                 \t\n"
		"test al, 0x80              \t\n"
		"jnz isset_rrl              \t\n"
		"  and byte ptr [rsp], 0xFE \t\n"
		"  jmp done_rrl             \t\n"
		"isset_rrl:                 \t\n"
		"  or byte ptr [rsp], 0x01  \t\n"
		"done_rrl:                  \t\n"
		"popfq                      \t\n"
	);
}

void ralr() {
	__asm(
		"rcr al, 1 \t\n"
		"rcl al, 1 \t\n"
	);
}

void cma() {
	__asm(
		"pushf \t\n"
		"xor al, 0xff \t\n"
		"popf \t\n"
	);
}


void _xthl() {
	__asm(
		"mov di, dx \t\n"
		"mov BYTE PTR [rbp + %[off_intValue]], dil \t\n"
		"inc WORD PTR [rbp + %[off_intAddress]]    \t\n"
		"mov dh, BYTE PTR [rbp + %[off_intValue]] \t\n"
		"mov dl, BYTE PTR [rbp + %[off_intValue]] \t\n"
		"ror di, 8 \t\n"
		:
		:
			[off_intValue]    "i" (offsetof (struct _T, intValue)),
			[off_intAddress]  "i" (offsetof (struct _T, intAddress))
		:
	);
}


void _sphl() {
	__asm(
		"mov si, dx\t\n"
	);
}

void _pchl() {
	__asm(
		"mov WORD PTR [rbp + %[off_pc]], dx \t\n"
		:
		:
			[off_pc]  "i" (offsetof (struct _T, PC))
		:
	);
}


void _adi() {
	__asm(
		"add al, 0x13 \t\n"
	);
}

void _aci() {
	__asm(
		"adc al, 0x13 \t\n"
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


void _jmp() {
	__asm (
		// jnc
		"jc z_false                             \n\t"
		"mov word ptr [rbp + %[off_pc]], 0x1234 \n\t"
		"jmp done_jmp                           \n\t"
	"z_false:                               \n\t"
		"pushf                                  \n\t"
		"add word ptr [rbp + %[off_pc]], 3      \n\t"
		"popf                                   \n\t"
	"done_jmp:                                  \n\t"
		"nop                                    \n\t"
		:
		:
			[off_pc] "i" (offsetof (struct _T, PC))
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

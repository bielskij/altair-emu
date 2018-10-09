; SIO (not 2SIO) echo test with receive interrupts

	org	0
	lxi	sp,0100h	;init stack pointer
	mvi	a,01h		;receive ints on
	out	00h
	ei			;enable 8080 interrutps

; The loop below represents "normal" processing a program may
;    be doing. We also verify that the accumulator does not
;    get changed by the interrupt routine.

loop	nop
	nop
	nop
	jmp	loop

; Interrupt service routine for RST7 is at 038h. This routine
;    saves the accumulator and PSW, then echoes the character.
;    If this interrupt is shared, then the commented statements 
;    to check for a character would be included and used
;    to branch to a a 2nd device to check. 

	org	038h		;RST7 entry address
	push	a		;save A and status flags
;	in	00h		;verify a new character present
;	rrc			;lsb has new data flag
;	jc	nextDev		no character, try next device
	in	01h		;read the character
	out	01h		;echo it
	pop	a		;restore A and status flags
	ei			;re-enable 8080 interrupts
	ret


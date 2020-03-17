; SIO (not 2SIO) echo test

	org	0
loop	in	00h		;wait for character
	rrc
	jc	loop		;nothing yet (negative logic)
	in	01h		;read the character
	out	01h		;echo it
	jmp	loop


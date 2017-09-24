.global start
start:
	mov sp, #0x8000
	
	bl ves_main
	
.global put32
put32:
	str r1, [r0]
	bx lr

.global get32
get32:
	ldr r0, [r0]
	bx lr

#include "../asm.h"

	.align 3
	.globl bullet_create
bullet_create:
	syscall0 NR_fork
	bnez $a0, 1f

	b bullet	# proc-bullet

1:
	syscall1_li NR_timer,BULLET_CREARTE_SPEED

	b bullet_create


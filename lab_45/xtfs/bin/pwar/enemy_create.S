#include "../asm.h"

	.align 3
	.globl enemy_create
enemy_create:
	syscall0 NR_fork
	bnez $a0, 1f

	b enemy		# proc-enemy

1:
	syscall1_li NR_timer,ENEMY_CREARTE_SPEED

	b enemy_create


#include "asm.h"

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, father

child:
	syscall1_a NR_output, str1
	syscall1_a NR_output, str
	syscall1_i NR_timer,1
	b child

father:
	syscall1_r NR_input, $t0
	syscall0 NR_kill

exit:
	syscall1_a NR_output, str2
	syscall1_a NR_output, str
	syscall0 NR_exit
	
str1:
	.string "this is child."
str2:
	.string "child is killed."
str:
	.string "\n"

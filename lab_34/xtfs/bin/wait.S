#include "asm.h"

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, f1

c1:
	syscall1_a NR_output, str1
	syscall1_a NR_output, str
	syscall0 NR_exit

f1:
	syscall0 NR_fork
	bnez $a0, f2
	
c2:
	syscall1_a NR_output, str2
	syscall1_a NR_output, str
	syscall0 NR_exit

f2:	
	syscall0 NR_wait
	bnez $a0, f2

exit:
	syscall1_a NR_output, str3
	syscall1_a NR_output, str
	syscall0 NR_exit
	
str1:
	.string "this is child_1."
str2:
	.string "this is child_2."
str3:
	.string "all child are over."
str:
	.string "\n"



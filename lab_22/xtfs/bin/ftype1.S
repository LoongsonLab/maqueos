#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_filetype, file
	bne $t0, $a0, exit
	syscall1_a NR_output, str
	b exit
close:
	syscall1_r NR_close,$t1
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
file:
	.string "xtsh"

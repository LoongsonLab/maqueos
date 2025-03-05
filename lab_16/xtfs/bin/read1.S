#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_open, file
	bne $t0, $a0, read
	syscall1_a NR_output, str
	b exit
read:
	or $t1, $a0, $r0
	syscall3_rai NR_read,$t1,buf_read,12
close:
	syscall1_r NR_close,$t1
print:
	syscall1_a NR_output, buf_read
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
file:
	.string "a1.txt"
read_count:
	.word 7
buf_read:
	.fill 512,1,0

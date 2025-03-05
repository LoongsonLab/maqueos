#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_open, file
	bne $t0, $a0, write
	syscall1_a NR_output, str
	b exit
write:
	or $t1, $a0, $r0
	syscall3_rai NR_write,$t1,buf_write,5
close:
	syscall1_r NR_close,$t1
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
file:
	.string "hello_xt"
buf_write:
	.ascii "123456789abcdefghijklmnopqrstuvwxyz!\n\0"
	.fill 500,1,0

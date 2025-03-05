#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_open, file1
    syscall1_a NR_open, file2
    syscall1_a NR_open, file3
	syscall0 NR_cur_files
	syscall1_r NR_close,$t1
exit:
	syscall0 NR_exit
file1:
	.string "t1.txt\0"
file2:
	.string "t2.txt\0"
file3:
	.string "t3.txt\0"
buf_read:
	.fill 512,1,0

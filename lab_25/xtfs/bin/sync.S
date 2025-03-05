#include "asm.h"

	.globl start
start:
	syscall0 NR_cur_files
exit:
	syscall0 NR_exit

str:
	.string "\n"

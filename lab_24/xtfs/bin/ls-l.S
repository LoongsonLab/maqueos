#include "asm.h"
	.globl start
start:
	syscall0 NR_ls_l
exit:
	syscall0 NR_exit

str:
	.string "\n"

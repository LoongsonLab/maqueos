#include "../asm.h"

	.align 3
	.globl refresh
refresh:
	syscall3_aai NR_shmem,share_name,share_vaddr_c_refresh,2

output:
	syscall1_p NR_refresh, share_vaddr_c_refresh
	syscall1_li NR_timer,REFRESH_SPEED
	b output

share_name:
	.string "plane"
share_vaddr_c_refresh:
	.long 0
#include "../asm.h"

	.globl start
start:
	syscall3_aai NR_shmem,share_name,share_vaddr_pwar,2

	li.d $t0, 8000
	li.d $t1, 32
	la $t2, share_vaddr_pwar
	ldptr.d $t2, $t2, 0  
clear:
	st.d $t1,$t2,0
	addi.d  $t2,$t2,1
	addi.d  $t0,$t0,-1
	bnez $t0, clear

fork_refresh:
	syscall0 NR_fork
	bnez $a0, fork_plane

	b refresh	# proc-refresh

fork_plane:
	syscall0 NR_fork
	bnez $a0, pause

	b plane		# proc-plane

pause:
	syscall0 NR_pause
	b pause

share_name:
	.string "plane"
share_vaddr_pwar:
	.long 0
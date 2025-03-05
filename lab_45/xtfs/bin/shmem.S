#include "asm.h"

#define OFFSET 4096

	.globl start
start:
	syscall3_aai NR_shmem,share_name,share_vaddr_f,2
	la $t0, share_vaddr_f
	ld.d $t0, $t0, 0
	li.d $t1, OFFSET
	add.d $t0, $t1, $t0
	ori $t1,$r0,51
	st.d $t1, $t0, 0
	syscall0 NR_fork
	bnez $a0, father
child:
	syscall3_aai NR_shmem,share_name,share_vaddr_c,2
	la $t0, share_vaddr_c
	ld.d $t0, $t0, 0
	li.d $t1, OFFSET
	add.d $t0, $t1, $t0
	ori $t1,$r0,49
	st.d $t1, $t0, 0 
	syscall0 NR_exit
father:
	la $t0, share_vaddr_f
	ld.d $t0, $t0, 0
	li.d $t1, OFFSET
	add.d $t0, $t1, $t0
	syscall1_r NR_output, $t0
	
	syscall0 NR_pause

	la $t0, share_vaddr_f
	ld.d $t0, $t0, 0
	li.d $t1, OFFSET
	add.d $t0, $t1, $t0
	syscall1_r NR_output, $t0

	syscall1_a NR_output, str
	syscall0 NR_exit

share_name:
	.string "plane"
share_vaddr_f:
	.quad 0
share_vaddr_c:
	.quad 0
str:
	.string "\n"

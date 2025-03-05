#include "../asm.h"

	.align 3
	.globl plane
plane:
	syscall3_aai NR_shmem,share_name,share_vaddr_c_plane,2

	la $t0, plane_index
	li.d $t1, 8000-160*4+80
	st.d $t1, $t0, 0

	bl print_plane

loop:
	la $t0, cmd
	syscall1_r NR_input, $t0
	ld.b $t0, $t0, 0

	li.d $t1, 97	     # a
	beq $t1, $t0, left
	li.d $t1, 115	     # s
	beq $t1, $t0, write

	b loop
left:
	bl clear_plane

	la $t0, plane_index
	ld.d $t1, $t0, 0
	addi.d $t1, $t1, -1
	st.d $t1, $t0, 0

	bl print_plane
	b loop

write:
	bl clear_plane

	la $t0, plane_index
	ld.d $t1, $t0, 0
	addi.d $t1, $t1, 1
	st.d $t1, $t0, 0

	bl print_plane
	b loop

print_plane:
	addi.d $sp, $sp, -0x28
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8
	st.d $t2, $sp, 0x10
	st.d $t3, $sp, 0x18
	st.d $t4, $sp, 0x20

	la $t4, share_vaddr_c_plane
	ld.d $t4, $t4, 0
	la $t0, plane_index
	ld.d $t0, $t0, 0	# offset of plane
	add.d $t4, $t4, $t0	# bit of plane

	la $t2, plane_str
	li.d $t0, 4
row:
	li.d $t1,5
col:
	ld.b $t3, $t2, 0
	st.b $t3,$t4,0

	addi.d  $t2,$t2,1
	addi.d  $t4,$t4,1
	addi.d  $t1,$t1,-1
	bnez $t1, col

	addi.d  $t4,$t4,155
	addi.d  $t0,$t0,-1
	bnez $t0, row

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	ld.d $t2, $sp, 0x10
	ld.d $t3, $sp, 0x18
	ld.d $t4, $sp, 0x20

	addi.d $sp, $sp, 0x28

	jirl $r0,$ra,0

clear_plane:
	addi.d $sp, $sp, -0x28
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8
	st.d $t2, $sp, 0x10
	st.d $t3, $sp, 0x18
	st.d $t4, $sp, 0x20

	la $t4, share_vaddr_c_plane
	ld.d $t4, $t4, 0  
	la $t0, plane_index
	ld.d $t0, $t0, 0
	add.d $t4, $t4, $t0

	li.d $t0, 4
row_clear:
	li.d $t1,5
col_clear:
	li.d $t3, 32
	st.b $t3,$t4,0

	addi.d  $t4,$t4,1
	addi.d  $t1,$t1,-1
	bnez $t1, col_clear

	addi.d  $t4,$t4,155
	addi.d  $t0,$t0,-1
	bnez $t0, row_clear

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	ld.d $t2, $sp, 0x10
	ld.d $t3, $sp, 0x18
	ld.d $t4, $sp, 0x20

	addi.d $sp, $sp, 0x28

	jirl $r0,$ra,0

share_vaddr_c_plane:
	.quad 0
plane_index:
	.quad 0
cmd:
	.quad 0
plane_str:
    .ascii "  *  "
    .ascii "*****"
    .ascii "  *  "
    .ascii " * * "
share_name:
	.string "plane"


#include "../asm.h"

	.align 3
	.globl bullet
bullet:
	syscall3_aai NR_shmem,share_name,share_vaddr_c_bullet,2
	
	la $t4, share_vaddr_c_bullet
	ld.d $t4, $t4, 0  	# base of bullet
	li.d $t3, BULLET_BIT
	add.d $t4,$t4,$t3
	ld.d $t4, $t4, 0  

	la $t0, bullet_index
	st.d $t4, $t0, 0

	b print

check_bullet:
	la $t1, share_vaddr_c_bullet
	ld.d $t1, $t1, 0  
	la $t0, bullet_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0
	ld.b $t1, $t1, 0
	ori $t2, $r0,0x20
	beq $t1,$t2,bullet_exit

check_enemy:
	la $t1, share_vaddr_c_bullet
	ld.d $t1, $t1, 0  
	la $t0, bullet_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0
	addi.d $t1, $t1, -160	# t1: pos of enemy
	ld.b $t0, $t1, 0	
	ori $t2, $r0,0x2a
	beq $t0,$t2,bullet_meet	# t0:enemy,t2:*

clear:
	bl clear_bullet

	la $t0, bullet_index
	ld.d $t1, $t0, 0  
	addi.d $t1, $t1, -160
	ori $t2, $r0, 0
	blt $t1,$t2,bullet_exit

	st.d $t1, $t0, 0

print:
	bl print_bullet
	syscall1_li NR_timer,BULLET_SPEED
	b check_bullet

bullet_meet:
	ori $t0, $r0,0x20
	st.b $t0, $t1, 0	# t1: pos of enemy

bullet_exit:
	bl clear_bullet		# clear bullet befor exit
	syscall0 NR_exit

clear_bullet:
	addi.d $sp, $sp, -0x10
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8

	la $t1, share_vaddr_c_bullet
	ld.d $t1, $t1, 0  
	la $t0, bullet_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0

	li.d $t0, 0x20
	st.b $t0,$t1,0

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	addi.d $sp, $sp, 0x10

	jirl $r0,$ra,0

print_bullet:
	addi.d $sp, $sp, -0x10
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8

	la $t1, share_vaddr_c_bullet
	ld.d $t1, $t1, 0  
	la $t0, bullet_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0

	li.d $t0, 0x41
	st.b $t0,$t1,0

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	addi.d $sp, $sp, 0x10

	jirl $r0,$ra,0

bullet_index:
	.quad 0
share_name:
	.ascii "plane"
share_vaddr_c_bullet:
	.quad 0
init:
	.quad 0
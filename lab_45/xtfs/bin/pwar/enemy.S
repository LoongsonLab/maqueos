
#include "../asm.h"

	.align 3
	.globl enemy
enemy:
	syscall3_aai NR_shmem,share_name,share_vaddr_c_enemy,2
	
	syscall0 NR_time
	andi $a0,$a0,0x7f

	la $t0, enemy_index
	st.d $a0, $t0, 0

	b print

check_enemy:
	la $t1, share_vaddr_c_enemy
	ld.d $t1, $t1, 0  
	la $t0, enemy_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0
	ori $t3, $r0,5
1:
	ld.b $t0, $t1, 0
	ori $t2, $r0,0x20
	beq $t0,$t2,enemy_exit

	addi.d  $t1,$t1,1	
	addi.d  $t3,$t3,-1
	bnez $t3, 1b

check_bullet:
	la $t1, share_vaddr_c_enemy
	ld.d $t1, $t1, 0  
	la $t0, enemy_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0
	addi.d $t1, $t1, 160
	ori $t3, $r0,5
1:
	ld.b $t0, $t1, 0
	ori $t2, $r0,0x41
	beq $t0,$t2,enemy_meet

	addi.d  $t1,$t1,1	
	addi.d  $t3,$t3,-1
	bnez $t3, 1b

clear:
	bl clear_enemy

	la $t0, enemy_index
	ld.d $t1, $t0, 0  
	addi.d $t1, $t1, 160

	ori $t2, $r0, 160
	ori $t3, $r0, 49
	mul.d $t2,$t2,$t3

	blt $t2,$t1,enemy_exit

	st.d $t1, $t0, 0

print:
	bl print_enemy
	syscall1_li NR_timer,ENEMY_SPEED
	b check_enemy

enemy_meet:
	ori $t0, $r0,0x20
	st.b $t0, $t1, 0

enemy_exit:
	bl clear_enemy
	syscall0 NR_exit

clear_enemy:
	addi.d $sp, $sp, -0x18
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8
	st.d $t2, $sp, 0x10

	la $t1, share_vaddr_c_enemy
	ld.d $t1, $t1, 0  
	la $t0, enemy_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0

	li.d $t2,5
1:
	li.d $t0, 0x20
	st.b $t0,$t1,0

	addi.d  $t1,$t1,1
	addi.d  $t2,$t2,-1
	bnez $t2, 1b

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	ld.d $t2, $sp, 0x10
	addi.d $sp, $sp, 0x18

	jirl $r0,$ra,0

print_enemy:
	addi.d $sp, $sp, -0x18
	st.d $t0, $sp, 0x0
	st.d $t1, $sp, 0x8
	st.d $t2, $sp, 0x10

	la $t1, share_vaddr_c_enemy
	ld.d $t1, $t1, 0  
	la $t0, enemy_index
	ld.d $t0, $t0, 0
	add.d $t1, $t1, $t0

	li.d $t2,5
1:
	li.d $t0, 0x2a
	st.b $t0,$t1,0

	addi.d  $t1,$t1,1
	addi.d  $t2,$t2,-1
	bnez $t2, 1b

	ld.d $t0, $sp, 0x0
	ld.d $t1, $sp, 0x8
	ld.d $t2, $sp, 0x10
	addi.d $sp, $sp, 0x18

	jirl $r0,$ra,0

enemy_index:
	.quad 0
share_name:
	.ascii "plane"
share_vaddr_c_enemy:
	.quad 0
init:
	.quad 0
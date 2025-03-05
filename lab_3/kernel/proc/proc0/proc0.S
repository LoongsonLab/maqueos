#define NR_fork 0
#define NR_input 1
#define NR_output 2
#define NR_exit 3
#define NR_pause 4
#define NR_drewPlane 5
.macro syscall0 A7
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_a A7, A0
	la $a0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, proc0_pause
proc1:
	syscall0 NR_fork
	bnez $a0, proc1_pause
proc2:
	syscall1_a NR_output, str
read:
	syscall1_a NR_input, char
	la $t0, char
	ld.b $t0, $t0, 0
	ori $t1, $r0, 112              
    beq $t0, $t1, call_drew_plane  
	syscall1_a NR_output, char
	ori $t1, $r0, 13
	bne $t0, $t1, read
	syscall0 NR_exit
proc1_pause:
	syscall0 NR_pause
	b proc1
proc0_pause:
	syscall0 NR_pause
	b proc0_pause
call_drew_plane :
    syscall0 NR_drewPlane
	b read


str:
	.string "xtsh#"
char:
	.byte 0
#define NR_fork 0
#define NR_input 1
#define NR_output 2
#define NR_exit 3
#define NR_pause 4
#define NR_drewPlane 5
#define NR_drewGplane 6
#define NR_drewBplane 7
#define NR_drewYplane 8
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
	#syscall1_a NR_output, char
	ori $t1, $r0, 114            # 设置 $t1 为 'r' 的 ASCII 码 103
    beq $t0, $t1, call_drew_plane # 如果字符是 ''，调用 drew_Gplane
	ori $t1, $r0, 103              # 设置 $t1 为 'g' 的 ASCII 码 103
    beq $t0, $t1, call_drew_Gplane # 如果字符是 'g'，调用 drew_Gplane

    ori $t1, $r0, 98               # 设置 $t1 为 'b' 的 ASCII 码 98
    beq $t0, $t1, call_drew_Bplane # 如果字符是 'b'，调用 drew_Bplane

    ori $t1, $r0, 121              # 设置 $t1 为 'y' 的 ASCII 码 121
    beq $t0, $t1, call_drew_Yplane # 如果字符是 'y'，调用 drew_Yplane
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
call_drew_Gplane :
    syscall0 NR_drewGplane
	b read
call_drew_Bplane :
    syscall0 NR_drewBplane
	b read
call_drew_Yplane :
    syscall0 NR_drewYplane
	b read	


str:
	.string "xtsh# "
char:
	.byte 0
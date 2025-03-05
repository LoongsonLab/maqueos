#define NR_fork 0
#define NR_input 1
#define NR_output 2
#define NR_exit 3
#define NR_pause 4
#define NR_mount 5
#define NR_exe 6
#define NR_shmem 7
#define NR_timer 8
#define NR_open 9
#define NR_close 10
#define NR_read 11
#define NR_create 12
#define NR_destroy 13
#define NR_write 14
#define NR_sync 15
#define NR_refresh 16

#define REFRESH_SPEED 1
#define BULLET_CREARTE_SPEED 10
#define BULLET_SPEED 1
#define ENEMY_CREARTE_SPEED 30
#define ENEMY_SPEED 3

#define BULLET_BIT 8000

#define ENEMY_BIT 4001
#define BULLET_HIT_MAP 2000

.macro syscall0 A7
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_i A7,A0
	ori $a0, $r0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_a A7, A0
	la $a0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_r A7, A0
	or $a0, $r0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_p A7,A0
	la $a0, \A0
	ld.d $a0, $a0, 0	 
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall2_ra A7, A0, A1
	or $a0, $r0, \A0
	la $a1, \A1
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall2_ar A7, A0, A1
	la $a0, \A0
	or $a1, $r0, \A1
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall2_aa A7,A0,A1
	la $a0, \A0
	la $a1, \A1
	ori $a7, $r0, \A7
	syscall 0
.endm

.macro syscall3_aai A7,A0,A1,A2
        la   $a0, \A0
        la   $a1, \A1
        ori $a2, $r0, \A2
        li.d $a7, \A7
        syscall 0
.endm
.macro syscall1_li A7,A0
        li.d $a0, \A0
        li.d $a7, \A7
        syscall 0
.endm
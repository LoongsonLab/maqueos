#define CSR_PRMD 0x1
#define CSR_ESTAT 0x5
#define CSR_ERA 0x6
#define CSR_PGD 0x1b
#define CSR_SAVE0 0x30
#define CSR_SAVE1 0x31
#define CSR_TLBRSAVE 0x8b
#define STACK_SIZE 0xf8
#define A0_OFFSET 0x10
#define ERA_OFFSET 0xf0

.macro store_load_regs cmd
	\cmd $ra, $sp, 0x0
	\cmd $tp, $sp, 0x8
	\cmd $a0, $sp, 0x10
	\cmd $a1, $sp, 0x18
	\cmd $a2, $sp, 0x20
	\cmd $a3, $sp, 0x28
	\cmd $a4, $sp, 0x30
	\cmd $a5, $sp, 0x38
	\cmd $a6, $sp, 0x40
	\cmd $a7, $sp, 0x48
	\cmd $t0, $sp, 0x50
	\cmd $t1, $sp, 0x58
	\cmd $t2, $sp, 0x60
	\cmd $t3, $sp, 0x68
	\cmd $t4, $sp, 0x70
	\cmd $t5, $sp, 0x78
	\cmd $t6, $sp, 0x80
	\cmd $t7, $sp, 0x88
	\cmd $t8, $sp, 0x90
	\cmd $r21, $sp,0x98
	\cmd $fp, $sp, 0xa0
	\cmd $s0, $sp, 0xa8
	\cmd $s1, $sp, 0xb0
	\cmd $s2, $sp, 0xb8
	\cmd $s3, $sp, 0xc0
	\cmd $s4, $sp, 0xc8
	\cmd $s5, $sp, 0xd0
	\cmd $s6, $sp, 0xd8
	\cmd $s7, $sp, 0xe0
	\cmd $s8, $sp, 0xe8
.endm

	.globl exception_handler
	.globl tlb_handler
	.globl fork_ret
fork_ret:
	addi.d $sp, $sp, -STACK_SIZE
	st.d $r0, $sp, A0_OFFSET
	b user_exception_ret

syscall:
	ld.d $t0, $sp, ERA_OFFSET
	addi.d $t0, $t0, 4
	st.d $t0, $sp, ERA_OFFSET
	la $t0, syscalls
	alsl.d $a7, $a7, $t0, 3
	ld.d $t0, $a7, 0 
	jirl $ra, $t0, 0
	st.d $a0, $sp, A0_OFFSET
	b user_exception_ret

exception_handler:
	csrwr $t0, CSR_SAVE1
	csrrd $t0, CSR_PRMD
	andi $t0, $t0, 0x3
	beqz $t0, kernel_exception
	b user_exception

tlb_handler:
	csrwr $t0, CSR_TLBRSAVE
	csrrd $t0, CSR_PGD
	lddir $t0, $t0, 1
	ldpte $t0, 0
	ldpte $t0, 1
	tlbfill
	csrrd $t0, CSR_TLBRSAVE
	ertn

user_exception:
	csrrd $t0, CSR_SAVE1
	csrwr $sp, CSR_SAVE0
	addi.d $sp, $sp, -STACK_SIZE
	store_load_regs st.d
	csrrd $t0, CSR_ERA
	st.d $t0, $sp, 0xf0
	csrrd $t0, CSR_ESTAT
	srli.d $t0, $t0, 16
	andi $t0, $t0, 0x3f
	ori $t1, $r0, 0xb
	beq $t0, $t1, syscall
	bl do_exception

user_exception_ret:
	bl do_signal
	ori $t0, $r0, 0x7
	csrwr $t0, CSR_PRMD
	ld.d $t0, $sp, 0xf0
	csrwr $t0, CSR_ERA
	store_load_regs ld.d
	addi.d $sp, $sp, STACK_SIZE
	csrwr $sp, CSR_SAVE0
	ertn

kernel_exception:
	csrrd $t0, CSR_SAVE1
	addi.d $sp, $sp, -0xf0
	store_load_regs st.d
	bl do_exception
	store_load_regs ld.d
	addi.d $sp, $sp, 0xf0
	ertn

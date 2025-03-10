
#include <xtos.h>

#define CSR_CRMD 0x0
#define CSR_PRMD 0x1
#define CSR_ECFG 0x4
#define CSR_ESTAT 0x5
#define CSR_EENTRY 0xc
#define CSR_TCFG 0x41
#define CSR_TICLR 0x44
#define CSR_TLBRENTRY 0x88
#define CSR_CRMD_IE (1UL << 2)
#define CSR_PRMD_PPLV (3UL << 0)
#define CSR_TCFG_EN (1UL << 0)
#define CSR_TCFG_PER (1UL << 1)
#define CSR_TICLR_CLR (1UL << 0)
#define CSR_ECFG_LIE_TI (1UL << 11)
#define CSR_ECFG_LIE_HWI0 (1UL << 2)
#define CSR_ESTAT_IS_TI (1UL << 11)
#define CSR_ESTAT_IS_HWI0 (1UL << 2)
#define CSR_ESTAT_ECODE (0x3fUL << 16)
#define CC_FREQ 4
#define L7A_SPACE_BASE (0x10000000UL | DMW_MASK)
#define L7A_INT_MASK (L7A_SPACE_BASE + 0x020)
#define L7A_HTMSI_VEC (L7A_SPACE_BASE + 0x200)
#define IOCSR_EXT_IOI_EN 0x1600
#define IOCSR_EXT_IOI_SR 0x1800
#define KEYBOARD_IRQ 3
#define SATA_IRQ 19
#define KEYBOARD_IRQ_HT 0
#define SATA_IRQ_HT 1
#define NR_TIMER 10

struct timer
{
	unsigned int alarm;
	struct process *wait;
};

unsigned long jiffies = 0;
struct timer timer_table[NR_TIMER];
extern struct process *current;
int (*syscalls[])() = {
	sys_fork, sys_input, sys_output, sys_exit, sys_pause,
	sys_mount, sys_exe, sys_shmem, sys_timer, sys_open,
	sys_close, sys_read, sys_create, sys_destroy, sys_write,
	sys_sync, sys_refresh};

int sys_timer(int seconds)
{
	int i;

	for (i = 0; i < NR_TIMER; i++)
		if (timer_table[i].alarm == 0)
			break;
	if (i == NR_TIMER)
		panic("panic: timer_table[] is empty!\n");
	timer_table[i].alarm = jiffies + seconds;
	timer_table[i].wait = current;
	current->state = TASK_UNINTERRUPTIBLE;
	schedule();
	return 0;
}
void timer_interrupt()
{
	int i;

	jiffies++;
	for (i = 0; i < NR_TIMER; i++)
	{
		if (timer_table[i].alarm == jiffies)
		{
			timer_table[i].alarm = 0;
			timer_table[i].wait->state = TASK_RUNNING;
		}
	}
	if ((--current->counter) > 0)
		return;
	current->counter = 0;
	if ((read_csr_32(CSR_PRMD) & CSR_PRMD_PPLV) == 0)
		return;
	schedule();
}
void do_exception()
{
	unsigned int estat;
	unsigned long irq;
	unsigned int ecode;

	estat = read_csr_32(CSR_ESTAT);
	ecode = (estat >> 16) & 0x3f;
	if (estat & CSR_ESTAT_ECODE)
	{
		if (ecode == 1 || ecode == 2 || ecode == 3)
			do_no_page();
		else if (ecode == 4)
			do_wp_page();
	}
	if (estat & CSR_ESTAT_IS_TI)
	{
		write_csr_32(CSR_TICLR_CLR, CSR_TICLR);
		timer_interrupt();
	}	
	if (estat & CSR_ESTAT_IS_HWI0)
	{
		irq = read_iocsr(IOCSR_EXT_IOI_SR);
		if (irq & (1UL << KEYBOARD_IRQ_HT))
		{
			keyboard_interrupt();
			write_iocsr(1UL << KEYBOARD_IRQ_HT, IOCSR_EXT_IOI_SR);
		}
		if (irq & (1UL << SATA_IRQ_HT))
		{
			disk_interrupt();
			write_iocsr(1UL << SATA_IRQ_HT, IOCSR_EXT_IOI_SR);
		}
	}
}
void int_on()
{
	unsigned int crmd;

	crmd = read_csr_32(CSR_CRMD);
	write_csr_32(crmd | CSR_CRMD_IE, CSR_CRMD);
}
void excp_init()
{
	unsigned int val;

	val = read_cpucfg(CC_FREQ);
	write_csr_32(CSR_ECFG_LIE_TI | CSR_ECFG_LIE_HWI0, CSR_ECFG);
	write_csr_64((unsigned long)val | CSR_TCFG_EN | CSR_TCFG_PER, CSR_TCFG);
	write_csr_64((unsigned long)exception_handler, CSR_EENTRY);
	write_csr_64((unsigned long)tlb_handler, CSR_TLBRENTRY);
	*(volatile unsigned long *)(L7A_INT_MASK) = ~(0x1UL << KEYBOARD_IRQ | 0x1UL << SATA_IRQ);
	*(volatile unsigned char *)(L7A_HTMSI_VEC + KEYBOARD_IRQ) = KEYBOARD_IRQ_HT;
	*(volatile unsigned char *)(L7A_HTMSI_VEC + SATA_IRQ) = SATA_IRQ_HT;
	write_iocsr((0x1UL << KEYBOARD_IRQ_HT | 0x1UL << SATA_IRQ_HT), IOCSR_EXT_IOI_EN);
}
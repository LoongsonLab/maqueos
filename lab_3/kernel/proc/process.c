#include <xtos.h>

#define CSR_PGDL 0x19
#define CSR_SAVE0 0x30
#define PROC_COUNTER 5
#define PTE_V (1UL << 0)
#define PTE_D (1UL << 1)
#define PTE_PLV (3UL << 2)

struct process *process[NR_PROCESS];//64
struct process *current;
char proc0_code[] = {
0x0b, 0x00, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x80, 0x70, 0x00, 0x44, 0x0b, 0x00, 0x80, 0x03,
0x00, 0x00, 0x2b, 0x00, 0x80, 0x58, 0x00, 0x44, 0x04, 0x00, 0x00, 0x1c, 0x84, 0x20, 0xc2, 0x28,
0x0b, 0x08, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x00, 0x1c, 0x84, 0x00, 0xc2, 0x28,
0x0b, 0x04, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x0c, 0x00, 0x00, 0x1c, 0x8c, 0xc1, 0xc1, 0x28,
0x8c, 0x01, 0x00, 0x28, 0x0d, 0xc0, 0x81, 0x03, 0x8d, 0x3d, 0x00, 0x58, 0x04, 0x00, 0x00, 0x1c,
0x84, 0x70, 0xc1, 0x28, 0x0b, 0x08, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x0d, 0x34, 0x80, 0x03,
0x8d, 0xc9, 0xff, 0x5f, 0x0b, 0x0c, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x0b, 0x10, 0x80, 0x03,
0x00, 0x00, 0x2b, 0x00, 0xff, 0x9b, 0xff, 0x53, 0x0b, 0x10, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00,
0xff, 0xfb, 0xff, 0x53, 0x0b, 0x14, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0xff, 0x9f, 0xff, 0x53,
0x78, 0x74, 0x73, 0x68, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    
	};

int sys_fork()
{
	int i;

	for (i = 1; i < NR_PROCESS; i++)
		if (!process[i])
			break;
	if (i == NR_PROCESS)
		panic("panic: process[] is empty!\n");
	process[i] = (struct process *)get_page();
	copy_mem((char *)process[i], (char *)current, PAGE_SIZE);
	process[i]->page_directory = get_page();
	copy_page_table(current, process[i]);
	process[i]->context.ra = (unsigned long)fork_ret;
	process[i]->context.sp = (unsigned long)process[i] + PAGE_SIZE;
	process[i]->context.csr_save0 = read_csr_64(CSR_SAVE0);
	process[i]->pid = i;
	process[i]->counter = PROC_COUNTER;
	process[i]->wait_next = 0;
	process[i]->signal_exit = 0;
	process[i]->father = current;
	process[i]->state = TASK_RUNNING;
	return i;
}
int sys_exit()
{
	current->state = TASK_EXIT;
	tell_father();
	schedule();
	return 0;
}
int sys_pause()
{
	current->state = TASK_INTERRUPTIBLE;
	schedule();
	return 0;
}
void free_process(struct process *p)
{
	int pid;
	
	pid = p->pid;
	free_page_table(p);
	free_page(p->page_directory);
	free_page((unsigned long)p);
	process[pid] = 0;
}
void sleep_on(struct process **p)
{
	current->state = TASK_UNINTERRUPTIBLE;
	current->wait_next = *p;
	*p = current;
	schedule();
}
void wake_up(struct process **p)
{
	struct process *first;
	
	first = *p;
	if (!first)
		return;
	*p = first->wait_next;
	first->wait_next = 0;
	first->state = TASK_RUNNING;
}
void schedule()
{
	int pid = 0;
	int i;
	struct process *old;

	for (i = 1; i < NR_PROCESS; i++)
	{
		if (!process[i] || process[i]->state != TASK_RUNNING || process[i]->counter == 0)
			continue;
		pid = process[i]->pid;
		break;
	}
	if (i == NR_PROCESS)
	{
		for (i = 0; i < NR_PROCESS; i++)
		{
			if (!process[i] || process[i]->state != TASK_RUNNING)
				continue;
			process[i]->counter = PROC_COUNTER;
			pid = process[i]->pid;
		}
	}
	if (current->pid == pid)
		return;
	old = current;
	current = process[pid];
	write_csr_64(current->page_directory & ~DMW_MASK, CSR_PGDL);
	invalidate();
	swtch(&old->context, &current->context);
}
void process_init()
{
	unsigned long page;
	int i;

	for (i = 0; i < NR_PROCESS; i++)
		process[i] = 0;
	process[0] = (struct process *)get_page();
	write_csr_64((unsigned long)process[0] + PAGE_SIZE, CSR_SAVE0);
	process[0]->page_directory = get_page();
	write_csr_64(process[0]->page_directory & ~DMW_MASK, CSR_PGDL);
	page = get_page();
	copy_mem((void *)page, proc0_code, sizeof(proc0_code));
	put_page(process[0], 0, page, PTE_PLV | PTE_D | PTE_V);
	process[0]->pid = 0;
	process[0]->exe_end = PAGE_SIZE;
	process[0]->counter = PROC_COUNTER;
	process[0]->wait_next = 0;
	process[0]->signal_exit = 0;
	process[0]->father = 0;
	process[0]->state = TASK_RUNNING;
	current = process[0];
}
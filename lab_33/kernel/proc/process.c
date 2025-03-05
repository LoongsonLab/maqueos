#include <xtos.h>

#define CSR_PGDL 0x19
#define CSR_SAVE0 0x30
#define NR_PROCESS 64
#define PTE_V (1UL << 0)
#define PTE_D (1UL << 1)
#define PTE_PLV (3UL << 2)

struct process *process[NR_PROCESS];
struct process *current;
char proc0_code[] = {0x00, 0x00, 0x00, 0x50};

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
	current = process[0];
}

void memcpy(char *to, char *from, int nr)
{
	for (int i = 0; i < nr; i++)
		to[i] = from[i];
}

void walk()
{
	unsigned long v_addr, p_addr;
	for (v_addr = 0x800; v_addr <=0x1200; v_addr += 0x100)
	{
		unsigned long  p1_table, *p1_d, p2_table,*p2_d;
		unsigned long offset;
		offset = v_addr & 0xfff;
		unsigned long base;
		base = (unsigned long)read_csr_32(CSR_PGDL) | DMW_MASK;

		p1_table = (v_addr & 0x3fe00000u) >> 21;
		p1_d = (unsigned long *)base + p1_table * 8;
		//print_debug("p1_d:", *p1_d);

		p2_table = (v_addr & 0x1ff000u) >> 12;
		p2_d = (unsigned long *)((*p1_d + p2_table * 8)| DMW_MASK);
		//print_debug("p2_d:", *p2_d);

		p_addr = (*p2_d + offset) & ~DMW_MASK;

		char str1[] = "\nv_addr: ";
		char str2[] = "\np_addr: ";
		print_debug(str1, v_addr);
		print_debug(str2, p_addr);
	}
}
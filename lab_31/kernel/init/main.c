#include <xtos.h>

void main()
{
	mem_init();
	con_init();
	excp_init();
	int_on();
	printk("hello world");
	while (1)
		;
}

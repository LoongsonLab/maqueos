#include <xtos.h>
extern unsigned long timer;
void main()
{
	mem_init();
	con_init();
	excp_init();
	int_on();
	print_debug("begin:",timer);
	for(int index=0;index<10000;index++)
	{
		get_page();
	}
	print_debug("  end:",timer);
	while (1)
		;
}
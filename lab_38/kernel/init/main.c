#include <xtos.h>

void main()
{
	con_init();
	con2_init();
	printk("hello, world.");
	while (1)
		;
}

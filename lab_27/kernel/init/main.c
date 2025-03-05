#include <xtos.h>

void main()
{
	con_init();
	//printk("hello, world.\n");
	write_char(127,0,0);
	while (1)
		;
}

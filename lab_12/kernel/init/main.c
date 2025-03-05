#include <xtos.h>

void main()
{
	con_init();
	con2_init();
	excp_init();
	int_on();
	while (1)
		;
}
#include <xtos.h>

#define NR_PIX_X 1280
#define NR_PIX_Y 800
#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8
#define NR_CHAR_X (NR_PIX_X / CHAR_WIDTH)
#define NR_CHAR_Y (NR_PIX_Y / CHAR_HEIGHT)
#define NR_BYTE_PIX 4
#define VRAM_BASE (0x40000000UL | DMW_MASK)
#define L7A_I8042_DATA (0x1fe00060UL | DMW_MASK)

int isFirst=1;//记录是否是第一次移动，清除数据污染
char* save=(char *)0x90000000403e8001;//显示器最大输出范围为403e8000，该空间不被使用
char* temp;//移动赋值的指针
char *pos;
extern char fonts[];
int x, y;
int sum_char_x[NR_CHAR_Y];
char digits_map[] = "0123456789abcdef";
char keys_map[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '`', 0,
	0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0, 0, 32, 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0, 0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,
	0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, 13, ']', 0, '\\', 0, 0,
	0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, '`', 0};

void write_char(char ascii, int xx, int yy)
{
	char *font_byte;
	int row, col;

	font_byte = &fonts[(ascii - 32) * CHAR_HEIGHT];
	pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
	for (row = 0; row < CHAR_HEIGHT; row++, font_byte++)
	{
		for (col = 0; col < CHAR_WIDTH; col++)
		{
			if (*font_byte & (1 << (7 - col)))
			{
				*pos++ = 0;
				*pos++ = 0;
				*pos++ = 255;
				*pos++ = 0;
			}
			else
				pos += NR_BYTE_PIX;
		}
		pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
	}
}
void erase_char(int xx, int yy)
{
	int row, col;

	pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
	for (row = 0; row < CHAR_HEIGHT; row++)
	{
		for (col = 0; col < CHAR_WIDTH; col++)
		{
			*pos++ = 0;
			*pos++ = 0;
			*pos++ = 0;
			*pos++ = 0;
		}
		pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
	}
}
void scrup()
{
	int i;
    char *to,*from;

	to = (char *)VRAM_BASE;
	from = (char *)(VRAM_BASE + (CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
	for (i = 0; i < (NR_PIX_Y - CHAR_HEIGHT) * NR_PIX_X * NR_BYTE_PIX; i++, to++, from++)
		*to = *from;
	for (i = 0; i < NR_CHAR_X; i++)
		erase_char(i, NR_CHAR_Y - 1);
	for (i = 0; i < NR_CHAR_Y - 1; i++)
		sum_char_x[i] = sum_char_x[i + 1];
	sum_char_x[i] = 0;
}
void cr_lf()
{
	x = 0;
	if (y < NR_CHAR_Y - 1)
		y++;
	else
		scrup();
}
void del()
{
	if (x)
	{
		x--;
		sum_char_x[y] = x;
	}
	else if (y)
	{
		sum_char_x[y] = 0;
		y--;
		x = sum_char_x[y];
	}
	erase_char(x, y);
}
void printk(char *buf)
{
	char c;
	int nr = 0;

	while (buf[nr] != '\0')
		nr++;
	erase_char(x, y);
	while (nr--)
	{
		c = *buf++;
		if (c > 31 && c < 127)
		{
			write_char(c, x, y);
			sum_char_x[y] = x;
			x++;
			if (x >= NR_CHAR_X)
				cr_lf();
		}
		else if (c == 10 || c == 13)
			cr_lf();
		else if (c == 127)
			del();
		else
			panic("panic: unsurpported char!\n");
	}
	write_char('_', x, y);
}
void panic(char *s)
{
	printk(s);
	while (1)
		;
}
void con_init()
{
	x = 0;
	y = 0;
}
void print_debug(char *str, unsigned long val)
{
	int i, j;
	char buffer[20];

	printk(str);
	buffer[0] = '0';
	buffer[1] = 'x';
	for (j = 0, i = 17; j < 16; j++, i--)
	{
		buffer[i] = (digits_map[val & 0xfUL]);
		val >>= 4;
	}
	buffer[18] = '\n';
	buffer[19] = '\0';
	printk(buffer);
}
void do_keyboard(unsigned char c)
{
	static unsigned long stack[10];
	static int index = 0;
	int row, col;
    
	if(c == 0x75 || c == 0x72 || c == 0x6b || c == 0x74)
	{
		if(isFirst!=1)
		{
        //写入原始数据save
        pos = (char *)(VRAM_BASE + (y * CHAR_HEIGHT * NR_PIX_X + x * CHAR_WIDTH) * NR_BYTE_PIX);
		for (row = 0; row < CHAR_HEIGHT; row++)
		{
			for (col = 0; col < CHAR_WIDTH; col++)
			{
				*pos=*temp;pos++;temp++;
				*pos=*temp;pos++;temp++;
				*pos=*temp;pos++;temp++;
				*pos=*temp;pos++;temp++;
			}
			pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
			temp++;
		}	
		temp=save;	//temp归位
		}
		else
		{
        //是第一次则写入空，直接用temp可能会有上次残留的数据污染
		erase_char(x,y);
		isFirst=0;
		}
	}
	if(c == 0x75) // up arrow
	{
		if(y > 0)//只有y>0时才能往上移动光标，防止越界
			y--;
	}
	else if(c == 0x72) // down arrow
	{
		if(y < NR_CHAR_Y - 1)//只有y<NR_CHAR_Y-1时才能往下移动光标，防止越界
			y++;
	}
	else if(c == 0x6b) // left arrow
	{
		if(x > 0)//只有x>0时才能往左移动光标，防止越界
		{
			x--;//光标向左移动一格
		}else if(x == 0 && y>0)//换上一行
		{
            y--;
			x=NR_CHAR_X-1;
		}
	}	
	else if(c == 0x74) // right arrow
	{
		if(x < NR_CHAR_X - 1)//只有x<NR_CHAR_X-1时才能往右移动光标，防止越界
		{
			x++;//光标向右移动一格
		}else if(x == NR_CHAR_X -1 && y <NR_CHAR_Y -1)//换下一行
		{
			y++;
			x=0;
		}
	}
	else
	{
		c = keys_map[c];//将键盘扫描码转化为ASCII字符
		if (c == 'a' && index < 10)
		{
			stack[index] = get_page();
			print_debug(" get a page: ", stack[index]);
			index++;
		}
		else if (c == 's' && index > 0)
		{
			index--;
			free_page(stack[index]);
			print_debug("free a page: ", stack[index]);
		}
	}
	//保存数据
	temp=save;
	pos = (char *)(VRAM_BASE + (y * CHAR_HEIGHT * NR_PIX_X + x * CHAR_WIDTH) * NR_BYTE_PIX);
	for (row = 0; row < CHAR_HEIGHT; row++)
	{
		for (col = 0; col < CHAR_WIDTH; col++)
		{
			*temp=*pos;temp++;pos++;
			*temp=*pos;temp++;pos++;
			*temp=*pos;temp++;pos++;
			*temp=*pos;temp++;pos++;
		}
		pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
		temp++;
	} 
	temp=save;
	erase_char(x,y);
	write_char('_', x, y);//在光标处写入下划线
}
void keyboard_interrupt()
{
	unsigned char c;
	unsigned char b;
	unsigned char a;

	c = *(volatile unsigned char *)L7A_I8042_DATA;
	b = *(volatile unsigned char *)L7A_I8042_DATA;
	a = *(volatile unsigned char *)L7A_I8042_DATA;
	// print_debug("c:",c);
	// print_debug("b:",b);
	// print_debug("a:",a);
	if (c == 0xf0)
	{
		return;
	}
    if(c == 0xe0)
	{
		if(b == 0xf0)
		{
			return;
		}
        do_keyboard(a);//up:0x75 down:0x72 left:0x6b right:0x74
		return;
	}
	do_keyboard(c);
}

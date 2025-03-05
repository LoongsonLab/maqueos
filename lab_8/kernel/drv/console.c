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
#define BUFFER_SIZE PAGE_SIZE

extern struct buffer buffer_table[16];
struct queue
{
	int count, head, tail;
	struct process *wait;
	char *buffer;
};

extern char fonts[];
int x, y;
struct queue read_queue;
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
	char *pos;

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
	char *pos;

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
	char *from, *to;

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
void clearTophalaf(){
	int yy=0;
	int xx=0;
	for(;yy<=NR_CHAR_Y/2- 2;yy++){
		for(xx=0;xx<=NR_CHAR_X;xx++)
		erase_char(xx,yy);
	}
	x=0;
	y=0;
	
	return;
}
// void cr_lf()
// {
// 	x = 0;
// 	if (y < NR_CHAR_Y - 1)
// 		y++;
// 	else
// 		scrup();
// }
void cr_lf()
{
	x = 0;
	if (y < NR_CHAR_Y/2- 2)
		y++;
	else
	   clearTophalaf();
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
void put_queue(char c)
{
	if (!c)
		return;
	if (read_queue.count == BUFFER_SIZE)
		return;
	read_queue.buffer[read_queue.head] = c;
	read_queue.head = (read_queue.head + 1) & (BUFFER_SIZE - 1);
	read_queue.count++;
	wake_up(&read_queue.wait);
}
int sys_output(char *buf)
{
	printk(buf);
	return 0;
}
int sys_input(char *buf)
{
	if (read_queue.count == 0)
		sleep_on(&read_queue.wait);
	*buf = read_queue.buffer[read_queue.tail];
	read_queue.tail = ((read_queue.tail) + 1) & (BUFFER_SIZE - 1);
	read_queue.count--;
	return 0;
}
void keyboard_interrupt()
{
	unsigned char c;

	c = *(volatile unsigned char *)L7A_I8042_DATA;
	if (c == 0xf0)
	{
		c = *(volatile unsigned char *)L7A_I8042_DATA;
		return;
	}
	put_queue(keys_map[c]);
}
void con_init()
{
	read_queue.count = 0;
	read_queue.head = 0;
	read_queue.tail = 0;
	read_queue.wait = 0;
	read_queue.buffer = (char *)get_page();

	x = 0;
	y = 0;
}
int dy = NR_CHAR_Y / 2; // 垂直居中
int dydetail=NR_CHAR_Y / 2+2;
int dxdetail=0;
int dx=0;
int step=0;
void printk_block_title(){

		y=dy-1;
		x=dx;
		printk("|**    triggered by read operation    **|");
		y=dy;
		x=dx;
		printk("bufferseq     type     blocknum     used\n");
		x=dx+60;
		y=dy-1;
	    printk("|**    triggered by write operation    **|");
		y=dy;
		x=dx+60;
		printk("bufferseq     type     blocknum     used\n");
		x=0;
		y=0;
		return ;
	
}
void write_int_to_screen(int num, int xx, int yy) {
    char buffer[12]; // 用于存储整数转化为字符串，长度足够大可以存储一个 32 位整数
    int i = 0;
    
    // 处理负数
    if (num < 0) {
        write_char('-', xx++, yy); // 负号显示并更新 xx
        num = -num;
    }
    
    // 将数字转换为字符串
    do {
        buffer[i++] = (num % 10) + '0'; // 将最后一位数字转成字符
        num /= 10;
    } while (num > 0);

    // 逆序输出
    while (i > 0) {
        write_char(buffer[--i], xx++, yy);
    }
}
void write_detail(){
	if(step==0){
       printk_block_title();
	   step++;
	}
	int seq=0;int type=0;short blocknum=0;int used;

	for(int i=0;i<16;i++,dydetail++){
		seq=buffer_table[i].seq;
		type=buffer_table[i].type;
		blocknum=buffer_table[i].blocknr;
		used=buffer_table[i].used;
		erase_char(dx,dydetail);
	    erase_char(dx+14,dydetail);
		erase_char(dx+15,dydetail);
	    erase_char(dx+23,dydetail);
		erase_char(dx+24,dydetail);
	    erase_char(dx+36,dydetail);
		write_int_to_screen(seq,dx,dydetail);
        write_int_to_screen(type,dx+14,dydetail);
		write_int_to_screen(blocknum,dx+23,dydetail);
        write_int_to_screen(used,dx+36,dydetail);
		
	}
	dx=dx+60;
	dydetail=NR_CHAR_Y/2+2;
	if(dx>=100)
	   dx=0;
    return;
}
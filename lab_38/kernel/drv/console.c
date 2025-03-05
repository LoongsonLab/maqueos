#include <xtos.h>

#define NR_PIX_X 1280
#define NR_PIX_Y_1 400
#define NR_PIX_Y_2 800
#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8
#define NR_CHAR_X (NR_PIX_X / CHAR_WIDTH)
#define NR_CHAR_Y_1 (NR_PIX_Y_1 / CHAR_HEIGHT)
#define NR_CHAR_Y_2 (NR_PIX_Y_2 / CHAR_HEIGHT)
#define NR_BYTE_PIX 4
#define VRAM_BASE 0x40000000UL

extern char fonts[];
int x, y;
int X,Y;
int sum_char_x[NR_CHAR_Y_2];

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
				*pos++ = 255;
				*pos++ = 0;
				*pos++ = 0;
				*pos++ = 0;
			}
			else
			{
				*pos++ = 255;
				*pos++ = 255;
				*pos++ = 255;
				*pos++ = 0;
			}
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
void scrup_1()
{
	int i;
	char *from, *to;

	to = (char *)VRAM_BASE;
	from = (char *)(VRAM_BASE + (CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
	for (i = 0; i < (NR_PIX_Y_1 - CHAR_HEIGHT) * NR_PIX_X * NR_BYTE_PIX; i++, to++, from++)
		*to = *from;
	for (i = 0; i < NR_CHAR_X; i++)
		erase_char(i, NR_CHAR_Y_1 - 1);
	for (i = 0; i < NR_CHAR_Y_1 - 1; i++)
		sum_char_x[i] = sum_char_x[i + 1];
	sum_char_x[i] = 0;
}
void scrup_2()
{
	int i;
	char *from, *to;

	to = (char *)(VRAM_BASE + 25*(CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
	from = (char *)(VRAM_BASE + 26*(CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
	for (i = NR_PIX_Y_1 + 1; i < (NR_PIX_Y_2 - CHAR_HEIGHT) * NR_PIX_X * NR_BYTE_PIX; i++, to++, from++)
		*to = *from;
	for (i = 0; i < NR_CHAR_X; i++)
		erase_char(i, NR_CHAR_Y_2 - 1);
	for (i = NR_PIX_Y_1 + 1; i < NR_CHAR_Y_2 - 1; i++)
		sum_char_x[i] = sum_char_x[i + 1];
	sum_char_x[i] = 0;
}
void cr_lf_1()
{
	x = 0;
	if (y < NR_CHAR_Y_1 - 1)
		y++;
	else
		scrup_1();
}
void cr_lf_2()
{
	X = 0;
	if (Y < NR_CHAR_Y_2 - 1)
		Y++;
	else
		scrup_2();
}
void del_1()
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
void del_2()
{
	if (X)
	{
		Y--;
		sum_char_x[Y] = X;
	}
	else if (Y)
	{
		sum_char_x[Y] = 0;
		Y--;
		X = sum_char_x[Y];
	}
	erase_char(X, Y);
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
				cr_lf_1();
		}
		else if (c == 10 || c == 13)
			cr_lf_1();
		else if (c == 127)
			del_1();
		else
			panic("panic: unsurpported char!\n");
	}
	write_char('_', x, y);
}
void print_kernel(char *buf)
{
	char c;
	int nr = 0;

	while (buf[nr] != '\0')
		nr++;
	erase_char(X, Y);
	while (nr--)
	{
		c = *buf++;
		if (c > 31 && c < 127)
		{
			write_char(c, X, Y);
			sum_char_x[Y] = X;
			X++;
			if (X >= NR_CHAR_X)
				cr_lf_2();
		}
		else if (c == 10 || c == 13)
			cr_lf_2();
		else if (c == 127)
			del_2();
		else
			panic("panic: unsurpported char!\n");
	}
	write_char('_', X, Y);
}
void print_plane(int x_plane,int y_plane)
{
	write_char('*',x_plane,y_plane);
	for (int i = x_plane - 2; i <= x_plane + 2 ; i++)
	{
		write_char('*',i,y_plane+1);
	}
	write_char('*',x_plane,y_plane+2);
	write_char('*',x_plane-1,y_plane+3);
	write_char('*',x_plane+1,y_plane+3);
	
}
void panic(char *s)
{
	printk(s);
	while (1)
		;
}
/*void panic_1(char *s)
{
	print_kernel(s);
	while (1)
		;
}*/
void con_init()
{
	x = 0;
	y = 0;
}
void con2_init()
{
	X = 0;
	Y = 25;
}

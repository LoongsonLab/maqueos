
#include <xtos.h>

#define CSR_CRMD 0x0
#define CSR_ECFG 0x4
#define CSR_ESTAT 0x5
#define CSR_EENTRY 0xc
#define CSR_TCFG 0x41
#define CSR_TICLR 0x44
#define CSR_CRMD_IE (1UL << 2)
#define CSR_TCFG_EN (1UL << 0)
#define CSR_TCFG_PER  0//(1UL << 1)
#define CSR_TICLR_CLR (1UL << 0)
#define CSR_ECFG_LIE_TI (1UL << 11)
#define CSR_ESTAT_IS_TI (1UL << 11)
#define CC_FREQ 4	
#define NR_PIX_X 1280
#define NR_PIX_Y 800
#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8
#define NR_CHAR_X (NR_PIX_X / CHAR_WIDTH)
#define NR_CHAR_Y (NR_PIX_Y / CHAR_HEIGHT)
#define NR_BYTE_PIX 4
#define VRAM_BASE 0x40000000UL

int plane_x = 3;
int plane_y = NR_CHAR_Y / 2; // 垂直居中
int direction = 1; // 1表示向右，-1表示向左
// 绘制飞机的函数（使用特定字符表示飞机）
void draw_plane(int x, int y) {
    
    write_char('*', x, y-1);  
	write_char('*', x-2, y);  
	write_char('*', x-1, y);  
	write_char('*', x, y);  

	write_char('*', x+1, y);  
	write_char('*', x+2, y);   
	write_char('*', x, y+1);  
	write_char('*', x-1, y+2);  
	write_char('*', x+1, y+2);  

    
}

// 模拟时钟中断
void clock_interrupt() {
	int x=plane_x;
	int y=plane_y;
    // 清除当前飞机的位置
    erase_char( x, y-1);  
	erase_char( x-2, y);  
	erase_char( x-1, y);  
	erase_char( x, y);  

	erase_char( x+1, y);  
	erase_char( x+2, y);   
	erase_char( x, y+1);  
	erase_char( x-1, y+2);  
	erase_char( x+1, y+2);  


    // 更新飞机位置
    plane_x += direction;

    // 检查边界
    if (plane_x >= NR_CHAR_X - 5) { // 右边界，飞机宽度为5
        plane_x = NR_CHAR_X - 5; // 限制到边界
        direction = -1; // 改变方向
    } else if (plane_x <= 0) { // 左边界
        plane_x = 0; // 限制到边界
        direction = 1; // 改变方向
    }

    // 重新绘制飞机
    draw_plane(plane_x, plane_y);
}

unsigned int val1=0xF4240;
void timer_interrupt()
{
	
    clock_interrupt();
	val1+= 0x10000;  // 增加初始值来延长触发间隔
    write_csr_64((unsigned long)val1 | CSR_TCFG_EN, CSR_TCFG);  // 更新定时器，不使用循环模式
}
void do_exception()
{
	unsigned int estat;

	estat = read_csr_32(CSR_ESTAT);
	if (estat & CSR_ESTAT_IS_TI)
	{
		write_csr_32(CSR_TICLR_CLR, CSR_TICLR);
		timer_interrupt();
	}
}
void int_on()
{
	unsigned int crmd;

	crmd = read_csr_32(CSR_CRMD);
	write_csr_32(crmd | CSR_CRMD_IE, CSR_CRMD);
}
void excp_init()
{
	unsigned int val;
	val = read_cpucfg(CC_FREQ);
	// val=0x1e240000;
	
	write_csr_64((unsigned long)val | CSR_TCFG_EN | CSR_TCFG_PER, CSR_TCFG);
	write_csr_64((unsigned long)exception_handler, CSR_EENTRY);
	write_csr_32(CSR_ECFG_LIE_TI, CSR_ECFG);
}
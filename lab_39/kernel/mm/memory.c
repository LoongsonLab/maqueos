#include <xtos.h>

#define CSR_DMW0 0x180
#define CSR_DMW0_PLV0 (1UL << 0)
#define MEMORY_SIZE 0x8000000
#define PAGE_SIZE 4096
#define NR_PAGE (MEMORY_SIZE >> 12)
#define RAM_BASE 0x90000000UL
#define START_FRAME (RAM_BASE >> 12)
#define BITS_PER_LONG (sizeof(unsigned long) * 8)

// char mem_map[NR_PAGE];
static unsigned long mem_map[NR_PAGE / BITS_PER_LONG]; // 位图数组,一个 unsigned long 类型的变量表示 32 个物理内存页的占用情况
static unsigned long next_free_page = 0; // 下一个空闲页的起始位置

unsigned long get_page()
{
	unsigned long page;

    // 从 next_free_page 开始扫描位图
    for (unsigned long i = next_free_page; i < NR_PAGE; i++)
    {
        // 查找未被占用的页
        unsigned long map = mem_map[i / BITS_PER_LONG];
        unsigned long bit = i % BITS_PER_LONG;
        if (map & (1UL << bit))
            continue;

        // 标记该页已被占用
        mem_map[i / BITS_PER_LONG] |= 1UL << bit;
        page = ((START_FRAME + i) << 12) | DMW_MASK;

        // 清零该页内容
        set_mem((char *)page, 0, PAGE_SIZE);

        // 更新下一个空闲页的起始位置
        next_free_page = i + 1;

        return page;
    }
	panic("panic: out of memory!\n");
	return 0;
}
void free_page(unsigned long page)
{
	unsigned long i;

    // 检查参数 page 是否合法
    if ((page & DMW_MASK) || !(page >= (START_FRAME << 12) && page < (NR_PAGE << 12)))
	{
		panic("panic: try to free invalid page!\n");
	}else{
        i = ((page & ~DMW_MASK) >> 12) - START_FRAME;
	    if (!(mem_map[i] & (1UL << (i % BITS_PER_LONG))))
		  panic("panic: try to free free page!\n");
	     mem_map[i] &= ~(1UL << (i % BITS_PER_LONG));
	}
        

	
}

void mem_init()
{
	int i;

	for (i = 0; i < NR_PAGE / BITS_PER_LONG; i++)
		mem_map[i] = 0;
	write_csr_64(CSR_DMW0_PLV0 | DMW_MASK, CSR_DMW0);
}
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#define BLOCK_SIZE 512
#define INODE_SIZE sizeof(struct inode)
#define NR_INODE (BLOCK_SIZE / INODE_SIZE)
#define NAME_LEN 9
struct inode
{
	int size;
	short index_table_blocknr;
	char type;
	char filename[NAME_LEN];
};
struct inode inode_table[NR_INODE] = {0};
char buf[BLOCK_SIZE / 2 * BLOCK_SIZE];
int main(int argc, char **argv){
    char *filename = argv[1];
    int fd = open("xtfs.img",O_RDONLY);
    read(fd,inode_table,BLOCK_SIZE);
    int i;
    for(i=0; i<NR_INODE; i++){
        if(strcmp(filename,inode_table[i].filename) == 0)
            break;
    }
    if(i == NR_INODE)
        printf("no file\n");
    int tablenr = inode_table[i].index_table_blocknr;
    short index_table[BLOCK_SIZE/sizeof(short)];
    lseek(fd,BLOCK_SIZE*tablenr,SEEK_SET);
    read(fd,index_table,BLOCK_SIZE);
    for(int i=0; i<BLOCK_SIZE/sizeof(short); i++){
        if(index_table[i] == 0)
            break;
        lseek(fd,BLOCK_SIZE*index_table[i],SEEK_SET);
        read(fd,buf+BLOCK_SIZE*i,BLOCK_SIZE);
    }
    close(fd);
    i = 0;
    while(buf[i] != 0)
        putchar(buf[i++]);
    return 0;
}
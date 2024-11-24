#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define MEM_PAGE_SIZE 0x1000  //每个页为4k

typedef struct mempool_s{
    int block_size; //将某个大的内存块拆分成指定块大小
    int free_count;//可以使用的块数
    void *mem; // 指向内存块开始位置
    void *ptr; // 下一次分配的开始位置
}mempool_t;

//初始化内存池
int memp_init(mempool_t *mp, size_t block_size){
    if(!mp) return -1;
    memset(mp, 0, sizeof(mempool_t));
    mp->block_size = block_size;
    mp->mem = malloc(MEM_PAGE_SIZE);
    mp->free_count = (MEM_PAGE_SIZE)/block_size;
    if(!mp->mem) return -1;
    mp->ptr = mp->mem;
    
    //为了确定内存块的释放及分配时机，建立块与块之间的链式结构
    char *ptr = (char *)mp->ptr; //取块的一个字节
    for(int i=0;i<mp->free_count-1;i++){
        *(char **)ptr = ptr+ block_size; //将块的一个字节设置为一个指针，该指针指向下一个块
        ptr += block_size;
    }
    *(char **)ptr = NULL; //最后一个指向空
    return 0;
}

void *_malloc(mempool_t *mp,size_t size){
    if (!mp || mp->ptr == NULL || mp->free_count == 0) return NULL;
    if(mp->block_size < size) return NULL;
    void *ptr = mp->ptr;
    mp->ptr = *(char **)ptr;
    mp->free_count -= 1;
    printf("malloc\n");
    return ptr;
}
//头插法释放块
void _free(mempool_t *mp,void *ptr){
    if(!mp || !ptr) return;
    *(char **)ptr = (char *)mp->ptr;
    mp->ptr = ptr;
    mp->free_count += 1;
    printf("free\n");
}

#define malloc(mp,size) _malloc(mp,size)
#define free(mp,ptr) _free(mp,ptr)
int main(){
    mempool_t mp;
    memp_init(&mp,32);

    void *ptr1 = malloc(&mp,5);
    void *ptr2 = malloc(&mp,15);
    void *ptr3 = malloc(&mp,5);
    void *ptr4 = malloc(&mp,15);
    printf("ptr1:%p\n",ptr1);
    printf("ptr2:%p\n",ptr2);
    free(&mp,ptr2);
    
    void *ptr5 = malloc(&mp,5);
    printf("ptr5:%p\n",ptr5);
    void *ptr6 = malloc(&mp,15);

    free(&mp,ptr1);
    free(&mp,ptr3);
    free(&mp,ptr4);
    free(&mp,ptr5);
    free(&mp,ptr6);
    
}
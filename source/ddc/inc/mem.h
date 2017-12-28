#ifndef MEM_H
#define MEM_H
#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"

//无论是32位还是64位下，前三个成员的成员始终是16字节倍数对齐的
//mutex_t 在32位下是24字节，64位下是40字节
//暂时先不加锁，如果出现多线程冲突再加
typedef struct
{
    //mutex_t lock;
    unsigned int size;
	unsigned int phy_addr;
	void *vir_addr;
}MEM_T;

int mem_init();

void *mem_alloc(int _size);

int mem_free(void *_ptr);

int get_hisi_phy_from_ptr(void *_ptr);

void *get_hisi_vir_from_ptr(void *_ptr);

int get_mem_size(void *_ptr);

int mem_test();


#ifdef __cplusplus
}
#endif
#endif


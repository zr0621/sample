#ifndef DATA_POOL_H
#define DATA_POOL_H
#ifdef __cplusplus
extern "C" {
#endif

// 媒体数据内存池管理

#include "thread.h"

#define byte unsigned char

typedef struct
{
	void *m_pOwner;
	mutex_t m_tLock;		//数据包锁
	int m_iRefCnt;			// 引用计数
	int m_iSeq;				//序列号
	int m_iMemSize;			// 包内存大小
	int m_iDataSize;		// 包实际数据长度
	int m_iType;			// 数据类型
	byte m_bData[0];		// 包数据
} package_t;


#define POOL_SIZE_LV0	(4 * 1024)
#define POOL_SIZE_LV1	(8 * 1024)
#define POOL_SIZE_LV2A	(12 * 1024)
#define POOL_SIZE_LV2	(16 * 1024)
#define POOL_SIZE_LV3A	(24 * 1024)
#define POOL_SIZE_LV3	(32 * 1024)
#define POOL_SIZE_LV4A	(40 * 1024)
#define POOL_SIZE_LV4B	(48 * 1024)
#define POOL_SIZE_LV4	(64 * 1024)
#define POOL_SIZE_LV5A	(80 * 1024)
#define POOL_SIZE_LV5	(128 * 1024)
#define POOL_SIZE_LV6	(256 * 1024)
#define POOL_SIZE_LV7	(512 * 1024)
#define POOL_SIZE_LV8	(1024 * 1024)


#define POOL_MAX_SIZE POOL_SIZE_LV8

#define POOL_GRP_NUM	14

#define GRP_PKG_NUM		256

#define GRP_SIZE_LV0	256
#define GRP_SIZE_LV1	256
#define GRP_SIZE_LV2A	128
#define GRP_SIZE_LV2	128
#define GRP_SIZE_LV3A	64
#define GRP_SIZE_LV3	64
#define GRP_SIZE_LV4A	16
#define GRP_SIZE_LV4B	16
#define GRP_SIZE_LV4	8
#define GRP_SIZE_LV5A	8
#define GRP_SIZE_LV5	8
#define GRP_SIZE_LV6	8
#define GRP_SIZE_LV7	2
#define GRP_SIZE_LV8	1


typedef struct
{
	int m_iCnt;//闲置块计数，此值用来控制内存碎片
	int m_iMaxSize;//此组可放置的块数
	mutex_t m_tLock;
	package_t *m_ptPkgs[GRP_PKG_NUM];
} group_t;

typedef struct
{
	int m_iMaxMemSize;
	int m_iSeq;
	int m_iSizeAlloc;
	int m_iSizeAllocHistory;
	mutex_t m_tLock;
	group_t m_tGrps[POOL_GRP_NUM];
}pool_t;

//一个数据组最多有256个数据包（数据块），实际数量根据实际情况调整，从1-256不等
//一个数据池最多有14个数据组，这个数量是固定的
//每个数据组的数据块的大小从4K到1M不等，用来申请内存和回收内存碎片
//源和业务对于数据的操作的基本单元就是数据包


pool_t *pool_create(int _iMaxMemSize);	//数据池创建
void pool_destroy(pool_t *_ptPool);	//数据池销毁
package_t *package_create(int _iSize, pool_t *_ptPool); //数据包创建
void package_refer_raise(package_t *_ptPkg);	//数据包引用,计数增加
void package_refer_reduce(package_t *_ptPkg); //数据包解引用,计数减少

void pool_debug_msg(char *_strMsg, pool_t *_ptPool);

#ifdef __cplusplus
}
#endif
#endif


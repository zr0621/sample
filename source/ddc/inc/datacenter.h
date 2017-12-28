#ifndef DATA_CENTER_H
#define DATA_CENTER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "datapool.h"
#include "thread.h"

typedef enum
{
	FIFO_TYPE_CYC = 1,	//cycle缓存环，缓存溢出时循环覆盖旧数据
	FIFO_TYPE_NORM = 2,	//nomal普通FIFO，缓存溢出时丢弃新数据
	FIFO_TYPE_SEQ = 3,	//sequence序列化FIFO，无数据丢失，缓存溢出时阻塞其它操作
	FIFO_TYPE_SEQ_NB = 4,	//noblock非阻塞序列化FIFO，但不阻塞操作，返回满fifo的数量
	FIFO_TYPE_IF = 5,        //如果fifo满了，看第一帧是否是I帧，如果不是I帧，就腾出空寸新数据
	FIFO_TYPE_BUTT
}fifo_type_e;

//fifo的每个节点指向一个package
typedef struct fifo_t
{
	struct fifo_t *m_ptNext;
	mutex_t m_tLock;
	int m_iType;			// fifo类型
	int m_iBase;			// fifo base指针
	int m_iTop;			// fifo top指针
	int m_iMaxPkgCnt;	//最大可缓存包数
	int m_iLastSeq;
	package_t *m_ptBuf[0];
} fifo_t;

//数据中心，相比数据分发中心透明，私有数据仍暴露给上层
//目前包含一个fifo，后续可以维护多个fifo
typedef struct
{
	mutex_t m_tLock;
	fifo_t *m_ptFifo;
} datacenter_t;	

datacenter_t *datacenter_create();
void datacenter_destroy(datacenter_t *_ptDc);
void datacenter_bind_fifo(fifo_t *_ptFifo, datacenter_t *_ptDc);
void datacenter_unbind_fifo(fifo_t *_ptFifo, datacenter_t *_ptDc);
int datacenter_push_pkg(package_t *_ptPkg, datacenter_t *_ptDc);
package_t *datacenter_pop_pkg(fifo_t *_ptFifo);
void clear_fifo(fifo_t *_ptFifo);
fifo_t *make_fifo(int _iPri, int _iSize);
void destroy_fifo(fifo_t *_ptFifo);
void change_fifo_type(int _iPri, fifo_t *_ptFifo);
int get_fifo_pack_num( fifo_t *_ptFifo);

#ifdef __cplusplus
}
#endif
#endif


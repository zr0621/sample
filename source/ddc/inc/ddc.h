//*********************************************
// 数据分发中心 Data Distribution Center
//
// 说明:
// 1 使用库前需要调用datacneter_PoolInit()函数
// 2 每个数据源对应一个DataCenter
// 3 每个需要对数据进行处理的业务对应一个DataFIFO
// 4 业务需要处理某个源的数据，需将其DataFIFO绑定到对应的DataCenter
// 5 源每产生一包数据，通过PushPackage将数据存入内存池
// 6 业务需要数据时，通过GetPackage将数据读出来
// 7 具体使用方法可以参考ddc_test接口
// 8 ddc封装了datacenter，遵循OOP思想封装隐藏了不必暴露给外面的数据
//*********************************************

#ifndef DDC_H
#define DDC_H
#ifdef __cplusplus
extern "C" {
#endif

#define DDC 	void
#define	FIFO 	void

#ifndef byte
#define byte unsigned char
#endif

typedef struct
{
	int m_iDataSize;			// 包实际数据长度
	int m_iType;				// 数据类型
	byte m_bData[0];			// 包数据
}ddc_package_t;

typedef enum
{
	DDC_FIFO_TYPE_CYC = 1,	//环形FIFO，缓存溢出时循环覆盖旧数据
	DDC_FIFO_TYPE_NORM = 2,	//普通FIFO，缓存溢出时丢弃新数据
	DDC_FIFO_TYPE_SEQ = 3,	//序列化FIFO，无数据丢失，缓存溢出时阻塞其它操作
	DDC_FIFO_TYPE_SEQ_NB = 4,//序列化FIFO，但不阻塞操作
	DDC_FIFO_TYPE_IF	= 5, //
}ddc_fifo_type_e;

// 数据缓存池初始化
int ddc_pool_init(int _max_size);
int ddc_pool_uninit();

// 创建一块内存
ddc_package_t *ddc_create_pkg(int _size);

// 增加内存引用记数 
void ddc_pkg_refer_raise(ddc_package_t *_pkg);
// 减少内存引用记数
void ddc_pkg_refer_reduce(ddc_package_t *_pkg);

// 创建一个FIFO队列
FIFO *ddc_make_fifo(int _type, int _size);
// 销毁一个队列
void ddc_destroy_fifo(FIFO *_fifo);
// 更改队列类型
void ddc_change_fifo_type(int _type, FIFO *_fifo);


// 从队列获取一包数据
ddc_package_t *ddc_pop_pkg(FIFO *_fifo);
// 清空队列数据
void ddc_clear_fifo(FIFO *_fifo);

// 创建一个数据分发中心
DDC *ddc_create();
// 销毁一个数据分发中心
void ddc_destroy(DDC *_ddc);
// 绑定fifo到数据分发中心
int ddc_bind_fifo(FIFO *_fifo, DDC *_ddc);
// 解绑定fifo到数据分发中心
int ddc_unbind_fifo(FIFO *_fifo, DDC *_ddc);
// 查询ddc是否绑定了fifo
int ddc_has_no_fifo_bind(DDC *_ddc);

// 将数据包交与数据分发中心分发
int ddc_push_package(ddc_package_t *_pkg, DDC *_ddc);

void ddc_debug_msg(char *_msg);

int ddc_get_fifo_pkg_num(FIFO *_fifo);

int ddc_test();

#ifdef __cplusplus
}
#endif
#endif


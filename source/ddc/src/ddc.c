//ddc调用datapool和datacenter，datacenter调用datapool
//ddc封装了datacenter内的细节，只给外部暴露简单易用的接口
//datacenter封装了fifo和pool，管理数据的先进先出，但不让外界直接操作fifo
//datapool封装了一个内存池，负责内存的分配和回收，管理内存碎片
//具体用法可以参考ddc_test接口
//参考海思MPP中的视频缓存池VB模块

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "platform.h"
#include "log.h"
#include "datapool.h"
#include "datacenter.h"
#include "ddc.h"
#include "mem.h"

static pool_t *g_data_pool = NULL;


// 数据缓存池初始化，比如128*1024*1024，即设定最大128MB
int ddc_pool_init(int _max_size)
{
    mem_init();
    if (NULL == g_data_pool)
	{
		g_data_pool = pool_create(_max_size);
	}
	
	return g_data_pool != NULL ? SUCCESS : FAILURE;
}

int ddc_pool_uninit()
{
	if (g_data_pool)
	{
		pool_destroy(g_data_pool);
		g_data_pool = NULL;
	}
	
	return SUCCESS;
}

//looks like oop
static ddc_package_t *_convert2ddc(package_t *_pkg)
{
	return (ddc_package_t *)(&_pkg->m_iDataSize);
}

static package_t *_convert2pkg(ddc_package_t *_pkg)
{
	//地址偏移m_iDataSize
	//(unsigned long)(&((type *)0)->member))是member在结构体中的偏移量
	char *addr = ((char *)_pkg)  - (long)&(((package_t *)0)->m_iDataSize);
	return (package_t *)addr;
}

ddc_package_t *ddc_create_pkg(int _size)
{
	int cnt = 1;
	package_t *pkg = NULL;

	do  //创建不成功则等待一段时间，尝试10次
	{
		pkg = package_create(_size, g_data_pool);

		if (pkg)
		{
			break;
		}
		usleep(100);
	} while (cnt++ < 10); //1ms

	if (NULL == pkg)
	{
		printf("alloc pkg %d error!\n", _size);
		return NULL;
	}
	else
	{
		return _convert2ddc(pkg);
	}
}

// 增加内存引用记数 
void ddc_pkg_refer_raise(ddc_package_t *_pkg)
{
	if (_pkg)
	{
		package_refer_raise(_convert2pkg(_pkg));
	}
}

// 减少内存引用记数
void ddc_pkg_refer_reduce(ddc_package_t *_pkg)
{
	if (_pkg)
	{
		package_refer_reduce(_convert2pkg(_pkg));
	}
}

// 创建一个FIFO队列
FIFO *ddc_make_fifo(int _type, int _size)
{
	return (FIFO *)make_fifo(_type, _size);
}

// 销毁一个队列
void ddc_destroy_fifo(FIFO *_fifo)
{
	destroy_fifo((fifo_t *)_fifo);
}

void ddc_change_fifo_type(int _type, FIFO *_fifo)
{
	change_fifo_type(_type, (fifo_t *)_fifo);
}

// 从队列获取一包数据
ddc_package_t *ddc_pop_pkg(FIFO *_fifo)
{
	package_t *pkg = datacenter_pop_pkg((fifo_t *)_fifo);
	
	return pkg ? _convert2ddc(pkg) : NULL;
}

// 清空队列数据
void ddc_clear_fifo(FIFO *_fifo)
{
	clear_fifo((fifo_t *)_fifo);
}

// 创建一个数据分发中心
DDC *ddc_create()
{
	return (DDC *)datacenter_create();
}

// 销毁一个数据分发中心
void ddc_destroy(DDC *_ddc)
{
	datacenter_destroy((datacenter_t *)_ddc);
}

// 绑定fifo到数据分发中心
int ddc_bind_fifo(FIFO *_fifo, DDC *_ddc)
{
	if (_fifo && _ddc)
	{
		datacenter_bind_fifo((fifo_t *)_fifo, (datacenter_t *)_ddc);
		return SUCCESS;
	}
	return FAILURE;
}

// 解绑定fifo到数据分发中心
int ddc_unbind_fifo(FIFO *_fifo, DDC *_ddc)
{
	if (_fifo && _ddc)
	{
		datacenter_unbind_fifo((fifo_t *) _fifo, (datacenter_t *)_ddc);

		return SUCCESS;
	}
	return FAILURE;
}

int ddc_has_no_fifo_bind(DDC *_ddc)
{
	datacenter_t * ddc = (datacenter_t *)_ddc;

	return (NULL == ddc->m_ptFifo);
}

// 将数据包交与数据分发中心分发
int ddc_push_pkg(ddc_package_t *_pkg, DDC *_ddc)
{
	if (_pkg && _ddc)
	{
		int ret = datacenter_push_pkg(_convert2pkg(_pkg), (datacenter_t *)_ddc);

		return ret > 0 ? FAILURE : SUCCESS;
	}

	return FAILURE;
}

void ddc_debug_msg(char *_msg)
{
	pool_debug_msg(_msg, g_data_pool);
}

int ddc_get_fifo_pkg_num(FIFO *_fifo)
{
	return  get_fifo_pack_num((fifo_t *)_fifo);
}

int ddc_test()
{
	LOG_INFO("ddc_test start");
    mem_init();
	int ret, i, j;

	//init
	LOG_INFO("ddc_pool_init start");
	ddc_pool_init(24*1024*1024);
	LOG_INFO("ddc_pool_init end");

	//create ddc
	LOG_INFO("ddc_create start");
	DDC *ptDdc = ddc_create();
	if(NULL == ptDdc)
	{
		return FAILURE;
	}
	LOG_INFO("ddc_make_fifo end");
	//create fifo
	FIFO *ptFifo = ddc_make_fifo(DDC_FIFO_TYPE_NORM,100);//定义类型和最大包数
	if(NULL == ptFifo)
	{
		return FAILURE;
	}
	LOG_INFO("ddc_make_fifo end");
    //维护DDC和FIFO，以控制数据的输入和输出
    
	//bind fifo
	ret = ddc_bind_fifo(ptFifo, ptDdc);
	if(SUCCESS != ret)
	{
		ddc_destroy_fifo(ptFifo);
		ddc_destroy(ptDdc);		
		return FAILURE;
	}
	LOG_INFO("ddc_bind_fifo end");
    
	//clear fifo
	ddc_clear_fifo(ptFifo);
	LOG_INFO("ddc_clear_fifo end");

    
	//create package
	int pkg_size[30]={20*1024,5*1024,15*1024,10*1024,1*1024,30*1024,
	36*1024,45*1024,50*1024,1*1024,100*1024,1*1024,
	1*1024,1*1024,1*1024,1*1024,1*1024,1*1024,
	1*1024,1*1024,1*1024,1*1024,1*1024,1*1024,
	1*1024,1*1024,1*1024,1*1024,1*1024,1*1024};
    int pkg_data[30]={12,23,34,45,56,67,78,89,90,
        91,92,94,95,96,97,98,99,100,
        101,102,103,104,105,106,107,108,109,
        110,119,123};
	ddc_package_t *ptWritePkg[30] = {0};//写数据测试用
    ddc_package_t *ptReadPkg[30] = {0};//读数据测试用
    for(i = 0; i < 30; i++)
    {
        ptWritePkg[i] = ddc_create_pkg(pkg_size[i]);
        if(ptWritePkg[i])
        {
            //ddc_pkg_refer_raise(ptWritePkg[i]);
            //除非想保留这块内存不想被释放成碎片，否则没必要单独增加引用次数
            //因为push_pkg接口内部raise了
            ptWritePkg[i]->m_iDataSize = pkg_size[i];
            memset(ptWritePkg[i]->m_bData, pkg_data[i], pkg_size[i]);
        }
    	ddc_push_pkg(ptWritePkg[i], ptDdc);
    	ptWritePkg[i] = NULL;
    }
    LOG_INFO("create and push pkg end");
    
    for(i = 0; i < 30; i++)
    {
        //pop data
    	ptReadPkg[i] = ddc_pop_pkg(ptFifo);
        for(j = 0; j < ptReadPkg[i]->m_iDataSize; j++)
    	{
    		LOG_INFO("ptReadPkg[%d]->m_bData[%d] = %d", i, j, ptReadPkg[i]->m_bData[j]);
    	}
        //pop完虽然fifo的base++了，但是内存块还没有释放
        //如果此块数据以后不再使用，可以调用ddc_pkg_refer_reduce
        //用以减少内存引用计数，从而交给pool去管理释放后的碎片
        ddc_pkg_refer_reduce(ptReadPkg[i]);
    }
    LOG_INFO("pop pkg end");

    //print pool dbg msg
	char dbg_msg[512]={0};
	memset(dbg_msg,0,512);
	ddc_debug_msg(dbg_msg);
	LOG_DEBUG("%s",dbg_msg);
    
    //uninit
    ddc_clear_fifo(ptFifo);
	ddc_unbind_fifo(ptFifo,ptDdc);
    ddc_destroy_fifo(ptFifo);
	ddc_destroy(ptDdc);
    ddc_pool_uninit();
	return SUCCESS;
}



// 数据内存池管理
//通过调用mem_alloc/mem_free兼容OS的malloc/free和海思的mmz_alloc/mmz_free
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

//为0则不会直接初始化时将数据块清0
#define PKG_MEM_INIT_ALL 1

#include "datapool.h"
#include "mem.h"

#define datapackage_lock_init(dpg)	mutex_init(&((dpg)->m_tLock))
#define datapackage_lock(dpg)		mutex_lock(&((dpg)->m_tLock))
#define datapackage_unlock(dpg)		mutex_unlock(&((dpg)->m_tLock))

#define datapool_lock_init(pool)	mutex_init(&((pool)->m_tLock))
#define datapool_lock(pool)			mutex_lock(&((pool)->m_tLock))
#define datapool_unlock(pool)		mutex_unlock(&((pool)->m_tLock))

#define datagroup_lock_init(grp)	mutex_init(&((grp)->m_tLock))
#define datagroup_lock(grp)			mutex_lock(&((grp)->m_tLock))
#define  datagroup_unlock(grp)		mutex_unlock(&((grp)->m_tLock))

#define	DO_CACHE	1

//因为要匹配最佳容量，所以数组下标0的容量为0
static int g_iPoolSize[] = {0, POOL_SIZE_LV0, POOL_SIZE_LV1, POOL_SIZE_LV2A,POOL_SIZE_LV2,POOL_SIZE_LV3A, POOL_SIZE_LV3,
	POOL_SIZE_LV4A,POOL_SIZE_LV4B,POOL_SIZE_LV4, POOL_SIZE_LV5A,POOL_SIZE_LV5, POOL_SIZE_LV6,POOL_SIZE_LV7, POOL_SIZE_LV8};

//这个g_iPoolSize和下面的m_iGrpSize主要是定义碎片的大小，对于新开辟的空间没有限制，只要总大小不超过内存池的_iMaxMemSize即可。

#define POOL_DEFAULT_SIZE		(32 * 1024 * 1024)	//默认32MB

pool_t *pool_create(int _iMaxMemSize)	//数据池创建
{
	pool_t *ptPool = (pool_t *)malloc(sizeof(pool_t));
	int m_iGrpSize[] = {GRP_SIZE_LV0, GRP_SIZE_LV1, GRP_SIZE_LV2A, GRP_SIZE_LV2, GRP_SIZE_LV3A ,GRP_SIZE_LV3,
		 GRP_SIZE_LV4A, GRP_SIZE_LV4B, GRP_SIZE_LV4, GRP_SIZE_LV5A, GRP_SIZE_LV5, GRP_SIZE_LV6, GRP_SIZE_LV7, GRP_SIZE_LV8};

	if (ptPool != NULL)
	{
		int i;
		
		memset(ptPool, 0, sizeof(*ptPool));
		ptPool->m_iSeq = 1;//它从1开始自增，每创建一个包+1
		datapool_lock_init(ptPool);

		for (i = 0; i < POOL_GRP_NUM; i++)
		{
			group_t *ptGrp = &ptPool->m_tGrps[i];
			
			ptGrp->m_iMaxSize = m_iGrpSize[i];//根据上面的定义，指定每个数据组的包容量
			datagroup_lock_init(ptGrp);
		}

		ptPool->m_iMaxMemSize = (_iMaxMemSize <= 0) ? POOL_DEFAULT_SIZE : _iMaxMemSize;
        //看上层是否指定，如果不指定数据池最大32MB
	}

	return ptPool;
}

//所有跟内存块(pkg)相关的分配和释放用mem_alloc和mem_free实现
static void package_group_destroy(group_t *_ptGrp)
{
	int i = 0;
	while (i < _ptGrp->m_iCnt)
	{
		mem_free(_ptGrp->m_ptPkgs[i++]);
	}
}

void pool_destroy(pool_t *_ptPool)	//数据池销毁
{
	if (_ptPool != NULL)
	{
		int i;

		for (i = 0; i < POOL_GRP_NUM; i++)
		{
			package_group_destroy(_ptPool->m_tGrps + i);
		}

		free(_ptPool);
	}
}

//待优化
//如果一个数据包申请的大小介于g_iPoolSize的两个值之间，则选择较大的数据块存放这个数据包
//每个数据池有14个数据组，每个数据组有若干数据块，同一个数据组中的数据块的大小是相同的
inline static int package_size_to_group(int _iSize, int *_piNewSize)
{
	int i;


	for (i = 0; i < POOL_GRP_NUM; i++)
	{
		if (_iSize > g_iPoolSize[i] && _iSize <= g_iPoolSize[i + 1])
		{
			*_piNewSize = g_iPoolSize[i + 1];
			return i;
		}
	}

	return -1;
}

//此处申请的大小是按照grp_size匹配后的，如4K、16K、64K等固定标准大小
inline static package_t *package_alloc(int _iSize)
{
    package_t *ptPkg = (package_t *)mem_alloc(sizeof(package_t) + _iSize);
	if (ptPkg != NULL)
	{
		#if PKG_MEM_INIT_ALL
		memset(ptPkg, 0, sizeof(package_t) + _iSize);
		#else
		memset(ptPkg, 0, sizeof(package_t));
		#endif
		ptPkg->m_iMemSize = _iSize;
		datapackage_lock_init(ptPkg);
	}

	return ptPkg;
}

#if DO_CACHE

static int pool_auto_free(pool_t *_ptPool) //释放 所有内存
{
    int i;
    int iFreeSize = 0;

	for (i = 0; i < POOL_GRP_NUM; i++)
	{
		int iSize = 0;
		group_t *ptGrp = &_ptPool->m_tGrps[i];

		datagroup_lock(ptGrp);
        //值大于0说明组内有闲置的内存块，从后往前释放，即从刚刚闲置的开始释放
		while (ptGrp->m_iCnt > 0)
		{
            package_t *ptPkg = ptGrp->m_ptPkgs[--ptGrp->m_iCnt];
			iSize += ptPkg->m_iMemSize;
			mem_free(ptPkg);
		}
		datagroup_unlock(ptGrp);

		if (iSize > 0)
		{
			datapool_lock(_ptPool);
			_ptPool->m_iSizeAlloc -= iSize;
			if (_ptPool->m_iSizeAlloc < 0)
			{
				_ptPool->m_iSizeAlloc = 0;
			}
			datapool_unlock(_ptPool);
            iFreeSize += iSize;
		}
	}
    return iFreeSize;
	
}

static inline void pool_auto_release(pool_t *_ptPool)
{
	int i;
    int iCnt = 0;
    int iMaxCnt = 0;

	for (i = 0; i < POOL_GRP_NUM; i++)
	{
		group_t *ptGrp = &_ptPool->m_tGrps[i];

		iCnt += ptGrp->m_iCnt;
		iMaxCnt += ptGrp->m_iMaxSize;
	}

	if (iCnt >= (iMaxCnt >> 1))//当已有数据包的数量大于等于所有数据组的一半容量时才自动释放
	{
		pool_auto_free(_ptPool);
	}
}

//数据包创建
//先根据要创建的大小选择一个合适的块存放，适配一个合适的组
//选定块（block即pkg）后，初始化pkg的参数
package_t *package_create(int _iSize, pool_t *_ptPool)  
{
    int iSize;
	package_t *ptPkg = NULL;
	
	if (_ptPool != NULL)
	{
		int iGid = package_size_to_group(_iSize, &iSize);//适配一个内存空间

		if (iGid >= 0)
		{
			group_t *ptGrp = &_ptPool->m_tGrps[iGid];
			int iSeq;

			datapool_lock(_ptPool);
			iSeq = _ptPool->m_iSeq++;//每创建一个包就记录一个序列号，查重
			if (iSeq >= 0x1FFFFFFF)//40亿
			{
				_ptPool->m_iSeq = 1;
			}
			datapool_unlock(_ptPool);
			
			if ((iSeq & 0xFFFF) == 0xFFFF)//20万
			{
				pool_auto_release(_ptPool);//包的数量太多基本不可能用完的时候就自动释放一部分
			}

			datagroup_lock(ptGrp);
            //如果此值还大于0，说明这个组还有空余的块(被释放的块)，优先用碎片空间。
            //先自减，然后分配，避免内存碎片，有效利用内存池,所以创建的包在用之前要清空一下，要不会有上个包的内容
			if (ptGrp->m_iCnt > 0)
			{
                ptPkg = ptGrp->m_ptPkgs[--ptGrp->m_iCnt];
				datagroup_unlock(ptGrp);
			}
            //如果没有空余的块（碎片空间），且已申请的内存还没超过既定上限，就申请一块。
			else if (_ptPool->m_iSizeAlloc < _ptPool->m_iMaxMemSize)
			{
                datagroup_unlock(ptGrp);
				ptPkg = package_alloc(iSize);//if PKG_MEM_INIT_ALL == true will memset 0

				if (ptPkg)
				{
					datapool_lock(_ptPool);
					_ptPool->m_iSizeAlloc += ptPkg->m_iMemSize;
					if (_ptPool->m_iSizeAlloc > _ptPool->m_iSizeAllocHistory)
					{
						_ptPool->m_iSizeAllocHistory = _ptPool->m_iSizeAlloc;
					}
					datapool_unlock(_ptPool);
				}
			}
            //内存超过POOL的既定上限
			else
			{
                printf("pkg create is oom, try to free some mem! \n");
                datagroup_unlock(ptGrp);

				if (pool_auto_free(_ptPool) > iSize)  //尝试释放内存
				{
                    ptPkg = package_alloc(iSize);
                    datapool_lock(_ptPool);
					_ptPool->m_iSizeAlloc += ptPkg->m_iMemSize;
                    datapool_unlock(_ptPool);
				}
			}
			

			if (ptPkg)
			{
				ptPkg->m_iDataSize = _iSize;
				ptPkg->m_pOwner = _ptPool;
				ptPkg->m_iRefCnt = 0;
				ptPkg->m_iSeq = iSeq;
			}
		}
        else
        {
            ;//should not come here, because it's too big
        }
	}

	return ptPkg;
}
#else
package_t *package_create(int _iSize, pool_t *_ptPool) 
{
    package_t *ptPkg = package_alloc(_iSize);

	if (ptPkg)
	{
		int iSeq;

		datapool_lock(_ptPool);
		iSeq = _ptPool->m_iSeq++;
		if (iSeq >= 0x1FFFFFFF)
		{
			_ptPool->m_iSeq = 1;
		}
		datapool_unlock(_ptPool);
		
		ptPkg->m_iDataSize = _iSize;
		ptPkg->m_pOwner = _ptPool;
		ptPkg->m_iRefCnt = 0;
		ptPkg->m_iSeq = iSeq;
	}

	return ptPkg;
}
#endif

void package_refer_raise(package_t *_ptPkg)	//数据包引用增加
{
	if (_ptPkg != NULL)
	{
		datapackage_lock(_ptPkg);
		_ptPkg->m_iRefCnt++;
		datapackage_unlock(_ptPkg);
	}
}

//所谓的缓存，就是用完的内存块先不释放掉，留着，给接下来的包用，
//如果满了，再考虑删除这些缓存块
#if DO_CACHE
//减少数据包的引用次数,此处为数据内存池缓存的关键
void package_refer_reduce(package_t *_ptPkg) 
{	
    if (_ptPkg != NULL)
	{
		int flg = 0;
		
		datapackage_lock(_ptPkg);
		
		_ptPkg->m_iRefCnt--;
        if(_ptPkg->m_iRefCnt < 0)
        {
            _ptPkg->m_iRefCnt = 0;
        }
		if (0 == _ptPkg->m_iRefCnt)
		{
			int iGid;
			int iSize;
			pool_t *ptPool = (pool_t *)_ptPkg->m_pOwner;
			group_t *ptGrp;

			iGid = package_size_to_group(_ptPkg->m_iMemSize, &iSize);
			datapackage_unlock(_ptPkg);

			if (iGid < 0)
			{
				// should not com here! 说明没有找到这个包对应的数据组
				printf("data pkg error! addr = %p, size = %d\n", _ptPkg, _ptPkg->m_iMemSize);
				//abort();
				package_t *tmp = NULL;
                tmp->m_iMemSize = 0;
				goto EXIT;
			}

			
			ptGrp = ptPool->m_tGrps + iGid;
			datagroup_lock(ptGrp);
            //如果这个数据组闲置块满了，就释放掉这个pkg，没法缓存了
			if (ptGrp->m_iCnt >= ptGrp->m_iMaxSize || ptGrp->m_iCnt >= GRP_PKG_NUM)
			{
                datagroup_unlock(ptGrp);
				flg = 1;
				datapool_lock(ptPool);
				ptPool->m_iSizeAlloc -= _ptPkg->m_iMemSize;
				datapool_unlock(ptPool);
			}
			else
			{
                //如果数据组的闲置块还没满，自增，并将此包作为一块闲置块放在组内记录下来，暂不释放
                ptGrp->m_ptPkgs[ptGrp->m_iCnt++] = _ptPkg;
				datagroup_unlock(ptGrp);
			}
			
		}
		else
		{
			datapackage_unlock(_ptPkg);
		}

	EXIT:		
		
		if (flg)
		{
			mem_free(_ptPkg);
		}
	}	
}
#else
void package_refer_reduce(package_t *_ptPkg) //数据包解引用
{	
	if (_ptPkg != NULL)
	{
		int flg = 0;

		datapackage_lock(_ptPkg);
		
		_ptPkg->m_iRefCnt--;
		if (0 == _ptPkg->m_iRefCnt)
		{
			flg = 1;
		}
		
		datapackage_unlock(_ptPkg);
		
		if (flg)
		{
			mem_free(_ptPkg);
		}
	}	
}
#endif

void pool_debug_msg(char *_strMsg, pool_t *_ptPool)
{
	if (_strMsg && _ptPool)
	{
		int i;
		char strBuf[256];
		
		sprintf(_strMsg, "\nData pool[%p] msg:\n", _ptPool);

		for (i = 0; i < POOL_GRP_NUM; i++)
		{
			sprintf(strBuf, "Group[%d] size = %dK, block = %d/%d, cache size = %dK\n",
				i, g_iPoolSize[i + 1] >> 10, _ptPool->m_tGrps[i].m_iCnt, _ptPool->m_tGrps[i].m_iMaxSize,
				(g_iPoolSize[i + 1] >> 10) * _ptPool->m_tGrps[i].m_iCnt);
			//按位右移10位相当于除2的10次方，即除1024，data_pool的大小从4K-1M不等
			strcat(_strMsg, strBuf);
		}

		sprintf(strBuf, "Total pkg size = %dK/%dK/%dK pkg_num = %d\n\n", _ptPool->m_iSizeAlloc >> 10, _ptPool->m_iSizeAllocHistory >> 10, _ptPool->m_iMaxMemSize >> 
		10,_ptPool->m_iSeq-1);
		strcat(_strMsg, strBuf);
	}
}



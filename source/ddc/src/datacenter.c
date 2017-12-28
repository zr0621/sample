//数据中心&队列管理

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
#include "datacenter.h"
#include "mem.h"

#define datacenter_lock_init(dc)	mutex_init(&((dc)->m_tLock))
#define datacenter_lock(dc)			mutex_lock(&((dc)->m_tLock))
#define datacenter_unlock(dc)		mutex_unlock(&((dc)->m_tLock))

#define fifo_lock_init(f)			mutex_init(&((f)->m_tLock))
#define fifo_lock(f)				mutex_lock(&((f)->m_tLock))
#define fifo_unlock(f)			mutex_unlock(&((f)->m_tLock))

datacenter_t *datacenter_create()
{
	datacenter_t *ptDc = (datacenter_t *)malloc(sizeof(datacenter_t));

	if (ptDc != NULL)
	{
		memset(ptDc, 0, sizeof(*ptDc));
		datacenter_lock_init(ptDc);
	}

	return ptDc;
}

void datacenter_destroy(datacenter_t *_ptDc)
{
	if (_ptDc != NULL)
	{
		free(_ptDc);
	}
}

void datacenter_bind_fifo(fifo_t *_ptFifo, datacenter_t *_ptDc)
{
	if (_ptFifo != NULL && _ptDc != NULL)
	{
		fifo_t *ptFifo;
		
		datacenter_lock(_ptDc);
		fifo_lock(_ptFifo);

        //说明已经被别的dc绑定过了，后面跟着其他fifo，所以不能绑定到其他dc
		if (_ptFifo->m_ptNext !=  NULL)
		{
			// should not come here
			goto EXIT;
		}

		ptFifo = _ptDc->m_ptFifo;
		if (ptFifo != NULL)
		{
			_ptFifo->m_ptNext = ptFifo;
            //如果这个dc之前绑定过fifo了，就把这个新的fifo下一个指向老的fifo,
            //相当于把两个fifo拼接起来了
			_ptDc->m_ptFifo = _ptFifo;
            //然后把当前的fifo更新成最新的这个fifo
		}
        //如果这个dc之前没有绑定过fifo，就正常绑定即可
		else
		{
			_ptDc->m_ptFifo = _ptFifo;
			_ptFifo->m_ptNext = NULL;
		}

	EXIT:
		fifo_unlock(_ptFifo);
		datacenter_unlock(_ptDc);
	}
}

void datacenter_unbind_fifo(fifo_t *_ptFifo, datacenter_t *_ptDc)
{
	if (_ptFifo != NULL && _ptDc != NULL)
	{
		fifo_t *ptPre = NULL;
		fifo_t *ptFifo = NULL;
		fifo_t *ptCur = NULL;
		
		datacenter_lock(_ptDc);
		fifo_lock(_ptFifo);

		for (ptCur = _ptDc->m_ptFifo; ptCur != NULL; ptCur = ptCur->m_ptNext)
		{
			if (ptCur == _ptFifo)
			{
				ptFifo = ptCur;
				break;
			}

			ptPre = ptCur;
		}

		if (ptFifo != NULL)
		{
			if (ptPre != NULL)
			{
				ptPre->m_ptNext = ptFifo->m_ptNext;
			}
			else
			{
				_ptDc->m_ptFifo = ptFifo->m_ptNext;
			}
		}

		_ptFifo->m_ptNext = NULL;

		fifo_unlock(_ptFifo);
		datacenter_unlock(_ptDc);
	}
}

int datacenter_push_pkg(package_t *_ptPkg, datacenter_t *_ptDc)
{
	int iRet = 0;
	
	if (_ptPkg != NULL && _ptDc != NULL)
	{
		int iCnt;

		fifo_t *ptFifo;
		
		datacenter_lock(_ptDc);

		package_refer_raise(_ptPkg);

		for (ptFifo = _ptDc->m_ptFifo; ptFifo != NULL; ptFifo = ptFifo->m_ptNext)
		{
			fifo_lock(ptFifo);
			//防止数据包重复发送
			if (ptFifo->m_iLastSeq == _ptPkg->m_iSeq)
			{
				printf("pkg proc twice!!!");
				goto NEXT;
			}
			iCnt = ptFifo->m_iTop - ptFifo->m_iBase;
			if (iCnt < ptFifo->m_iMaxPkgCnt)//FIFO没有溢出时
			{
				int iIndx = (ptFifo->m_iTop++) % ptFifo->m_iMaxPkgCnt;
				ptFifo->m_ptBuf[iIndx] = _ptPkg;
				ptFifo->m_iLastSeq = _ptPkg->m_iSeq;
				package_refer_raise(_ptPkg);
			}
			else if (FIFO_TYPE_CYC == ptFifo->m_iType)//当环形溢出则丢弃旧数据
			{
				printf("fifo seq circle!!!");
				int iIndx = (ptFifo->m_iTop++) % ptFifo->m_iMaxPkgCnt;
				package_t *ptOld = ptFifo->m_ptBuf[iIndx];

				package_refer_reduce(ptOld);
				
				ptFifo->m_ptBuf[iIndx] = _ptPkg;
				ptFifo->m_iLastSeq = _ptPkg->m_iSeq;
				package_refer_raise(_ptPkg);	
				ptFifo->m_iBase++;

				printf("fifo %p is full, do recover %p => %p\n", ptFifo, ptOld, _ptPkg);
			}
			else if (FIFO_TYPE_SEQ == ptFifo->m_iType)
			{
				printf("fifo seq is block!!!");
				int iIndx;
				do
				{
					fifo_unlock(ptFifo); 
					usleep(11);
					fifo_lock(ptFifo);
					iCnt = ptFifo->m_iTop - ptFifo->m_iBase;
				}
				while(iCnt >= ptFifo->m_iMaxPkgCnt);
				iIndx = (ptFifo->m_iTop++) % ptFifo->m_iMaxPkgCnt;
				ptFifo->m_ptBuf[iIndx] = _ptPkg;
				ptFifo->m_iLastSeq = _ptPkg->m_iSeq;
				package_refer_raise(_ptPkg);			
			}
			else if (FIFO_TYPE_SEQ_NB == ptFifo->m_iType)
			{
				printf("fifo seq no block!!!");
				iRet++;
			}
			else if (FIFO_TYPE_IF == ptFifo->m_iType)
			{
                printf("fifo if!!!");
                int iIndx;
				package_t *ptPkg = NULL;

                //遍历这个fifo，找到第一个pkg，如果不是I帧则删除给这个包腾空间
                while (ptFifo->m_iBase < ptFifo->m_iTop)
				{
					iIndx = ptFifo->m_iBase % ptFifo->m_iMaxPkgCnt;

					ptPkg = ptFifo->m_ptBuf[iIndx];

                    //如果是I帧则保留
					if (0 == ptPkg->m_iType)
					{
						break;
					}
					else
					{
						package_refer_reduce(ptPkg);
						ptFifo->m_iBase++;
					}
				}
                //腾出删除P帧后的空间，存放这个pkg
				if (ptFifo->m_iTop - ptFifo->m_iBase <  ptFifo->m_iMaxPkgCnt)
				{
					iIndx = (ptFifo->m_iTop++) % ptFifo->m_iMaxPkgCnt;
					ptFifo->m_ptBuf[iIndx] = _ptPkg;
					ptFifo->m_iLastSeq = _ptPkg->m_iSeq;
					package_refer_raise(_ptPkg);
				}
			}
			else
			{
				printf("fifo %p is full, drop pkg %p", ptFifo, _ptPkg);
			}

		NEXT:			
			fifo_unlock(ptFifo);
		}

		package_refer_reduce(_ptPkg);
		
		datacenter_unlock(_ptDc);
	}	

	return iRet;
}

package_t *datacenter_pop_pkg(fifo_t *_ptFifo)
{
	package_t *ptPkg = NULL;

	if (_ptFifo != NULL)
	{
		fifo_lock(_ptFifo);

		if (_ptFifo->m_iTop > _ptFifo->m_iBase)
		{
			int iIndx = (_ptFifo->m_iBase++) % _ptFifo->m_iMaxPkgCnt;

			ptPkg = _ptFifo->m_ptBuf[iIndx];
			if (_ptFifo->m_iBase == _ptFifo->m_iTop)
			{
				_ptFifo->m_iBase = 0;
				_ptFifo->m_iTop = 0;
			}
		}
		
		fifo_unlock(_ptFifo);
	}

	return ptPkg;
}

void clear_fifo(fifo_t *_ptFifo)
{
	if (_ptFifo != NULL)
	{
		int i;

		fifo_lock(_ptFifo);

		for (i = _ptFifo->m_iBase; i < _ptFifo->m_iTop; i++)
		{
			int iIndx = i % _ptFifo->m_iMaxPkgCnt;
			package_t *ptPkg = _ptFifo->m_ptBuf[iIndx];

			package_refer_reduce(ptPkg);
		}

		_ptFifo->m_iBase = 0;
		_ptFifo->m_iTop = 0;
		_ptFifo->m_iLastSeq = 0;

		fifo_unlock(_ptFifo);
	}
}

fifo_t *make_fifo(int _iPri, int _iSize)
{
	fifo_t *ptFifo = (fifo_t *)malloc(sizeof(fifo_t) + _iSize * sizeof(package_t *));

	if (ptFifo != NULL)
	{
		memset(ptFifo, 0, sizeof(*ptFifo));

		ptFifo->m_iMaxPkgCnt = _iSize;
		ptFifo->m_iType = _iPri;
		fifo_lock_init(ptFifo);
	}

	return ptFifo;
}

void destroy_fifo(fifo_t *_ptFifo)
{
	if (_ptFifo != NULL)
	{
		clear_fifo(_ptFifo);
		free(_ptFifo);
	}
}

void change_fifo_type(int _type, fifo_t *_fifo)
{
	if (_fifo)
	{
		fifo_lock(_fifo);
		_fifo->m_iType = _type;
		fifo_unlock(_fifo);
	}
}

int get_fifo_pack_num( fifo_t *_ptFifo)
{
	if (_ptFifo)
	{
		return (_ptFifo->m_iTop - _ptFifo->m_iBase);
	}
	else
	{
		return 0;
	}
}


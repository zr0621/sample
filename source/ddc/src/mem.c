
#include <malloc.h>

#ifdef HISI_PLATFORM
#include "hi_comm_sys.h"
#include "hi_comm_video.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "hi_comm_vb.h"
#include "hi_defines.h"
#define MPP_SYS_ALIGN_WIDTH 64
#define USE_HI_MMZ 1
#else
#define USE_HI_MMZ 0
#endif

#include "mem.h"
#include "log.h"
#include "utils.h"

//使用前需要调用mem_init初始化
//_mem_alloc 输入size，返回mem_info_t结构指针，NULL为失败
//_mem_free 输入结构指针，调用free，指针置空，返回成功失败

#define mem_lock_init(mem)	mutex_init(&((mem)->lock))
#define mem_lock(mem)		mutex_lock(&((mem)->lock))
#define mem_unlock(mem)		mutex_unlock(&((mem)->lock))


static void *_mem2void(MEM_T *_mem)
{
	return (void *)(&_mem->vir_addr);
}

static MEM_T *_void2mem(void *_ptr)
{
	char *addr = ((char *)_ptr)  - (unsigned long)&(((MEM_T *)0)->vir_addr);
	return (MEM_T *)addr;
}


#if USE_HI_MMZ

int calc_pic_vb_blk_size(SIZE_S *pstSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth)
{
    HI_U32 u32VbSize;
    HI_U32 u32HeaderSize;

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
        printf("pixel format[%d] input failed!\n", enPixFmt);
        return HI_FAILURE;
    }

    if (16 != u32AlignWidth && 32 != u32AlignWidth && 64 != u32AlignWidth)
    {
        printf("system align width[%d] input failed!\n", \
                   u32AlignWidth);
        return HI_FAILURE;
    }
    
    u32VbSize = (ALIGN_CEILING(pstSize->u32Width, u32AlignWidth) * \
                 ALIGN_CEILING(pstSize->u32Height, u32AlignWidth) * \
                 ((PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt) ? 2 : 1.5));

    VB_PIC_HEADER_SIZE(pstSize->u32Width, pstSize->u32Height, enPixFmt, u32HeaderSize);
    u32VbSize += u32HeaderSize;

    return u32VbSize;
}

static int _mem_init()
{
    //printf("Init hisi mpp sys & vb\n");
    
    MPP_SYS_CONF_S stSysConf = {0};
    stSysConf.u32AlignWidth = MPP_SYS_ALIGN_WIDTH;
    
    VB_CONF_S stVbConf;
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    unsigned int u32BlkSize;
    stVbConf.u32MaxPoolCnt = 128;
    SIZE_S size;
    size.u32Height = 1088;
    size.u32Width = 1920;
    u32BlkSize = calc_pic_vb_blk_size(&size, PIXEL_FORMAT_YUV_SEMIPLANAR_420, MPP_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 15;
    
    CHECK_RET(HI_MPI_VB_Exit(),"HI_MPI_VB_Exit");
    CHECK_RET(HI_MPI_SYS_Exit(),"HI_MPI_SYS_Exit");

    CHECK_RET(HI_MPI_VB_SetConf(&stVbConf),"HI_MPI_VB_SetConf");
    CHECK_RET(HI_MPI_VB_Init(),"HI_MPI_VB_Init");

    CHECK_RET(HI_MPI_SYS_SetConf(&stSysConf),"HI_MPI_SYS_SetConf");
    CHECK_RET(HI_MPI_SYS_Init(),"HI_MPI_SYS_Init");

    return HI_SUCCESS;
}

//ret phy addr is data's phy addr
//ret vir addr is data's vir addr
//not alloc's addr, if use alloc's need to call get_*_from_ptr api
static MEM_T *_mem_alloc(int _size)
{
    int ret = 0;
    MEM_T *mem = NULL;
    MEM_T temp = {0};
    memset(&temp,0,sizeof(MEM_T));
    temp.size = sizeof(MEM_T) + _size;
    ret = HI_MPI_SYS_MmzAlloc_Cached(&(temp.phy_addr), (void**)&(temp.vir_addr), "mem_alloc", NULL, temp.size);
    if(ret == 0)
    {
        mem = (MEM_T *)(temp.vir_addr);
        mem->size = temp.size;
        mem->phy_addr = temp.phy_addr+0x8;//偏移到data数据所在的物理地址，空出size和phy。
        mem->vir_addr = &mem->vir_addr;//物理地址和虚拟地址同步偏移8。
        printf("alloc phy_addr = %x\n",temp.phy_addr);
        printf("alloc vir_addr = %p\n",temp.vir_addr);
        printf("mem->size = %d\n",mem->size);
        printf("mem->phy_addr = %x\n",mem->phy_addr);
        printf("mem->vir_addr = %p\n",mem->vir_addr);
        return mem;
    }
	else
    {
        printf("HI_MPI_SYS_MmzAlloc_Cached  ERROR_CODE = %x\n",ret);
        return NULL;
    }   
}

static int _mem_free(MEM_T *_mem)
{
    MEM_T temp = {0};
    memset(&temp,0,sizeof(MEM_T));
    temp.phy_addr = _mem->phy_addr;
    temp.vir_addr = _mem->vir_addr;
    _mem->vir_addr = NULL;
	_mem->phy_addr = 0;
    _mem->size = 0;
    HI_MPI_SYS_MmzFree(temp.phy_addr, temp.vir_addr);
	_mem = NULL;
	return 0;
}

#else

static int _mem_init()
{
    //printf("~~~~~~~~~~nothing to do~~~~~~~~~~\n");
    return 0;
}

static MEM_T *_mem_alloc(int _size)
{
    MEM_T *mem = (MEM_T *)malloc(sizeof(MEM_T) + _size);
    if(mem)
    {
        mem->size = _size;
    	mem->phy_addr = 0xffffffff;
        mem->vir_addr = &mem->vir_addr;
        return mem;
    }
	else
    {
        free(mem);
        return NULL;
    }   
}

static int _mem_free(MEM_T *_mem)
{
	_mem->vir_addr = NULL;
	_mem->phy_addr = 0;
    _mem->size = 0;
	free(_mem);
	_mem = NULL;
	return 0;
}

#endif

int mem_init()
{
    return _mem_init();
}

void *mem_alloc(int _size)
{
    MEM_T *mem = _mem_alloc(_size);
    if(mem)
    {
        return _mem2void(mem);
    }
    else
    {
        return NULL;
    }
}

int mem_free(void *_ptr)
{
    MEM_T *mem = NULL;
    if(_ptr)
    {
        mem = _void2mem(_ptr);
    }
    return _mem_free(mem);
}

//this _ptr must be the one alloc return
int get_hisi_phy_from_ptr(void *_ptr)
{
    MEM_T *mem = NULL;
    if(_ptr)
    {
        printf("_ptr is %p\n",_ptr);
        mem = _void2mem(_ptr);
        printf("mem is %p\n",mem);
    }
    printf("mem->phy_addr is %x\n",mem->phy_addr);
    printf("mem->phy_addr-0x8 is %x\n",mem->phy_addr-0x8);
    return mem->phy_addr-0x8;
}

//using for matching phy when call mpp&ive api
void *get_hisi_vir_from_ptr(void *_ptr)
{
    MEM_T *mem = NULL;
    if(_ptr)
    {
        printf("_ptr is %p\n",_ptr);
        mem = _void2mem(_ptr);
        printf("mem is %p\n",mem);
    }
    return mem;
    //this mem's addr is hisi vir addr
}

int get_mem_size(void *_ptr)
{
    MEM_T *mem = NULL;
    if(_ptr)
    {
        mem = _void2mem(_ptr);
    }
    return mem->size;
}

int mem_test()
{
    #if 0
    _mem_init();
    
    MEM_T *mem = _mem_alloc(32*1024);
    if(mem)
    {
        LOG_INFO("mem->size = %d",mem->size);
        LOG_INFO("mem->phy_addr = %x",mem->phy_addr);
        LOG_INFO("mem->vir_addr = %p",mem->vir_addr);

        _mem_free(mem);
        return 0;
    }
    else
    {
        return -1;
    }
    #else
    mem_init();
    int phy_addr = 0;
    int mem_size = 0;
    void *vir_addr = NULL;
    void *ptr = mem_alloc(64*1024);
    memset(ptr,0,64*1024);
    if(ptr)
    {
        LOG_INFO("ptr is %p\n",ptr);
        vir_addr = ptr - 0x8;
        LOG_INFO("hisi vir_addr is %p\n",vir_addr);
        phy_addr = get_hisi_phy_from_ptr(ptr);
        LOG_INFO("phy_addr is %x\n",phy_addr);
        mem_size = get_mem_size(ptr);
        LOG_INFO("mem_size is %d\n",mem_size);
        mem_free(ptr);
    }
    return 0;
    #endif
}


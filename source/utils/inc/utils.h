#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "platform.h"

#define ASSERT(expr, exec)               \
	do{ 								  \
		if (1)\
		{\
			if (!(expr)) {					  \
				printf("\nASSERT failed at:\n"\
					   "  >File name: %s\n"   \
					   "  >Function : %s\n"   \
					   "  >Line No. : %d\n"   \
					   "  >Condition: %s\n",  \
						__FILE__,__FUNCTION__, __LINE__, #expr);\
				exec;\
			} \
		}\
		else\
		{\
			/* do nothing*/\
		}\
	}while(0)
	
#define CHECK_RET(express,name)\
    do{\
        int ret;\
        ret = express;\
        if (SUCCESS != ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, ret);\
            return ret;\
        }\
    }while(0)


#define MALLOC(p, size) \
    do{\
        if(!p)\
        {\
            p = malloc(size);\
        }\
    } while(0)

#define FREE(p) \
    do{\
    	if(p)\
    	{\
    		free(p);\
    		p=NULL;\
    	}\
    } while(0)

//将val变为min,max范围内的值
#define VALIDVALUE(val, min, max)		(val<min)?min:((val>max)?max:val)
    
//判断(x,y)是否在区域(top,left,top+width,left+height)内
#define IS_IN_AREA(x, y, top, left, width, height)\
    (x>top && x<(top+width) && y>left && y<(left+height))
    
//判断当前时间是否在时间段内,时间值为当天的第N秒
#define IS_IN_TIME_RANGE(nTimeNow, nBeginTime, nEndTime) \
    ((nBeginTime < nEndTime && (nTimeNow >= nBeginTime && nTimeNow <= nEndTime)) ||\
     (nBeginTime >= nEndTime && (nTimeNow >= nBeginTime || nTimeNow <= nEndTime)) )


//对齐，align必须是2的N次方。
//向上对齐，例如：ALIGN_CEILING(5,4) = 8
#define ALIGN_CEILING(x,align)     ( ((x) + ((align) - 1) ) & ( ~((align) - 1) ) )
//向下对齐，例如：ALIGN_FLOOR(5,4) = 4
#define ALIGN_FLOOR(x,align)       (  (x) & (~((align) - 1) ) )


//数组个数
#define ITEMNUM(arrayCtl) (sizeof(arrayCtl)/sizeof(arrayCtl[0]))

//对strncpy的封装
//strncpy不会再字符串后面加0，手动加上一个
#define STRNCPY(dst,src,nDstLen)\
    do{\
    	const char *__pSrc_ = (const char *)(src);\
    	if(__pSrc_ != NULL)\
    	{\
    		strncpy((char*)(dst),__pSrc_,(nDstLen));\
    		(dst)[(nDstLen-1)]=0;\
    	}\
    	else\
    	{\
    		(dst)[0]=0;\
    	}\
    }while(0)

//strcpy字符串到数组
//注意	:dstArray必须是char型数组，且不能强转为指针。那样sizeof(dstArray)出错
//例如这样的用法是错的:
//		unsigned char a[10];
//		STRCPY_ARRAY((char*)dstArray, src);
#define STRCPY_ARRAY(dstArray, src)\
    do{\
    	const char *__pSrc_ = (const char *)(src);\
    	if(__pSrc_ != NULL)\
    	{\
    		strncpy((char*)(dstArray),__pSrc_,sizeof(dstArray)-1);\
    		(dstArray)[(sizeof(dstArray))-1]=0;\
    	}\
    	else\
    	{\
    		(dstArray)[0]=0;\
    	}\
    }while(0)

//strcpy字符串到数组
#define STRCPY_ARRAY_ex(dstArray, src, arrarSize)\
    do{\
    	const char *__pSrc_ = (const char *)(src);\
    	if(__pSrc_ != NULL)\
    	{\
    		strncpy((char*)(dstArray),__pSrc_,(arrarSize)-1);\
    		(dstArray)[(arrarSize)-1]=0;\
    	}\
    	else\
    	{\
    		(dstArray)[0]=0;\
    	}\
    }while(0)


//日期时间
typedef struct
{
	int year;
	int month;	//1~12
	int day;	//1~31
	int hour;	//0~23
	int minute;	//0~59
	int second;	//0~59
}datetime_t;

void get_localtime(datetime_t * datetime);

uint64 get_exact_time();

int int_comp (const void *a, const void *b);

void int_sort(const int* ary, int cnt);


#ifdef __cplusplus
}
#endif

#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "utils.h"

void get_localtime(datetime_t * datetime)
{
	time_t time_seconds = time(NULL);
	//printf("time_seconds == %d\n",(int)time_seconds);
	struct tm now_time;
	localtime_r(&time_seconds, &now_time);
	//printf("now_time : %d-%d-%d %d:%d:%d\n", now_time.tm_year + 1900,now_time.tm_mon + 1,now_time.tm_mday,now_time.tm_hour,now_time.tm_min,now_time.tm_sec);
	datetime->year = now_time.tm_year + 1900;
	datetime->month = now_time.tm_mon + 1;
	datetime->day = now_time.tm_mday;
	datetime->hour = now_time.tm_hour;
	datetime->minute = now_time.tm_min;
	datetime->second = now_time.tm_sec;
	//now_time.tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	//now_time.tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
	//now_time.tm_isdst;/* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负 */
	//printf("datetime : %04d-%02d-%02d %02d:%02d:%02d\n", datetime->year,datetime->month,datetime->day,datetime->hour,datetime->minute,datetime->second);
}

//ms级
uint64 get_ms_time()
{
    struct timeval tv0;
    gettimeofday(&tv0, 0);
    return (uint64)(tv0.tv_sec*1000 + tv0.tv_usec/1000);
}

//from参数与get_exact_time配合使用，打印程序开销时间，深绿色
int64 prt_cost_time(char * str, int64 from)
{
	int64 now = get_ms_time();
	printf("[0;32m""%s %lld ms\n""[0;39m", str, now - from);
	return now;
}

//sort integer from big to small
//if u want tu compare string, could use strcmp or memcmp 
int int_comp (const void *a, const void *b)  
{  
	return *(int *)b - *(int *)a;  
}  

//arg1 整形数组
//arg2 数组的个数
//usage:
//int num[16]={9,5,2,7};
//printf("ITEMNUM(num) = %d\n",ITEMNUM(num));
//_sort(num,ITEMNUM(num));
void int_sort(const int* ary, int cnt)
{
	//int i = 0;
	printf("ITEMNUM(ary) = %ld\n",(long int)ITEMNUM(ary));
	/*for(i=0;i<cnt;i++)
	{
		printf("before sort ary[%d] = %d\n",i,ary[i]);
	}*/
	qsort((void *)ary,cnt,sizeof(ary[0]),int_comp);
	/*for(i=0;i<cnt;i++)
	{
		printf("after sort ary[%d] = %d\n",i,ary[i]);
	}*/
}



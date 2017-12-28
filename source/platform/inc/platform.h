#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __cplusplus
extern "C"{
#endif

#define TRUE 1
#define FALSE 0

//返回值
#define SUCCESS 0
#define FAILURE -1

//错误码
#define ERR_NO							0   //成功
#define ERR_FAILURE				  		-1  //失败
#define ERR_UNKOWN				  		-2  //未知错误
#define ERR_BAD_PARAM					-3  //参数错误
#define ERR_NO_MEMORY					-4  //内存不足
#define ERR_NOT_HANDLED  				-5  //没有被处理
#define ERR_BAD_HANDLE					-6  //错误的句柄
#define ERR_OPEN_FAIL					-7  //打开错误
#define ERR_READ_FAIL					-8  //读错误
#define ERR_WRITE_FAIL					-9  //写错误
#define ERR_NO_RESOURCE					-10 //资源不足
#define ERR_NOT_FIND					-11 //没有找到
#define ERR_NOT_INIT					-12 //未初始化
#define ERR_TIMEOUT						-13 //超时
#define ERR_BUSY						-14 //忙
#define ERR_SEND_FAIL					-15 //发送失败
#define ERR_BAD_PASSWD					-16 //密码错误
#define ERR_NOT_SUPPORT				  	-17  //不支持的功能


#if defined WIN32 || defined WIN64
	typedef unsigned __int8         uint8;
	typedef unsigned __int16        uint16;
	typedef unsigned __int32        uint32;
	typedef unsigned __int64        uint64;
	typedef __int8					int8;
	typedef __int16					int16;
	typedef __int32					int32;
	typedef __int64					int64;
	typedef float                   Float;
	typedef double					Double;
	typedef int                     BOOL;
	typedef void *                  Handle;
	#ifndef NULL
		#define NULL    0L
	#endif
	
#else
	
	typedef unsigned char           uint8;
	typedef unsigned short          uint16;
	typedef unsigned int            uint32;
	typedef unsigned long long      uint64;
	typedef signed char             int8;
	typedef signed short            int16;
	typedef signed int              int32;
	typedef signed long long        int64;
	typedef float                   Float;
	typedef double					Double;
	typedef int                     BOOL;
    typedef unsigned int            size_t32;
    typedef long unsigned int       size_t64;
	typedef void *                  Handle;
	#ifndef NULL
		#define NULL    0L
	#endif
	
#endif

//日期显示格式
typedef enum
{
    TIME_FORMAT_MMDDYYYY=0,		//10-30-2017
    TIME_FORMAT_YYYYMMDD,		//2017-10-30
    TIME_FORMAT_DDMMYYYY,		//30-10-2017
    TIME_FORMAT_MAX,
}time_format_e;

#ifdef __cplusplus
}
#endif

#endif

#ifndef LOG_H
#define LOG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>

//log print color
#define NOCOLOR_PRINT 0x0
#define CYAN_PRINT 0x01
#define L_CYAN_PRINT 0x02
#define RED_PRINT 0x03
#define L_RED_PRINT 0x04
#define YELLOW_PRINT 0x05
#define L_YELLOW_PRINT 0x06
#define GREEN_PRINT 0x07
#define L_GREEN_PRINT 0x08
#define BLUE_PRINT 0x09
#define L_BLUE_PRINT 0x10
#define PURPLE_PRINT 0x11
#define L_PURPLE_PRINT 0x12
#define GREY_PRINT 0x13
#define WHITE_PRINT 0x14
#define BLACK_PRINT 0x15
#define L_BLACK_PRINT 0x16



//log level
//ALL Level 是最低等级的，用于打开所有日志记录。
//TRACE level 相比DEBUG更细致化的记录事件消息。
//DEBUG level 主要用于开发过程中打印一些运行信息。
//INFO level 用于生产环境中输出程序运行的一些重要信息。
//WARN level 表明会出现潜在错误的情形。
//ERROR level 指出虽然发生错误事件，但仍然不影响系统的继续运行。
//FATAL level 指出每个严重的错误事件将会导致应用程序的退出。
//OFF Level 是最高等级的，用于关闭所有日志记录。
//比如在这里定义了INFO级别，则应用程序中所有DEBUG级别的日志信息将不被打印出来。
//也是说大于等于的级别的日志才输出。

#define	LEVEL_ALL	0x01
#define LEVEL_TRACE 0x02
#define LEVEL_DEBUG	0x03
#define LEVEL_INFO	0x04
#define LEVEL_WARN	0x05
#define LEVEL_ERROR	0x06
#define LEVEL_FATAL	0x07
#define LEVEL_OFF	0x08

#ifndef LEVEL_SET
#define LEVEL_SET LEVEL_ALL
#endif

int log_print(unsigned int handle, char* pszfmt, ...);

//定义DEBUG_PRINT，则打印Printf。否则不打印。
#ifdef DEBUG_PRINT
	#define PRINT(fmt, args...) log_print(NOCOLOR_PRINT,"[FILE:%s FUNC:%s LINE:%d] " fmt,__FILE__,__func__, __LINE__,##args)
#else
	#define PRINT(fmt, args...) do{} while(0)
#endif

#define LOG_TRACE(format, ...)\
	do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr((__FILE__),'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_TRACE >= LEVEL_SET) \
		{ \
			log_print(GREEN_PRINT,(char *)"[TRACE] [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
		else \
		{ \
			; \
		} \
	} while (0)  

#define LOG_DEBUG(format, ...)\
	do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr((__FILE__),'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_DEBUG >= LEVEL_SET) \
		{ \
			log_print(BLUE_PRINT,(char *)"[DEBUG] [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
		else \
		{ \
			; \
		} \
	} while (0)  

#define LOG_INFO(format, ...)\
	do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr(__FILE__,'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_INFO >= LEVEL_SET) \
		{ \
			log_print(CYAN_PRINT,(char *)"[INFO]  [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
		else \
		{ \
			; \
		} \
	} while (0)  

#define LOG_WARN(format, ...)\
    do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr(__FILE__,'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_WARN >= LEVEL_SET) \
		{ \
			log_print(YELLOW_PRINT,(char *)"[WARN]  [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
        else \
        { \
			; \
		} \
    } while (0)  

#define LOG_ERROR(format, ...)\
	do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr(__FILE__,'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_ERROR >= LEVEL_SET) \
		{ \
			log_print(RED_PRINT,(char *)"[ERROR] [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
		else \
		{ \
			; \
		} \
	} while (0)  

#define LOG_FATAL(format, ...)\
	do {\
		char file_name[32]; \
		char *ptr = (char *)strrchr(__FILE__,'/'); \
		sprintf(file_name,"%s",ptr+1); \
		if(LEVEL_FATAL >= LEVEL_SET) \
		{ \
			log_print(PURPLE_PRINT,(char *)"[FATAL] [FILE:%s FUNC:%s LINE:%d] " format,\
				file_name, __func__, __LINE__,##__VA_ARGS__);\
		} \
		else \
		{ \
			; \
		} \
	} while (0)  

#ifdef __cplusplus
		}
#endif

#endif

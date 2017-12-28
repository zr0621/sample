/*日志打印、日志读写、日志检索*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include "log.h"


#define LOGFILE_DIR "./log/"
#define MAX_LOGFILE_SIZE (10 * 1024 * 1024) //日志文件大小限制为10MB,每天一个

static int get_tid()//获取线程ID，区分32位和64位系统
{
#if 0
	#ifdef __x86_64__
		printf("__x86_64__\n");
	#elif __i386__
		printf("__i386__\n");
	#endif
#endif
#if 1
	//return (int)syscall(SYS_gettid);
	return (int)syscall(__NR_gettid);
	//return pthread_self();
#else
	if((sizeof(char *)*8) == 32)//获取char*类型的大小，乘8即为OS的位数。 
	{
		return (int)syscall(224);//32位系统下获取线程id的系统调用
	}
	else if((sizeof(char *)*8) == 64)
	{
		return (int)syscall(186);//64位系统下获取线程id的系统调用
	}
	else
	{
		return (int)getpid();//两者都不是则返回进程id
	}
#endif
}

static void file_check(FILE *_pFile)
{
	struct stat buf;
	FILE *pFile = _pFile;
	if(pFile == NULL)
	{
		return;
	}
	
	time_t time_seconds = time(NULL);
	struct tm now_time;
	localtime_r(&time_seconds, &now_time);
	char file_name[16];
	memset(file_name, 0, 16);
	sprintf(file_name,"%04d%02d%02d.debug.log",now_time.tm_year + 1900,now_time.tm_mon+1,now_time.tm_mday);
	char file_path[32];
	memset(file_path, 0, 32);
	sprintf(file_path,"%s%s",LOGFILE_DIR,file_name);
	
	int ret = stat(file_path, &buf);
	if(ret == 0)
	{
		if(buf.st_size >= MAX_LOGFILE_SIZE)
		{
			//清空文件
			if(0 == ftruncate(fileno(pFile), 0))
			{
				fseek(pFile, 0, SEEK_SET);
				fflush(pFile);
			}
			else
			{
				perror("ftruncate error!");
			}
		}
	}
}

int log_print(unsigned int handle, char* pszfmt, ...)
{
	
	va_list struAp;
	time_t now;
	struct tm *ptm_now;
	struct tm stTmNow;
	int ret;

	char format[1024];//= NULL;
	if(strlen(pszfmt)+3 > 1024)return -1;
	memset(format, 0, 1024);
	//if(pszfmt == NULL) return -1;

	//	format = calloc(strlen(pszfmt)+3,1);
	//	if(format == NULL) return -2;
	strcpy(format,pszfmt);
	if(format[strlen(pszfmt)-1]=='\n')
	{
		format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\r' ?'\0':'\r';
	}
	else if(format[strlen(pszfmt)-1]=='\r')
	{
		format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\n' ?'\0':'\n';
	}
	else
	{
		format[strlen(pszfmt)] ='\r';
		format[strlen(pszfmt)+1] ='\n';
	}
	now=time(&now);
	localtime_r(&now, &stTmNow);
	ptm_now = &stTmNow;
	//ptm_now = gmtime(&now);
	if(YELLOW_PRINT == handle)
	{ 	
		printf("[0;33m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_YELLOW_PRINT == handle)
	{ 	
		printf("[1;33m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(RED_PRINT == handle)
	{
		printf("[0;31m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_RED_PRINT == handle)
	{
		printf("[1;31m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(GREEN_PRINT == handle)
	{
		printf("[0;32m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_GREEN_PRINT == handle)
	{
		printf("[1;32m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(BLUE_PRINT == handle)
	{
		printf("[0;34m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_BLUE_PRINT == handle)
	{
		printf("[1;34m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(PURPLE_PRINT == handle)
	{
		printf("[0;35m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_PURPLE_PRINT == handle)
	{
		printf("[1;35m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(WHITE_PRINT == handle)
	{
		printf("[1;37m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(BLACK_PRINT == handle)
	{
		printf("[0;30m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_BLACK_PRINT == handle)
	{
		printf("[1;30m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    (int)syscall(224));
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(GREY_PRINT == handle)
	{
		printf("[0;37m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(CYAN_PRINT == handle)
	{
		printf("[0;36m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(L_CYAN_PRINT == handle)
	{
		printf("[1;36m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");
	}
	else if(NOCOLOR_PRINT == handle)
	{
		printf("[0;39m""[%04d/%02d/%02d %02d:%02d:%02d %d] ",
		    ptm_now->tm_year+1900,
		    ptm_now->tm_mon+1,
		    ptm_now->tm_mday,
		    (ptm_now->tm_hour)%24,
		    ptm_now->tm_min,
		    ptm_now->tm_sec,
		    get_tid());
		va_start(struAp, pszfmt);
		ret = vprintf(format, struAp);
		va_end(struAp);
		printf("[0;39m");		
	}
	else
	{
		FILE *file;  
		file = fopen("log.txt","a+");
		fprintf(file,"[%04d/%02d/%02d %02d:%02d:%02d %d]",
			ptm_now->tm_year+1900,
			ptm_now->tm_mon+1,
			ptm_now->tm_mday,
			(ptm_now->tm_hour)%24,
			ptm_now->tm_min,
			ptm_now->tm_sec,
			get_tid());
		va_start(struAp, pszfmt);
		ret = vfprintf(file,format, struAp);
		va_end(struAp);
		fclose(file);
	}

	static FILE *file = NULL;
	static	pthread_mutex_t tMutex = PTHREAD_MUTEX_INITIALIZER;

	if(0 != access("log.enable", F_OK))//需要建立一个log.enable名字的文件
	{
		if(file != NULL)
		{
			fclose(file);
			file = NULL;
		}
		return 0;
	}

	char file_name[16];
	sprintf(file_name,"%04d%02d%02d.log",ptm_now->tm_year+1900,ptm_now->tm_mon+1,ptm_now->tm_mday);
	char file_path[32];
	//sprintf(file_path,"%s%s",LOGFILE_DIR,file_name);
	sprintf(file_path,"%s",file_name);
	//printf("file_path = %s\n",file_path);
	
	if(0 != access(file_path, F_OK))
	{
		if(file != NULL)
		{
			fclose(file);
			file = NULL;
		}
	}

	if(file == NULL)
	{
		file = fopen(file_path, "a+");
		if(file == NULL)
		{
			printf("open log file failed\n");
		}
	}

	pthread_mutex_lock(&tMutex);
	
	file_check(file);
	
	fprintf(file,"[%04d/%02d/%02d %02d:%02d:%02d %d]",
		ptm_now->tm_year+1900,
		ptm_now->tm_mon+1,
		ptm_now->tm_mday,
		(ptm_now->tm_hour)%24,
		ptm_now->tm_min,
		ptm_now->tm_sec,
		(int)syscall(224));
	char content_write[1024];
	va_start(struAp, pszfmt);
	vfprintf(file,content_write, struAp);
	va_end(struAp);
	fflush(file);
	pthread_mutex_unlock(&tMutex);

//	free(format);
	return ret;
}

//----------------------------------------------------------------------------------

/*int log_vprint(char*pszfmt, ...)
{
	va_list struAp;
	int ret;
	char* format= NULL;
	
	if(pszfmt == NULL) return -1;

	format = calloc(strlen(pszfmt)+3,1);
	if(format == NULL) return -2;
	strcpy(format,pszfmt);
	if(format[strlen(pszfmt)-1]=='\n')
	{
		format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\r' ?'\0':'\r';
	}
	else if(format[strlen(pszfmt)-1]=='\r')
	{
		format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\n' ?'\0':'\n';
	}
	else
	{
		format[strlen(pszfmt)] ='\r';
		format[strlen(pszfmt)+1] ='\n';
	}

	va_start(struAp, pszfmt);
	ret = vprintf(format, struAp);
	va_end(struAp);
	free(format);
	return ret;
}*/


#include "timer.h"
#include "platform.h"
#include "thread.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_TIMER_CNT 20
#define MAX_LEN_TIMER_NAME 16

typedef enum{
	TIMER_STATUS_IDLE,	//空闲
	TIMER_STATUS_RUNNING,	//运行中
	TIMER_STATUS_STOPED,	//停止
	TIMER_STATUS_WAITING_DEL //等待删除
}timer_status_e;
	
typedef struct{
	char name[MAX_LEN_TIMER_NAME];
	timer_status_e status;	
	unsigned int usecond;//超时时间，us微秒
	timer_callback_ptr callbackptr;
	void *param;
	unsigned int ucounted;//已经过的时间,us微秒
}timer_info_t;

typedef struct
{
	BOOL running;
	thread_t thread;
	int iMqHandle;
	Handle mutex;
}thread_group_t;

static timer_info_t timer_info[MAX_TIMER_CNT];

static thread_group_t timer_thread_group;

static void _timer_process()
{
	int i;
	int ticks = 100;
	BOOL ret;

	while(timer_thread_group.running)
	{
		usleep(ticks*1000);
		for (i=0;i<MAX_TIMER_CNT;i++)
		{
			if (timer_info[i].status == TIMER_STATUS_RUNNING && timer_info[i].usecond >= 0)//负值，则永不超时
			{
				timer_info[i].ucounted += ticks;
				if (timer_info[i].ucounted > timer_info[i].usecond)
				{
					ret = timer_info[i].callbackptr(timer_info[i].param);
					mutex_lock(timer_thread_group.mutex);
					if (timer_info[i].status == TIMER_STATUS_RUNNING)
					{
						//这说明，在回调期间，它被reset了，否则肯定不可能为0
						if (timer_info[i].ucounted == 0)
						{
						}
						else
						{
							timer_info[i].ucounted = 0;
							if (!ret)
								timer_info[i].status = TIMER_STATUS_STOPED;
						}
					}
					mutex_unlock(timer_thread_group.mutex);
				}
			}
			else if (timer_info[i].status == TIMER_STATUS_WAITING_DEL)
			{
				timer_info[i].status = TIMER_STATUS_IDLE;
			}
		}
	}
}

/**
 *@brief timer init
 *@return 0 if success
 *
 */
static int timer_init(void)
{
	memset(timer_info, 0, MAX_TIMER_CNT*sizeof(timer_info_t));
	timer_thread_group.running = TRUE;
	timer_thread_group.mutex = mutex_create();
	
	timer_thread_group.thread = thread_create_detached((void *)_timer_process, NULL);

	return 0;
}

/**
 *@brief timer deinit
 *@return 0 if success
 *
 */
int timer_deinit(void)
{
	timer_thread_group.running = FALSE;
	thread_join(timer_thread_group.thread, NULL);
	mutex_delete(timer_thread_group.mutex);
	return 0;
}

int timer_new(const char *name, int millisecond, timer_callback_ptr callback, void *param)
{
	int i;
	static int bInit = FALSE;
	if(!bInit)
	{
		timer_init();
		bInit = TRUE;
	}

	mutex_lock(timer_thread_group.mutex);

	for (i=0;i<MAX_TIMER_CNT;i++)
	{
		if (timer_info[i].status == TIMER_STATUS_IDLE)
		{
			strncpy(timer_info[i].name, name, sizeof(timer_info[i].name)-1);
			timer_info[i].callbackptr = callback;
			timer_info[i].param = param;
			timer_info[i].ucounted = 0;
			timer_info[i].usecond = millisecond;
			timer_info[i].status = TIMER_STATUS_RUNNING;
			break;
		}
	}
		
	mutex_unlock(timer_thread_group.mutex);
	if (i == MAX_TIMER_CNT)
	{
		printf("No free timer to create now...\n");
		return -1;
	}
	return i;
}


/**
 *@brief reset the timer
 *@param id retval of #utl_timer_create
 *@param usecond the new time to delay
 *@return 0 if success
 *
 */
int timer_reset(int id, int usecond, timer_callback_ptr callback, void *param)
{
	if (id < 0 || id > MAX_TIMER_CNT)
	{
		printf("ERROR: Wrong id reseted: %d\n", id);
		return ERR_BAD_PARAM;
	}
	mutex_lock(timer_thread_group.mutex);
	timer_info[id].callbackptr = callback;
	timer_info[id].param = param;
	timer_info[id].ucounted = 0;
	timer_info[id].usecond = usecond;
	timer_info[id].status = TIMER_STATUS_RUNNING;
	mutex_unlock(timer_thread_group.mutex);

	return 0;
}

/**
 *@brief delete the timer
 *@param id retval of #utl_timer_create
 *@retval <0 if failed
 *@retval >=0 id of timer
 *
 */
int timer_destroy(int id)
{
	mutex_lock(timer_thread_group.mutex);
	timer_info[id].status = TIMER_STATUS_WAITING_DEL;
	mutex_unlock(timer_thread_group.mutex);
	return 0;
}


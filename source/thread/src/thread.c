#include "thread.h"
#include <stdlib.h>

//创建分离线程,参数同pthread_create
//注意:忽略了pthread_attr_t参数
int thread_create_detached(void *(*thread_ptr)(void *), void *arg)
{
	pthread_t pid;
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//分离线程
	pthread_attr_setstacksize(&attr, stacksize);					//栈大小
	
	ret = pthread_create(&pid, &attr, thread_ptr, arg);
	
	pthread_attr_destroy (&attr);

	if(ret == 0)
	{
		return (int)pid;
	}
	else
	{
		return ret;
	}
}

//创建线程,栈大小设为LINUX_THREAD_STACK_SIZE。参数同pthread_create
//注意:忽略了pthread_attr_t参数
int thread_create_joinable(void *(*thread_ptr)(void *), void *arg)
{
	pthread_t pid;
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);	//非分离线程
	pthread_attr_setstacksize(&attr, stacksize);					//栈大小
	
	ret = pthread_create(&pid, &attr, thread_ptr, arg);
	
	pthread_attr_destroy (&attr);
	
	if(ret == 0)
	{
		return (int)pid;
	}
	else
	{
		return ret;
	}
}

int thread_join(thread_t thread, void **retval)
{
	return pthread_join(thread, retval);
}

//创建线程,栈大小设为LINUX_THREAD_STACK_SIZE。参数同pthread_create
//优先级设置为51。默认非分离线程
//注意:忽略了pthread_attr_t参数
//当setschedpolicy == SCHED_RR 或者 SCHED_FIFO 时，该接口必须要在超级用户下运行！！！
int thread_create_detached_priority(void *(*thread_ptr)(void *), void *arg)
{
	pthread_t pid;
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	struct sched_param param;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, stacksize);					//栈大小

	param.sched_priority = 51;	//优先级
	pthread_attr_setschedpolicy(&attr,SCHED_RR);
	pthread_attr_setschedparam(&attr,&param);
	pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话

	ret = pthread_create(&pid, &attr, thread_ptr, arg);
	
	pthread_attr_destroy (&attr);
	
	if(ret == 0)
	{
		return (int)pid;
	}
	else
	{
		return ret;
	}
}

Handle mutex_create()
{
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	return mutex;
}

int mutex_delete(Handle mutex)
{
	int ret = pthread_mutex_destroy(mutex);
	if(ret != 0)
	{
		return ret;
	}
	else
	{
		mutex = NULL;
		free(mutex);
		return 0;
	}
}

Handle cond_create()//创建一个条件变量
{
	pthread_cond_t *cond = malloc(sizeof(pthread_cond_t));
	pthread_cond_init(cond, NULL);
	return cond;
}

int cond_delete(void *cond)//销毁一个条件变量
{
	int ret = pthread_cond_destroy(cond);
	if(ret != 0)
	{
		return ret;
	}
	else
	{
		cond = NULL;
		free(cond);
		return 0;
	}
}

int cond_wait(Handle cond, Handle mutex)//等待条件变量成立，使用前后加锁解锁
{
	int ret = pthread_cond_wait(cond, mutex);
	return ret;
}

int cond_activate(Handle cond)//按入队顺序，激活一个等待线程
{
	int ret = pthread_cond_signal(cond);
	return ret;
}

int cond_activate_all(Handle cond)//激活所有等待线程
{
	int ret = pthread_cond_broadcast(cond);
	return ret;
}


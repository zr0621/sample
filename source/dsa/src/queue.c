#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include "queue.h"
#include "platform.h"

//队列信息
typedef struct
{
	sem_t sSem;//信号量
	int iFront,iRear;
	int iCnt;
	int iMsgSize;
	int iQueueSize;
	char **ppQueue;
	char szName[32];
} queue_info_t;

#define MAX_QUEUE_CNT	32
static queue_info_t sQueues[MAX_QUEUE_CNT];//多队列
static int iQueueCnt = 0;//队列数

static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 *@brief 创建一个消息队列
 *@param name 消息队列的名字
 *@param msgsize 每个消息的尺寸
 *@param queuesize 消息队列中能保存的消息的个数
 *@retval >=0 消息队列的handle
 *@retval -1 创建消息队列时，内存分配失败
 *
 */
int queue_create(const char *name, int msgsize, int queuesize)
{
	int i = 0;
	pthread_mutex_lock(&_mutex);
	sQueues[iQueueCnt].iMsgSize = msgsize;
	sQueues[iQueueCnt].iQueueSize = queuesize;

	sQueues[iQueueCnt].ppQueue = (char **)malloc((sizeof(char *)*(sQueues[iQueueCnt].iQueueSize)));
	if(NULL == sQueues[iQueueCnt].ppQueue)
	{
		pthread_mutex_unlock(&_mutex);
		return -1;
	}
	for (i = 0; i < sQueues[iQueueCnt].iQueueSize; i++)
	{
		sQueues[iQueueCnt].ppQueue[i] = (char *)malloc(sQueues[iQueueCnt].iMsgSize);
		memset(sQueues[iQueueCnt].ppQueue[i], 0, sQueues[iQueueCnt].iMsgSize);
		if (NULL == sQueues[iQueueCnt].ppQueue[i])
		{
			int j = 0;
			for (j = 0; j < i ; j++)
			{
				free(sQueues[iQueueCnt].ppQueue[j]);
			}
			free(sQueues[iQueueCnt].ppQueue);
			pthread_mutex_unlock(&_mutex);
			return ERR_NO_RESOURCE;
		}
	}

	strcpy(sQueues[iQueueCnt].szName, name);
	sQueues[iQueueCnt].iFront = 0;
	sQueues[iQueueCnt].iRear = 0;
	sQueues[iQueueCnt].iCnt = 0;
	sem_init(&sQueues[iQueueCnt].sSem, 0, 0);
	pthread_mutex_unlock(&_mutex);
	return 	iQueueCnt++;
}

/**
 *@brief 销毁一个消息队列
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@return 0
 *
 */
int queue_destroy(int handle)
{
	pthread_mutex_lock(&_mutex);
	sem_destroy(&sQueues[handle].sSem);

	int i = 0;
	for (i = 0; i < sQueues[handle].iQueueSize; i++)
	{
		free(sQueues[handle].ppQueue[i]);
	}
	free(sQueues[handle].ppQueue);
	iQueueCnt--;
	pthread_mutex_unlock(&_mutex);
	return 0;
}

/**
 *@brief 发送消息
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 要发送的消息的指针
 *@note 消息的长度在 #queue_create 时已指定
 *@return 0 成功，-1 队满，暂时不能发送
 *
 */
int queue_send(int handle, void *msg)
{
	//pthread_mutex_lock(&sQueues[handle].mutex);
	pthread_mutex_lock(&_mutex);
	if (sQueues[handle].iCnt == sQueues[handle].iQueueSize)
	{
		printf("queue %s is full\n", sQueues[handle].szName);
		pthread_mutex_unlock(&_mutex);
		return ERR_NO_RESOURCE;
	}
	//printf("arch_queue_send, cnt=%d, iFront=%d,iRear=%d\n",sQueues[handle].iCnt, sQueues[handle].iFront,sQueues[handle].iRear);

	memcpy(sQueues[handle].ppQueue[sQueues[handle].iFront], (char *)msg, sQueues[handle].iMsgSize);
	sQueues[handle].iFront = (sQueues[handle].iFront + 1) % (sQueues[handle].iQueueSize);
	(sQueues[handle].iCnt)++;
	pthread_mutex_unlock(&_mutex);
	sem_post(&sQueues[handle].sSem);
	return 0;
}

/**
 *@brief 发送消息到队列头 (调用queue_recv时该消息会被首先取出)
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 要发送的消息的指针
 *@note 消息的长度在 #queue_create 时已指定
 *@return 0 成功，-1 队满，暂时不能发送
 *
 */
int queue_send_at_head(int handle, void *msg)
{
	//pthread_mutex_lock(&sQueues[handle].mutex);
	pthread_mutex_lock(&_mutex);
	if (sQueues[handle].iCnt == sQueues[handle].iQueueSize)
	{
		//printf("the queue %s is full , can't send \n", sQueues[handle].szName);
		pthread_mutex_unlock(&_mutex);
		return ERR_NO_RESOURCE;
	}
	//printf("arch_queue_send_at_head, cnt=%d, iFront=%d,iRear=%d\n",sQueues[handle].iCnt, sQueues[handle].iFront,sQueues[handle].iRear);
	
	int iRear = (sQueues[handle].iRear - 1 + sQueues[handle].iQueueSize) % (sQueues[handle].iQueueSize);
	memcpy(sQueues[handle].ppQueue[iRear], (char *)msg, sQueues[handle].iMsgSize);
	sQueues[handle].iRear = iRear;
	(sQueues[handle].iCnt)++;
	pthread_mutex_unlock(&_mutex);
	sem_post(&sQueues[handle].sSem);
	return 0;
}

#include "string.h"
#include "errno.h"
/**
 *@brief 接收消息
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 输出，用于接收数据的指针
 *@param usecTimeout <0阻塞，0非阻塞，>0超时时间，单位微秒
 *@note 消息的长度在 #queue_create 时已指定
 *@return 0 成功，错误号表示超时
 *
 */
int queue_recv(int handle, void *msg, int usecTimeout)
{
	char * buf = (char *)msg;
	if (usecTimeout < 0)
	{
		sem_wait(&sQueues[handle].sSem);
	}
	else
	{
		int sts = -1;
		while(1)
		{
			sts = sem_trywait(&sQueues[handle].sSem);
			if (sts == 0)
			{
				break;
			}

			if (usecTimeout <= 0)
				break;
			usleep(10*1000);
			usecTimeout -= 10*1000;
		}
		if (-1 == sts)
		{
			return ERR_TIMEOUT;
		}
	}
	pthread_mutex_lock(&_mutex);
	//printf("arch_queue_rec, cnt=%d, iFront=%d,iRear=%d\n",sQueues[handle].iCnt, sQueues[handle].iFront,sQueues[handle].iRear);
	memcpy(buf, sQueues[handle].ppQueue[sQueues[handle].iRear], sQueues[handle].iMsgSize);
	sQueues[handle].iRear = (sQueues[handle].iRear + 1) % (sQueues[handle].iQueueSize); //出队列
	(sQueues[handle].iCnt)--;
	pthread_mutex_unlock(&_mutex);

	return 0;
}

/**
 *@brief 提前看一下下一个消息是啥，不影响消息队列中的内容
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 输出，用于接收数据的指针
 *@return 0 成功，错误号表示暂时没有
 */
int queue_peek(int handle, void *msg)
{
	int ret = ERR_NOT_FIND;
	char * buf = (char *)msg;
	pthread_mutex_lock(&_mutex);
	if (sQueues[handle].iCnt > 0)
	{
		memcpy(buf, sQueues[handle].ppQueue[sQueues[handle].iRear], sQueues[handle].iMsgSize);
		ret = 0;
	}
	pthread_mutex_unlock(&_mutex);

	return ret;
}

/**
 *@brief 获取消息个数
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param out_cntMsg [out]消息个数
 *@param out_maxMsg [out]消息队列最大长度
 *@return 0 成功，-1 出错
 */
int queue_get_count(int handle, int *out_cntMsg, int *out_maxMsg)
{
	*out_cntMsg = sQueues[handle].iCnt;
	*out_maxMsg = sQueues[handle].iQueueSize;
	return 0;
}


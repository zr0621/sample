
#ifndef QUEUE_H
#define QUEUE_H


#ifdef __cplusplus
extern "C"{
#endif


/**
 *@brief 创建一个消息队列
 *@param name 消息队列的名字
 *@param msgsize 每个消息的尺寸
 *@param queuesize 消息队列中能保存的消息的个数
 *@retval >=0 消息队列的handle
 *@retval -1 创建消息队列时，内存分配失败
 *
 */
int queue_create(const char *name, int msgsize, int queuesize);

/**
 *@brief 销毁一个消息队列
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@return 0
 *
 */
int queue_destroy(int handle);

/**
 *@brief 发送消息
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 要发送的消息的指针
 *@note 消息的长度在 #jv_queue_create 时已指定
 *@return 0 成功，-1 队满，暂时不能发送
 *
 */
int queue_send(int handle, void *msg);

/**
 *@brief 发送消息到队列头 (调用queue_recv时该消息会被首先取出)
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 要发送的消息的指针
 *@note 消息的长度在 #queue_create 时已指定
 *@return 0 成功，-1 队满，暂时不能发送
 *
 */
int queue_send_at_head(int handle, void *msg);

/**
 *@brief 接收消息
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 输出，用于接收数据的指针
 *@param usecTimeout <0阻塞，0非阻塞，>0超时时间，单位微秒
 *@note 消息的长度在 #queue_create 时已指定
 *@return 0 成功，错误号表示超时
 *
 */
int queue_recv(int handle, void *msg, int usecTimeout);

/**
 *@brief 提前看一下下一个消息是啥，不影响消息队列中的内容
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param msg 输出，用于接收数据的指针
 *@return 0 成功，错误号表示暂时没有
 */
int queue_peek(int handle, void *msg);

/**
 *@brief 获取消息个数
 *@param handle 消息队列的句柄，其值为#queue_create 的返回值
 *@param out_cntMsg [out]消息个数
 *@param out_maxMsg [out]消息队列最大长度
 *@return 0 成功，-1 出错
 */
int queue_get_count(int handle, int *out_cntMsg, int *out_maxMsg);


#ifdef __cplusplus
}
#endif

#endif


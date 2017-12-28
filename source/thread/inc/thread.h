#ifndef THREAD_H
#define THREAD_H

#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>
#include "platform.h"

#define LINUX_THREAD_STACK_SIZE (512*1024)

//it is unsigned long int actually
typedef pthread_t thread_t;

//sizeof(pthread_mutex_t) is 24byte in 32bit, 40byte in 64bit
typedef pthread_mutex_t mutex_t;

//sizeof(pthread_cond_t) is 24byte in 32bit, 48byte in 64bit
typedef pthread_cond_t cond_t;


//创建分离线程,参数同pthread_create
//如果没有pthread_join，请调用本接口
//注意:忽略了pthread_attr_t参数
int thread_create_detached(void *(*thread_ptr)(void *), void *arg);

//创建普通线程,栈大小设为LINUX_THREAD_STACK_SIZE。参数同pthread_create
//注意:忽略了pthread_attr_t参数
//如果使用本接口，必须有pthread_join对应，否则会有内存泄漏。
int thread_create_joinable(void *(*thread_ptr)(void *), void *arg);
int thread_join(thread_t thread, void **retval);

//创建线程,栈大小设为LINUX_THREAD_STACK_SIZE。参数同pthread_create
//优先级设置为51。默认为分离线程
//注意:忽略了pthread_attr_t参数
int thread_create_detached_priority(void *(*thread_ptr)(void *), void *arg);

Handle mutex_create();

int mutex_delete(Handle mutex);

#define mutex_init(mutex) pthread_mutex_init((pthread_mutex_t *)mutex,NULL)

#define mutex_lock(mutex) pthread_mutex_lock((pthread_mutex_t *)mutex)

#define mutex_unlock(mutex) pthread_mutex_unlock((pthread_mutex_t *)mutex)

Handle cond_create();

int cond_delete(Handle cond);

int cond_wait(Handle cond, Handle mutex);

int cond_activate(Handle cond);

int cond_activate_all(Handle cond);


#ifdef __cplusplus
}
#endif

#endif

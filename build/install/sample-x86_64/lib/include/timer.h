#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"

//不是很准，10s会误差不到1s

/**
 *@brief timer callback type definition
 *@retval TRUE timer will run continue
 *@retval FALSE timer will stop.
 *
 */

typedef BOOL (*timer_callback_ptr)(void* param);

/**
 *@brief timer deinit
 *@return 0 if success
 *
 */
int timer_deinit(void);

/**
 *@brief create one timer
 *@param millisecond time delay, with millisecond.
 *		if millisecond is negative, none timeout will occured
 *@param callback function ptr to call back
 *@param param param of callback 
 *@retval <0 if failed
 *@retval >=0 id of timer
 *
 * 注意!!!!!:
 	1 每过millisecond，callback会被回调一次。
 	2 callback的返回值会影响定时器的运行状态。
 		callback返回TRUE，定时器继续运行。
 		callback返回FALSE, 定时器被停止。reset后才会继续运行
 */
int timer_new(const char *name, int millisecond, timer_callback_ptr callback, void *param);


/**
 *@brief reset the timer
 *@param id retval of #timer_create
 *@param millisecond the new time to delay
 *@return 0 if success
 *
 */
int timer_reset(int id, int millisecond, timer_callback_ptr callback, void *param);

/**
 *@brief delete the timer
 *@param id retval of #utl_timer_create
 *@retval <0 if failed
 *@retval >=0 id of timer
 *
 */
int timer_destroy(int id);

#ifdef __cplusplus
}
#endif

#endif



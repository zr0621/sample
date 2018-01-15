#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "log.h"
#include "dsa.h"
#include "dlist.h"
#include "utils.h"
#include "platform.h"
#include "timer.h"
#include "socket.hpp"
#include "ddc.h"
#include "mem.h"
#include "mppive.h"

//this program is designed to run in the linux system, 
//try to be compatible with 32 bit and 64 bit differences, 
//but not necessarily compatible with other systems! 

static BOOL _test_timer(void *param)
{
	datetime_t datetime;
	get_localtime(&datetime);
	LOG_INFO("datetime : %04d-%02d-%02d %02d:%02d:%02d\n", datetime.year,datetime.month,datetime.day,datetime.hour,datetime.minute,datetime.second);
	return TRUE;
}

static void _timer_start()
{	
	int timer_id1 = timer_new("_test_timer1", 1*1000, _test_timer, NULL);
	int timer_id2 = timer_new("_test_timer2", 1*1000, _test_timer, NULL);
	int timer_id3 = timer_new("_test_timer3", 1*1000, _test_timer, NULL);
	LOG_DEBUG("timer_id1 = %d",timer_id1);
	LOG_DEBUG("timer_id2 = %d",timer_id2);
	LOG_DEBUG("timer_id3 = %d",timer_id3);
	return;
}


int main(int argc, char *argv[])
{
    //print info about this process
	LOG_FATAL("==== this program is complied in %s %s ====",__DATE__,__TIME__);

    //red
    //printf("\33[31m");
	//printf("===========test==========\n"); 
	//printf("\33[0m ");

	return mpp_test();

    return mem_test();

    return ddc_test();
	
	return dlist_test();
	
	/*if (argc != 3)
	{
		LOG_DEBUG("Bad Param! For example: ./sample.bin prefix suffix");
		return ERR_BAD_PARAM;
	}*/
	LOG_TRACE("argc = %d",argc);
	int i = 0;
	for(i=0;i<argc;i++)
	{
		LOG_TRACE("argv[%d] = %s",i,argv[i]);
	}

	_timer_start();

	Socket *ps = new Socket;
	ps->sock_create(INET_IPV4,IPV4_TCP);
	delete ps;

	while(1)
	{
		usleep(1000*1000);
	}
	return SUCCESS;
}


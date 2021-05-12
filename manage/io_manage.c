//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&广播处理&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.c

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//IO检测间隔时间，单位毫秒
#define IO_CHECK_INTERVAL_MS		(50)


static pthread_t io_manage_thread_install_id;


void io_manage_init(void)
{

}

static void* io_manage_thread(void* param)
{	
	printf("io_manage_thread start....\r\n");

	while(1)
	{
		usleep(IO_CHECK_INTERVAL_MS*1000);
		io_check_level_proc();
	}
}
void	io_manage_thread_install(void)
{
	//创建线程
	//pthread_create(&io_manage_thread_install_id, NULL, (void*)io_manage_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&io_manage_thread_install_id,  io_manage_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}





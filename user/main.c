/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-27 11:55:44
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 13:20:17
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 主函数 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: main.c
//*文件作用: 主函数
//*文件作者:  mmaple
//*创建日期: 2012年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "include.h"

static void init(void)
{
	print_init_mutex();
	FiPrint("init...\r\n");

	//系统信息初始化
	sys_init();
	//模块初始化
	module_init();
	//线程初始化
	thread_init();
}
static void uninit()
{
	
	FiPrint("uninit...\r\n");
	print_release_mutex();
}
int main(int argc, char **argv)
{
	printf( "***Start main, compile datetime:%s %s!!!\r\n", __DATE__, __TIME__ );

	init();
	sleep(2);
	while(1)
	{
		//printf("sleep 10s\r\n");
		sleep(1);
	}	
	uninit();

}



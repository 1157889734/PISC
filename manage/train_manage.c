/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-01 09:11:40
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-16 09:08:43
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&广播处理&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.c

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "include.h"

static pthread_t train_manage_thread_install_id;

void train_manage_init(void)
{
}
void train_manage_uninit(void)
{
}

static void* train_manage_thread(void* param)
{	
	static uint8 num_tmp=0;
	printf("train_manage_thread start....\r\n");

	while(1)
	{
		usleep(500*1000);
		num_tmp++;
		//过程数据发送
		pisc_local_send_default_data();
		//获得配置文件的优先级
		broadcast_get_conf_pri();

		//主机
		if(pisc_get_master_status())
		{
			//5秒钟
			if((num_tmp%10)==0)
			{
				//车头屏显示
				head_led_send_content();
				//客室屏显示
				lcu_led_send_content();
				//客室屏滚动速度
				lcu_led_roll_sec();
				//动态地图亮度
				dmp_set_light_level();
				//广播优先级
				//broadcast_send_pri();
				//音量值
				broadcast_send_vol();
				//读取配置文件的越站信息
				pisc_update_jump_stations_from_conf();
			}
			if((num_tmp%2)==0)
			{
				//发送时间
				pisc_local_send_time();
			}
		}
	}

}
void	train_manage_thread_install(void)
{
	//创建线程
	//pthread_create(&train_manage_thread_install_id, NULL, (void*)train_manage_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&train_manage_thread_install_id, train_manage_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}




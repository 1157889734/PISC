/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-27 11:53:07
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-11-24 10:40:40
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:sys.c
//*文件作用:系统模块
//*文件作者:
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "include.h"
#include "../driver/libledgpio.h"


void sys_init(void)
{	
	soundcard_init();
	io_init();
	//led_init();
	gpio_init();
	data_proc_matrix_init();

}
void module_init(void)
{	
	broadcast_init();
	train_manage_init();
	pisc_local_init();	//本地数据初始化,设备号可能在后面的初始化中用到
	dcp_init();
	pisc_init();
	tms_init();	
	dva_init();//DVA设备初始化
	media_init();
	ptu_init();
	occ_init();
	EmerGentInit();
}

void thread_init(void)
{
	//功放心跳处理
	fep_audio_thread_install();
	UdpMultiCast_Init();
	//广播音频网络层初始化
	broadcast_audio_thread_install();
	//总线控制端口
	ctrl_port_thread_install();
	//列车运行过程管理
	train_manage_thread_install();
	//设备状态管理
	dev_status_thread_install();	
	//io电平检测
	io_manage_thread_install();
	//MP3解码
	mp3_decoder_thread_install();
	//广播
	broadcast_thread_install();

}




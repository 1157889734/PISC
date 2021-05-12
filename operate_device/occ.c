/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-09 08:42:30
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-09 09:12:25
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dcp.c

//*文件作用:PA总线移植层

//*文件作者:mmaple

//*创建日期:2012年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include"include.h"
#include "../driver/libledgpio.h"
#define CHANNEL_OCC	(CHANNEL_LEFT)

void occ_init(void)
{
}
void occ_set_occ_status(uint8 flag)
{
	printf("occ_set_occ_status: %d\r\n",flag);

	//led灯显示
	//led_onoff(OCC_LED_BIT,flag);
	led_ctrl(LED4_OCC,flag);

	if(flag)
	{
		broadcast_proc(DEV_TYPE_PISC,pisc_local_get_my_dev_id(),BROADCAST_OCC);	
	}
	else 
	{
		broadcast_stop_proc(BROADCAST_OCC);
	}
}

//发送媒体音频数据
void occ_send_audio(void)
{
	uint16 buf_tmp[AUDIO_CHANNEL_SIZE_SHORT];
	uint16 len_tmp=0;
	//声卡采样
	printf("occ send audio.......\r\n");
	if(BH_TRUE==soundcard_get_data(CHANNEL_OCC,buf_tmp,&len_tmp))
	{
	#if 1
		static uint32 num_tmp=0;	
		num_tmp++;
		if((num_tmp%100)==0)
		{
			printf("occ_send_audio, len_tmp: %d\r\n",len_tmp);
			print_buf((uint8 *)buf_tmp,30);
		}
		#endif	
		broadcast_audio_send_audio(broadcast_get_broadcast_operate_dev_type(),broadcast_get_broadcast_operate_dev_id(),(uint8 *)buf_tmp,len_tmp);
	}
}



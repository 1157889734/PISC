/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-08 15:36:11
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-08 15:45:26
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.c
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"


void dva_init(void)
{	

}


void dva_play_list(uint16 *list, uint16 num)
{
}

void dva_stop(void)
{
}

//发送音频数据
void dva_send_audio(void)
{
#if 1
	uint16 buf_tmp[512];

	//printf("+++++++++++++++++++++dva_send_audio\r\n");
	if(BH_TRUE==mp3_decoder_get_pcm_data(buf_tmp,512))
	{
	  #if 0
		uint16 len_tmp=0;
		static uint32 num_tmp=0;	
		num_tmp++;
		if((num_tmp%100)==0)
		{
			printf("broadcast_audio_send_audio\r\n");
			print_buf(buf_tmp,30);
		}
		#endif
		
		broadcast_audio_send_audio(broadcast_get_broadcast_operate_dev_type(),broadcast_get_broadcast_operate_dev_id(),(uint8 *)buf_tmp,1024);
	}
#endif
}



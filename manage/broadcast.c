//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&广播处理&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.c

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "include.h"
#include "../driver/libledgpio.h"


#define MP3_DECODER	(1) //需要进行MP3解码
#define NO_MP3_DECODER	(0)	//不需要进行mp3解码


//函数声明
static void broadcast_init_dva(void);
static void broadcast_media(void);
static void broadcast_occ(void);
static void broadcast_live(void);
static void broadcast_live_stop(void);
static void broadcast_urgent(void);
static void broadcast_urgent_stop(void);
static void broadcast_close_door(void);
static void broadcast_pre(void);
static void broadcast_arrived(void);
static void broadcast_occ_stop(void);
static void broadcast_closedoor_stop(void);
static void broadcast_pre_stop(void);
static void broadcast_arr_stop(void);
static void broadcast_media_stop(void);


static void broadcast_set_broadcast_mp3_decode_flag(uint8 mp3_decode_flag);
static uint8 broadcast_get_broadcast_mp3_decode_flag(void);


//广播处理
typedef struct
{
	uint8 operate_dev_type;//广播操作设备类型
	uint8 operate_dev_id;//广播操作设备号
	uint8 broadcast_type;//广播类型
	uint8 cycle_flag;
	uint8 broadcast_pri;//广播优先级
	uint8 conf_key[32];		//配置文件的key值
	uint16 play_list_tmp[10];
	uint16 play_list_num;
	uint8 mp3_decode_flag;
	uint8 mp3_file_path[32];
	uint8 lcu_led_file_path[64];
	void (*broadcast_proc)(void);//广播处理
	void (*get_list)(uint16 *,uint8 *, uint16 *);
	void (*send_audio)(void);//广播音频发送
	void (*stop_proc)(void);//广播停止
}broadcast_proc_list_t;
static broadcast_proc_list_t broadcast_proc_list[]=
{
	{DEV_TYPE_PISC,	1,BROADCAST_OCC,0,			PRI_NONE,		"pisc_1_occ",{0},0,NO_MP3_DECODER,"","",	broadcast_occ,NULL,	occ_send_audio,		broadcast_occ_stop},
	{DEV_TYPE_PISC,	2,BROADCAST_OCC,0,		PRI_NONE,		"pisc_2_occ",{0},0,NO_MP3_DECODER,	"","",	broadcast_occ,NULL,		occ_send_audio,		broadcast_occ_stop},
	{DEV_TYPE_DCP,		1,BROADCAST_LIVE,0,			PRI_NONE,		"dcp_1_live",{0},0,	NO_MP3_DECODER,"","",	broadcast_live,NULL,	NULL,		broadcast_live_stop},
	{DEV_TYPE_DCP,		2,BROADCAST_LIVE,	0,		PRI_NONE,		"dcp_2_live",{0},0,NO_MP3_DECODER,	"","",	broadcast_live, NULL,	NULL, 	broadcast_live_stop},
	{DEV_TYPE_DCP,		1,BROADCAST_URGENT,1,		PRI_NONE,		"dcp_1_urgent",{0},0,MP3_DECODER,	MP3_URGENT_PATH,LCU_LED_URGENT_PATH,	broadcast_urgent,dva_broadcast_rule_get_urgent_list, 	dva_send_audio, 	broadcast_urgent_stop},
	{DEV_TYPE_DCP,		2,BROADCAST_URGENT,1,		PRI_NONE,		"dcp_2_urgent",{0},0,	MP3_DECODER,MP3_URGENT_PATH,LCU_LED_URGENT_PATH,	broadcast_urgent,dva_broadcast_rule_get_urgent_list, 	dva_send_audio, 	broadcast_urgent_stop},
	{DEV_TYPE_TMS,		1,BROADCAST_URGENT,1,		PRI_NONE,		"tms_1_urgent",{0},0,MP3_DECODER,	MP3_URGENT_PATH,LCU_LED_URGENT_PATH,	broadcast_urgent,dva_broadcast_rule_get_urgent_list,	dva_send_audio, 	broadcast_urgent_stop},
	{DEV_TYPE_TMS,		2,BROADCAST_URGENT,	1,	PRI_NONE,		"tms_2_urgent",{0},0,	MP3_DECODER,MP3_URGENT_PATH,LCU_LED_URGENT_PATH,broadcast_urgent,dva_broadcast_rule_get_urgent_list,	dva_send_audio, 	broadcast_urgent_stop},
	{DEV_TYPE_TMS,		1,BROADCAST_CLOSE_DOOR,0,		PRI_NONE,"tms_1_closedoor",{0},0,NO_MP3_DECODER,"","",	broadcast_close_door,	NULL,	NULL,	broadcast_closedoor_stop},
	{DEV_TYPE_TMS,		2,BROADCAST_CLOSE_DOOR,0,		PRI_NONE,"tms_2_closedoor",{0},0,NO_MP3_DECODER,"","",	broadcast_close_door, 	NULL, 	NULL,  broadcast_closedoor_stop},
	{DEV_TYPE_PISC,	1,BROADCAST_CLOSE_DOOR,0,		PRI_NONE,"pisc_1_closedoor",{0},0,NO_MP3_DECODER,"","",	broadcast_close_door,	NULL,	NULL,	broadcast_closedoor_stop},
	{DEV_TYPE_PISC,	2,BROADCAST_CLOSE_DOOR,0,		PRI_NONE,"pisc_2_closedoor",{0},0,NO_MP3_DECODER,"","",	broadcast_close_door,	NULL,	NULL,  broadcast_closedoor_stop},
	{DEV_TYPE_DCP,		1,BROADCAST_PRE,0,			PRI_NONE,	"dcp_1_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,	dva_broadcast_rule_get_pre_list,		dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_DCP,		2,BROADCAST_PRE,0,			PRI_NONE,	"dcp_2_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,	dva_broadcast_rule_get_pre_list,		dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_DCP,		1,BROADCAST_ARRIVE,0,		PRI_NONE,	"dcp_1_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_DCP,		2,BROADCAST_ARRIVE,0,		PRI_NONE,	"dcp_2_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_TMS,		1,BROADCAST_PRE,	0,		PRI_NONE,	"tms_1_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,	dva_broadcast_rule_get_pre_list,		dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_TMS,		2,BROADCAST_PRE,	0,		PRI_NONE,	"tms_2_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,	dva_broadcast_rule_get_pre_list,		dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_TMS,		1,BROADCAST_ARRIVE,0,		PRI_NONE,	"tms_1_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_TMS,		2,BROADCAST_ARRIVE,	0,	PRI_NONE,	"tms_2_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_PISC,	1,BROADCAST_PRE,	0,		PRI_NONE,	"pisc_1_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,		dva_broadcast_rule_get_pre_list,	dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_PISC,	2,BROADCAST_PRE,	0,		PRI_NONE,	"pisc_2_pre",{0},0,MP3_DECODER,MP3_PRE_PATH,LCU_LED_PRE_PATH,	broadcast_pre,		dva_broadcast_rule_get_pre_list,		dva_send_audio, 	broadcast_pre_stop},
	{DEV_TYPE_PISC,	1,BROADCAST_ARRIVE,0,		PRI_NONE,	"pisc_1_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_PISC,	2,BROADCAST_ARRIVE,0,		PRI_NONE,	"pisc_2_arr",{0},0,MP3_DECODER,MP3_ARR_PATH,LCU_LED_ARR_PATH,	broadcast_arrived,dva_broadcast_rule_get_arr_list,	dva_send_audio, 	broadcast_arr_stop},	
	{DEV_TYPE_PISC,	1,BROADCAST_MEDIA,	0,	PRI_NONE,	"pisc_1_media",{0},0,NO_MP3_DECODER,"","",	broadcast_media,	NULL,	media_send_audio,	broadcast_media_stop},	
	{DEV_TYPE_PISC,	2,BROADCAST_MEDIA,0,		PRI_NONE,	"pisc_2_media",{0},0,NO_MP3_DECODER,"","",	broadcast_media,	NULL,	media_send_audio,	broadcast_media_stop},	


	{0,0,0,0,PRI_NONE,"",{0},0,NO_MP3_DECODER,"","",NULL,NULL,NULL,NULL,},
};

typedef struct
{
	uint8 operate_dev_type;
	uint8 operate_dev_id;
	uint8 broadcast_type;
	uint8 mp3_decode_flag;
	uint8 cycle_flag;
	uint8 urgent_no;
	uint16 playlist[8];
	uint8 playlist_len;
}broadcast_t;

static broadcast_t broadcast_current={0,0,BROADCAST_NONE,NO_MP3_DECODER,0,0,{0},0};


//广播优先级
typedef struct
{
	uint8 pisc_1_occ;
	uint8 pisc_2_occ;
	uint8 dcp_1_live;
	uint8 dcp_2_live;
	uint8 dcp_1_urgent;
	uint8 dcp_2_urgent;
	uint8 tms_1_urgent;
	uint8 tms_2_urgent;
	uint8 tms_1_closedoor;
	uint8 tms_2_closedoor;
	uint8 pisc_1_closedoor;
	uint8 pisc_2_closedoor;	
	uint8 dcp_1_pre;
	uint8 dcp_2_pre;	
	uint8 dcp_1_arr;
	uint8 dcp_2_arr;		
	uint8 tms_1_pre;
	uint8 tms_2_pre;	
	uint8 tms_1_arr;
	uint8 tms_2_arr;	
	uint8 pisc_1_pre;
	uint8 pisc_2_pre;	
	uint8 pisc_1_arr;
	uint8 pisc_2_arr;	
	uint8 media_1;
	uint8 media_2;	
	uint8 reserve[4];
}broadcast_priority_t;
static broadcast_priority_t broadcast_priority=
{
	1,
	2,
};
//广播音量
typedef struct
{
	uint8 dcp_live;
	uint8 dcp_monitor;
	uint8 dcp_ehp_monitor;
	uint8 occ;
	uint8 media;
	uint8 powamp[16];
	uint8 ehp_talk;
	uint8 ehp_monitor;
	uint8 dva;
	uint8 reserve[6];
}broadcast_vol_t;
static broadcast_vol_t broadcast_vol={0};


void broadcast_init(void)
{
	//获得配置文件的优先级
	broadcast_get_conf_pri();

	broadcast_init_dva();

#if 0
	//灭灯
	led_onoff(MP3_LED_BIT,0);	
	led_onoff(MEDIA_LED_BIT,0);			
	led_onoff(MIC_LED_BIT,0);	
#endif
    led_ctrl(LED9_MP3,0);
    led_ctrl(LED8_MEDIA,0);
	led_ctrl(LED10_MIC,0);
}



void broadcast_proc(uint8 operate_dev_type, uint8 operate_dev_id,uint8 pisc_broadcast_type)
{
	uint8 i=0;
	uint8 broadcast_pri_tmp=0;
	//uint16 play_file_path_list[FILE_LIST_NUM_MAX][FILE_NAME_LENGTH_MAX]={{0}};
	uint8 play_file_path_list[FILE_LIST_NUM_MAX][FILE_NAME_LENGTH_MAX]={{0}};
	uint16 play_list_tmp[FILE_LIST_NUM_MAX]={0};
	uint8 language_list_tmp[FILE_LIST_NUM_MAX]={0};
	uint16 play_list_num=0;
	
	broadcast_proc_list_t const *list=broadcast_proc_list;	
	//当前广播优先级
	broadcast_pri_tmp=broadcast_get_pri(operate_dev_type,operate_dev_id,broadcast_get_broadcast_type());
	printf("broadcast_proc,broadcast_pri_tmp: %d...\r\n",broadcast_pri_tmp);
	while(list->broadcast_type)
	{
		if(list->operate_dev_type==operate_dev_type
			&& list->operate_dev_id==operate_dev_id
			&& list->broadcast_type==pisc_broadcast_type)
		{
			printf("list->broadcast_pri: %d-------->file_path:%s\r\n",list->broadcast_pri,list->lcu_led_file_path);
			if(list->broadcast_pri<=broadcast_pri_tmp||pisc_broadcast_type==BROADCAST_NONE)
			{
				broadcast_stop_proc(broadcast_get_broadcast_type());
				broadcast_set_broadcast_type(pisc_broadcast_type);
				broadcast_set_broadcast_operate_dev_type(operate_dev_type);
				broadcast_set_broadcast_operate_dev_id(operate_dev_id);
				broadcast_set_broadcast_cycle_flag(list->cycle_flag);
				broadcast_set_broadcast_mp3_decode_flag(list->mp3_decode_flag);

				//有播放列表
				if(list->get_list)
				{
					//获取播放列表，包括音频和客室屏拼接显示
					list->get_list(play_list_tmp,language_list_tmp,&play_list_num);
					if(MP3_DECODER==list->mp3_decode_flag)
					{
						printf("list->mp3_file_path: %s\r\n",list->mp3_file_path);
						printf("****play_list_num: %d\r\n",play_list_num);

					
						for(i=0;i<play_list_num;i++)
						{
							sprintf((char *)play_file_path_list[i],"%s/%d/%03d.mp3",list->mp3_file_path,language_list_tmp[i],play_list_tmp[i]);
							printf("play_file_path_list[%d]: %s\r\n",i,play_file_path_list[i]);
						}	

						
						//播放音频
						mp3_decoder_play_list(CHANNEL_LEFT,(uint8 *)play_file_path_list,play_list_num*2);
						printf("begin sleep 1s\r\n");
						//等待，很重要
						sleep(1);
						printf("sleep 1s\r\n");
						//启动播放线程
						mp3_decoder_set_run_flag(1);	
						
						//亮mp3灯
						//led_onoff(MP3_LED_BIT,1);	
						led_ctrl(LED9_MP3,1);					
					}

					//客室中文显示设置
					lcu_led_set_content((uint8 *)list->lcu_led_file_path,(uint16 *)play_list_tmp,play_list_num);
					//客室屏显示内容发送
					lcu_led_send_content();
				}
				//广播
				list->broadcast_proc();	
			}
			break;
		}
		list++;
	}
}

void broadcast_stop_proc(uint8 pisc_broadcast_type)
{
	broadcast_proc_list_t const *list=broadcast_proc_list;	
	printf("***broadcast_stop_proc: %d\r\n",pisc_broadcast_type);
	while(list->broadcast_type)
	{
		if(list->broadcast_type==pisc_broadcast_type)
		{
			if(list->mp3_decode_flag)
			{
				mp3_decoder_stop_play(CHANNEL_LEFT);
				//mp3灯
				//led_onoff(MP3_LED_BIT,0);	
				led_ctrl(LED9_MP3,0);			
			}
			if(list->cycle_flag)
			{
				broadcast_set_broadcast_cycle_flag(0);
			}
			
			list->stop_proc();	
			broadcast_set_broadcast_type(BROADCAST_NONE);
			break;
		}
		list++;
	}
}
static void broadcast_init_dva(void)
{
#if 0
	uint16 play_list_tmp[4]={0};
	play_list_tmp[0]=9999;

	//播放测试 音频
	mp3_decoder_play_list(CHANNEL_LEFT,play_list_tmp, 1);
	mp3_decoder_set_run_flag(1);
#endif	
}

//发送优先级
void broadcast_send_pri(void)
{
	broadcast_priority.dcp_1_arr=broadcast_get_pri(DEV_TYPE_DCP,1,BROADCAST_ARRIVE);
	broadcast_priority.dcp_2_arr=broadcast_get_pri(DEV_TYPE_DCP,2,BROADCAST_ARRIVE);
	broadcast_priority.dcp_1_live=broadcast_get_pri(DEV_TYPE_DCP,1,BROADCAST_LIVE);
	broadcast_priority.dcp_2_live=broadcast_get_pri(DEV_TYPE_DCP,2,BROADCAST_LIVE);
	broadcast_priority.dcp_1_pre=broadcast_get_pri(DEV_TYPE_DCP,1,BROADCAST_PRE);
	broadcast_priority.dcp_1_pre=broadcast_get_pri(DEV_TYPE_DCP,2,BROADCAST_PRE);
	broadcast_priority.dcp_1_urgent=broadcast_get_pri(DEV_TYPE_DCP,1,BROADCAST_URGENT);
	broadcast_priority.dcp_2_urgent=broadcast_get_pri(DEV_TYPE_DCP,2,BROADCAST_URGENT);

	broadcast_priority.tms_1_arr=broadcast_get_pri(DEV_TYPE_TMS,1,BROADCAST_ARRIVE);
	broadcast_priority.tms_2_arr=broadcast_get_pri(DEV_TYPE_TMS,2,BROADCAST_ARRIVE);
	broadcast_priority.tms_1_pre=broadcast_get_pri(DEV_TYPE_TMS,1,BROADCAST_PRE);
	broadcast_priority.tms_2_pre=broadcast_get_pri(DEV_TYPE_TMS,2,BROADCAST_PRE);
	broadcast_priority.tms_1_urgent=broadcast_get_pri(DEV_TYPE_TMS,1,BROADCAST_URGENT);
	broadcast_priority.tms_2_urgent=broadcast_get_pri(DEV_TYPE_TMS,2,BROADCAST_URGENT);
	broadcast_priority.tms_1_closedoor=broadcast_get_pri(DEV_TYPE_TMS,1,BROADCAST_CLOSE_DOOR);
	broadcast_priority.tms_2_closedoor=broadcast_get_pri(DEV_TYPE_TMS,2,BROADCAST_CLOSE_DOOR);

	broadcast_priority.pisc_1_arr=broadcast_get_pri(DEV_TYPE_PISC,1,BROADCAST_ARRIVE);
	broadcast_priority.pisc_2_arr=broadcast_get_pri(DEV_TYPE_PISC,2,BROADCAST_ARRIVE);
	broadcast_priority.pisc_1_pre=broadcast_get_pri(DEV_TYPE_PISC,1,BROADCAST_PRE);
	broadcast_priority.pisc_2_pre=broadcast_get_pri(DEV_TYPE_PISC,2,BROADCAST_PRE);
	broadcast_priority.pisc_1_closedoor=broadcast_get_pri(DEV_TYPE_PISC,1,BROADCAST_CLOSE_DOOR);
	broadcast_priority.pisc_2_closedoor=broadcast_get_pri(DEV_TYPE_PISC,2,BROADCAST_CLOSE_DOOR);
	broadcast_priority.pisc_1_occ=broadcast_get_pri(DEV_TYPE_PISC,1,BROADCAST_OCC);
	broadcast_priority.pisc_2_occ=broadcast_get_pri(DEV_TYPE_PISC,2,BROADCAST_OCC);

	broadcast_priority.media_1=broadcast_get_pri(DEV_TYPE_PISC,1,BROADCAST_MEDIA);
	broadcast_priority.media_2=broadcast_get_pri(DEV_TYPE_PISC,2,BROADCAST_MEDIA);

	ctrl_port_send_cmd(0xffff,0xff,0xff,BROADCAST_SET_PRI,(uint8 *)&broadcast_priority,sizeof(broadcast_priority_t));
}

void broadcast_send_vol(void)
{
	uint8 i=0;
	int8 str_tmp[32]="";

	//功放音量
	for(i=0;i<16;i++)
	{
		sprintf((char *)str_tmp,"powamp_%d_vol",i+1);
		if(GetValueFromEtcFile(PISC_CONF_PATH,"powamp",(const char*)str_tmp,(char *)str_tmp,sizeof(str_tmp)-1)==0)
		{
			broadcast_vol.powamp[i]=atoi((char *)str_tmp);
		}			
	}
	
	//控制盒
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dcp","live_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		//口播音量
		broadcast_vol.dcp_live=atoi((char *)str_tmp);
	}			
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dcp","broadcast_monitor_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		//口播监听音量
		broadcast_vol.dcp_monitor=atoi((char *)str_tmp);
	}	
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dcp","talk_monitor_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		//对讲监听音量
		broadcast_vol.dcp_ehp_monitor=atoi((char *)str_tmp);
	}	
	
	//报警器
	if(GetValueFromEtcFile(PISC_CONF_PATH,"ehp","talk_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		//话筒音量
		broadcast_vol.ehp_talk=atoi((char *)str_tmp);
	}		
	if(GetValueFromEtcFile(PISC_CONF_PATH,"ehp","talk_monitor_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		//对讲监听音量
		broadcast_vol.ehp_monitor=atoi((char *)str_tmp);
	}	

	//occ
	if(GetValueFromEtcFile(PISC_CONF_PATH,"occ","occ_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		broadcast_vol.occ=atoi((char *)str_tmp);
	}	
	//dva
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dva","dva_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		broadcast_vol.dva=atoi((char *)str_tmp);
	}		
	//media
	if(GetValueFromEtcFile(PISC_CONF_PATH,"media","media_volume",(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		broadcast_vol.media=atoi((char *)str_tmp);
	}	
	ctrl_port_send_cmd(0xffff,0xff,0xff,BROADCAST_SET_VOL,(uint8 *)&broadcast_vol,sizeof(broadcast_vol_t));
}

void broadcast_get_conf_pri(void)
{
	int8 str_tmp[32];
	uint8 i=0;
	//初始化优先级
	while(broadcast_proc_list[i].broadcast_type)
	{
		if(GetValueFromEtcFile(PISC_CONF_PATH,"priority",(const char *)broadcast_proc_list[i].conf_key,(char *)str_tmp,sizeof(str_tmp)-1)==0)
		{
			broadcast_proc_list[i].broadcast_pri=atoi((char *)str_tmp);
		}
		i++;
	}
}

extern int broad_record_music;
void broadcast_send_audio(void)
{
	broadcast_proc_list_t const *list=broadcast_proc_list;	
	//printf("broadcast_get_broadcast_type(): %d\r\n",broadcast_get_broadcast_type());
    broad_record_music = 0x01;
	while(list->broadcast_type)
	{
		if(list->broadcast_type==broadcast_get_broadcast_type())
		{
			list->send_audio();	
			broad_record_music = 0x02;
			break;
		}
		list++;
	}	
	if(broad_record_music == 0x01)
	{
		//printf("发送音频....\r\n");
		//发送音频
		audio_broadcast_send();
	}
}

static void broadcast_media(void)
{
}
static void broadcast_media_stop(void)
{
}

static void broadcast_occ(void)
{
	
}

static void broadcast_occ_stop(void)
{
}

void broadcast_set_urgent_no(uint8 urgent_no)
{
	broadcast_current.urgent_no=urgent_no;
}
uint8 broadcast_get_urgent_no(void)
{
	return broadcast_current.urgent_no;
}

static void broadcast_live(void)
{
	printf("broadcast_live\r\n");
	//led_onoff(MIC_LED_BIT-1,1);
}
static void broadcast_live_stop(void)
{
	//led_onoff(MIC_LED_BIT-1,0);
}
static void broadcast_urgent(void)
{

}
static void broadcast_urgent_stop(void)
{
}
static void broadcast_close_door(void)
{
}
static void broadcast_pre(void)
{
}
static void broadcast_pre_stop(void)
{	

}
static void broadcast_arrived(void)
{


}
static void broadcast_arr_stop(void)
{	
}

static void broadcast_closedoor_stop(void)
{	
}

uint8 broadcast_get_pri(uint8 operate_dev_type, uint8 operate_dev_id,uint8 pisc_broadcast_c)
{
	broadcast_proc_list_t const *list=broadcast_proc_list;	
	while(list->operate_dev_type)
	{
		if(list->operate_dev_type==operate_dev_type
			&& list->operate_dev_id==operate_dev_id
			&& list->broadcast_type==pisc_broadcast_c)
		{
			return list->broadcast_pri;
		}
		list++;
	}
	return 0xff;
}

void broadcast_set_broadcast_type(uint8 pisc_broadcast_c)
{
	if(broadcast_current.broadcast_type!=pisc_broadcast_c)
	{
		broadcast_current.broadcast_type=pisc_broadcast_c;
		printf("pisc_broadcast: 0x%x\r\n",broadcast_current.broadcast_type);
	}
}
uint8 broadcast_get_broadcast_type(void)
{
	return broadcast_current.broadcast_type;
}
void broadcast_set_broadcast_operate_dev_type(uint8 operate_dev_type)
{
	if(broadcast_current.operate_dev_type!=operate_dev_type)
	{
		broadcast_current.operate_dev_type=operate_dev_type;
		printf("operate_dev_type: 0x%x\r\n",broadcast_current.operate_dev_type);
	}
}
uint8 broadcast_get_broadcast_operate_dev_type(void)
{
	return broadcast_current.operate_dev_type;
}
void broadcast_set_broadcast_operate_dev_id(uint8 operate_dev_id)
{
	if(broadcast_current.operate_dev_id!=operate_dev_id)
	{
		broadcast_current.operate_dev_id=operate_dev_id;
		printf("operate_dev_id: 0x%x\r\n",broadcast_current.operate_dev_id);
	}
}
uint8 broadcast_get_broadcast_operate_dev_id(void)
{
	return broadcast_current.operate_dev_id;
}

void broadcast_set_broadcast_cycle_flag(uint8 cycle_flag)
{
	if(broadcast_current.cycle_flag!=cycle_flag)
	{
		broadcast_current.cycle_flag=cycle_flag;
		printf("cycle_flag: 0x%x\r\n",broadcast_current.cycle_flag);
	}
}
uint8 broadcast_get_broadcast_cycle_flag(void)
{
	return broadcast_current.cycle_flag;
}

static void broadcast_set_broadcast_mp3_decode_flag(uint8 mp3_decode_flag)
{
	if(broadcast_current.mp3_decode_flag!=mp3_decode_flag)
	{
		broadcast_current.mp3_decode_flag=mp3_decode_flag;
		printf("mp3_decode_flag: 0x%x\r\n",broadcast_current.mp3_decode_flag);
	}
}
static uint8 broadcast_get_broadcast_mp3_decode_flag(void)
{
	return broadcast_current.mp3_decode_flag;
}


static pthread_t broadcast_thread_id=-1;//线程id
static void* broadcast_thread(void* param)
{	
	printf("broadcast_thread start....\r\n");

	pthread_detach(pthread_self());	//by zhq 20100527	


	while(1)
	{
		//主
		if(pisc_get_master_status())
		{
			//当前正在播放MP3解码广播
			if(broadcast_get_broadcast_mp3_decode_flag())
			{
				//当前播放完毕
				if(mp3_decode_get_is_playing()==0)
				{
					//如果有循环标志
					if(broadcast_get_broadcast_cycle_flag())
					{
						//重复播放
						broadcast_proc(broadcast_get_broadcast_operate_dev_type(),broadcast_get_broadcast_operate_dev_id(),broadcast_get_broadcast_type());
					}
					//没有循环标志
					else
					{
						if(BROADCAST_NONE!=broadcast_get_broadcast_type())
						{
							//停止广播
							broadcast_stop_proc(broadcast_get_broadcast_type());
						}
					}
				
				}
			}
		}
		sleep(1);
	}
}

void	broadcast_thread_install(void)
{
	//创建线程
	//pthread_create(&broadcast_thread_id, NULL, (void*)broadcast_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&broadcast_thread_id, broadcast_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}



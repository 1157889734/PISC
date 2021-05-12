//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车总控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tms.c

//*文件作用:列车总控制器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"


//tms上下行 
#define TMS_RUNDIR_UP		(1) 
#define TMS_RUNDIR_DOWN	(2)


//开关门 左右侧
#define TMS_DOOR_LEFT 1
#define TMS_DOOR_RIGHT 2
#define TMS_DOOR_BOTH_SIDE 3

//tms设置时间
static void tms_set_time(tba_bus_pack_t *ReceiveMsg);
//门关好
static void tms_set_close_door(tba_bus_pack_t  *ReceiveMsg);
//tms设置预到站触发
static void tms_set_pre_trigger(tba_bus_pack_t *ReceiveMsg);
//tms设置到站触发
static void tms_set_arr_trigger(tba_bus_pack_t *ReceiveMsg);
//tms设置到站触发
static void tms_stop_broadcast(tba_bus_pack_t *ReceiveMsg);
//接收到一包tms数据的处理
static void tms_recv_pack_proc(uint8  *buf);
//tms设置站点信息
static void tms_set_station_info(tba_bus_pack_t *ReceiveMsg);
//tms设置上下行
static void tms_set_dir(tba_bus_pack_t *ReceiveMsg);
//tms设置紧急广播触发标志
static void tms_emergency_trigger(tba_bus_pack_t *ReceiveMsg);
//tms设置循环广播触发
static void tms_cycle_broadcast_trigger(tba_bus_pack_t *ReceiveMsg);


static dev_proc_t tms_proc_table_M[] = 
{
	{TAB_BUS_PACK_HEAD_LEN+13,		0,		BIT_16,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//year
	{TAB_BUS_PACK_HEAD_LEN+15,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//mon
	{TAB_BUS_PACK_HEAD_LEN+16,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//day
	{TAB_BUS_PACK_HEAD_LEN+17,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//hour
	{TAB_BUS_PACK_HEAD_LEN+18,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//min
	{TAB_BUS_PACK_HEAD_LEN+19,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_time				},	//sec

	{TAB_BUS_PACK_HEAD_LEN+21,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_close_door			},	//

	{TAB_BUS_PACK_HEAD_LEN+3,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_dir				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+4,		0,		BIT_8,	WHATEVER,	0,	(MsgHandler_Callback)tms_set_station_info					},	//下一站变更
	{TAB_BUS_PACK_HEAD_LEN+5,		0,		BIT_8,	WHATEVER,	0,	(MsgHandler_Callback)tms_set_station_info					},	//下一站变更
	{TAB_BUS_PACK_HEAD_LEN+6,		0,		BIT_8,	WHATEVER,	0,	(MsgHandler_Callback)tms_set_station_info					},	//起始站变更
	{TAB_BUS_PACK_HEAD_LEN+7,		0,		BIT_8,	WHATEVER,	0,	(MsgHandler_Callback)tms_set_station_info					},	//起始站变更
	
	{TAB_BUS_PACK_HEAD_LEN+29,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_pre_trigger					},	//当前站变更
#if 1	
	{TAB_BUS_PACK_HEAD_LEN+30,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_set_arr_trigger					},	//当前站变更
#else
	{TAB_BUS_PACK_HEAD_LEN+30,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_stop_broadcast					},	//当前站变更
#endif	
	{TAB_BUS_PACK_HEAD_LEN+31,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_stop_broadcast					},	//当前站变更


	{TAB_BUS_PACK_HEAD_LEN+34,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_emergency_trigger				},	//紧急广播触发
	{TAB_BUS_PACK_HEAD_LEN+37,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)tms_cycle_broadcast_trigger				},	//紧急广播触发

	{TAB_BUS_PACK_HEAD_LEN, 		0,		BIT_4,	WHATEVER,	0,	(MsgHandler_Callback)tms_recv_pack_proc					},	


	{0,	0,	0,	0,		0,	NULL													}
};



//主状态处理矩阵
static dev_proc_t tms_proc_table_S[] = 
{	
	{TAB_BUS_PACK_HEAD_LEN, 		0,		BIT_4,	WHATEVER,		0,	(MsgHandler_Callback)tms_recv_pack_proc					},	

	{0,	0,	0,	0,		0,	NULL													}
};

//状态基索引
static mode_matrix_t  tms_process_table[]=
{
	{PISC_STATUS_M,  tms_proc_table_M,},			//主状态处理矩阵
	{PISC_STATUS_S,  tms_proc_table_S,},		//主状态处理矩阵
	
	{0, NULL,},
};

//状态基索引
static cmd_matrix_t  tms_cmd_process_table[]=
{
	{CTRL_CMD_DEFAULT,  tms_process_table,},			//主状态处理矩阵
	
	{0, NULL,},
};

struct//22-31
{
	uint8 pisc_error:1; //下行		
	uint8 dcp_error:1; //下行		
	uint8 head_led_error:1; //下行		
	uint8 recorder_error:1;
	uint8 recon_error:1;
	uint8 reserve:3;
}__attribute((packed))cab_dev_error_t;		

typedef struct
{
	uint8 ehp_1_error:1; //下行		
	uint8 ehp_2_error:1; //下行				
	uint8 ehp_3_error:1; //下行		
	uint8 reserve:1;
	uint8 lcu_led_1_error:1; //下行			
	uint8 lcu_led_2_error:1; //下行		
	uint8 reserve_2:2;

	uint8 powamp_1_error:1;
	uint8 powamp_2_error:1	;
	uint8 reserve_3:2;
	uint8 noise_det_1_error:1;
	uint8 noise_det_2_error:1	;
	uint8 reserve_4:2;
	
	uint8 dmp_1_error:1;
	uint8 dmp_2_error:1;
	uint8 dmp_3_error:1;
	uint8 dmp_4_error:1;
	uint8 dmp_5_error:1;
	uint8 dmp_6_error:1;
	uint8 dmp_7_error:1;
	uint8 dmp_8_error:1;
}lcu_dev_error_t;

//发送数据包结构
typedef struct
{
	//cab_dev_error_t cab[CAB_NUM_MAX];
	lcu_dev_error_t lcu[LCU_NUM_MAX];
}__attribute((packed))tms_send_pack_t;

//预到站播放标志
static uint8 pre_broadcast_flag=0;
//到站播放标志
static uint8 arr_broadcast_flag=0;

void tms_init(void)
{
	//初始化设备有效性
	dev_status_init_dev_valid(DEV_TYPE_TMS);
	//添加设备到处理状态矩阵
	printf("tms init...\r\n");
	data_proc_matrix_add_device_to_matrix_table(DEV_TYPE_TMS, tms_cmd_process_table);	 
}
static void tms_set_close_door(tba_bus_pack_t  *ReceiveMsg)
{
	tms_recv_pack_t *ptr_tmp = (tms_recv_pack_t*)ReceiveMsg ->app_data;
	printf("ptr_tmp->doorside: %d\r\n",ptr_tmp->doorside);
	if(pisc_get_master_status()!=MASTER_STATUS)return;

	if(ptr_tmp->close_door_well)
	{
		pisc_set_open_left_door_status(0);
		pisc_set_open_right_door_status(0);
	}		
	
}

static void tms_set_station_info(tba_bus_pack_t *ReceiveMsg)
{
	station_info_t station_info;
	uint8 station_change_flag=0;
	uint8 end_station_change_flag=0;
	tms_recv_pack_t *ptr_tmp = (tms_recv_pack_t*)ReceiveMsg ->app_data;
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(pisc_get_master_status()!=MASTER_STATUS)return;
	pisc_get_station_info(&station_info);
	//站点改变，赋值
	if(station_info.start_station!=ptr_tmp->start_station)
	{
		station_info.start_station=ptr_tmp->start_station;
		station_change_flag=1;
	}
	if(station_info.end_station!=ptr_tmp->end_station)
	{
		station_info.end_station=ptr_tmp->end_station;
		station_change_flag=1;
		end_station_change_flag=1;
	}
	if(station_info.next_station!=ptr_tmp->next_station)
	{
		station_info.next_station=ptr_tmp->next_station;
		station_change_flag=1;
		//清开门侧
		
	}
	if(station_info.current_station!=ptr_tmp->current_station)
	{
		station_info.current_station=ptr_tmp->current_station;
		station_change_flag=1;
	}
	if(station_change_flag)
	{
		//填充站点信息
		pisc_set_station_info(station_info);

		if(end_station_change_flag)
		{
			head_led_set_content(LANGUAGE_C);
			head_led_set_content(LANGUAGE_E);
			//车头屏显示
			head_led_send_content();
			
		}
		
		//清标志
		pre_broadcast_flag=BH_FALSE;
		arr_broadcast_flag=BH_FALSE;
	}
}

static void tms_set_dir(tba_bus_pack_t *ReceiveMsg)
{
	tms_recv_pack_t *ptr_tmp = (tms_recv_pack_t*)ReceiveMsg ->app_data;
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(pisc_get_master_status()!=MASTER_STATUS)return;

	if(TMS_RUNDIR_UP==ptr_tmp->dir)pisc_set_updown(PISC_DIR_UP);
	else pisc_set_updown(PISC_DIR_DOWN);
}
static void tms_emergency_trigger(tba_bus_pack_t *ReceiveMsg)
{
	uint16 urgent_id_tmp=0;
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t *)ReceiveMsg->app_data;
	
	printf("tms_recv_pack_p->urgent_flag: %d, urgent_id_tmp: %d\r\n",tms_recv_pack_p->urgent_flag,tms_recv_pack_p->urgent_msg_id);
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(pisc_get_master_status()!=MASTER_STATUS)return;
	urgent_id_tmp=tms_recv_pack_p->urgent_msg_id;


	if(!tms_recv_pack_p->urgent_flag)
	{
		if(BROADCAST_URGENT==broadcast_get_broadcast_type())broadcast_stop_proc(BROADCAST_URGENT);
	}
	if(tms_recv_pack_p->urgent_flag)
	{
		if(urgent_id_tmp)
		{
			broadcast_set_urgent_no(urgent_id_tmp);
			broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_URGENT);
		}
	}
		
}

static void tms_set_time(tba_bus_pack_t *ReceiveMsg)
{
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t  *)ReceiveMsg->app_data;
	pisc_local_set_time((uint8 *)&tms_recv_pack_p->tms_time,sizeof(tms_time_t));
}
static void tms_set_pre_trigger(tba_bus_pack_t *ReceiveMsg)
{
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t	*)ReceiveMsg->app_data;
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(pisc_get_master_status()!=MASTER_STATUS)return;
	printf("****tms_recv_pack_p->pre_flag: %d\r\n",tms_recv_pack_p->pre_flag);
	printf("ReceiveMsg->src_dev_type: %d, src_dev_id: %d\r\n",ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id);
	#if 1
	//预到站触发
	if(tms_recv_pack_p->pre_flag)
	{
		//预到站广播
		//broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_PRE); 	
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		pisc_set_pre_flag(1);
		pisc_set_arr_flag(0);

		if(TMS_DOOR_LEFT==tms_recv_pack_p->doorside)
		{
			pisc_set_open_left_door_status(1);
			pisc_set_open_right_door_status(0);
		}
		else if(TMS_DOOR_RIGHT==tms_recv_pack_p->doorside)
		{
			pisc_set_open_left_door_status(0);
			pisc_set_open_right_door_status(1);
		}
		else if(TMS_DOOR_BOTH_SIDE==tms_recv_pack_p->doorside)
		{
			pisc_set_open_left_door_status(1);
			pisc_set_open_right_door_status(1);
		}	
		else
		{
			pisc_set_open_left_door_status(0);
			pisc_set_open_right_door_status(0);
		}		
	}
	#endif
		tms_autolcd_sendData();
}

static void tms_set_arr_trigger(tba_bus_pack_t *ReceiveMsg)
{
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t	*)ReceiveMsg->app_data;
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(pisc_get_master_status()!=MASTER_STATUS)return;
	printf("tms_recv_pack_p->arr_flag: %d\r\n",tms_recv_pack_p->arr_flag);
	if(tms_recv_pack_p->arr_flag)
	{
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_ARRIVE);
		pisc_set_pre_flag(0);
		pisc_set_arr_flag(1);		
	}
		tms_autolcd_sendData();
}
static void tms_stop_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t	*)ReceiveMsg->app_data;
	//无钥匙，返回
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	printf("tms_recv_pack_p->manual_broadcast_stop: %d\r\n",tms_recv_pack_p->manual_broadcast_stop);
	if(tms_recv_pack_p->manual_broadcast_stop)
	{		
		if(BROADCAST_PRE==broadcast_get_broadcast_type())broadcast_stop_proc(BROADCAST_PRE);
		if(BROADCAST_ARRIVE==broadcast_get_broadcast_type())broadcast_stop_proc(BROADCAST_ARRIVE);
	}
}


static void tms_cycle_broadcast_trigger(tba_bus_pack_t *ReceiveMsg)
{	
	tms_recv_pack_t  *tms_recv_pack_p=(tms_recv_pack_t *)ReceiveMsg->app_data;
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	printf("tms_cycle_broadcast_trigger\r\n");
	//测试循环广播
	if(tms_recv_pack_p->cycle_broadcast_flag)
	{	
		//pisc_set_urgent_trigger(urgent_no); //默认第一段紧急广播
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_URGENT);
	}
	else
	{
		broadcast_stop_proc(BROADCAST_URGENT);	
	}	
}


static void tms_recv_pack_proc(uint8* buf)
{	
	led_toggle(TMS_LED_BIT);
	if(pisc_get_work_mode() != PISC_ATC_MODE)return;
	if(!pisc_get_master_status())return;
	data_proc_matrix_set_default_data((tba_bus_pack_t *)buf);
}




typedef struct {
	 struct{
		uint8_t nexldor:1;
		uint8_t nexrdor:1;
		uint8_t opldor:1;
		uint8_t oprdor:1;
		uint8_t clldor:1;
		uint8_t clrdor:1;
		uint8_t down:1;
		uint8_t up:1;	
	 }__attribute((packed))doorctl;
	 uint8_t start_stacode;
	 uint8_t end_stacode;
	 uint8_t cur_stacode;
	 uint8_t nex_stacode;
	 struct{
		 uint8_t reseve:5;
		 uint8_t arr_bit:1;
		 uint8_t prearr_bit:1;
		 uint8_t live_bit:1;
	 }__attribute((packed))action;
	 uint8_t sta_8_0;
	 uint8_t sta_16_9;
	 uint8_t sta_24_17;
	 uint8_t sta_32_18;
	 uint8_t sta_40_33;
	 uint8_t sta_48_41;
	 uint8_t sta_56_49;
	 uint8_t sta_64_57;
	 uint8_t sta_72_65;
	 uint8_t sta_80_73;
}__attribute((packed))AutoLCD_Type;


AutoLCD_Type autoLCDCtrl={0x00};

void tms_autolcd_sendData()
{
	int i = 0;
	pisc_default_data_t *pisc_p = get_local_pisdata();
    station_info_t * station_info = &pisc_p->station_info;
	autoLCDCtrl.start_stacode =station_info->start_station;
	autoLCDCtrl.end_stacode   =station_info->end_station;
	autoLCDCtrl.cur_stacode   =station_info->current_station;
	autoLCDCtrl.nex_stacode   =station_info->next_station;
	autoLCDCtrl.doorctl.nexldor       = 0;
	autoLCDCtrl.doorctl.nexrdor       = 0;
	autoLCDCtrl.doorctl.opldor        =pisc_p->signal_1.open_left_door_flag;
	autoLCDCtrl.doorctl.oprdor        =pisc_p->signal_1.open_right_door_flag;
	autoLCDCtrl.doorctl.clldor        =pisc_p->signal_1.close_left_door_flag;
	autoLCDCtrl.doorctl.clrdor        =pisc_p->signal_1.close_right_door_flag;
	if(pisc_p->signal_1.run_dir == 0x01)
	{
	 autoLCDCtrl.doorctl.down          =0x01;
	 autoLCDCtrl.doorctl.up            =0x00;
	}else
	{
	 autoLCDCtrl.doorctl.down          =0x00;
	 autoLCDCtrl.doorctl.up            =0x01;
	}
	autoLCDCtrl.action.arr_bit  = pisc_p->signal_1.arr_flag;
	autoLCDCtrl.action.prearr_bit  = pisc_p->signal_1.pre_flag;
	autoLCDCtrl.action.live_bit   = 0;
	for (i = 0; i<sizeof(autoLCDCtrl);++i)
	{
		printf(" %x",((char*)&autoLCDCtrl)[i]);
	}
	printf("\r\n===========Len:%d\n",sizeof(autoLCDCtrl));
	LCD_UDP_MultiCast(0x01,(unsigned char*)&autoLCDCtrl,sizeof(autoLCDCtrl));
}


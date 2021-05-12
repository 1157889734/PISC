//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dcp.c

//*文件作用:PA总线移植层

//*文件作者:mmaple

//*创建日期:2012年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "include.h"
#include "../driver/libledgpio.h"

#define DCP_DIR_UP		(0)
#define DCP_DIR_DOWN	(1)



//控制盒过程数据结构1
typedef struct
{
	//广播控制位
	struct //1-4
	{
		uint8 reserve:4;	// 1
		uint8 down:1;
		uint8 up:1;
		uint8 manual_flag:1;
		uint8 master_flag:1;

		uint8 test:1;// 2
		uint8 reserve_1:1;
		uint8 stop_arr_broadcast:1;
		uint8 start_arr_broadcast:1;
		uint8 stop_pre_broadcast:1;
		uint8 start_pre_broadcast:1;
		uint8 urgent_broadcast_flag:1;//紧急广播触发		
		uint8 live_broadcast_flag:1;//口播

		uint8 reserve2:6;	// 3
		uint8 stop_icc_flag:1;	//停止司机对讲
		uint8 start_icc_flag:1;

		uint8 reserve3:7;// 4
		uint8 ehp_flag:1;	 //司机乘客对讲
	}__attribute((packed))signal_1;

	uint8 start_station;  // 5
	uint8 end_station;   // 6 终点站
	uint8 current_station;  // 7 当前站
	uint8 next_station;   //8下一站
	uint8 urgent_broadcast_no;// 9 紧急广播代码
	uint8 ehp_no;// 10 紧急广播代码
	uint8 dcp_status; // 11
	uint8 reserve; // 12
	
}__attribute((packed))dcp_default_data_t;

static void dcp_set_dir(tba_bus_pack_t *ReceiveMsg);
static void dcp_set_station_info(tba_bus_pack_t *ReceiveMsg);
static void dcp_stop_arrive_broadcast(tba_bus_pack_t *ReceiveMsg);
static void dcp_start_arrive_broadcast(tba_bus_pack_t *ReceiveMsg);
static void dcp_stop_pre_broadcast(tba_bus_pack_t *ReceiveMsg);
static void dcp_start_pre_broadcast(tba_bus_pack_t *ReceiveMsg);
static void dcp_emergency_trigger(tba_bus_pack_t *ReceiveMsg);
static void dcp_live_broadcast(tba_bus_pack_t *ReceiveMsg);
static void dcp_test(tba_bus_pack_t *ReceiveMsg);


//主状态处理矩阵
static dev_proc_t dcp_process_data_table_M[] = 
{

	{TAB_BUS_PACK_HEAD_LEN+0, 		4,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_dir				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+0, 		5,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_dir				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+1, 		0,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_test 				},	//到站触发
	{TAB_BUS_PACK_HEAD_LEN+1, 		2,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_stop_arrive_broadcast					},	//到站触发
	{TAB_BUS_PACK_HEAD_LEN+1, 		3,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_start_arrive_broadcast 				},	//到站触发
	{TAB_BUS_PACK_HEAD_LEN+1, 		4,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_stop_pre_broadcast 				},	//到站触发
	{TAB_BUS_PACK_HEAD_LEN+1, 		5,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_start_pre_broadcast				},	//到站触发
	{TAB_BUS_PACK_HEAD_LEN+1, 		6,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_emergency_trigger				},	//紧急广播触发
	{TAB_BUS_PACK_HEAD_LEN+4,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_station_info					},	//起始站变更
	{TAB_BUS_PACK_HEAD_LEN+5,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_station_info					},	//终点站变更
	{TAB_BUS_PACK_HEAD_LEN+6,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_station_info					},	//当前站变更
	{TAB_BUS_PACK_HEAD_LEN+7,		0,		BIT_8,	UNEQUAL,	0,	(MsgHandler_Callback)dcp_set_station_info					},	//下一站变更
	{TAB_BUS_PACK_HEAD_LEN+1,		7,		BIT_1,	WHATEVER,	0,	(MsgHandler_Callback)dcp_live_broadcast				},	//紧急广播触发
	

	{0,	0,	0,	0,		0,	NULL													}
};

//主状态处理矩阵
static dev_proc_t dcp_process_data_table_S[] = 
{	

	{0,	0,	0,	0,		0,	NULL													}
};

static mode_matrix_t  dcp_process_table[]=
{
	{PISC_STATUS_M,  dcp_process_data_table_M,},			//主状态处理矩阵
	{PISC_STATUS_S,  dcp_process_data_table_S,},		//主状态处理矩阵
	
	{0, NULL,},
};

//状态基索引
static cmd_matrix_t  dcp_cmd_process_table[]=
{
	{CTRL_CMD_DEFAULT,  dcp_process_table,},			//主状态处理矩阵
	
	{0, NULL,},
};

void dcp_init(void)
{
	//初始化设备有效性
	dev_status_init_dev_valid(DEV_TYPE_DCP);
	data_proc_matrix_add_device_to_matrix_table(DEV_TYPE_DCP, dcp_cmd_process_table);
}

static void dcp_set_dir(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	printf("dcp_set_dir\r\n");
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
		
	if(ptr_tmp ->signal_1.down)
	{
		pisc_set_updown(PISC_DIR_DOWN);
	}
	else
	{
		pisc_set_updown(PISC_DIR_UP);

	}
}

static void dcp_set_station_info(tba_bus_pack_t *ReceiveMsg)
{
	station_info_t station_info;
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
	printf("dcp_set_station_info\r\n");
	
	//更新下一站
	if(station_info.next_station != ptr_tmp ->next_station)
	{
		//清开门侧
	}
	station_info.start_station = ptr_tmp ->start_station;
	station_info.end_station = ptr_tmp ->end_station;
	station_info.current_station = ptr_tmp ->current_station;
	station_info.next_station = ptr_tmp ->next_station;
	pisc_set_station_info(station_info);
}

static void dcp_stop_arrive_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
	printf("dcp_stop_arrive_broadcast: %d\r\n",ptr_tmp ->signal_1.stop_arr_broadcast);

	if(ptr_tmp ->signal_1.stop_arr_broadcast)
	{
		broadcast_stop_proc(BROADCAST_ARRIVE);
		//pisc_set_arr_trigger(0);
	}
}

static void dcp_start_arrive_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
	printf("dcp_start_arrive_broadcast: %d\r\n",ptr_tmp ->signal_1.start_arr_broadcast);

	if(ptr_tmp ->signal_1.start_arr_broadcast)
	{
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_ARRIVE);
		//pisc_set_arr_trigger(1);
	}
}

static void dcp_stop_pre_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
	printf("dcp_stop_pre_broadcast: %d\r\n",ptr_tmp ->signal_1.stop_pre_broadcast);
	if(ptr_tmp ->signal_1.stop_pre_broadcast)
	{		
		broadcast_stop_proc(BROADCAST_PRE);
	}
}

static void dcp_start_pre_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	printf("dcp_start_pre_broadcast, manual_flag: %d, master_flag: %d,start_pre_broadcast: %d\r\n",ptr_tmp ->signal_1.manual_flag,ptr_tmp ->signal_1.master_flag,ptr_tmp ->signal_1.start_pre_broadcast);
	print_buf((uint8 *)&(ptr_tmp ->signal_1),4);
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;

	if(ptr_tmp ->signal_1.start_pre_broadcast)
	{		
		printf("****dcp_start_pre_broadcast, manual_flag: %d, ptr_tmp ->signal_1.start_pre_broadcast: %d\r\n",ptr_tmp ->signal_1.manual_flag,ptr_tmp ->signal_1.start_pre_broadcast);
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_PRE);
	}
}

static void dcp_emergency_trigger(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	uint8 urgent_no = ptr_tmp ->urgent_broadcast_no;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;

	printf("dcp_emergency_trigger: %d\r\n",ptr_tmp ->signal_1.urgent_broadcast_flag);
	if(ptr_tmp ->signal_1.urgent_broadcast_flag)
	{		
		broadcast_set_urgent_no(urgent_no);
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_URGENT);
	}
	else
	{
		broadcast_stop_proc(BROADCAST_URGENT);
	}	
}

uint8_t livebraod_flag = 0x00;
uint8_t get_live_bradcast()
{

	return  livebraod_flag;
}

static void dcp_live_broadcast(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	uint8 dev_type_tmp=ReceiveMsg->src_dev_type;
	uint8 dev_id_tmp=ReceiveMsg->src_dev_id;
	static uint8 live_flag_tmp=0;
	//无钥匙，返回
	//if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;

	//口播标志没有变化
	if(live_flag_tmp==ptr_tmp ->signal_1.live_broadcast_flag)return;
	printf("dcp_live_broadcast: %d\r\n",ptr_tmp ->signal_1.live_broadcast_flag);
	livebraod_flag = ptr_tmp ->signal_1.live_broadcast_flag;

	live_flag_tmp=ptr_tmp ->signal_1.live_broadcast_flag;
	if(ptr_tmp ->signal_1.live_broadcast_flag)
	{		
		//broadcast_proc(dev_type_tmp,dev_id_tmp,BROADCAST_LIVE);	//segment fault，不明原因
		//led_onoff(MIC_LED_BIT,1);
		led_ctrl(LED10_MIC,1);
	}
	else
	{
		//broadcast_stop_proc(BROADCAST_LIVE);
		led_ctrl(LED10_MIC,0);
	}	
}



static void dcp_test(tba_bus_pack_t *ReceiveMsg)
{
	dcp_default_data_t *ptr_tmp = (dcp_default_data_t *)ReceiveMsg->app_data;
	//无钥匙，返回
	if(!ptr_tmp ->signal_1.manual_flag)return;
	if(!ptr_tmp ->signal_1.master_flag)return;
	printf("dcp_test: %d\r\n",ptr_tmp ->signal_1.test);

	if(ptr_tmp ->signal_1.test)
	{
		//广播测试内容
		broadcast_proc(ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id,BROADCAST_TEST);
		//车头屏显示测试

		//客室led屏显示测试		
	}


	
}


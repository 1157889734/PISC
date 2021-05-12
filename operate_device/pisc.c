//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 中央控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:pisc.c
//*文件作用:中央控制器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#include "../include/include.h"

static void pisc_key_proc(tba_bus_pack_t *ReceiveMsg);
static void pisc_master_proc(tba_bus_pack_t *ReceiveMsg);
static void pisc_update_local_default_to_master(tba_bus_pack_t *ReceiveMsg);
static void pisc_recv_default_pack(tba_bus_pack_t *ReceiveMsg);



static dev_proc_t  pisc_process_data_table_M[] =
{
	{TAB_BUS_PACK_HEAD_LEN+0,		6,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)pisc_key_proc				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+0,		7,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)pisc_master_proc				},	//DCP上下行切换

	{0,		0,		0,		0,			0,		NULL													}
};

static  dev_proc_t pisc_process_data_table_S[] =
{
	{TAB_BUS_PACK_HEAD_LEN+0,		6,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)pisc_key_proc				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+0,		7,		BIT_1,	UNEQUAL,	0,	(MsgHandler_Callback)pisc_master_proc				},	//DCP上下行切换

	{TAB_BUS_PACK_HEAD_LEN+0, 		0,		BIT_1,	WHATEVER,	0,	(MsgHandler_Callback)pisc_update_local_default_to_master				},	//DCP上下行切换
	{TAB_BUS_PACK_HEAD_LEN+0,		0,		BIT_1,	WHATEVER,	0,	(MsgHandler_Callback)pisc_recv_default_pack			},	//DCP上下行切换

	{0,		0,		0,		0,			0,		NULL													}
};

static mode_matrix_t   pisc_process_table[]=
{
	{PISC_STATUS_M, pisc_process_data_table_M},			//主状态处理矩阵
	{PISC_STATUS_S, pisc_process_data_table_S},		//主状态处理矩阵

	{0, 0,},
};

//状态基索引
static cmd_matrix_t  pisc_cmd_process_table[]=
{
	{CTRL_CMD_DEFAULT,  pisc_process_table,},			//主状态处理矩阵
	//{CTRL_CMD_VOLUME_CTRL,	NULL,},			//主状态处理矩阵
	//{CTRL_CMD_FEP,	NULL,}, 		//主状态处理矩阵
	
	{0, NULL,},
};

void pisc_init(void)
{
	//初始化设备有效性
	dev_status_init_dev_valid(DEV_TYPE_PISC);
	data_proc_matrix_add_device_to_matrix_table(DEV_TYPE_PISC, pisc_cmd_process_table);
}


static void pisc_update_local_default_to_master(tba_bus_pack_t *ReceiveMsg)
{
	pisc_default_data_t *pisc_p=(pisc_default_data_t *)ReceiveMsg->app_data;
	printf("enter...cmd:%d\r\n",ReceiveMsg->cmd);
	if(CTRL_CMD_DEFAULT!=ReceiveMsg->cmd)return;
	if(pisc_p->signal_1.master_flag)
	{
		if(ReceiveMsg->src_dev_id==pisc_local_get_other_dev_id())
		{
			//FiPrint("pisc_update_local_default_to_master, dev_type: 0x%x,dev_id: %d\r\n",ReceiveMsg->src_dev_type,ReceiveMsg->src_dev_id);		
		//print_buf(ReceiveMsg->app_data,20);
			pisc_set_station_info(pisc_p->station_info);
			pisc_set_updown(pisc_p->signal_1.run_dir);		
		}
	}
}

static void pisc_recv_default_pack(tba_bus_pack_t *ReceiveMsg)
{
	data_proc_matrix_set_default_data( ReceiveMsg);
}


static uint8 other_pisc_key_status=0;
uint8 pisc_get_other_pisc_key_status(void)
{
	return other_pisc_key_status;
}
void pisc_set_other_pisc_key_status(uint8 status)
{
	if(other_pisc_key_status!=status)
	{
		other_pisc_key_status=status;
		master_proc();
	}
}

static uint8 other_pisc_master_status=0;
uint8 pisc_get_other_pisc_master_status(void)
{
	return other_pisc_master_status;
}
void pisc_set_other_pisc_master_status(uint8 status)
{
	if(other_pisc_master_status!=status)
	{
		other_pisc_master_status=status;
	
		master_proc();
	}
}

static void pisc_key_proc(tba_bus_pack_t *ReceiveMsg)
{
	uint8 slave_flag=0;
	pisc_default_data_t *pisc_p=(pisc_default_data_t *)ReceiveMsg->app_data;
	if(CTRL_CMD_DEFAULT!=ReceiveMsg->cmd)return;
	if(ReceiveMsg->src_dev_id==pisc_local_get_other_dev_id())
	{
		FiPrint("pisc_key_proc, other key: %d\r\n",pisc_p->signal_1.active_flag);
		pisc_set_other_pisc_key_status(pisc_p->signal_1.active_flag);
	}
}
static void pisc_master_proc(tba_bus_pack_t *ReceiveMsg)
{
	pisc_default_data_t *pisc_p=(pisc_default_data_t *)ReceiveMsg->app_data;
	if(CTRL_CMD_DEFAULT!=ReceiveMsg->cmd)return;
	if(ReceiveMsg->src_dev_id==pisc_local_get_other_dev_id())
	{
		FiPrint("pisc_master_proc, other master: %d\r\n",pisc_p->signal_1.master_flag);
		pisc_set_other_pisc_master_status(pisc_p->signal_1.master_flag);
	}
}



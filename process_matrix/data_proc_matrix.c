//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: Pa_bus_process.c

//*文件作用: 内部协议接收

//*文件作者: mmaple

//*创建日期: 2012/03
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"


typedef struct
{
	uint8 dev_type;
	uint8 dev_id;
	uint16 data_len;
	uint8 data_bak[128];
}__attribute((packed))dev_pack_bak_t;

static dev_pack_bak_t dev_pack_bak_table[20]=
{
		{0,0, 0,{0}},
};

static dev_matrix_t dev_matrix_table[20]=
{
	{0,NULL},
};


static uint8 dev_index=0;

//static void data_proc_matrix_init_default_data(void);

//获得上一包保存过的数据
static void data_proc_matrix_get_default_data(uint8 dev_type,uint8 dev_id, uint8 * buf, uint16* len);

void data_proc_matrix_init(void)
{
	memset((uint8 *)&dev_matrix_table,0,sizeof(dev_matrix_table));
	//data_proc_matrix_init_default_data();
	
}

void data_proc_matrix_init_default_data(uint8 dev_type, uint8 dev_id)
{
	static uint8 i=0;
	//添加tms设备
	dev_pack_bak_table[i].dev_type=dev_type;
	dev_pack_bak_table[i].dev_id=dev_id;
	dev_pack_bak_table[i].data_len=80;
	i++;
	printf("+++++++++++++++++++++++++++data_proc_matrix_init_default_data:%d-->%d\n",dev_type,dev_id);
	memset((uint8 *)dev_pack_bak_table[i].data_bak, 0,sizeof(dev_pack_bak_t));
}

static void data_proc_matrix_get_default_data(uint8 dev_type, uint8 dev_id, uint8 * buf, uint16* len)
{	
	dev_pack_bak_t *dev_pack_tmp = dev_pack_bak_table;	//临时状态指令表		
	while(dev_pack_tmp->dev_id)
	{
		if(dev_type==dev_pack_tmp->dev_type && dev_id==dev_pack_tmp->dev_id)
		{
			printf("$$$$$$$$$$$$$DevType:%d DevId:%d\n",dev_pack_tmp->dev_type,dev_pack_tmp->dev_id);
			*len=dev_pack_tmp->data_len;
 			memcpy(buf,(uint8 *)&dev_pack_tmp->data_bak[0],dev_pack_tmp->data_len);
			break;
		}		
		dev_pack_tmp++;		
	}	
}
//保存处理过的数据，与下一包进行比较
void data_proc_matrix_set_default_data(tba_bus_pack_t *ReceiveMsg)
{
	tba_bus_pack_t tba_bus_tmp;
	dev_pack_bak_t *dev_pack_tmp = &dev_pack_bak_table[0];	
	memcpy((uint8 *)&tba_bus_tmp,(uint8 *)ReceiveMsg,ReceiveMsg->app_length+6);	

	while(dev_pack_tmp->dev_id)
	{		
		if(tba_bus_tmp.src_dev_type==dev_pack_tmp->dev_type && tba_bus_tmp.src_dev_id==dev_pack_tmp->dev_id)
		{
#if 0	
			if(DEV_TYPE_TMS==tba_bus_tmp.src_dev_type)
			{
				printf("DEV_TYPE_TMS :\r\n");
				print_buf((uint8 *)dev_pack_tmp->data_bak,ReceiveMsg->app_length+6);
			}
#endif			
			dev_pack_tmp->data_len=ReceiveMsg->app_length+6;
			printf("enter.......id:%d\r\n",dev_pack_tmp->dev_id);
			memcpy((uint8 *)dev_pack_tmp->data_bak,(uint8 *)&tba_bus_tmp,ReceiveMsg->app_length+6);
			break;
		}
		dev_pack_tmp++;
	}
}


//添加设备到处理矩阵
void data_proc_matrix_add_device_to_matrix_table(uint8 dev_type, cmd_matrix_t *cmd_tbl)
{
	if(dev_index>=(sizeof(dev_matrix_table)/sizeof(dev_matrix_t)))
		{
			printf("data_proc_matrix_add_device_to_matrix_table, too many dev,exit...\r\n");
			exit(0);
		}
	dev_matrix_table[dev_index].dev_type=dev_type;
	dev_matrix_table[dev_index].cmd_matrix_table=cmd_tbl;
	dev_index++;
}
//矩阵处理函数
static uint8 buf_bak2[256]={0};	
void data_proc_matrix_Dispatch(uint8 dev_type, uint8 dev_id,uint16 cmd, uint8 *buf, uint16 len, uint8 matrix_index_len)
{

	uint8 buf_recv_tmp[sizeof(tba_bus_pack_t)]={0};
	dev_matrix_t  const *dev_matrix_p = &dev_matrix_table[0];	//临时状态指令表	
	if((buf == NULL))return; //判断过程数据		
	if(len>sizeof(buf_recv_tmp))
	{
		printf("data_proc_matrix_Dispatch,dev_type: %d, too large len: 0x%x, pack_size_max: %d\r\n",dev_type,len,sizeof(buf_recv_tmp));
		print_buf(buf,30);
		return;
	}
	memcpy(buf_recv_tmp, &buf[matrix_index_len], len);	
	//FiPrint("data_proc_matrix_Dispatch,dev_type: 0x%x,dev_id: %d, cmd: 0x%x",dev_type,dev_id,cmd);
	//printf("dev_type:%d,dev_id: %d, cmd: 0x%x,dev_index:%d,matrix_index_len:%d\r\n",dev_type,dev_id,dev_index,matrix_index_len);
	while(dev_matrix_p ->dev_type)
	{		
	  
		if(dev_matrix_p ->dev_type == dev_type) //查找设备
		{
			
			cmd_matrix_t const *cmd_matrix_p=dev_matrix_p ->cmd_matrix_table;		
			while(cmd_matrix_p->cmd)
			{
				if(dev_type==1)
					{
					//FiPrint("cmd: 0x%x\r\n",cmd_matrix_p->cmd);
					}
				//命令字不符合，返回
				if(cmd==cmd_matrix_p->cmd)
				{
					//printf("dev_matrix_p ->dev_type:%d,dev_type%d,cmd:%d,matrix_p->cmd:%d\r\n",dev_matrix_p ->dev_type,dev_type,cmd,cmd_matrix_p->cmd);
			
					mode_matrix_t const *mode_matrix_p=cmd_matrix_p ->mode_matrix_table;		
					while(NULL!=mode_matrix_p)
					{
						uint8 master_status=0;
						master_status=pisc_get_master_status();			

						//printf("master..._status:%d\r\n",master_status);	
						if( master_status==mode_matrix_p->pisc_master_mode)
						{
							uint16 len_tmp=0;
							dev_proc_t const *dev_proc_p=mode_matrix_p->dev_proc_table;
							memset(buf_bak2,0,sizeof(buf_bak2));
							data_proc_matrix_get_default_data(dev_type,dev_id, buf_bak2,&len_tmp);

							while(dev_proc_p->msghandler != NULL)
							{
								if(dev_proc_p->char_index<=len)
								{
									if(dev_proc_p->judge == UNEQUAL)
									{
										if(dev_proc_p->bit_num>BIT_8)
										{
											//判断命令字并且CALLBACK函数不为空
											if((((buf_recv_tmp[dev_proc_p->char_index+1]<<8|buf_recv_tmp[dev_proc_p->char_index]) ^  
											(buf_bak2[matrix_index_len+dev_proc_p->char_index]|(buf_bak2[matrix_index_len+dev_proc_p->char_index+1]<<8))) 
												& (dev_proc_p->bit_num << dev_proc_p->bit_index)) != 0)  
											{	
												dev_proc_p->msghandler((tba_bus_pack_t *)buf);
											}								
										}
										else
										{
											//判断命令字并且CALLBACK函数不为空
											if(((buf_recv_tmp [dev_proc_p->char_index] ^  (buf_bak2[matrix_index_len+dev_proc_p->char_index])) 
												& (dev_proc_p->bit_num << dev_proc_p->bit_index)) != 0)  
											{	
												if(dev_type == 3)
													printf("+++++++++++++++_u8DevType:%d  index:%d\n",dev_type,dev_proc_p->char_index);
												dev_proc_p->msghandler((tba_bus_pack_t *)buf);
											}
										}
									}
									else if(dev_proc_p->judge == EQUAL)
									{		
										if(dev_proc_p->bit_num>0xff)
										{
											if(((buf_recv_tmp[dev_proc_p->char_index+1]<<8|buf_recv_tmp[dev_proc_p->char_index]) & (dev_proc_p->bit_num << dev_proc_p->bit_index))
											== (dev_proc_p->value << dev_proc_p->bit_index))
											{		
												dev_proc_p->msghandler((tba_bus_pack_t *)buf_recv_tmp); //调用CALLBACK函数
											}
										
										}
										else
										{
											if((buf_recv_tmp[dev_proc_p->char_index] & (dev_proc_p->bit_num << dev_proc_p->bit_index))
											== (dev_proc_p->value << dev_proc_p->bit_index))
											{		
												dev_proc_p->msghandler((tba_bus_pack_t *)buf_recv_tmp); //调用CALLBACK函数
											}
										}
									}
									else if(dev_proc_p->judge == WHATEVER)
									{
										dev_proc_p->msghandler((tba_bus_pack_t *)buf_recv_tmp); //调用CALLBACK函数
									}					
									else if(dev_proc_p->judge == BIGGER)
									{		
										if((buf_recv_tmp[dev_proc_p->char_index] & (dev_proc_p->bit_num << dev_proc_p->bit_index))
										> (dev_proc_p->value << dev_proc_p->bit_index))
										{		
											dev_proc_p->msghandler((tba_bus_pack_t *)buf_recv_tmp); //调用CALLBACK函数
										}
									}		
								}
								dev_proc_p++ ; 
							}	
							return;
						}
						mode_matrix_p++;				
					}	
				}
				cmd_matrix_p++; 	
			}
		}
		
		dev_matrix_p++;
	}
	return;
}





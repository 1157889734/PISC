/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-01 09:34:14
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-03 16:40:26
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&主备处理&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:master.c
//*文件作用:主备处理
//*文件作者:mmaple
//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"


static uint8 master_slave_type=0;
void master_init_master_slave_type(void)
{
	master_slave_type=SLAVE_TYPE_INIT+1;//配置成主设备
	printf("++++++master_slave_type:%d\r\n",master_slave_type);
	pisc_set_master_status(master_slave_type&0x0f);
}

static void master_set_master_slave_type(uint8 type)
{
	printf("type :%d,master_slave_type:%d\r\n",master_slave_type,type);
	if(master_slave_type!=type)
	{
		printf("master_set_master_slave_type: 0x%x\r\n",type);
		master_slave_type=type;
	
		pisc_set_master_status(master_slave_type&0x0f);
	}
	
}
uint8 master_get_master_slave_type(void)
{
	return master_slave_type;
}

void master_proc(void)
{
	uint8 master_type_tmp=master_get_master_slave_type();
	printf("master_type_tmp:%d\r\n",master_type_tmp);
	//对端中央控制器故障
	if(DEV_STATUS_OK!=dev_status_get_dev_status(DEV_TYPE_PISC,pisc_local_get_other_dev_id()))
	{
		master_type_tmp=MASTER_TYPE_OTHER_PISC_ERROR;
	}		
	else
	{
		//对方有钥匙，降备
		if(pisc_get_other_pisc_key_status())
		{
			master_type_tmp=SLAVE_TYPE_OTHER_KEY;
		}
		//对方没钥匙
		else
		{
			//本地有钥匙
			if(pisc_local_get_key_status())
			{
				master_type_tmp=MASTER_TYPE_KEY;			
			}
			else
			{
				if(pisc_local_get_my_dev_id()==1)
				{
					if(SLAVE_STATUS==pisc_get_other_pisc_master_status())
					{
						master_type_tmp=MASTER_TYPE_DEV_ID_LOW;
					}
				}
				else
				{
					master_type_tmp=SLAVE_TYPE_DEV_ID_HIGH;
				}				
			}
		}
	}

	master_set_master_slave_type(master_type_tmp);
	

}




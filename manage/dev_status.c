/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-03 09:45:11
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-08 09:52:11
 */ 
/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-03 09:45:08
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-03 10:19:01
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dev_status.c

//*文件作用:列车通信总线管理器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//超时次数
#define DEV_STATUS_TIMEOUT_NUM_MAX		(5)

#define DEV_VALID	(1)
#define DEV_UNVALID	(0)

typedef struct
{
	uint8 dev_status;
	uint8 dev_status_name[32];
}dev_status_name_t;
static const dev_status_name_t dev_status_name_table[]=
{
	{DEV_STATUS_OK,"正常"},
	{DEV_STATUS_ERROR,"错误"},
	{DEV_STATUS_NO_DEV,"无此设备"},
	
	{0,""},
};

//令牌巡检表结构
typedef struct
{
	uint8 dev_type;
	uint8 dev_id;			//设备号
	uint8 dev_type_en_name[16];
	uint8 dev_name[32];
	uint8 valid_flag;
	uint8 dev_status; //设备状态
	uint8 dev_status_name[32];
	uint8 dev_timeout_num;
}__attribute((packed))dev_status_t;
//设备状态

#if 0
static dev_status_t dev_status_table[]=
{	
	{DEV_TYPE_PISC,1,"pisc","中央控制器1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_PISC,2,"pisc","中央控制器2",DEV_VALID,DEV_STATUS_OK,"正常",0},

	{DEV_TYPE_DCP,1,"dcp","广播控制盒1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_DCP,2,"dcp","广播控制盒2",DEV_VALID,DEV_STATUS_OK,"正常",0},

	{DEV_TYPE_TMS,1,"tms","列车接口单元1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_TMS,2,"tms","列车接口单元2",DEV_VALID,DEV_STATUS_OK,"正常",0},

	{DEV_TYPE_HEAD_LED,1,"head_led","车头屏1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_HEAD_LED,2,"head_led","车头屏2",DEV_VALID,DEV_STATUS_OK,"正常",0},

	{DEV_TYPE_LCU_LED,1,"lcu_led","客室屏1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_LCU_LED,2,"lcu_led","客室屏2",DEV_VALID,DEV_STATUS_OK,"正常",0},
 
	{0,0,"",{0},0,0,"正常",0},
};
#else
//设备状态
static dev_status_t dev_status_table[]=
{	
	{DEV_TYPE_PISC,1,"pisc","中央控制器1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_PISC,88,"pisc","中央控制器2",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_DCP,1,"dcp","广播控制盒1",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_TMS,1,"tms","列车接口单元1",DEV_VALID,DEV_STATUS_OK,"正常",0},
    {DEV_TYPE_EHP,3,"ehp","紧急报警器",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{DEV_TYPE_AMP,15,"amp","功率放大器",DEV_VALID,DEV_STATUS_OK,"正常",0},
	{0,0,"",{0},0,0,"正常",0},
};
#endif
static pthread_t dev_status_thread_install_id;

void dev_status_init(void)
{
}

//初始化设备有效状态
void dev_status_init_dev_valid(uint8 dev_type)
{
	uint8 i=0;
	uint8 dev_i=0;
	uint8 dev_id_tmp=0;
	uint8 dev_valid_flag=0;
	uint8 dev_status_tmp=0;
	uint8 temp_str[32]={0};
	int8 val_str[4]="";
	uint8 dev_type_en_name_tmp[16]="";
	uint8 dev_num_max=0;

	while(dev_status_table[i].dev_type)
	{
		if(dev_type==dev_status_table[i].dev_type)
		{
			memcpy(dev_type_en_name_tmp,dev_status_table[i].dev_type_en_name,strlen((const char*)dev_status_table[i].dev_name));
			break;
		}
		i++;
	}

	//设备最大数目
	if(GetValueFromEtcFile(PISC_CONF_PATH,(const char*)dev_type_en_name_tmp,"dev_num_max",(char *)val_str,sizeof(val_str)-1)==0)
	{
		dev_num_max=atoi((const char*)val_str);
	}	
	for(dev_i=0;dev_i<dev_num_max;dev_i++)
	{
		dev_id_tmp=dev_i+1;
		sprintf((char *)temp_str,"%d_valid",dev_id_tmp);
		//查找设备是否有效
		if(GetValueFromEtcFile(PISC_CONF_PATH,(const char*)dev_type_en_name_tmp,(const char*)temp_str,(char *)val_str,sizeof(val_str)-1)==0)
		{
			dev_valid_flag=atoi((const char*)val_str);
		}		
		//设备是否有效
		dev_status_table[i].valid_flag=dev_valid_flag;
		sprintf((char *)temp_str,"%d_status",dev_id_tmp);
		//查找设备状态
		if(GetValueFromEtcFile(PISC_CONF_PATH,(const char*)dev_type_en_name_tmp,(const char*)temp_str,(char *)val_str,sizeof(val_str)-1)==0)
		{
			dev_status_tmp=atoi((const char*)val_str);
		}		
		//设备有效,添加到设备状态表
		if(dev_valid_flag)
		{
			//注册数据处理函数
			data_proc_matrix_init_default_data(dev_type,dev_id_tmp);
		}			
	}
}

//获得设备状态
uint8 dev_status_get_dev_status(uint8 dev_type,uint8 dev_id)
{
	uint8 i=0;
	while(dev_status_table[i].dev_type)
	{
		if(dev_type==dev_status_table[i].dev_type
			&& dev_id==dev_status_table[i].dev_id)
		{
			return dev_status_table[i].dev_status;
		}
		i++;
	}
	//没有设备
	return DEV_STATUS_NO_DEV;
}
static void dev_status_get_dev_status_name(uint8 dev_status, uint8 *dev_status_name)
{
	uint8 j=0;
	while(dev_status_name_table[j].dev_status)
	{
		if(dev_status==dev_status_name_table[j].dev_status)
		{
			memcpy(dev_status_name,dev_status_name_table[j].dev_status_name,strlen((const char*)dev_status_name_table[j].dev_status_name));
			break;
		}
		j++;
	}
}

//设置设备状态
void dev_status_set_dev_status(uint8 dev_type, uint8 dev_id, uint8 status)
{
	uint8 i=0;
	//printf("+++++++++dev_type:%d,dev_id:%d++++++\r\n",dev_type,dev_id);
	while(dev_status_table[i].dev_type)
	{
		//printf("table_type:%d,--table--id:%d,status:%d\r\n",dev_status_table[i].dev_type,dev_status_table[i].dev_id,status);
		if(dev_type==dev_status_table[i].dev_type
			&& dev_id==dev_status_table[i].dev_id)
		{
			
			//清设备超时计数
			if(DEV_STATUS_OK==status)
			{
				dev_status_table[i].dev_timeout_num=0;
			}

			if(status!=dev_status_table[i].dev_status)
			{
				//更新设备状态
				dev_status_table[i].dev_status=status;
				//获得设备状态的名称
				dev_status_get_dev_status_name(dev_status_table[i].dev_status,dev_status_table[i].dev_status_name);
				//写入日志
				//log_write_dev_status(dev_status_table[i].dev_name,dev_status_table[i].dev_status_name);
				
			}
			return;
		}
		i++;
	}
}


static void* dev_status_thread(void* param)
{	
	printf("dev_status_thread start....\r\n");

	while(1)
	{
		uint8 i=0;
		while(dev_status_table[i].dev_type)
		{
			dev_status_table[i].dev_timeout_num++;
			if(dev_status_table[i].dev_timeout_num>=DEV_STATUS_TIMEOUT_NUM_MAX)
			{
				//填充设备状态
				dev_status_set_dev_status(dev_status_table[i].dev_type,dev_status_table[i].dev_id,DEV_STATUS_ERROR);
				//printf("+++++++table_dev_type:%d,id:%d,PisLocal_devid:%d\r\n",dev_status_table[i].dev_type,dev_status_table[i].dev_id,pisc_local_get_my_dev_id());
				if(DEV_TYPE_PISC==dev_status_table[i].dev_type && (pisc_local_get_my_dev_id()!=dev_status_table[i].dev_id))
				{
					//对端故障，处理主备
					master_proc();
				}
			}
			i++;
		}
		sleep(1);
	}
}
void	dev_status_thread_install(void)
{
	//创建线程
	//pthread_create(&dev_status_thread_install_id, NULL, (void*)dev_status_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&dev_status_thread_install_id,  dev_status_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}



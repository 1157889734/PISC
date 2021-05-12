//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 中央控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:pisc.c
//*文件作用:中央控制器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "include.h"
#include "../driver/libledgpio.h"


#define INIT_WORKMODE	PISC_ATC_MODE //自动  默认工作模式   


#define CMD_GET_GPF1 	(0x03)
#define CMD_SET_GPF1 	(0x04)
#define CMD_GET_GPF2 	(0x05)
#define CMD_SET_GPF2 	(0x06)
#define CMD_GET_GPF3 	(0x07)
#define CMD_SET_GPF3 	(0x08)
#define CMD_GET_GPF4 	(0x09)
#define CMD_SET_GPF4 	(0x0a)
#define CMD_GET_GPF6 	(0x0d)
#define CMD_SET_GPF6 	(0x0e)


#define IO_IN_ADDR_1					(CMD_GET_GPF6)
//#define IO_IN_ADDR_2					(CMD_GET_GPF7)
//#define IO_IN_ADDR_3					(CMD_GET_GPG0)
//#define IO_IN_ADDR_4					(CMD_GET_GPG1)



static pisc_default_data_t 	local_pisc_data_bak;		//过程数据帧
static uint8 my_dev_id=88;
static uint32 my_dev_ip=0xa8a86688;
static uint16 my_train_id=0x0001;


//初始化设备id
static void pisc_local_init_my_dev_id(void);
//初始化设备ip
static void pisc_local_init_my_dev_ip(uint8 dev_id);
static void pisc_set_version(uint32 ver);


void pisc_local_init(void)
{	
	//初始化设备id
	pisc_local_init_my_dev_id();
	//初始化设备ip
	pisc_local_init_my_dev_ip(pisc_local_get_my_dev_id());

	pisc_init_process_data();	
}



uint32 pisc_local_get_my_dev_ip(void)
{
	return my_dev_ip;
}
uint16 pisc_local_get_my_train_id(void)
{
	return my_train_id;
}

static void pisc_local_init_my_dev_id(void)
{
	//my_dev_id=2-io_get_level(IO_IN_ADDR_1);
	//my_dev_id = 88;
	printf("my_dev_id: %d\r\n",my_dev_id);
}

static void pisc_local_init_my_dev_ip(uint8 dev_id)
{
	char str_tmp_local[32]={0};
	int8 str_tmp_right[32]={0};
	uint8 ip_4=dev_id;
	uint8 reboot_flag=0;

	if(GetValueFromEtcFile("/etc/net.conf","netconfig","IPADDR",str_tmp_local,sizeof(str_tmp_local)-1)==0)
	{
		sprintf((char *)str_tmp_right,"%s%d",NET_IP_HEAD,ip_4);

		printf("enter....net confg\r\n");
		//如果ip不正确
		if(memcmp(str_tmp_local,(char *)str_tmp_right,strlen(str_tmp_local))!=0)
		{
			reboot_flag=1;
			//设置ip
			SetValueToEtcFile("/etc/net.conf","netconfig","IPADDR",(char *)str_tmp_right);
		}
	}
	if(GetValueFromEtcFile("/etc/net.conf","netconfig","MAC",(char *)str_tmp_local,sizeof(str_tmp_local)-1)==0)
	{
		sprintf((char *)str_tmp_right,"%s%02d",NET_MAC_HEAD,ip_4);
		//如果ip不正确
		if(memcmp(str_tmp_local,str_tmp_right,strlen((char *)str_tmp_local))!=0)
		{
			reboot_flag=1;
			//设置mac
			SetValueToEtcFile("/etc/net.conf","netconfig","MAC",(char *)str_tmp_right);
		}
	}

	//设置正确ip
	my_dev_ip=(0xa8a86600|ip_4);
	printf("my_dev_ip: %x*****%x<%d>\r\n",my_dev_ip,ip_4,ip_4);
	
	if(reboot_flag)
	{
		system("reboot");
		printf("reboot\r\n");
	}

	
}

uint8 pisc_local_get_my_dev_id(void)
{
	return my_dev_id;
}
uint8 pisc_local_get_other_dev_id(void)
{
	return 3-my_dev_id;
}

void pisc_local_set_key_status(uint8 flag)
{
	if(flag!=local_pisc_data_bak.signal_1.active_flag)
	{
		printf("pisc_local_set_key_status: %d\r\n",flag);
		//led灯显示
		 //led_onoff(KEY_LED_BIT,flag);
		led_ctrl(LED5_ACTIVE,flag);
		
		local_pisc_data_bak.signal_1.active_flag=flag;
		//处理主备
		master_proc();
	}
}

uint8 pisc_local_get_key_status(void)
{
	return local_pisc_data_bak.signal_1.active_flag;		
}

void pisc_init_process_data(void)
{
	station_info_t station_info;
	uint8 temp_str[8]={0};
	memset((uint8 *)&local_pisc_data_bak, 0, sizeof(pisc_default_data_t));
	station_info.start_station = 1;
	station_info.end_station = 20;
	station_info.current_station = 1;
	station_info.next_station = 2;	
	pisc_local_set_key_status(0);
	pisc_set_station_info(station_info);
	pisc_set_updown(PISC_DIR_UP);
	pisc_set_work_mode(INIT_WORKMODE);	
	master_init_master_slave_type();
	pisc_set_version(MAJOR_VERSION);//设置版本号


	//客室led滚动速度
	if(GetValueFromEtcFile(PISC_CONF_PATH,"lcu_led","speed_time",(char *)temp_str,sizeof(temp_str)-1)==0)
	{
		local_pisc_data_bak.lcu_led_roll_sec=atoi((char *)temp_str);
	}		
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dmp","light_level",(char *)temp_str,sizeof(temp_str)-1)==0)
	{
		local_pisc_data_bak.dmp_light_level=atoi((char *)temp_str);
	}	
	//版本号 v1.2 svn430, 发送数据为0x01 0x02 0x04 0x30
	local_pisc_data_bak.version=((0<<24)|(2<<16)|(10<<8)|(1<<0));
}
static void pisc_set_version(uint32 ver)
{
	local_pisc_data_bak.version=ver;

}
void pisc_set_work_mode(uint8 mode)
{
	if(mode==PISC_ATC_MODE)
	{
		local_pisc_data_bak.signal_1.atc_mode_flag = 1;
		local_pisc_data_bak.signal_1.tms_mode_flag = 0;
		local_pisc_data_bak.signal_1.dcp_mode_flag = 0;
	}
	else if(mode==PISC_DCP_MANUAL_MODE)
	{
		local_pisc_data_bak.signal_1.atc_mode_flag = 0;
		local_pisc_data_bak.signal_1.tms_mode_flag = 0;
		local_pisc_data_bak.signal_1.dcp_mode_flag = 1;
	}	
}

uint8 pisc_get_work_mode(void)
{
	if(local_pisc_data_bak.signal_1.atc_mode_flag)
	{
		return PISC_ATC_MODE;
	}
	else if(local_pisc_data_bak.signal_1.dcp_mode_flag)
	{
		return PISC_DCP_MANUAL_MODE;
	}
	return PISC_DCP_MANUAL_MODE;
}

uint8 pisc_get_master_status(void)
{
	return local_pisc_data_bak.signal_1.master_flag;
}

void pisc_set_master_status(uint8 master_flag_c)
{
	printf("mster_flag_c:%d,local_pisc_data_bak.signal_1.master_flag=%d\r\n",master_flag_c,local_pisc_data_bak.signal_1.master_flag);
	if(master_flag_c!=local_pisc_data_bak.signal_1.master_flag)
	{
		printf("pisc_set_master_status: %d\r\n",master_flag_c);

#if 0		
		led_onoff(MANUAL_LED_BIT,master_flag_c);
#endif
		//置过程数据标志位
		local_pisc_data_bak.signal_1.master_flag=master_flag_c;

		//升主
		if(master_flag_c)
		{
			//初始化模块数据
			dcp_init();
			pisc_init();
			tms_init(); 
			dva_init();//DVA设备初始化
			media_init();
			ptu_init();
			occ_init();

		}
		//降备
		else
		{
			printf("became slave, stop broadcasting...\r\n");
			//停止所有广播
			broadcast_stop_proc(broadcast_get_broadcast_type());
		}
	}
}

void pisc_set_updown(uint8 updown)
{	
	if(local_pisc_data_bak.signal_1.run_dir!=updown)
	{
		local_pisc_data_bak.signal_1.run_dir=updown;

		if(PISC_DIR_UP==updown)
		{
			printf("pisc_set_updown: up\r\n");
		}
		else if(PISC_DIR_DOWN==updown)
		{
			printf("pisc_set_updown: down\r\n");
		}		
		else
		{
			printf("pisc_set_updown: wrong dir: %d\r\n",updown);
		}				
	}
}
uint8 pisc_get_updown(void)
{
	return local_pisc_data_bak.signal_1.run_dir ;
}

void pisc_set_pre_flag(uint8 flag)
{	
	if(local_pisc_data_bak.signal_1.pre_flag!=flag)
	{
		printf("pisc_set_pre_flag: %d\r\n",flag);
		local_pisc_data_bak.signal_1.pre_flag=flag;
	}

}
uint8 pisc_get_pre_flag(void)
{
	return local_pisc_data_bak.signal_1.pre_flag ;
}

void pisc_set_arr_flag(uint8 flag)
{	
	if(local_pisc_data_bak.signal_1.arr_flag!=flag)
	{
		printf("pisc_set_arr_flag: %d\r\n",flag);
		local_pisc_data_bak.signal_1.arr_flag=flag;
	}
}
uint8 pisc_get_arr_flag(void)
{
	return local_pisc_data_bak.signal_1.arr_flag ;
}
void pisc_set_dmp_light(uint8 light_level)
{	
	if(local_pisc_data_bak.dmp_light_level!=light_level)
	{
		printf("pisc_set_dmp_light: %d\r\n",light_level);
		local_pisc_data_bak.dmp_light_level=light_level;
	}
}
uint8 pisc_get_dmp_light(void)
{
	return local_pisc_data_bak.dmp_light_level ;
}
void pisc_set_lcu_led_roll_sec(uint8 roll_sec)
{	
	if(local_pisc_data_bak.lcu_led_roll_sec!=roll_sec)
	{
		printf("pisc_set_lcu_led_roll_sec: %d\r\n",roll_sec);
		local_pisc_data_bak.lcu_led_roll_sec=roll_sec;
	}
}
uint8 pisc_get_lcu_led_roll_sec(void)
{
	return local_pisc_data_bak.lcu_led_roll_sec ;
}

void pisc_set_station_info(station_info_t station_info_c)
{
	if(memcmp((uint8 *)&local_pisc_data_bak.station_info,(uint8 *)&station_info_c,sizeof(station_info_t)))
	{
		local_pisc_data_bak.station_info.start_station = station_info_c.start_station;
		if(local_pisc_data_bak.station_info.end_station != station_info_c.end_station)
		{
			local_pisc_data_bak.station_info.end_station = station_info_c.end_station;
		}
		local_pisc_data_bak.station_info.current_station = station_info_c.current_station;
		local_pisc_data_bak.station_info.next_station = station_info_c.next_station;

		printf("pisc_set_station_info:+++++++++++++ \r\n");
		//print_buf((uint8 *)&station_info_c, sizeof(station_info_t));

		printf("+++station_info_start:%d\r\n",local_pisc_data_bak.station_info.start_station);
		printf("+++station_info_current:%d\r\n",local_pisc_data_bak.station_info.current_station);
		printf("+++station_info_next:%d\r\n",local_pisc_data_bak.station_info.next_station);
		printf("+++station_info_end:%d\r\n",local_pisc_data_bak.station_info.end_station);
		
	}
}

void pisc_get_station_info(station_info_t *station_info_c)
{
	station_info_c->start_station=local_pisc_data_bak.station_info.start_station;
	station_info_c->end_station=local_pisc_data_bak.station_info.end_station;
	station_info_c->current_station=local_pisc_data_bak.station_info.current_station;
	station_info_c->next_station=local_pisc_data_bak.station_info.next_station;
}
uint8 pisc_get_open_left_door_status(void)

{
	return local_pisc_data_bak.signal_1.open_left_door_flag;
}
void pisc_set_open_left_door_status(uint8 status_c)
{
	if(local_pisc_data_bak.signal_1.open_left_door_flag!=status_c)
	{
		printf("pisc_set_open_left_door_status: %d\r\n",status_c);
		local_pisc_data_bak.signal_1.open_left_door_flag=status_c;
	}
}

uint8 pisc_get_open_right_door_status(void)
{
	return local_pisc_data_bak.signal_1.open_right_door_flag;
}
void pisc_set_open_right_door_status(uint8 status_c)
{
	if(local_pisc_data_bak.signal_1.open_right_door_flag!=status_c)
	{
		printf("pisc_set_open_right_door_status: %d\r\n",status_c);
		local_pisc_data_bak.signal_1.open_right_door_flag=status_c;
	}
}

uint8 pisc_get_close_left_door_status(void)
{
	return local_pisc_data_bak.signal_1.close_left_door_flag;
}
uint8 pisc_get_close_right_door_status(void)
{
	return local_pisc_data_bak.signal_1.close_right_door_flag;
}
void pisc_set_close_door_status(uint8 status_c)
{
	local_pisc_data_bak.signal_1.close_left_door_flag=status_c;
	local_pisc_data_bak.signal_1.close_right_door_flag=status_c;
}

void pisc_local_send_default_data(void)
{
	//发送显示数据
	ctrl_port_send_cmd(0xffff,0xff,0xff,CTRL_CMD_DEFAULT,(uint8 *)&local_pisc_data_bak,sizeof(pisc_default_data_t));
}
static tms_time_t local_time;
void pisc_local_set_time(uint8 *time_buf, uint16 time_len)
{
	if(time_len==sizeof(tms_time_t))
	{
		memcpy((uint8 *)&local_time,time_buf,sizeof(tms_time_t));
	}
	else
	{
		printf("pisc_local_set_time len wrong: %x\r\n",time_len);
	}
}
void pisc_local_get_time(uint8 *time_buf)
{
	time_t cTime = time(NULL);
	struct tm *tm_p;
	tm_p=localtime(&cTime);

	local_time.year=1900+tm_p->tm_year;
	local_time.mon=tm_p->tm_mon+1;
	local_time.day=tm_p->tm_mday;
	local_time.hour=tm_p->tm_hour;
	local_time.min=tm_p->tm_min;
	local_time.sec=tm_p->tm_sec;	

	memcpy(time_buf,(uint8 *)&local_time,sizeof(tms_time_t));
}

void pisc_local_send_time(void)
{
	tms_time_t tms_time_tmp;
	pisc_local_get_time((uint8 *)&tms_time_tmp);
	ctrl_port_send_cmd(0xffff,0xff,0xff,BROADCAST_SET_TIME,(uint8 *)&tms_time_tmp,sizeof(tms_time_t));
}


uint32 pisc_get_jump_stations(void)
{
	return local_pisc_data_bak.jump_stations;
}
void pisc_set_jump_stations(uint32 jump_stations_c)
{
	local_pisc_data_bak.jump_stations=jump_stations_c;
}
void pisc_update_jump_stations_from_conf(void)
{
	uint8 temp_str[32]={0};
	uint8 i=0;
	uint8 skip_station_tmp=0;

	for(i=0;i<32;i++)
	{
		sprintf(( char*)temp_str,"%d_skip_valid",i+1);
		//越站信息
		//if(GetValueFromEtcFile(PISC_CONF_PATH,"pisc","skip_stations",(char *)temp_str,sizeof(temp_str)-1)==0)
		if(GetValueFromEtcFile(PISC_CONF_PATH,"pisc",(const char *)temp_str,(char *)temp_str,sizeof(temp_str)-1)==0)
		{
			skip_station_tmp|=(atoi((char *)temp_str)<<i);
		}
	}
	pisc_set_jump_stations(skip_station_tmp);
	
}




pisc_default_data_t *	get_local_pisdata(void)
{
	return &local_pisc_data_bak;	
}


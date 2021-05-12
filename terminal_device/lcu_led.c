//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.c
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//显示命令字
#define LCU_LED_SHOW_CMD	(0x0009)


#define LCU_LED_PACK_NUM_MAX		(5)
#define LCU_LED_PACK_CONTENT_MAX		(1024)
#define LCU_LED_CONTENT_MAX		(10240)

#define LCU_LED_LANGUAGE_NUM_MAX		(2)


typedef struct
{
	uint8 broadcast_type;
	uint8 language;
	const uint8 filename[16];
	void (*get_play_list)(uint16* ,uint8*, uint16* );//广播处理
}lcu_led_show_type_t;
static const lcu_led_show_type_t lcu_led_show_type[]=
{
	{BROADCAST_PRE,LANGUAGE_C,"1/pre/",dva_broadcast_rule_get_pre_list},
	{BROADCAST_PRE,LANGUAGE_E,"2/pre/",dva_broadcast_rule_get_pre_list},
	{BROADCAST_ARRIVE,LANGUAGE_C,"1/arrive/",dva_broadcast_rule_get_arr_list},
	{BROADCAST_ARRIVE,LANGUAGE_E,"2/arrive/",dva_broadcast_rule_get_arr_list},
	{BROADCAST_URGENT,LANGUAGE_C,"1/urgent/",NULL},
	{BROADCAST_URGENT,LANGUAGE_E,"2/urgent/",NULL},
	
	{0,0,"",NULL},
		
};

typedef struct
{
	uint8 pack_num;
	uint8 current_packet_num;
	uint8 color;
	uint8 content[LCU_LED_PACK_CONTENT_MAX];
}lcu_led_pack_t;

typedef struct
{
	uint8 language;
	uint8 color;
	uint32 content_len;
	uint8 content[LCU_LED_CONTENT_MAX];
}lcu_led_send_content_t;
static lcu_led_send_content_t lcu_led_send_content_table=
{
	LANGUAGE_C,
	1,
	0,
	{0},
};

void lcu_led_init(void)
{	
	//初始化设备有效性
	dev_status_init_dev_valid(DEV_TYPE_LCU_LED);
}
//设置客室屏的显示内容
void lcu_led_set_content(uint8* path,uint16 *list,uint16 list_num)
{
	uint8 file_name_str[128]="";
	uint8 j=0;
	uint16 file_len_tmp=0;
	int len_tmp=0;
	uint8 buf_tmp[10240]={0};
	uint8 hex_buf_tmp[10240]={0};
	
	//清除数据
	lcu_led_send_content_table.content_len=0;
	printf("++++++path:%s\r\n",path);
	for(j=0;j<list_num;j++)
	{
		//配置文件名
		if(0==j)sprintf((char *)file_name_str,"%s/%d/%03d.conf",path,LANGUAGE_C,list[j]);
		else sprintf((char *)file_name_str,"%s/%d/%03d.conf",path,LANGUAGE_E,list[j]);
		//显示内容
		standard_file_read((const char*)file_name_str,(char *)buf_tmp,&len_tmp);
		printf("j: %d,file_name_str: %s,len_tmp: %d,%s\r\n",j,file_name_str,len_tmp,LCU_LED_ARR_PATH);
		file_len_tmp=len_tmp;
		buf_to_hex(hex_buf_tmp,buf_tmp,file_len_tmp,&file_len_tmp);
		print_buf(hex_buf_tmp,file_len_tmp);
		lcu_led_send_content_table.color=hex_buf_tmp[0];
		memcpy((uint8 *)&lcu_led_send_content_table.content[lcu_led_send_content_table.content_len],(uint8 *)&hex_buf_tmp[1],file_len_tmp);
		
		lcu_led_send_content_table.content_len+=file_len_tmp;
	}


}

void lcu_led_send_content(void)
{
	uint8 i=0;
	lcu_led_pack_t lcu_led_pack_tmp;
	uint16 send_len_tmp=0;
	uint16 content_len_tmp=0;
	uint8 pack_num_tmp=(lcu_led_send_content_table.content_len/LCU_LED_PACK_CONTENT_MAX+1);
	for(i=0;i<pack_num_tmp;i++)
	{
		if(i<(pack_num_tmp-1))content_len_tmp=LCU_LED_PACK_CONTENT_MAX;
		else content_len_tmp=(lcu_led_send_content_table.content_len%LCU_LED_PACK_CONTENT_MAX);

		//填充数据包
		lcu_led_pack_tmp.pack_num=pack_num_tmp;
		lcu_led_pack_tmp.current_packet_num=(i+1);
		lcu_led_pack_tmp.color=lcu_led_send_content_table.color; 	
		memcpy(lcu_led_pack_tmp.content,(uint8 *)&lcu_led_send_content_table.content[LCU_LED_PACK_CONTENT_MAX*i],content_len_tmp);

		//发送数据长度
		send_len_tmp=sizeof(lcu_led_pack_t)-LCU_LED_PACK_CONTENT_MAX+content_len_tmp;
		//发送显示数据
		ctrl_port_send_cmd(0xffff,DEV_TYPE_LCU_LED,0xff,LCU_LED_SHOW_CMD,(uint8 *)&lcu_led_pack_tmp,send_len_tmp);
		//FiPrint("led con:\r\n");
		//print_buf((uint8 *)&lcu_led_pack_tmp,send_len_tmp);
	}
}


void lcu_led_roll_sec(void)
{
	uint16 temp_str[4]={0};

	//客室led滚动速度
	if(GetValueFromEtcFile(PISC_CONF_PATH,"lcu_led","speed_time",(char *)temp_str,sizeof(temp_str)-1)==0)
	{
		pisc_set_lcu_led_roll_sec(atoi((char *)temp_str));
	}		
}



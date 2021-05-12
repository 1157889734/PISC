//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.c
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//显示命令字
#define HEAD_LED_SHOW_CMD	(0x0008)

#define HEAD_LED_PACK_CONTENT_MAX		(1024)
#define HEAD_LED_CONTENT_MAX		(10240)


typedef struct
{
	uint8 language;
	uint8 color;
	uint32 content_len;
	uint8 content[HEAD_LED_CONTENT_MAX];
}head_led_send_content_t;
static head_led_send_content_t head_led_send_content_table[]=
{
	{LANGUAGE_C,COLOR_R,0,""},
	{LANGUAGE_E,COLOR_G,0,""},
	
	{0,0,0,"",},
};



typedef struct
{
	uint8 pack_num;
	uint8 current_packet_num;
	uint8 color;
	uint8 language;
	uint8 content[HEAD_LED_PACK_CONTENT_MAX];
}head_led_pack_t;


void head_led_init(void)
{	
	//初始化设备有效性
	dev_status_init_dev_valid(DEV_TYPE_HEAD_LED);

}

void head_led_set_content(uint8 language_c)
{
	station_info_t station_info_tmp;
	uint8 file_name_str[128]="";
	uint8 buf_tmp[1280*4]={0};
	uint8 hex_buf[1280*4]={0};
	uint8 i=0;
	int file_len_tmp=0;
	uint16 hex_len=0;

	pisc_get_station_info(&station_info_tmp);	
	
	//查找显示内容
	sprintf((char *)file_name_str,"%s%d/%03d.conf",HEAD_LED_CONF_PATH,language_c,station_info_tmp.end_station);

	while(head_led_send_content_table[i].language)
	{
		if(language_c==head_led_send_content_table[i].language)
		{
			//清数据
			head_led_send_content_table[i].content_len=0;
			//读文件
			standard_file_read((const char *)file_name_str,(char *)buf_tmp,&file_len_tmp);
			printf("file_name_str: %s, file_len_tmp: %d\r\n",file_name_str,file_len_tmp);
			//print_buf(buf_tmp,90);
			hex_len=file_len_tmp;
			buf_to_hex(hex_buf,buf_tmp,hex_len,&hex_len);
			//printf("head hex: %d\r\n",hex_len);
			//print_buf(hex_buf,hex_len);
			//颜色
			head_led_send_content_table[i].color=hex_buf[0];
			memcpy((uint8 *)&head_led_send_content_table[i].content[0],&hex_buf[1],hex_len-1);
			head_led_send_content_table[i].content_len+=hex_len;
			//printf("head_led_send_content_table[i].content_len: %d\r\n",head_led_send_content_table[i].content_len);
			break;
		}
		i++;
	}
}

void head_led_send_content(void)
{
	uint8 i=0;
	uint8 j=0;
	head_led_pack_t head_led_pack_tmp;
	uint16 pack_len_tmp=0;
	uint16 send_len_tmp=0;
	for(i=0;i<2;i++)
	{
		uint8 pack_num_tmp=(head_led_send_content_table[i].content_len/HEAD_LED_PACK_CONTENT_MAX+1);
		for(j=0;j<pack_num_tmp;j++)
		{
			if(j<(pack_num_tmp-1))pack_len_tmp=HEAD_LED_PACK_CONTENT_MAX;
			else pack_len_tmp=(head_led_send_content_table[i].content_len%HEAD_LED_PACK_CONTENT_MAX);
			//printf("pack_len_tmp: %d\r\n",pack_len_tmp);
			//填充数据包
			head_led_pack_tmp.pack_num=pack_num_tmp;
			head_led_pack_tmp.current_packet_num=(j+1);
			head_led_pack_tmp.color=head_led_send_content_table[i].color; 	
			head_led_pack_tmp.language=head_led_send_content_table[i].language; 	
			memcpy(head_led_pack_tmp.content,(uint8 *)&head_led_send_content_table[i].content,pack_len_tmp);
			//发送长度
			send_len_tmp=sizeof(head_led_pack_t)-HEAD_LED_PACK_CONTENT_MAX+pack_len_tmp;
			//发送显示数据
			ctrl_port_send_cmd(0xffff,DEV_TYPE_HEAD_LED,0xff,HEAD_LED_SHOW_CMD,(uint8 *)&head_led_pack_tmp,send_len_tmp);
			printf("head cont:+++++++++%d-%d\r\n",i,j);
			//print_buf((uint8 *)&head_led_pack_tmp,send_len_tmp);
		}
	}
}





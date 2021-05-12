/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-27 11:50:33
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 11:50:35
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 设备错误处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:log.c

//*文件作用:设备错误处理

//*文件作者: mmaple

//*创建日期:2010年9月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"

#define EXE_OK			1
#define EXE_FAIL		0


#define LOG_TYPE_BROADCAST_LOG		1
#define LOG_TYPE_OCC_LOG					2
#define LOG_TYPE_TALK_LOG				3
#define LOG_TYPE_DEV_STATUS_LOG		4
#define LOG_TYPE_RONGYU_LOG		5


typedef struct
{
	uint8 log_type;
	const uint8 log_file_name[32];
}log_t;
static const log_t log_table[]=
{
	{LOG_TYPE_BROADCAST_LOG,	"broadcast.log"},
	{LOG_TYPE_OCC_LOG,			"occ.log"},
	{LOG_TYPE_TALK_LOG,			"talk.log"},
	{LOG_TYPE_RONGYU_LOG,		"rongyu.log"},
	{LOG_TYPE_DEV_STATUS_LOG,"dev_status.log"},
	{0,{0}},
};

void log_erase(void)
{
	system("rm 20130531_test.log");
}
void log_write(const char *filename, uint8* buf, uint16 len)
{
	int8 str_tmp[128]={0};
	FILE *fd=NULL;
	sprintf((char *)str_tmp,"%s",buf);
	fd = fopen( filename,"a");
	fwrite(str_tmp,strlen((const char *)str_tmp),1,fd);		
	fclose(fd);
}

void log_write_dev_status(uint8 *dev_name,uint8* dev_status)
{
	uint8 str_tmp[128]={0};
	uint8 i=0;

	printf("***log_write_dev_status, %s: %s\r\n",dev_name,dev_status);
	while(log_table[i].log_type)
	{
		if(LOG_TYPE_DEV_STATUS_LOG==log_table[i].log_type)
		{
			tms_time_t tms_time_tmp={0};
			pisc_local_get_time((uint8 *)&tms_time_tmp);
		
			//1970-09-01, 00:00:53,中央控制器1,故障
			sprintf((char *)str_tmp,"%04d-%02d-%02d,%02d:%02d:%02d,%s,%s\r\n",tms_time_tmp.year,tms_time_tmp.mon,tms_time_tmp.day,tms_time_tmp.hour,tms_time_tmp.min,tms_time_tmp.sec,dev_name,dev_status);
			log_write((const char*)log_table[i].log_file_name,str_tmp,strlen((const char *)str_tmp));
			break;
		}
		i++;
	}
}



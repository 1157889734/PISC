//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&广播处理&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.c

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//static pthread_t debug_info_manage_thread_install_id;

void debug_info_print_proc(uint8 pack_type,uint8 dev_type,uint8 dev_id,uint32 remote_port, uint8 cmd, uint8 *buf, uint16 len)
{
	uint8 print_flag=0;
	int8 str_tmp[32]="";
	int8 sec_str_tmp[16]="";

	if(DEBUG_INFO_TYPE_RECV==pack_type)
	{
		sprintf((char *)sec_str_tmp,"recv_%d",remote_port);
	}
	else
	{
		sprintf((char *)sec_str_tmp,"send_%d",remote_port);
	}		
	sprintf((char *)str_tmp,"0x%02x_%d_0x%04x",dev_type,dev_id,cmd);


	//mp3信息显示
	if(GetValueFromEtcFile(DEBUG_CONF_PATH,(const char*)sec_str_tmp,(const char*)str_tmp,(char *)str_tmp,sizeof(str_tmp)-1)==0)
	{
		print_flag=atoi((char *)str_tmp);
	}	

	if(print_flag)
	{
		printf("port: %d, cmd: 0x%04x\r\n",remote_port,cmd);
		print_buf(buf,len);	
	}
}




//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 虚拟设备处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:device.c

//*文件作用:虚拟设备处理

//*文件作者:mmaple

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"

#define PISC_HEAD		(0x7e)



void pisc_pack_data(uint16 dst_train_id, uint8 dst_dev_type, uint8 dst_dev_id,uint32 dst_dev_ip,
									uint16 src_train_id, uint8 src_dev_type, uint8 src_dev_id,uint32 src_dev_ip,
									uint16 cmd,
									uint8 *src_data, uint16 src_len, uint8* dst_data, uint16* dst_len)
{
	tba_bus_pack_t *pisc_cmd_p=(tba_bus_pack_t *)dst_data;
	pisc_cmd_p->header=PISC_HEAD;
	pisc_cmd_p->dst_train_id=dst_train_id;
	pisc_cmd_p->dst_dev_type=dst_dev_type;
	pisc_cmd_p->dst_dev_id=dst_dev_id;	
	pisc_cmd_p->dst_dev_ip=dst_dev_ip;
	pisc_cmd_p->src_train_id=src_train_id;
	pisc_cmd_p->src_dev_type=src_dev_type;
	pisc_cmd_p->src_dev_id=src_dev_id;	
	pisc_cmd_p->src_dev_ip=src_dev_ip;	
	pisc_cmd_p->cmd=cmd;	
	pisc_cmd_p->app_length=src_len;	

	memcpy((uint8 *)pisc_cmd_p->app_data,src_data,src_len);

	*dst_len=(sizeof(tba_bus_pack_t)-PISC_DATA_MAX_SIZE+src_len);
}



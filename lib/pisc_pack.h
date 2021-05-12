//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&定时器0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:timer0.h

//*文件作用:定时器0驱动程序

//*文件作者: mmaple

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PISC_PACK_H
#define PISC_PACK_H

#define PISC_DATA_MAX_SIZE		(1280)
//PISC数据包，数据的开始index
#define PISC_PACK_DATA_INDEX		(sizeof(tba_bus_pack_t)-PISC_DATA_MAX_SIZE)

//控制命令
typedef struct
{
	uint8 header;
	uint16 dst_train_id;	
	uint8 dst_dev_type;
	uint8 dst_dev_id;	
	uint32 dst_dev_ip;
	uint16 src_train_id;	
	uint8 src_dev_type;
	uint8 src_dev_id;	
	uint32 src_dev_ip;
	uint16 cmd;//控制字
	uint16 app_length;//数据长度
	uint8 app_data[PISC_DATA_MAX_SIZE];
}__attribute((packed))tba_bus_pack_t;

//21 byte
#define TAB_BUS_PACK_HEAD_LEN			(sizeof(tba_bus_pack_t)-PISC_DATA_MAX_SIZE)

void pisc_pack_data(uint16 dst_train_id, uint8 dst_dev_type, uint8 dst_dev_id,uint32 dst_dev_ip,
									uint16 src_train_id, uint8 src_dev_type, uint8 src_dev_id,uint32 src_dev_ip,
									uint16 cmd,
									uint8 *src_data, uint16 src_len, uint8* dst_data, uint16* dst_len);


#endif



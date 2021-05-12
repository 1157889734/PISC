//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dev_status.h

//*文件作用:列车通信总线管理器

//*文件作者:mmaple

//*创建日期:2011年1月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#ifndef DEV_STATUS_H
#define DEV_STATUS_H

//巡检表总数
#define DEV_STATUS_TABLE_NUM		192	 //一直重启
//#define DEV_STATUS_TABLE_NUM		228	//  6*(8个动态+ 2个led +1 个lcu + 2个报警器 +) + 2 *(1个tms +1 个媒体 + 3个dva + 1个车头屏)

#define DEV_STATUS_OK				(1)
#define DEV_STATUS_ERROR			(2)
#define DEV_STATUS_NO_DEV		(3)

//虚拟设备
#define DEV_TYPE_ALL		0xFF	//全部设备
#define DEV_ID_ALL			0xFF	//全部设备
#define DEV_IP_ALL			0xFF	//全部设备


//设备类型
#define DEV_TYPE_PISC	(0x01)
#define DEV_TYPE_DCP	(0x02)
#define DEV_TYPE_TMS	(0x03)
#define DEV_TYPE_RECON	(0x04)
#define DEV_TYPE_RECORDER	(0x05)
//#define RESERVE	(0x06)
#define DEV_TYPE_DMP	(0x07)
#define DEV_TYPE_EHP	(0x08)
#define DEV_TYPE_LCU_LED	(0x09) //媒体解码器用0x09
#define DEV_TYPE_HEAD_LED	(0x0A)
#define DEV_TYPE_AMP	(0x0B)
#define DEV_TYPE_SWITCH	(0x0C)
#define DEV_TYPE_PTU	(0x0D)




//初始化设备有效状态
void dev_status_init_dev_valid(uint8 dev_type);
//添加设备到设备状态表
uint8 dev_status_add_dev(uint8 dev_type,uint8 dev_id,uint8 status);

//获得设备状态
uint8 dev_status_get_dev_status(uint8 dev_type,uint8 dev_id);
//设置设备状态
void dev_status_set_dev_status(uint8 dev_type, uint8 dev_id, uint8 status);
void	dev_status_thread_install(void);


#endif

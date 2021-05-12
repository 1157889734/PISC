//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&定时器0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:timer0.h
//*文件作用:定时器0驱动程序
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef MANAGE_H
#define MANAGE_H


#include "sys.h"
#include "dev_status.h"
#include "broadcast.h"
#include "master.h"
#include "log.h"
#include "dva_broadcast_rule.h"
#include "mp3_decoder.h"
#include "train_manage.h"
#include "io_manage.h"
#include "debug_info_manage.h"


//令牌巡检表结构
typedef struct
{
	uint8 dev_id;			//设备号
	uint16 interval;		//巡检间隔 0xFFFF--代表立即巡检  0--代表不参与巡检	
	uint8 poll_error_num;		//错误的巡检数量---通信质量分子
	uint8 poll_error_max_num;		//错误的巡检数量---通信质量分子
	uint8 dev_status;
}__attribute((packed))token_polling_table_t;


#endif


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 广播处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.h

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TRAIN_MANAGE_H
#define TRAIN_MANAGE_H

#define BROADCAST_SET_VOL	(0x0002)
#define BROADCAST_SET_PRI	(0x0007)
#define BROADCAST_SET_TIME	(0x000A)




#define CAB_NUM_MAX	(2)
#define LCU_NUM_MAX	(8)

void train_manage_init(void);
void train_manage_uninit(void);
void	train_manage_thread_install(void);

#endif



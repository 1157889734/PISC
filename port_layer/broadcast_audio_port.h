//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 主函数 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: net_manager.h

//*文件作用: 主函数

//*文件作者: mmaple

//*创建日期: 2009/7
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef BROADCAST_AUDIO_H
#define BROADCAST_AUDIO_H

#define BROADCAST_AUDIO_DATA_SIZE		(1024)


void	 broadcast_audio_thread_install(void);
void broadcast_audio_send_audio(uint8 operate_dev_type, uint8 operate_dev_id,uint8 *buf, uint16 length);

#endif


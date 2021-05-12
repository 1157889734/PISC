//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 读声卡数据 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:read_soundcard.h
//*文件作用:读声卡数据
//*文件作者:mmaple
//*创建日期:2009/11
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//头文件中有SOUNDCARD_H的宏定义
#ifndef SOUNDCARD_H_H
#define SOUNDCARD_H_H

#define AUDIO_SAMPLE_SPEED	(44100)
//音频包样本数量
#define AUDIO_SAMPLE_NUM	512
//音频采样精度
#define AUDIO_SAMPLE_BITS	16
//音频队列长度
#define AUDIO_FIFO_SIZE		10

//音频接收端口
#define AUDIO_RECV_PORT		3000
//每一路音频一包数据的缓冲区大小 单位/short
#define AUDIO_CHANNEL_SIZE_SHORT	(AUDIO_SAMPLE_NUM) //AUDIO_SAMPLE_NUM*10*2 字节
//网络接收一包数据的大小 单位/字节
#define AUDIO_PACKET_SIZE_BYTE (AUDIO_SAMPLE_NUM*AUDIO_SAMPLE_BITS/8) //网络接收数据包 字节数


//通道号定义
#define CHANNEL_LEFT	1
#define CHANNEL_RIGHT	2

//声卡初始化
void soundcard_init(void);
//获取声卡数据
uint8 soundcard_get_data(uint8 channel,uint16 *audio_buf, uint16* audio_len);

#endif




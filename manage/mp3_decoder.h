/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-26 15:12:01
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 11:33:31
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&定时器0驱动程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:timer0.h
//*文件作用:定时器0驱动程序
//*文件作者: mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef MP3_DECODER_H
#define MP3_DECODER_H
//基本数据类型
typedef unsigned char  uint8;                   /* 无符号8位整型变量                        */
typedef volatile unsigned char vuint8;                  /* 无符号16位整型变量                       */
typedef signed   char  int8;                    /* 有符号8位整型变量                        */
typedef unsigned short uint16;                  /* 无符号16位整型变量                       */
typedef volatile unsigned short vuint16;                  /* 无符号16位整型变量                       */
typedef signed   short int16;                   /* 有符号16位整型变量                       */
typedef unsigned int   uint32;                  /* 无符号32位整型变量                       */
typedef signed   int   int32;                   /* 有符号32位整型变量                       */
typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度）                 */
//文件列表最多文件个数
#define FILE_LIST_NUM_MAX		64

//文件列表最大文件名长度
#define FILE_NAME_LENGTH_MAX		256


//int mp3_decoder_play_list(uint8 channel,uint16 *list_buf, uint16 list_num);
int mp3_decoder_play_list(uint8 channel,uint8 *path,uint16 list_num);
void mp3_decoder_stop_play(uint8 channel);
extern void mp3_decoder_set_pcm_data(uint16 *buf, uint16 len);
uint8 mp3_decoder_get_pcm_data(uint16 *buf, uint16 len);
void	mp3_decoder_thread_install(void);
void mp3_decoder_set_run_flag(uint8 onoff);

uint8 mp3_decode_get_is_playing(void);


#endif


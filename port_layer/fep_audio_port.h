/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-11-24 10:22:52
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-11-24 10:29:46
 */
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 主函数 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: net_manager.h

//*文件作用: 主函数

//*文件作者: mmaple

//*创建日期: 2009/7
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef FEP_AUDIO_H
#define FEP_AUDIO_H


void	fep_audio_thread_install(void);
void    UdpMultiCast_Init(void);
void    LCD_UDP_MultiCast(unsigned cmd,char*pdata,unsigned int len);
#endif


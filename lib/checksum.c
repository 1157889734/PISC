/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-27 11:54:50
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 11:54:50
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& crc16  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:crc16.c
//*文件作用:
//*文件作者:mmaple
//*创建日期:2013年1月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"



uint8 checksum(uint8 *buf,uint8 length)
{
	uint8 i=0;
	uint8 checksum=0;
	for(i=0;i<length;i++)checksum+=buf[i];
	return checksum;
}



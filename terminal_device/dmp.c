/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-08 08:37:54
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-08 15:36:04
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.c
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

#define DMP_SET_LIGHT_LEVEL	(0x020C)

void dmp_init(void)
{	

}

void dmp_set_light_level(void)
{
	uint8 temp_str[32]={0};
	if(GetValueFromEtcFile(PISC_CONF_PATH,"dmp","light_level",(char *)temp_str,sizeof(temp_str)-1)==0)
	{
		pisc_set_dmp_light(atoi((const char *)temp_str));
	}	
}




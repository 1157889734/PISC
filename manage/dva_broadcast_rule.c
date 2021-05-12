/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-01 09:13:05
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-11-24 11:21:06
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.c
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

#define DVA_BROADCAST_URGENT_PLAY_NUM_MAX			(FILE_LIST_NUM_MAX)

//预到站
#define DVA_BROADCAST_RULE_PRE_WELCOME_INDEX			(000) //欢迎词，中文，上行，预到站
#define DVA_BROADCAST_RULE_PRE_DIR_INDEX			(100) //中文，上行，预到站
#define DVA_BROADCAST_RULE_PRE_NON_DES_INDEX		(200) //中文，上行，预到站
#define DVA_BROADCAST_RULE_PRE_DES_INDEX				(300) //中文，上行，预到站
#define DVA_BROADCAST_RULE_PRE_END_INDEX			(400) //中文，上行，预到站

//到站
#define DVA_BROADCAST_RULE_ARR_NON_DES_INDEX		(000) //中文，上行，预到站
#define DVA_BROADCAST_RULE_ARR_DES_INDEX				(100) //中文，上行，预到站


void dva_broadcast_rule_init(void)
{

}

void dva_broadcast_rule_get_pre_list(uint16* pre_list, uint8* language_list,uint16* pre_list_num)
{

	uint8 index=0;
	station_info_t station_info;
	pisc_get_station_info(&station_info);
	
	pre_list[index]=DVA_BROADCAST_RULE_PRE_WELCOME_INDEX+station_info.next_station;
	language_list[index]=LANGUAGE_C;
	index++;
	pre_list[index]=DVA_BROADCAST_RULE_PRE_DIR_INDEX+station_info.end_station;
	language_list[index]=LANGUAGE_C;
	index++;
	if(station_info.next_station!=station_info.end_station)
	{
		pre_list[index]=DVA_BROADCAST_RULE_PRE_NON_DES_INDEX+station_info.next_station;
		language_list[index]=LANGUAGE_C;
		index++;
	}
	else
	{
		pre_list[index]=DVA_BROADCAST_RULE_PRE_DES_INDEX+station_info.next_station;
		language_list[index]=LANGUAGE_C;
		index++;
	}
	pre_list[index]=DVA_BROADCAST_RULE_PRE_END_INDEX+station_info.next_station;
	language_list[index]=LANGUAGE_C;
	index++;

	//拼接段数
	*pre_list_num=index;
}

void dva_broadcast_rule_get_arr_list(uint16* list, uint8* language_list, uint16* list_num)
{

	uint8 index=0;
	station_info_t station_info;
	pisc_get_station_info(&station_info);
	
	if(station_info.next_station!=station_info.end_station)
	{
		list[index]=DVA_BROADCAST_RULE_ARR_NON_DES_INDEX+station_info.next_station;
		
		language_list[index]=LANGUAGE_C;
		index++;
	}
	else
	{
		list[index]=DVA_BROADCAST_RULE_ARR_DES_INDEX+station_info.next_station;
		
		language_list[index]=LANGUAGE_C;
		index++;
	}

	//拼接段数
	*list_num=index;
}

void dva_broadcast_rule_get_urgent_list(uint16* list, uint8* language_list, uint16* list_num)
{
	list[0]=broadcast_get_urgent_no();
	language_list[0]=LANGUAGE_C;
	list[1]=broadcast_get_urgent_no();
	language_list[1]=LANGUAGE_E;
	*list_num=1;
}




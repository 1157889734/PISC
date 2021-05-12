//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.h

//*文件作用:数字报站器

//*文件作者:mmaple

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#ifndef DVA_BROADCAST_RULE_H
#define DVA_BROADCAST_RULE_H


void dva_broadcast_rule_get_pre_list(uint16* pre_list, uint8* language_list, uint16* pre_list_num);
void dva_broadcast_rule_get_arr_list(uint16* pre_list,  uint8* language_list,uint16* pre_list_num);
void dva_broadcast_rule_get_urgent_list(uint16* list,  uint8* language_list,uint16* list_num);


#endif




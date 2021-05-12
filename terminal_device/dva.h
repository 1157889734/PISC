//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.h

//*文件作用:数字报站器

//*文件作者:mmaple

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#ifndef DVA_H
#define DVA_H


void dva_init(void);
void dva_play_list(uint16 *list, uint16 list_num);
void dva_stop(void);
void dva_send_audio(void);


#endif



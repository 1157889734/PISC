//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 数字报站器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:dva.h
//*文件作用:数字报站器
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#ifndef LCU_LED_H
#define LCU_LED_H


void lcu_led_init(void);
void lcu_led_show(void);
void lcu_led_roll_sec(void);
//设置客室屏的显示内容
//void lcu_led_set_content(uint8 broadcast_type_c,uint8 language_c);
void lcu_led_set_content(uint8* path,uint16 *list,uint16 list_num);
//发送客室led屏显示内容
void lcu_led_send_content(void);


#endif



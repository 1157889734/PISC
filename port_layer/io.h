//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 半自动处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:halt_ato.c

//*文件作用:列车通信总线管理器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef IO_H
#define IO_H

typedef void (*io_handler_callback)(uint8);

//输入输出管脚定义
#define INPUT_PIN	1
#define OUTPUT_PIN	0

//电平定义
#define IO_LEVEL_HIGH	1
#define IO_LEVEL_LOW	0


#define IO_VALID		(IO_LEVEL_LOW)
#define IO_UNVALID		(IO_LEVEL_HIGH)


//io管脚初始化
void io_init(void);
//设置io管脚电平状态
void io_set_level(uint8 pin_cmd, uint8 level);
//回读io管脚电平状态
uint8 io_get_level(uint8 pin_cmd);
//io管脚初始化
void io_add_pin_init(uint8 pin_cmd,io_handler_callback handler);

#endif



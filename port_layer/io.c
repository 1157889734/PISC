//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 半自动处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:halt_ato.c

//*文件作用:列车通信总线管理器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include"../include/include.h"


//管脚防抖次数
#define ACTIVE_MAX		5

//内核文件:   linux-2.6.30.4/driver/char/sbc2440_my_gpio.c
#define IO_DEVICE_NAME	"/dev/my_gpio"


//该处宏定义的值与内核相同，不可修改
#define CMD_GET_GPF1 	(0x03)
#define CMD_SET_GPF1 	(0x04)
#define CMD_GET_GPF2 	(0x05)
#define CMD_SET_GPF2 	(0x06)
#define CMD_GET_GPF3 	(0x07)
#define CMD_SET_GPF3 	(0x08)
#define CMD_GET_GPF4 	(0x09)
#define CMD_SET_GPF4 	(0x0a)
#define CMD_GET_GPF5 	(0x0b)
#define CMD_SET_GPF5 	(0x0c)
#define CMD_GET_GPF6 	(0x0d)
#define CMD_SET_GPF6 	(0x0e)

#define CMD_GET_GPG0 	(0x11)
#define CMD_SET_GPG0 	(0x12)
#define CMD_GET_GPG2 	(0x23)
#define CMD_SET_GPG2 	(0x24)
#define CMD_GET_GPG3 	(0x13)
#define CMD_SET_GPG3 	(0x14)
#define CMD_GET_GPG5 	(0x15)
#define CMD_SET_GPG5 	(0x16)
#define CMD_GET_GPG6 	(0x17)
#define CMD_SET_GPG6 	(0x18)
#define CMD_GET_GPG7 	(0x19)
#define CMD_SET_GPG7 	(0x1a)
#define CMD_GET_GPG10 	(0x1b)
#define CMD_SET_GPG10 	(0x1c)

#define CMD_GET_GPF7    (0x1d)
#define CMD_GET_GPG1    (0x1e)
#define CMD_GET_GPG4    (0x1f)
#define CMD_GET_GPM0    (0x20)


//IO定义
#define IO_IN_KEY							(CMD_GET_GPG2)
#define IO_IN_OPEN_LEFT_DOOR		(CMD_GET_GPG0)
#define IO_IN_OPEN_RIGHT_DOOR	(CMD_GET_GPG3)
#define IO_IN_CLOSE_DOOR				(CMD_GET_GPG5)
#define IO_IN_OCC							(CMD_GET_GPG6)
#define IO_IN_POWER_1_STATUS		(CMD_GET_GPG7)
#define IO_IN_POWER_2_STATUS		(CMD_GET_GPG10)


static int fd = -1;

void io_init(void)
{
	
	//int fd = open( dev, O_RDWR ); 	  //| O_NOCTTY | O_NDELAY	
	fd=open(IO_DEVICE_NAME, 0 ); 	  //| O_NOCTTY | O_NDELAY	
	if (-1 == fd)	
	{			
		perror("Can't Open Serial Port lala");
		return;		
	}	
	else	
	{	
	}

	//钥匙
	io_add_pin_init(IO_IN_KEY,pisc_local_set_key_status);
	//occ信号
	io_add_pin_init(IO_IN_OCC,occ_set_occ_status);
#if 1
	//开门信号,左
	io_add_pin_init(IO_IN_OPEN_LEFT_DOOR,pisc_set_open_left_door_status);
	//开门信号，右
	io_add_pin_init(IO_IN_OPEN_RIGHT_DOOR,pisc_set_open_right_door_status);
	 //关门信号
	io_add_pin_init(IO_IN_CLOSE_DOOR,pisc_set_close_door_status); 	
	//电源1 状态检测
	io_add_pin_init(IO_IN_POWER_1_STATUS,NULL);
	//电源2 状态检测
	io_add_pin_init(IO_IN_POWER_2_STATUS,NULL); 		 
#endif	
}

void io_set_level(uint8 pin_cmd, uint8 level)
{
	//if (ioctl(fd,CMD_SET_GPF2,level)<0)
	if (ioctl(fd,pin_cmd,level)<0)
	{
		printf("fd set error\r\n");
	}	
	else
	{
		printf("fd set %d\r\n",level);
	}	
}

uint8 io_get_level(uint8 pin_cmd)
{
	int data2 = -1;
	//if (ioctl(fd,CMD_GET_GPF2,&data2)<0)
	if (ioctl(fd,pin_cmd,&data2)<0)
	{
		//qDebug("read mike error");
		
		printf("fd read error\r\n");
	}		
	else
		{
	//printf("read io data: %d\r\n",data2);
		}
	return data2;
}

//初始化io管脚
void io_add_pin_init(uint8 pin_cmd,io_handler_callback handler)
{
	uint8 level;
	level=IO_LEVEL_HIGH;
	io_check_table_init(pin_cmd,UNEQUAL,	level,level,0,ACTIVE_MAX,handler);
}





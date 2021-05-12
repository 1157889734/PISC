/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-01 09:03:44
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-08 16:15:57
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:config.h

//*文件作用:配置
//*文件作者:

//*创建日期:2012、02
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&




#ifndef CONFIG_H
#define CONFIG_H



//#define DIGITAL_PISC
#define TEST_PISC



#define BH_TRUE	(1)
#define BH_FALSE		(0)

#define BH_VALID		(1)
#define BH_UNVALID		(0)


#define PISC_CONF_PATH			("/home/data/conf/conf/pisc.conf")
#define DEBUG_CONF_PATH			("/home/data/conf/conf/debug.conf")

#define HEAD_LED_CONF_PATH			("/home/data/conf/conf/head_led/")
#define LCU_LED_PRE_PATH			("/home/data/conf/conf/lcu_led/pre_1")
#define LCU_LED_ARR_PATH			("/home/data/conf/conf/lcu_led/arr_1")
#define LCU_LED_URGENT_PATH			("/home/data/conf/conf/lcu_led/urgent")
#define LCU_LED_ADV_PATH			("/home/data/conf/conf/lcu_led/adv")	//广告
#define MP3_PRE_PATH			("/home/data/conf/mp3/pre_1")
#define MP3_ARR_PATH			("/home/data/conf/mp3/arr_1")
#define MP3_URGENT_PATH			("/home/data/conf/mp3/urgent")
#define MP3_ADV_PATH			("/home/data/conf/mp3/adv")//广告



#define NET_IP_HEAD		     "168.168.102."
#define NET_MAC_HEAD		"10:23:45:67:88:"

//语言类型
#define LANGUAGE_C	(1) //中文
#define LANGUAGE_E	(2) //英文

//颜色
#define COLOR_R	(1) //红
#define COLOR_G	(2) //绿
#define COLOR_Y	(3) //黄


#ifdef DIGITAL_PISC
#define PRODECT_NAME		("数字pisc 中央控制器")

//硬件版本号 1 2....99
#define HARDWARE_VERSION		1
//增加功能版本号
#define ADD_FUNCTION_VERSION	0
//大版本号
#define MAJOR_VERSION			1
//小版本号
#define MINOR_VERSION			1

#endif

#ifdef TEST_PISC
#define PRODECT_NAME		("测试pisc 中央控制器")

//硬件版本号 1 2....99
#define HARDWARE_VERSION		0
//增加功能版本号
#define ADD_FUNCTION_VERSION	0
//大版本号
#define MAJOR_VERSION			1
//小版本号
#define MINOR_VERSION			1

#endif




#endif



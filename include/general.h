/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-26 15:12:01
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 10:49:48
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:general.h

//*文件作用:

//*文件作者:

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#ifndef		GENERAL_H
#define		GENERAL_H



#define		FALSE	0
#define		TRUE	1

#define	EQUAL		1
#define	UNEQUAL		2
#define	WHATEVER		3
#define	BIGGER		4
#define	SMALLER		5


#define	BIT_1	0x1
#define	BIT_2	0x3
#define	BIT_3	0x7
#define	BIT_4	0xF
#define	BIT_5	0x1F
#define	BIT_6	0x3F
#define	BIT_7	0x7F
#define	BIT_8	0xFF
#define	BIT_9	0x01FF
#define	BIT_10	0x03FF
#define	BIT_11	0x07FF
#define	BIT_12	0x0FFF
#define	BIT_13	0x1FFF
#define	BIT_14	0x3FF
#define	BIT_15	0x7FFF
#define	BIT_16	0xFFFF


#ifndef NULL
#define NULL	(void *)0
#endif



//KEILC51编译器兼容定义
#define xdata
#define data
#define idata
#define code const

//基本数据类型
typedef unsigned char  uint8;                   /* 无符号8位整型变量                        */
typedef volatile unsigned char vuint8;                  /* 无符号16位整型变量                       */
typedef signed   char  int8;                    /* 有符号8位整型变量                        */
typedef unsigned short uint16;                  /* 无符号16位整型变量                       */
typedef volatile unsigned short vuint16;                  /* 无符号16位整型变量                       */
typedef signed   short int16;                   /* 有符号16位整型变量                       */
typedef unsigned int   uint32;                  /* 无符号32位整型变量                       */
typedef signed   int   int32;                   /* 有符号32位整型变量                       */
typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度）                 */


#define B0000 0x00
#define B0001 0x01
#define B0010 0x02
#define B0011 0x03
#define B0100 0x04
#define B0101 0x05
#define B0110 0x06
#define B0111 0x07
#define B1000 0x08
#define B1001 0x09
#define B1010 0x0A
#define B1011 0x0B
#define B1100 0x0C
#define B1101 0x0D
#define B1110 0x0E
#define B1111 0x0F



//在预处理时自动生成变量名称 packet_##fn---把不同的fn替换到packet_后面
#define __packetlist(nm, fn) \
static packetlist_t packet_##fn={nm, fn}



#endif


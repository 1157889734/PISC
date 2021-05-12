/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-11-23 09:54:57
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-11-24 08:45:06
 */

//#include "emergent.h"
#include "../include/include.h"

#if 0
//过程数据处理表
typedef struct
{
	uint8 				char_index;				//字节索引
	uint8 				bit_index;				//位索引
	uint16 				bit_num;				//位长度
	uint8 				judge;					//判断

	uint16				value;					//判断用值,equal时用
	MsgHandler_Callback 	msghandler ; 			//Callback指针
}dev_proc_t;
#endif
#define GROUPIP "225.0.0.10"
#define LOCAL_PORT 9999
#define DEST_PORT  8888 

typedef struct  _Emergent_Process_t{
uint8_t  emergen_stat;//报警状态
uint8_t  run_stat;//运行状态
uint8_t  mic_volum;//画图音量
uint8_t  talk_volum;//对讲监听音量
uint32_t  softver;//程序版本
uint8_t   noise_val;//噪声值

}ST_Emergen_PorcessData_t;

static void emergen_ctrl_proc(tba_bus_pack_t *ReceiveMsg);
static void emergen_ctrl_proc_talk(tba_bus_pack_t *ReceiveMsg);
static void emergen_ctrl_proc_stop(tba_bus_pack_t *ReceiveMsg);
void EmergenCyUdpMultiCast_Init(void);
void Emergency_UDP_MultiCast(char*pdata,unsigned int len);

static uint8_t emergency_status = 0x00;
//主状态处理矩阵
static dev_proc_t  emergen_process_table_M[] =
{
	{TAB_BUS_PACK_HEAD_LEN+0,		6,		BIT_2,	EQUAL,	0x01,	(MsgHandler_Callback)emergen_ctrl_proc},	//紧急报警控制
    {TAB_BUS_PACK_HEAD_LEN+0,		6,		BIT_2,	EQUAL,	0x02,	(MsgHandler_Callback)emergen_ctrl_proc_talk},	//紧急报警控制
    {TAB_BUS_PACK_HEAD_LEN+0,		6,		BIT_2,	EQUAL,	0x00,	(MsgHandler_Callback)emergen_ctrl_proc_stop},	//紧急报警控制
	{0,		0,		0,		0,			0,		NULL}
};
static dev_proc_t emergen_process_table_S[] = 
{	
	{0,	0,	0,	0,		0,	NULL	}
};

static mode_matrix_t   emergen_process_table[]=
{
	{PISC_STATUS_M, emergen_process_table_M},			//主状态处理矩阵
    {PISC_STATUS_S, emergen_process_table_S},			//C状态处理矩阵
	{0, 0,},
};

//状态基索引
static cmd_matrix_t  pisc_process_table[]=
{
	{CTRL_CMD_DEFAULT,  emergen_process_table,},			//主状态处理矩阵
	{0, NULL,},
};
void EmerGentInit(void)
{
  EmergenCyUdpMultiCast_Init();
  data_proc_matrix_add_device_to_matrix_table(DEV_TYPE_EHP, pisc_process_table);
}


static void emergen_ctrl_proc(tba_bus_pack_t *ReceiveMsg)
{
    char ac_pecu[30]={0x00,0x14,0x00};
	ST_Emergen_PorcessData_t *ST_emergent=(ST_Emergen_PorcessData_t *)ReceiveMsg->app_data;
    emergency_status = 0x01;
    Emergency_UDP_MultiCast(ac_pecu,30);
}

static void emergen_ctrl_proc_talk(tba_bus_pack_t *ReceiveMsg)
{
    char ac_pecu[30]={0x00,0x14,0x00};
    emergency_status = 0x02;
	ST_Emergen_PorcessData_t *ST_emergent=(ST_Emergen_PorcessData_t *)ReceiveMsg->app_data;
    printf("================Emergent Talk....\r\n");
     Emergency_UDP_MultiCast(ac_pecu,30);
   
}


static void emergen_ctrl_proc_stop(tba_bus_pack_t *ReceiveMsg)
{
    char ac_pecu[30]={0x00,0x00,0x00};
    emergency_status = 0x00;
	ST_Emergen_PorcessData_t *ST_emergent=(ST_Emergen_PorcessData_t *)ReceiveMsg->app_data;
    //printf("================emergen_ctrl_proc_stop....\r\n");
    Emergency_UDP_MultiCast(ac_pecu,30);
   
}






static int Emergen_multicastSocket =0;
int UdpMultiCastSenderinit(uint8_t *mulicst_ip,uint16_t port)
{
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);/* open a socket. only udp support multicast */
	if (-1 == sockfd)
	{
		printf("[initUdpMultiCastSender]socket fail\n");
		return -1;
	}
 
	struct sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = inet_addr(mulicst_ip); /* multicast group ip */ 
	int bindret = bind(sockfd,(struct sockaddr *)(&myaddr),sizeof(struct sockaddr));
	if (-1 == bindret)
	{
		perror("[initUdpMultiCastSender]bind fail\n");
		close(sockfd);
		return -1;
	}
	return sockfd;
}





int  UdpMutiCast_SendData(int sockfd,char*pdata,unsigned int len)
{
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = inet_addr(GROUPIP);
	destAddr.sin_port = htons(DEST_PORT);
	int sendLen =sendto(sockfd,pdata,len,0,(struct sockaddr *)(&destAddr),sizeof(struct sockaddr));
	return sendLen;

}
void EmergenCyUdpMultiCast_Init(void)
{

   Emergen_multicastSocket = UdpMultiCastSenderinit(GROUPIP,LOCAL_PORT);
   //printf("void EmergenCy mulitcast.........%d\n",Emergen_multicastSocket);

}


static const unsigned int crc8Table[256] =
{
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};
// 计算CRC
unsigned char calcCRC(unsigned char *datax, unsigned int len)
{
	unsigned char crc8 = 0;

    while (len --)
    {
        crc8 = crc8 ^(*datax++);
        crc8 = crc8Table[crc8];
    }

    return crc8;
}

void Emergency_UDP_MultiCast(char*pdata,unsigned int len)
{
   int i = 0;
   unsigned short dat_len = len;
   unsigned char udp_multidata[100]={0x00};
   udp_multidata[0] = 0xFA;//STX
   udp_multidata[1] = 0X00;//VER
   udp_multidata[2] = 0X88;//SRCADDR
   udp_multidata[3] = 0XFF;//all dev
   udp_multidata[4] = 0X00;//FUNC_
   udp_multidata[5] = 0x05;//FUNC
   udp_multidata[6] = len&0xff;//LEN
   udp_multidata[7] = (len>>8)&0xff;
   udp_multidata[8] = calcCRC(pdata,len);//
   for(i = 0; i<len;++i)
   {
        udp_multidata[9+i] = pdata[i];
   }
   udp_multidata[9+i] = 0xFC;
   

   UdpMutiCast_SendData(Emergen_multicastSocket,udp_multidata,dat_len+10);
}


uint8_t EmerGencyStatusGet(void)
{

   return emergency_status;
    
}
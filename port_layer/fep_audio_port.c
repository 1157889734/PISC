//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 网管通信 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: net_manager.c
//*文件作用: 网管通信
//*文件作者: mmaple
//*创建日期: 2009/7
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//广播音频
#define FEP_AUDIO_LOCAL_PORT		(53152)
#define FEP_AUDIO_REMOTE_PORT	    (53151)
#define FEP_AUDIO_REMOTE_IP		("168.168.102.255")

static socklen_t socklen=0;
static int cmd_socket_fd=-1;
static struct sockaddr_in localaddr,remoteaddr;


static pthread_t fep_audio_thread_id; 		//pa总线链路层

//***********************************************************************************************************************
//函数作用:创建幽灵进程
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static void*  fep_audio_thread(void* param)
{	
	uint8 cmd_recv_buf[64];//接收缓冲区
	int length=0;
	
	printf("fep_audio_thread start....\r\n");
	
	
	//送SOCKET地址结构的长度
	socklen = sizeof(struct sockaddr_in);	
	//创建UDP套接字	
	cmd_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(cmd_socket_fd<0)
	{
		printf("socket creating err in udptalk\n");
		exit(1);
	}			
	//填充本地IP和端口
	memset(&localaddr,0,socklen);
	localaddr.sin_family=AF_INET;
	localaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	localaddr.sin_port=htons(FEP_AUDIO_LOCAL_PORT);
	//填充对方IP和端口--发送的时候有用
	memset(&remoteaddr,0,socklen);
	remoteaddr.sin_family=AF_INET;	

	//绑定本地端口
	if(bind(cmd_socket_fd, (struct sockaddr *)&localaddr, socklen)<0)
	{	
		printf("bind local address err in udptalk!\n");
		exit(2);
	}


//等待MP3解码出足够的包后，再接收心跳，现场实际使用时应该不需要
#if 0
	while(1)
	{
		if(mp3_decoder_get_init_flag())
		{
			break;
		}
		usleep(100*1000);
		//printf("wait for mp3 init...\r\n");		
	}
	printf("mp3 init ok...\r\n"); 	
#endif	

	while(1)
	{		

		//读取socket		
		length=recvfrom(cmd_socket_fd, cmd_recv_buf,sizeof(cmd_recv_buf), 0, (struct sockaddr *)&remoteaddr, &socklen);
		
		//判断是否有数据
		if(length>0)
		{
			//printf("recv fep: %d\r\n",length);
			//print_buf(cmd_recv_buf,length);
			//根据当前广播类型，调用播放函数
			broadcast_send_audio();

		}
	}
	

	
}
   
   
void	fep_audio_thread_install(void)
{
	//创建线程
	//pthread_create(&fep_audio_thread_id, NULL, (void*)fep_audio_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&fep_audio_thread_id,  fep_audio_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}

                                          

#define GROUPIP "239.2.1.1"
#define LOCAL_PORT 50153
#define DEST_PORT  50152 

int multicastSocket =0;
int initUdpMultiCastSender()
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
	myaddr.sin_port = htons(LOCAL_PORT);
	myaddr.sin_addr.s_addr = inet_addr(GROUPIP); /* multicast group ip */ 
	int bindret = bind(sockfd,(struct sockaddr *)(&myaddr),sizeof(struct sockaddr));
	if (-1 == bindret)
	{
		perror("[initUdpMultiCastSender]bind fail\n");
		close(sockfd);
		return -1;
	}
	return sockfd;
}





int  UdpMutiCast_Send(int sockfd,char*pdata,unsigned int len)
{
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = inet_addr(GROUPIP);
	destAddr.sin_port = htons(DEST_PORT);
	int sendLen =sendto(sockfd,pdata,len,0,(struct sockaddr *)(&destAddr),sizeof(struct sockaddr));
	return sendLen;

}
void UdpMultiCast_Init(void)
{

   multicastSocket = initUdpMultiCastSender();
   printf("void UdpMultiCast_Init.........%d\n",multicastSocket);

}

void LCD_UDP_MultiCast(unsigned cmd,char*pdata,unsigned int len)
{
   int i = 0;
   unsigned short dat_len = len;
   unsigned char udp_multidata[100]={0x00};
   udp_multidata[0] = 0x7E;
   udp_multidata[1] = cmd;
   udp_multidata[2] = dat_len&0xff;
   udp_multidata[3] = (dat_len>>8)&0xff;
   for(i = 0;i<dat_len;++i)
   {
	   udp_multidata[4+i] =pdata[i];
   }
   UdpMutiCast_Send(multicastSocket,udp_multidata,dat_len+4);
}
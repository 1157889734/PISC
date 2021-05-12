//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 网管通信 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: net_manager.c

//*文件作用: 网管通信

//*文件作者: mmaple

//*创建日期: 2009/7
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#include "../include/include.h"

//广播音频
#define RECV_CTRL_LOCAL_PORT		(50152)
#define CTRL_REMOTE_PORT	(50152)
#define CTRL_REMOTE_IP		("168.168.102.255")
//#define CTRL_REMOTE_IP_HEX		(0xff65a8c0)
#define CTRL_REMOTE_IP_HEX		(0xa8a866ff)

//发送时的本地端口号
#define SEND_CTRL_LOCAL_PORT		(50151)



static socklen_t socklen=0;
static int cmd_socket_fd=-1;
//static struct sockaddr_in localaddr,remoteaddr;


static socklen_t send_socklen=0;
static int send_cmd_socket_fd=-1;
static struct sockaddr_in send_localaddr,send_remoteaddr;


static pthread_t ctrl_port_thread_id;

//***********************************************************************************************************************
//函数作用:向中心网管发送数据
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static void ctrl_port_send_data(uint8 *buf, uint16 length)
{
	send_remoteaddr.sin_port=htons(CTRL_REMOTE_PORT);
	if(inet_pton(AF_INET,CTRL_REMOTE_IP,&send_remoteaddr.sin_addr)<=0)
	{
		printf("Wrong source IP address: %s\n",CTRL_REMOTE_IP);
		return;
	}
	//printf("ctrl_port_send_data:\r\n");
	//print_buf(buf,length);
	//发送
	sendto(send_cmd_socket_fd,buf,length,0,(struct sockaddr *)&send_remoteaddr,send_socklen);
}

//***********************************************************************************************************************
//函数作用:
//参数说明:
//注意事项:
//返回说明:
//***********************************************************************************************************************
void ctrl_port_send_cmd(uint16 dst_train_id, uint8 dst_dev_type, uint8 dst_dev_id,uint16 cmd, uint8 *buf, uint16 len)
{
	uint8 send_buf_tmp[PISC_DATA_MAX_SIZE];
	uint16 send_len=0;

	//封包
	pisc_pack_data(dst_train_id,dst_dev_type,dst_dev_id,CTRL_REMOTE_IP_HEX,
								0x0001,DEV_TYPE_PISC,pisc_local_get_my_dev_id(),pisc_local_get_my_dev_ip(),
								cmd,
								buf,len,send_buf_tmp,&send_len);

	//发送
	ctrl_port_send_data(send_buf_tmp,send_len);
	//打印显示
	debug_info_print_proc(DEBUG_INFO_TYPE_SEND,dst_dev_type,dst_dev_id,CTRL_REMOTE_PORT,cmd,send_buf_tmp,send_len);

}

static void ctrl_port_send_init(void)
{
	int 	    yes;
	int                set;	

	//送SOCKET地址结构的长度
	send_socklen = sizeof(struct sockaddr_in);	
	//创建UDP套接字	
	send_cmd_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(send_cmd_socket_fd<0)
	{
		printf("socket creating err in udptalk\n");
		exit(1);
	}			

	yes = 1;
	set = setsockopt(send_cmd_socket_fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
	
	//填充本地IP和端口
	memset(&send_localaddr,0,socklen);
	send_localaddr.sin_family=AF_INET;
	send_localaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	send_localaddr.sin_port=htons(SEND_CTRL_LOCAL_PORT);
	//填充对方IP和端口--发送的时候有用
	memset(&send_remoteaddr,0,send_socklen);
	send_remoteaddr.sin_family=AF_INET;	

	//绑定本地端口
	if(bind(send_cmd_socket_fd, (struct sockaddr *)&send_localaddr, send_socklen)<0)
	{	
		printf("send_bind local address err in udptalk, port: %d\r\n",send_localaddr.sin_port);
		exit(2);
	}


}

//***********************************************************************************************************************
//函数作用:创建幽灵进程
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static void* ctrl_port_thread(void* param)
{	
	uint8 cmd_recv_buf[1024];//接收缓冲区
	tba_bus_pack_t *tba_bus_p=(tba_bus_pack_t *)cmd_recv_buf;
	int length=0;
	static struct sockaddr_in localaddr,remoteaddr;

	int 	    yes;
	int                set;	
	printf("ctrl_port_thread start....\r\n");
	
	
	//送SOCKET地址结构的长度
	socklen = sizeof(struct sockaddr_in);	
	//创建UDP套接字	
	cmd_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(cmd_socket_fd<0)
	{
		printf("socket creating err in udptalk\n");
		exit(1);
	}			

	yes = 1;
	set = setsockopt(cmd_socket_fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
	
	//填充本地IP和端口
	memset(&localaddr,0,socklen);
	localaddr.sin_family=AF_INET;
	localaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	localaddr.sin_port=htons(RECV_CTRL_LOCAL_PORT);
	//填充对方IP和端口--发送的时候有用
	memset(&remoteaddr,0,socklen);
	remoteaddr.sin_family=AF_INET;	

	//绑定本地端口
	if(bind(cmd_socket_fd, (struct sockaddr *)&localaddr, socklen)<0)
	{	
		printf("bind local address err in udptalk, port: %d\r\n",localaddr.sin_port);
		exit(2);
	}

	ctrl_port_send_init();

	while(1)
	{		
		//读取socket		
		length=recvfrom(cmd_socket_fd, cmd_recv_buf,sizeof(cmd_recv_buf), 0, (struct sockaddr *)&remoteaddr, &socklen);
		
		//判断是否有数据
		if(length>0)
		{
			//if(tba_bus_p->src_dev_type==0x01)
			{
				//printf("recv pisc data, dev_type: 0x%x dev_id: %d, cmd: 0x%xtba_bus_p->cmd\r\n",tba_bus_p->src_dev_type,tba_bus_p->src_dev_id,tba_bus_p->cmd);
				//print_buf(cmd_recv_buf,20);
			}
			//printf("remote ip:%s tba_bus_p->src_dev_type:%d\r\n",inet_ntoa(remoteaddr.sin_addr),tba_bus_p->src_dev_type);
			//打印显示
			debug_info_print_proc(DEBUG_INFO_TYPE_RECV,tba_bus_p->src_dev_type,tba_bus_p->src_dev_id,RECV_CTRL_LOCAL_PORT,tba_bus_p->cmd,(uint8 *)tba_bus_p,TAB_BUS_PACK_HEAD_LEN+tba_bus_p->app_length);
			//设备状态ok

			dev_status_set_dev_status(tba_bus_p->src_dev_type,tba_bus_p->src_dev_id,DEV_STATUS_OK);
			//处理数据
			data_proc_matrix_Dispatch(tba_bus_p->src_dev_type,tba_bus_p->src_dev_id,tba_bus_p->cmd,(uint8 *)tba_bus_p,TAB_BUS_PACK_HEAD_LEN+tba_bus_p->app_length,0);
		}			
	}
}
void	ctrl_port_thread_install(void)
{
	//创建线程
	//pthread_create(&ctrl_port_thread_id, NULL, (void*)ctrl_port_thread, NULL); 	//PA内部协议数据采集线程创建
	ThreadCreate(&ctrl_port_thread_id, ctrl_port_thread, NULL); 	//PA内部协议数据采集线程创建
	return;
}


                                          

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 广播处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:broadcast.h

//*文件作用:广播处理

//*文件作者:mmaple

//*创建日期:2011年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DEBUG_INFO_MANAGE_H
#define DEBUG_MANAGE_H

#define DEBUG_INFO_TYPE_RECV		(1)
#define DEBUG_INFO_TYPE_SEND		(2)

void debug_info_print_proc(uint8 pack_type,uint8 dev_type,uint8 dev_id, uint32 remote_port, uint8 cmd, uint8 *buf, uint16 len);


#endif



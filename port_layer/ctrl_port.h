//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 主函数 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: net_manager.h

//*文件作用: 主函数

//*文件作者: mmaple

//*创建日期: 2009/7
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef CTRL_PORT_H
#define CTRL_PORT_H


//void ctrl_port_send_cmd(uint16 cmd, uint8 *buf, uint16 len);
void ctrl_port_send_cmd(uint16 dst_train_id, uint8 dst_dev_type, uint8 dst_dev_id,uint16 cmd, uint8 *buf, uint16 len);
void	ctrl_port_thread_install(void);

#endif


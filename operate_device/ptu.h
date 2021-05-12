//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& PC设置软件 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:setup.h

//*文件作用:PC设置软件

//*文件作者:mmaple

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PTU_H
#define PTU_H

#define PTU_DIR_UP			0
#define PTU_DIR_DOWN		1

void ptu_init(void);
void ptu_send_log_dev_status_packet(uint8 *buf, uint16 len);

void ptu_stop_tba_bus_send_pack(void);
void ptu_resume_tba_bus_send_pack(void);
void ptu_send_dev_status(uint8 *buf, uint16 len);

//通过pisc_id获得对应的车厢号
uint8 ptu_get_cab_id_from_dev_id(uint8 dev_id);

#endif

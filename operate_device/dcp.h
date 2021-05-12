/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-11 11:18:52
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-11 11:24:59
 */ 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车通信总线接口 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tba_bus.h

//*文件作用:列车通信总线接口

//*文件作者:

//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DCP_H
#define DCP_H

#define	MIC_NORMAL		0
#define	MIC_REQ			2
#define	MIC_WORKING		1

void dcp_init(void);

uint8_t get_live_bradcast();
#endif



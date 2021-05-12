/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-12-03 15:10:20
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-03 15:10:20
 */
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 总线管理器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: tba_bus_process.h

//*文件作用: 内部协议接收

//*文件作者: mmaple

//*创建日期: 2009/06
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef DATA_PROC_MATRIX
#define DATA_PROC_MATRIX

#define CTRL_CMD_DEFAULT			(0x0001)
#define CTRL_CMD_VOLUME_CTRL	    (0x0002)
#define CTRL_CMD_FEP		        (0x0003)


//typedef void (*MsgHandler_Callback)(uint8* buf );
typedef void (*MsgHandler_Callback)(tba_bus_pack_t *);


typedef void (*fun_p)(uint8* buf );


//过程数据处理表
typedef struct
{
	uint8 				char_index;				//字节索引
	uint8 				bit_index;				//位索引
	//uint8 				bit_num;				//位长度
	uint16 				bit_num;				//位长度
	uint8 				judge;					//判断
	//uint8				value;					//判断用值,equal时用
	uint16				value;					//判断用值,equal时用
	MsgHandler_Callback 	msghandler ; 			//Callback指针
}dev_proc_t;

typedef struct
{
	uint8 				pisc_master_mode;
	dev_proc_t*			dev_proc_table;
}mode_matrix_t;

typedef struct
{
	uint16 				cmd;
	mode_matrix_t*			mode_matrix_table;
}cmd_matrix_t;

typedef struct
{	
	uint8				dev_type;					//设备id	
	cmd_matrix_t*		cmd_matrix_table;
}dev_matrix_t;


void data_proc_matrix_init(void);
void data_proc_matrix_init_default_data(uint8 dev_type, uint8 dev_id);
void data_proc_matrix_add_device_to_matrix_table(uint8 device_type, cmd_matrix_t *mode_cmd_tbl);
void data_proc_matrix_Dispatch(uint8 dev_type, uint8 dev_id,uint16 cmd, uint8 *buf, uint16 len, uint8 matrix_index_len);
void data_proc_matrix_set_default_data(tba_bus_pack_t *ReceiveMsg);



#endif



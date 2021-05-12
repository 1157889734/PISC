/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-12-03 15:08:41
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-04 13:28:00
 */
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 列车总控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:tms.h

//*文件作用:列车总控制器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef TMS_H
#define TMS_H

//tms时间
typedef struct
{
	uint16 year;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}__attribute((packed))tms_time_t;


//接收数据包结构,60字节
typedef struct
{
	uint16 train_id;//
	uint8 line_no; //
	uint8 dir; // 10-11
	uint8 current_station;
	uint8 next_station; //12-13
	uint8 start_station; // 14-15
	uint8 end_station; // 16-17
	uint16 to_current_station_dis; //12-13
	uint16 to_next_station_dis; // 14-15
	struct//28-29
	{
		 uint8 current_station_valid:1; //28	 
		 uint8 next_station_valid:1; //上行
		 uint8 start_station_valid:1; //28	 
		 uint8 end_station_valid:1; //上行
		 uint8 to_current_station_dis_valid:1; //12-13
		 uint8 to_next_station_dis_valid:1; // 14-15
		 uint8 atc_valid:1; //
		 uint8 time_valid:1; //29
	}__attribute((packed))signal_1;

	tms_time_t tms_time;

	uint8 doorside; // 20-21
	uint8 open_door;
	uint8 close_door_well; // 22-23
	uint8 speed_30km_valid; // 24-25
	uint8 speed_5km_valid; // 24-25
	uint8 train_stop_status; // 24-25
	uint16 train_position; // 24-25
	uint8 manual_flag; // 24-25
	uint8 pre_flag; // 24-25
	uint8 arr_flag; // 24-25
	uint8 manual_broadcast_stop; // 24-25
	uint16 urgent_msg_id; // 18-19
	uint8 urgent_flag; // 24-25
	uint16 train_speed; // 24-25
	uint8 cycle_broadcast_flag; // 18-19
	uint8 project_id;
	uint8 skip_flag;
	uint8 skip_station[8];

	uint8 cab_1_door_release;
	//uint8 lcu_door_release[LCU_NUM_MAX];
	uint8 cab_2_door_release;

	struct//60
	{
		 uint8 test_broadcast_flag:1; 
		 uint8 test_head_led:1;
		 uint8 test_lcu_led:1;
		 uint8 test_dmp:1;
		 uint8 reserve:4;
	}__attribute((packed))signal_2;
}__attribute((packed))tms_recv_pack_t;


//tms设备初始化
void tms_init(void);


#endif



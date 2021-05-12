//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 中央控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:pisc.h
//*文件作用:中央控制器
//*文件作者:
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PISC_H
#define PISC_H


#define PISC_STATUS_M		1
#define PISC_STATUS_S		0


//PISC工作模式
#define PISC_ATC_MODE		      0x01
#define PISC_DCP_MANUAL_MODE      0x02	
#define PISC_HALF_AUTO_MODE		      0x03


//开关门 左右侧
#define PISC_DOOR_LEFT 2
#define PISC_DOOR_RIGHT 1

//上下行
#define PISC_DIR_UP		2
#define PISC_DIR_DOWN	1

#define PISC_DIR_UP_GO_AHEAD_CAL(x)		(x+1)
#define PISC_DIR_DOWN_GO_AHEAD_CAL(x)		(x-1)
#define PISC_DIR_UP_GO_BACK_CAL(x)		(x-1)
#define PISC_DIR_DOWN_GO_BACK_CAL(x)		(x+1)


//第23字节，要设置的关键字
#define PISC_SETUPCMD_NONE               0x00//无效的关键字
#define PISC_SETUPCMD_MAP_INTIAL_STATION 0x01//动态地图的初始站
#define PISC_SETUPCMD_MAP_BRIGHTNESS     0x02//动态地图的亮度



typedef struct
{
	uint8 start_station;//起始站2
	uint8 end_station;//终点站3
	uint8 current_station;//当前站4
	uint8 next_station;//下一站5
}__attribute((packed))station_info_t;

//过程数据结构1
typedef struct
{
	//广播控制位
	struct //1-4
	{
		uint8 atc_mode_flag:1;
		uint8 tms_mode_flag:1;
		uint8 dcp_mode_flag:1;
		uint8 reserve:3;	// 1
		uint8 active_flag:1;
		uint8 master_flag:1;

		uint8 test:2;// 2
		uint8 close_right_door_flag:1;
		uint8 close_left_door_flag:1;
		uint8 open_right_door_flag:1;
		uint8 open_left_door_flag:1;
		uint8 run_dir:2;// 1 下行，2上行

		uint8 reserve2:6;	// 3
		uint8 arr_flag:1;	//
		uint8 pre_flag:1;

		uint8 reserve3:1;// 4
		uint8 dcp_arr_broadcasting:1;	 //
		uint8 dcp_pre_broadcasting:1;
		uint8 dcp_urgent_broadcasting:1;	 //
		uint8 reserve4:4;
	}__attribute((packed))signal_1;

	station_info_t station_info;	//5-8
	uint8 status;		// 9
	uint8 dmp_light_level;	// 10
	uint8 lcu_led_roll_sec; // 11
	uint32 jump_stations;	//13-16
	uint32 version;	// 12
}__attribute((packed))pisc_default_data_t;



void pisc_init(void);
uint8 pisc_get_other_pisc_key_status(void);
void pisc_set_other_pisc_key_status(uint8 status);
uint8 pisc_get_other_pisc_master_status(void);
void pisc_set_other_pisc_master_status(uint8 status);


#endif



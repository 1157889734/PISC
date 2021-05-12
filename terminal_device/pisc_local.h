/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-11-24 10:22:52
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-12-15 16:20:40
 */
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 中央控制器 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:pisc.h
//*文件作用:中央控制器
//*文件作者:
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef PISC_LOCAL_H_A
#define PISC_LOCAL_H_A

#ifdef _OSCAR_DEBUG_ //方便参考结构体
struct tm 
{
	int tm_sec;         /* seconds */
	int tm_min;         /* minutes */
	int tm_hour;        /* hours */
	int tm_mday;        /* day of the month */
	int tm_mon;         /* month */
	int tm_year;        /* year */
	int tm_wday;        /* day of the week */
	int tm_yday;        /* day in the year */
	int tm_isdst;       /* daylight saving time */
};
#endif // _OSCAR_DEBUG_



typedef enum
{
	MANUAL_LED_BIT=0,
	TMS_LED_BIT,
	ATC_LED_BIT,
	OCC_LED_BIT,
	KEY_LED_BIT,
	MEDIA_LED_BIT,
	MP3_LED_BIT,
	MIC_LED_BIT,
}pisc_led_bit_e;


void pisc_local_init(void);
void pisc_init_process_data(void);
void pisc_local_proc(void);
uint8 pisc_local_get_my_dev_id(void);
uint8 pisc_local_get_other_dev_id(void);
uint32 pisc_local_get_my_dev_ip(void);
uint16 pisc_local_get_my_train_id(void);
void pisc_local_set_key_status(uint8 flag);
uint8 pisc_local_get_key_status(void);
void pisc_set_station_info(station_info_t station_info_c);
void pisc_get_station_info(station_info_t *station_info_c);
void pisc_set_work_mode(uint8 mode);
uint8 pisc_get_work_mode(void);
uint8 pisc_get_master_status(void);
void pisc_set_master_status(uint8 master_flag_c);
void pisc_set_updown(uint8 updown);
uint8 pisc_get_updown(void);
void pisc_set_pre_flag(uint8 flag);
uint8 pisc_get_pre_flag(void);
void pisc_set_arr_flag(uint8 flag);
uint8 pisc_get_arr_flag(void);
void pisc_set_dmp_light(uint8 light_level);
uint8 pisc_get_dmp_light(void);
void pisc_set_lcu_led_roll_sec(uint8 roll_sec);
uint8 pisc_get_lcu_led_roll_sec(void);

uint8 pisc_get_open_left_door_status(void);
void pisc_set_open_left_door_status(uint8 status_c);
uint8 pisc_get_open_right_door_status(void);
void pisc_set_open_right_door_status(uint8 status_c);
uint8 pisc_get_close_left_door_status(void);
uint8 pisc_get_close_right_door_status(void);
void pisc_set_close_door_status(uint8 status_c);

void pisc_local_send_default_data(void);
void pisc_local_set_time(uint8 *time_buf, uint16 time_len);
void pisc_local_get_time(uint8 *time_buf);
void pisc_local_send_time(void);
uint32 pisc_get_jump_stations(void);
void pisc_set_jump_stations(uint32 jump_stations_c);
void pisc_update_jump_stations_from_conf(void);

pisc_default_data_t *	get_local_pisdata(void);//by sunzhguy

#endif



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 主函数 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称: main.c
//*文件作用: 主函数
//*文件作者:  mmaple
//*创建日期: 2012年3月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include "../include/include.h"

#define CHIP_ADDR   0x20
#define I2C_DEV_PATH "/dev/i2c-0"

#define LED_ON_VALUE		(0)	//0值表示点亮
#define LED_OFF_VALUE		(1)


static int i2c_fd;

static uint8 led_onoff_status=0xff;

void led_init(void)
{
	int error = 0;
	i2c_fd = open(I2C_DEV_PATH, O_RDWR);
	if (i2c_fd < 0)
	{
		printf("i2c open failed %d\n", CHIP_ADDR);
		return;
	}
	error = ioctl(i2c_fd, I2C_SLAVE, CHIP_ADDR);
	if (error < 0)
	{
		printf("i2c set I2C_SLAVE error, error is %d\n", error);
	}
	else
	{
	}

	//	  error = ioctl(i2c_fd, I2C_TENBIT, 0);
	//	  if(error < 0)
	//	  {
	//		  printf("i2c set I2C_TENBIT error, error is %d\n", error);
	//	  }

	error = ioctl(i2c_fd, I2C_TIMEOUT, 0);
	if (error < 0)
	{
		printf("i2c set I2C_TIMEOUT error, error is %d\n", error);
	}

	error = ioctl(i2c_fd, I2C_RETRIES, 1);
	if (error < 0)
	{
		printf("i2c set I2C_RETRIES error, error is %d\n", error);
	}
}

void led_onoff(uint8 led_no, uint8 onoff)
{
	int res;
	uint8 led_onoff_status_tmp=led_onoff_status;

	if(!LED_ON_VALUE)
	{
		if(onoff)led_onoff_status_tmp &= ~(1<<led_no);
		else led_onoff_status_tmp |= (1<<led_no);
	}	


	if(led_onoff_status_tmp!=led_onoff_status)
	{
		led_onoff_status=led_onoff_status_tmp;
		#if 0
		   res = write(i2c_fd, &led_onoff_status, 1);
				if (res <= 0)
				{
				printf("i2c write error, error is %d,i2c_fd:%d\r\n", res,i2c_fd);
				}
				else
				{
					//printf("i2c write ok, led_onoff_status: 0x%x, res = %d\r\n", led_onoff_status, res);
				}		
	  	#endif
				
	}
}
static uint8 led_get_onoff(uint8 led_no)
{
	uint8 tmp=((led_onoff_status>>led_no)&1);
	if(tmp==LED_ON_VALUE)return 1;
	else return 0;
}
void led_toggle(uint8 led_no)
{
	uint8 tmp=(1-led_get_onoff(led_no));
	if(led_no==MIC_LED_BIT)
	{
		printf("tmp: %d\r\n",tmp);
	}
	led_onoff(led_no,tmp);
}


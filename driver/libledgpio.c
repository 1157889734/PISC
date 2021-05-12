
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libledgpio.h"


#define  MAX_GPIO_CNT  30

// strings libgpio.a |grep version
const char *LIB_INFO = "libgpio version: v0.0.1 (2020-04-21, 14:30)";

/*gpio init 将相关gpio export  初始化为输入输出 默认值等等*/
const char *const GPIOSysfsPath = "/sys/class/gpio";

typedef struct _gpio_t
{
  int num;
  int direction;
  int def_value;
} gpio_t;

static int gpios_cnt = 0;
static gpio_t gpios[MAX_GPIO_CNT];

static int check_gpios_isexist(int gpio)
{
  int i = 0;
  for (i = 0; i < gpios_cnt; ++i)
  {
    if (gpios[i].num == gpio)
      return 0;
  }
  return 1;
}

/*export gpio*/
static int export_gpio(const int gpio)
{
  char value[5];
  int rw, fd;
  char file_name[30];
  char gpio_name[30];
  sprintf(file_name, "%s/export", GPIOSysfsPath);
  fd = open(file_name, O_WRONLY);
  if (fd < 0)
  {
    return -1;
  }
  sprintf(gpio_name, "%s/gpio%d", GPIOSysfsPath, gpio);
  if (!access(gpio_name, 0))
    return 0;
  sprintf(value, "%d", gpio);
  rw = write(fd, value, strlen(value));
  if (rw < 0)
  {
    close(fd);
    return -1;
  }
  fsync(fd);
  close(fd);
  return 0;
}
#if 0
/*unexport gpio*/
static int unexport_gpio(const int gpio)
{
	  char value[5];
	   int rw, fd;
	   char file_name[30];
	   char gpio_name[30];
      sprintf(file_name, "%s/export", GPIOSysfsPath);
	   fd = open(file_name, O_WRONLY);
	   if(fd < 0){
		   return -1;
	   }
	   sprintf(gpio_name, "%s/gpio%d", GPIOSysfsPath, gpio);
	   if(!access(gpio_name, 0))
		   return 0;
	   sprintf(value, "%d", gpio);
	   rw = write(fd, value, strlen(value));
     if(rw < 0){
	     close(fd);
		  return -1;
      }
	   fsync(fd);
	   close(fd);
	   return 0;
}
#endif

/**
 * @descripttion: set gpio direction
 * @param {gpio:gpio num value: "in","out"} 
 * @return: success :0 failed <0
 */
static int set_direction_gpio(const int gpio, const char *value)
{
  int rw, fd;
  char file_name[100];
  sprintf(file_name, "%s/gpio%d/direction", GPIOSysfsPath, gpio);
  fd = open(file_name, O_WRONLY);
  if (fd < 0)
  {
    return -1;
  }
  rw = write(fd, value, strlen(value));
  if (rw < 0)
  {
    close(fd);
    return -1;
  }
  fsync(fd);
  close(fd);
  return 0;
}

static int write_gpio(const int gpio, int value)
{
  int rw, fd;
  char file_name[100];
  char str_value;
  sprintf(file_name, "%s/gpio%d/value", GPIOSysfsPath, gpio);
  fd = open(file_name, O_WRONLY);
  if (fd < 0)
  {
    return -1;
  }
  str_value = (value == 1) ? '1' : '0';
  rw = write(fd, &str_value, 1);
  if (rw < 0)
  {
    close(fd);
    return -1;
  }
  fsync(fd);
  close(fd);
  return 0;
}
static int gpio_value(const int gpio)
{
  int fd = -1;
  int rd = -1;
  unsigned char value;
  char gpio_path[60];
  sprintf(gpio_path, "%s/gpio%d/value", GPIOSysfsPath, gpio);
  fd = open(gpio_path, O_RDONLY | O_NONBLOCK);
  if (fd < 0)
  {
    return -1;
  }
  rd = read(fd, &value, 1);
  if (rd < 0)
  {
    close(fd);
    return -1;
  }
  fsync(fd);
  close(fd);
  return (int)(value - '0');
}


static void gpio_defaultconfig(int gpio,int  direction, int defaut_vue)
{

  gpios[gpios_cnt].num = gpio;
  gpios[gpios_cnt].direction = direction;
  gpios[gpios_cnt].def_value = defaut_vue;
  gpios_cnt++;
  //printf("gpios_cnt:%d\r\n",gpios_cnt);

}
/*WATCHDOG GPIO*/
//#define    EN_WDI           GPIO_PIN(GPIO(3),PIN(1))
//#define    FEED_WDI         GPIO_PIN(GPIO(3),PIN(2))
//#define    GPIO_WDI         GPIO_PIN(GPIO(3),PIN(3))

/**
 * @descripttion: GPIO init
 * @param {null} 
 * @return: 0:sucess 1:failed
 */
int gpio_init(void)
{

  int i = 0;
  #if 0
  gpio_defaultconfig(LED60_SYSRUN,0,1);
  gpio_defaultconfig(LED1_MANUAL,0,1);
  gpio_defaultconfig(LED2_TMS,0,1);
  gpio_defaultconfig(LED3_ATC,0,1);
  gpio_defaultconfig(LED4_OCC,0,1);
  gpio_defaultconfig(LED5_ACTIVE,0,1);
  gpio_defaultconfig(LED8_MEDIA,0,1);
  gpio_defaultconfig(LED9_MP3,0,1);
  gpio_defaultconfig(LED10_MIC,0,1);
  gpio_defaultconfig(LED60_ERR,0,1);
  #endif

  /*光耦 GPIO*/
  gpio_defaultconfig(OCEP_CLOSE5,1,1);
  gpio_defaultconfig(OCEP_OCCACT6,1,1);
  gpio_defaultconfig(OCEP_KEY,1,1);
  gpio_defaultconfig(OCEP_SPEED2,1,1);
  gpio_defaultconfig(OCEP_LOPEN3,1,1);
  gpio_defaultconfig(OCEP_ROPEN4,1,1);
  /*光耦 ADDR*/
  gpio_defaultconfig(OCEP_CPU_ADDR1,1,1);
  gpio_defaultconfig(OCEP_CPU_ADDR2,1,1);
  gpio_defaultconfig(OCEP_CPU_ADDR3,1,1);
  gpio_defaultconfig(OCEP_CPU_ADDR4,1,1);

 /*WATCHDOG*/
  gpio_defaultconfig(EN_WDIL,0,0);
  gpio_defaultconfig(FEED_WDIL,0,0);
  gpio_defaultconfig(GPIO_WDIL,1,1);


  for (i = 0; i < gpios_cnt; ++i)
  {
    if (export_gpio(gpios[i].num) == 0)
    {
      if (gpios[i].direction == 0)
      {
        if (set_direction_gpio(gpios[i].num, "out") == 0x00)
        {
          if (write_gpio(gpios[i].num, gpios[i].def_value) != 0x00)
          {
            printf("libgpio: error init gpio:%d,write defult value:%d\r\n", gpios[i].num, gpios[i].def_value);
          }
        }
        else
        {
          printf("libgpio: error init gpio:%d,set_direction:%d\r\n", gpios[i].num, gpios[i].direction);
        }
      }
      else
      {
        if (set_direction_gpio(gpios[i].num, "in") != 0x00)
        {
          printf("libgpio: error init gpio:%d,set_direction:%d\r\n", gpios[i].num, gpios[i].direction);
        }
      }
    }
    else
    {
      printf("libgpio: error init gpio%d export r\n", gpios[i].num);
    }
  }
  return 0;
}

/**
 * @descripttion: get gpio value
 * @param {in} gpio gpio number
 * @return: get the gpio value 0 or 1; -1:failed
 */
int gpio_input_value(int gpio)
{
  if (check_gpios_isexist(gpio) == 0x00)
  {
    return gpio_value(gpio);
  }
  return -1;
}

/**
 * @descripttion: set gpio value
 * @param {in} gpio gpio number 
 * @param {in} value  set gpio level (0 or 1) 
 * @return: 0:sucess 1:failed
 */
int gpio_output_ctrl(int gpio, int value)
{
  if (check_gpios_isexist(gpio) == 0x00)
  {
    return write_gpio(gpio, value);
  }
  else
  {
    printf("gpio %d is not exsit!\r\n", gpio);
  }

  return -1;
}


/**
 * @descripttion: set gpio input or output mode
 * @param {in} gpio 
 * @param {in} mode 1:input  0:output 
 * @return:0:sucess 1:failed
 */
int gpio_mode_set(int gpio,int mode)
{
 if (check_gpios_isexist(gpio) == 0x00)
  {
    if(mode == 0)
    return set_direction_gpio(gpio, "out");
    if(mode == 1)
    return set_direction_gpio(gpio, "in");
  }else
  {
    printf("gpio %d is not exsit!\r\n", gpio);
  }
  return -1;

}



int  led_ctrl(int nled,int status)
{
  int fd,rw;
 const char *leds_file[20]={"leds-sysrun","leds-manual","leds-tms","leds-act","leds-occ",
                       "leds-active","leds-meia","leds-mp3","leds-mic","leds-err"};
  char led_file[100]={};
   
  sprintf(led_file,"/sys/class/leds/%s/brightness",leds_file[nled]);
  fd = open(led_file, O_WRONLY);
  if (fd < 0)
  {
    printf("nled:%d open %s fd==%d\r\n",nled,led_file,fd);
    return -1;
  }
  if(status == 0x00)
  {
    rw = write(fd, "0", 1);
  }else
  {
    rw = write(fd, "1", 1);
  }
  if (rw < 0)
  {
    close(fd);
    return -1;
  }
  fsync(fd);
  close(fd);
  return 0;

}
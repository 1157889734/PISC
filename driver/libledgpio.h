/*
 * @Descripttion: gpio lib
 * @vesion: v1.0.0
 * @Author: superstring sunzhguy
 * @Date: 2020-04-20 16:13:05
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-04-23 15:38:44
 */

#ifndef  _LIBGPIO_H
#define  _LIBGPIO_H

#define   GPIO(m)        (m)
#define   PIN(n)         (n)
#define   GPIO_PIN(x,y)  ((GPIO(x)-1)*32 +PIN(y))

/*LED Ctrl GPIO */
#define   LED60_SYSRUN      0
#define   LED1_MANUAL       1
#define   LED2_TMS          2
#define   LED3_ATC          3
#define   LED4_OCC          4
#define   LED5_ACTIVE       5
#define   LED8_MEDIA        6
#define   LED9_MP3          7
#define   LED10_MIC         8
#define   LED60_ERR         9
/*光耦 GPIO*/
#define   OCEP_CLOSE5       GPIO_PIN(GPIO(1),PIN(1))
#define   OCEP_OCCACT6      GPIO_PIN(GPIO(1),PIN(2))
#define   OCEP_KEY          GPIO_PIN(GPIO(4),PIN(21))
#define   OCEP_SPEED2       GPIO_PIN(GPIO(4),PIN(22))
#define   OCEP_LOPEN3       GPIO_PIN(GPIO(4),PIN(23))
#define   OCEP_ROPEN4       GPIO_PIN(GPIO(4),PIN(24))

/*光耦 ADDR*/
#define   OCEP_CPU_ADDR1     GPIO_PIN(GPIO(1),PIN(8))
#define   OCEP_CPU_ADDR2     GPIO_PIN(GPIO(1),PIN(9))
#define   OCEP_CPU_ADDR3     GPIO_PIN(GPIO(1),PIN(3))
#define   OCEP_CPU_ADDR4     GPIO_PIN(GPIO(1),PIN(5))


/*WATCHDOG GPIO*/
#define EN_WDIL           GPIO_PIN(GPIO(3),PIN(1))
#define FEED_WDIL         GPIO_PIN(GPIO(3),PIN(2))
#define GPIO_WDIL         GPIO_PIN(GPIO(3),PIN(3))

/**
 * @descripttion: GPIO init
 * @param {null} 
 * @return: 0:sucess 1:failed
 */
int  gpio_init(void);

/**
 * @descripttion: get gpio value
 * @param {in} gpio gpio number
 * @return: get the gpio value 0 or 1; -1:failed
 */
int  gpio_input_value(int gpio);

/**
 * @descripttion: set gpio value
 * @param {in} gpio gpio number 
 * @param {in} value  set gpio level (0 or 1) 
 * @return: 0:sucess 1:failed
 */
int  gpio_output_ctrl(int gpio,int value);

/**
 * @descripttion: ctrl led on off
 * @param {in} nled which led  
 * @param {in} status on :1 off:0
 * @return: 
 */
int  led_ctrl(int nled,int status);



#endif
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 系统模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:include.h

//*文件作用:头文件包含

//*文件作者: mmaple

//*创建日期:2011年4月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#ifndef INCLUDE_H
#define INCLUDE_H


#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      /*文件控制定义*/
#include <sys/ioctl.h>
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h> //线程


//#include <iostream.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 
//#include <fstream.h>

#include <sys/soundcard.h>



//mpeg编解码库
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libavdevice/avdevice.h"


#include <stdlib.h>
#include <string.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#if 1



#include <termios.h>    /*PPSIX 终端控制定义*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <stdbool.h>
#include <sys/socket.h>//Socket基本函数定义
#include <netinet/in.h>//sockaddr_in结构定义
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>


//mpeg编解码库
//#include "ffmpeg/avutil.h"
//#include "ffmpeg/avformat.h"
//#include "ffmpeg/avcodec.h"



#include <termios.h>    /*PPSIX 终端控制定义*/
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 


#endif


#include "general.h"
#include "config.h"

#include "../lib/lib.h"
#include "../driver/driver.h"
#include "../port_layer/port_layer.h"
#include "../operate_device/operate_device.h"
#include "../terminal_device/terminal_device.h"
#include "../process_matrix/matrix.h"
#include "../manage/manage.h"


#endif


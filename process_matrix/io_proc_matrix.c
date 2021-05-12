//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 半自动处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:halt_ato.c

//*文件作用:列车通信总线管理器

//*文件作者:mmaple

//*创建日期:2010年12月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

#include"../include/include.h"

//#define IO_CHECK_TABLE_NUM_MAX		16
#define IO_CHECK_TABLE_NUM_MAX		64


typedef struct
{
	uint8 pin_cmd;		//io序号
	uint8 trigger_type;	//触发类型，电平或是沿
	uint8 io_level;
	uint8 io_level_bak;
	uint8 check_num;		//检测次数，防抖
	uint8 check_num_max;		//检测次数，防抖

	io_handler_callback			io_handler;	
}io_check_t;
static io_check_t  io_check_table[IO_CHECK_TABLE_NUM_MAX]={{0}};

static uint8 io_check_table_i=0;
void io_check_table_init( uint8 pin_cmd, uint8 trigger,uint8 level, uint8 level_bak,uint8 num,uint8 num_max,io_handler_callback handler)
{	
	io_check_table[io_check_table_i].pin_cmd=pin_cmd;
	io_check_table[io_check_table_i].trigger_type=trigger;
	io_check_table[io_check_table_i].io_level=level;
	io_check_table[io_check_table_i].io_level_bak=level_bak;
	io_check_table[io_check_table_i].check_num=num;
	io_check_table[io_check_table_i].check_num_max=num_max;
	io_check_table[io_check_table_i].io_handler=handler;
	if(io_check_table_i>=IO_CHECK_TABLE_NUM_MAX)
	{
		printf("too many io, exit\r\n");
		exit(0);
	}
	else io_check_table_i++;
}

void io_check_init(void)
{
	
}

void io_check_level_proc(void)
{
	uint8 i=0;
	uint8 status_tmp=0;

	while(i<io_check_table_i)
	{
		uint8 io_level_tmp;
		io_level_tmp=io_get_level(io_check_table[i].pin_cmd);
	
		if(io_level_tmp==io_check_table[i].io_level)
		{
			io_check_table[i].check_num++;
			if(io_check_table[i].check_num>=io_check_table[i].check_num_max)
			{
				io_check_table[i].check_num=io_check_table[i].check_num_max;
				if(io_check_table[i].io_level!=io_check_table[i].io_level_bak)
				{					
					printf("io change, io_level[%d]: %d\r\n",i,io_check_table[i].io_level);
					if(NULL!=io_check_table[i].io_handler)
					{					
						if(IO_LEVEL_LOW==io_check_table[i].io_level)status_tmp=BH_VALID;
						else status_tmp=BH_UNVALID;
						io_check_table[i].io_handler(status_tmp);
					}
					io_check_table[i].io_level_bak=io_level_tmp;
				}
			}
		}
		else 
		{			
			io_check_table[i].check_num=0;
			io_check_table[i].io_level=io_level_tmp;
		}
		i++;
	}
}



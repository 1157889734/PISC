
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 虚拟设备处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:device.c
//*文件作用:虚拟设备处理
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//#include "include.h"

//mpeg编解码库

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//封装格式
#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"
//缩放
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"

#include "mp3_decoder.h"
#include "broadcast.h"
#include "config.h"
#include "soundcard.h"
#include "broadcast_audio_port.h"
#include "kring_buffer.h"



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 虚拟设备处理 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:device.c
//*文件作用:虚拟设备处理
//*文件作者:mmaple
//*创建日期:2007年5月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


#define  FFMPEG_NEW_VERSION
#define SOUND_FILE_PATH 		"/sddisk/"


#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000



//#define AUDIO_PACKET_SIZE		(300)
#define AUDIO_PACKET_SIZE		(1024)


//#define AUDIO_PACK_INIT_SIZE			(1024*5)
#define AUDIO_PACK_INIT_SIZE			(512)

//音频通道播放结构
typedef struct
{
	char file_list[FILE_LIST_NUM_MAX][FILE_NAME_LENGTH_MAX];//文件列表
	int current_file_index;//当前正在播放的文件索引
	int current_file_eof;//当前文件播放完毕
	int pause_flag;//暂停标志
	int is_playing;//正在播放
	
	AVFormatContext *pFormatCtx;//媒体格式指针
	AVCodecContext *pAudioCodecCtx;//音频解码器指针
	AVCodecContext *pVideoCodecCtx;//视频解码器指针
	int g_audioStream;//音频流编号
	int g_videoStream;//视频流编号
	
	short audio_write_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2];//音频播放缓冲区
	int audio_write_length;//音频数据长度
}audio_channel_t;
static audio_channel_t audio_channel_left=
{
	{""},//文件列表
	0,//当前正在播放的文件索引
	1,//当前文件播放完毕
	0,//暂停标志
	0,//正在播放
	
	NULL,//媒体格式指针
	NULL,//音频解码器指针
	NULL,//视频解码器指针
	-1,//音频流编号
	-1,//视频流编号
	
	{0},//音频播放缓冲区
	0,//音频数据长度
};
static uint16 audio_pcm_buf[AUDIO_PACKET_SIZE*BROADCAST_AUDIO_DATA_SIZE]={0};
static uint32 audio_pack_fifo_in=0;
static uint32 audio_pack_fifo_out=0;


static pthread_t mp3_decoder_thread_id;


static uint8 mp3_init_flag=0;

struct  kring_buffer * ring_buf =NULL;


static void mp3_decode_set_current_file_eof(uint8 flag);
static uint8 mp3_decode_get_current_file_eof(void);
static void mp3_decode_set_is_playing(uint8 flag);
uint8 mp3_decode_get_is_playing(void);


uint8 mp3_decoder_get_init_flag(void)
{
	return mp3_init_flag;
}
void mp3_decoder_set_init_flag(uint8 flag)
{
	if(mp3_init_flag!=flag)
	{
		printf("mp3_decoder_set_init_flag: %d\r\n",flag);
		mp3_init_flag=flag;
	}
}


//***********************************************************************************************************************
//函数作用:打印媒体信息
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static void display_AVCodecContext(AVCodecContext *pCodecCtx)
{	
	printf("pCodecCtx->bit_rate:%d \n", pCodecCtx->bit_rate);
	printf("pCodecCtx->sample_rate:%d \n", pCodecCtx->sample_rate);
	printf("pCodecCtx->channels:%d \n", pCodecCtx->channels);
	printf("pCodecCtx->frame_size:%d \n", pCodecCtx->frame_size);
	printf("pCodecCtx->frame_number:%d \n", pCodecCtx->frame_number);
	printf("pCodecCtx->delay:%d \n", pCodecCtx->delay);
	printf("pCodecCtx->frame_bits:%d \n", pCodecCtx->frame_bits);
	printf("\n");
}


//***********************************************************************************************************************
//函数作用:打开媒体文件 并初始化解码库
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static int av_init(char *file_name, AVFormatContext **pFormatCtx, AVCodecContext **pAudioCodecCtx, int *p_audioStream, AVCodecContext **pVideoCodecCtx, int *p_videoStream)
{
	int i;
    
	//注册所有解码库
	av_register_all();
	printf("av_init,file_name: %s\r\n",file_name);


	//打开媒体文件文件
	#ifdef FFMPEG_NEW_VERSION
     if(avformat_open_input(pFormatCtx,file_name,NULL,NULL))
	#else
	if(av_open_input_file(pFormatCtx,file_name,NULL,0,NULL))
	#endif
	{
		printf(" Can't open %s! ...\n",file_name);
		return -1;	
	}


	//查找相应的媒体格式
	#ifdef FFMPEG_NEW_VERSION
    if(avformat_find_stream_info(*pFormatCtx,NULL)<0)
	#else
	if(av_find_stream_info(*pFormatCtx)<0)
	#endif
	{
		printf(" Can't find stream info! \n");
		return -1;	
	}
	
	//寻找第一桢音频和视频数据
	for(i=0;i<(*pFormatCtx)->nb_streams;i++)
	{
		if((*pFormatCtx)->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO/*CODEC_TYPE_AUDIO*/)
		{
			*p_audioStream = i;
			printf("*p_audioStream: %d\r\n",*p_audioStream);
		}
		else if((*pFormatCtx)->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO/*CODEC_TYPE_VIDEO*/)
		{
			*p_videoStream = i;
		}
	}
	printf("over audiostream\r\n");
	//判断是否有媒体数据
	if(*p_audioStream==-1 && *p_videoStream==-1)
	{
		printf("Error: No Audio and Videa frame \n");
		return -1;
	}
	
	//判断是否有音频数据 查找音频解码器
	if(*p_audioStream!=-1)
	{
		AVCodec *pAudioCodec;
		
		*pAudioCodecCtx=(*pFormatCtx)->streams[*p_audioStream]->codec;
		pAudioCodec=avcodec_find_decoder((*pAudioCodecCtx)->codec_id);
		//printf("(*pAudioCodecCtx)->codec_id=%d\n",(*pAudioCodecCtx)->codec_id);
		if(pAudioCodec == NULL)return -1;
		#ifdef FFMPEG_NEW_VERSION   //nt avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
        if(avcodec_open2((*pAudioCodecCtx), pAudioCodec,NULL)<0)
		#else
		if(avcodec_open((*pAudioCodecCtx), pAudioCodec)<0)
		#endif
		{
			printf("Open Audio codec failed\n");
			return -1;
		}
		//打印媒体音频信息
		display_AVCodecContext(*pAudioCodecCtx);
	}
	else
	{
		printf("Error: No Audio \n");
		return -1;
	}
	return 0;
}

typedef struct abc_t{
    fp64 pts;                            ///< presentation time stamp in time_base units
    fp64 dts;                            ///< decompression time stamp in time_base units
    uint8 *data123;
    int   size;
    int   stream_index;
    int   flags;
    int   duration;                         ///< presentation duration in time_base units (0 if not available)
    void  (*destruct)(struct abc_t *);
    void  *priv;
    fp64 pos;                            ///< byte position in stream, -1 if unknown
} abc_t;

uint8 av_play(AVFormatContext * pFormatCtx,AVCodecContext * pAudioCodecCtx, int audioStream, AVCodecContext * pVideoCodecCtx, int videoStream)
{
		//abc_t packet;
	uint32 len;
	uint16 decompressed_audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	int32 decompressed_audio_buf_size;
    AVPacket packet;

while(1)
{

	//read frame and play
	if ((av_read_frame(pFormatCtx, (AVPacket *)&packet) >= 0))
	{
		//Audio frame

		if(packet.stream_index==audioStream)
		{
			#ifdef FFMPEG_NEW_VERSION
			AVFrame frame;
		   int got_frame = 0;
			//int avcodec_decode_audio4(AVCodecContext *avctx, AVFrame *frame,
            //              int *got_frame_ptr, const AVPacket *avpkt);
			len=avcodec_decode_audio4(pAudioCodecCtx,&frame,&got_frame,&packet);
			if(len<0)
			{
				printf("+----- error in decoding audio frame\n");
				av_free_packet((AVPacket *)&packet);
				continue;
			}
			if(got_frame)
			{

				int data_size = 0;// resample(&frame, decompressed_audio_buf, &decompressed_audio_buf_size);
				//printf("got_frame:%d,data_size:%d,buf_size:%d\r\n",got_frame,data_size,decompressed_audio_buf_size);
                 //printf("got_frame:%d,data_size:%d,buf_size:%d,pAudioCodecCtx->channels:%d\r\n",got_frame,data_size,decompressed_audio_buf_size,p
                 //printf("nb_samples:%d,frame.linesize[0]=%d,frame.linesize[1]=%d\r\n",frame.nb_samples,frame.linesize[0],frame.linesize[1]);
                  if(pAudioCodecCtx->channels == 0x01)//单通道
                     {
                      int bytes = av_get_bytes_per_sample(pAudioCodecCtx->sample_fmt & 0xFF);
                      //printf("plane_size:%d,frame.nb_samples:%d,sample_fmt:%d\r\n",bytes,frame.nb_samples,pAudioCodecCtx->sample_fmt);
                      decompressed_audio_buf_size =frame.linesize[0];
                       memcpy((unsigned char *)decompressed_audio_buf, (unsigned char *)frame.data[0],decompressed_audio_buf_size);
					 }


			}
			#else

			len=avcodec_decode_audio2 (pAudioCodecCtx,(int16_t *)decompressed_audio_buf,&decompressed_audio_buf_size,packet.data123,packet.size);
			if(len<0)
			{
				printf("+----- error in decoding audio frame\n");
				av_free_packet((AVPacket *)&packet);
				continue;
			}

			#endif

			//如果是立体声 把两个通道合成 送通道缓冲区
			if(pAudioCodecCtx->channels==2)
			{
				uint16 buf_tmp[4096];
				uint16 i=0;
				uint16 decode_len=0;

				//printf("decompressed_audio_buf_size: %d\r\n",decompressed_audio_buf_size);

				//采样率展宽
				if(pAudioCodecCtx->sample_rate==44100)
				{
					decode_len=decompressed_audio_buf_size/4;
					uint16 half_len=decompressed_audio_buf_size/2;
					//printf("decode_len: %d\r\n",decode_len);
					
					//for(i=0;i<decode_len;i++)
					for(i=0;i<half_len;i++)
					{
						buf_tmp[i]=decompressed_audio_buf[i*2];
					}
					//printf("*************,decode_len: %d, half_len: %d\r\n",decode_len,half_len);
						//print_buf(buf_tmp,decode_len*2);
						//printf("&&&&&&&&&,decompressed_audio_buf_size: %d\r\n",decompressed_audio_buf_size);
						//print_buf(decompressed_audio_buf,decompressed_audio_buf_size);
					mp3_decoder_set_pcm_data(buf_tmp,decode_len);
				}					
 				else if(pAudioCodecCtx->sample_rate==22050)
 				{
					decode_len=decompressed_audio_buf_size/2;
					//uint16 half_len=decompressed_audio_buf_size/2;
 				
	 				//decode_len=decompressed_audio_buf_size;
					memcpy(buf_tmp,decompressed_audio_buf,decompressed_audio_buf_size);
					
					mp3_decoder_set_pcm_data(buf_tmp,decode_len);
 				}
			}		
			else
			{
				mp3_decoder_set_pcm_data(decompressed_audio_buf,decompressed_audio_buf_size/2);
			}	
			//Free frame (that was allocated by av_read_frame)
			av_free_packet((AVPacket *)&packet);
			
			return 0;
		}
		else
		{
			//Free frame (that was allocated by av_read_frame)
			av_free_packet((AVPacket *)&packet);
		
		}
			


		//usleep(10*1000);
	}
	//播放完毕
	else
	{
		
		return 1;
	}
}
}

//***********************************************************************************************************************
//函数作用:关闭解码器和文件
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void av_close(AVFormatContext **pFormatCtx, AVCodecContext * pAudioCodecCtx, int audioStream, AVCodecContext * pVideoCodecCtx,  int videoStream)
{
	//printf("av_close\r\n");
	//如果有音频 关闭音频解码器
	if(audioStream!=-1)avcodec_close(pAudioCodecCtx);
	//如果有视频 关闭视频解码器
	if(videoStream!=-1)avcodec_close(pVideoCodecCtx);
	//关闭媒体文件
	#ifdef FFMPEG_NEW_VERSION
    avformat_close_input(pFormatCtx);
	#else
	av_close_input_file(pFormatCtx);
	#endif
}


#if 0
static void mp3_decoder_clear_pcm_data(void)
{
	printf("mp3_decoder_clear_pcm_data, sizeof(audio_pcm_buf): %d\r\n",sizeof(audio_pcm_buf));
	memset(audio_pcm_buf,0,sizeof(audio_pcm_buf));
	audio_pack_fifo_in=0;
	audio_pack_fifo_out=0;
}
void mp3_decoder_set_pcm_data(uint16 *buf, uint16 len)
{
	uint32 i=0;
	uint16 buf_tmp[10240]={0};
	memcpy(buf_tmp,buf,len*2);

	for(i=0;i<len;i++)
	{

#if 0	
		//if(audio_pack_fifo_in>=(1024*5))
		if(audio_pack_fifo_in>=(AUDIO_PACK_INIT_SIZE))		
		//if(audio_pack_fifo_in>=(BROADCAST_AUDIO_DATA_SIZE*AUDIO_PACK_INIT_SIZE))
		{
			if(!mp3_decoder_get_init_flag())
			{
				mp3_decoder_set_init_flag(1);
				//exit(0);
			}		
		}
#else
mp3_decoder_set_init_flag(1);

#endif
		audio_pcm_buf[audio_pack_fifo_in]=buf_tmp[i];
		
		if(audio_pack_fifo_in >= (sizeof(audio_pcm_buf)/2))
		{
			audio_pack_fifo_in=0;
		}
		audio_pack_fifo_in++;		
		while(audio_pack_fifo_in==audio_pack_fifo_out)
		{
			usleep(500*1000);
		}
		

	}
}
uint8 mp3_decoder_get_pcm_data(uint16 *buf, uint16 len)
{
	uint32 i=0;
	uint16 buf_tmp[4096]={0};

	for(i=0;i<len;i++)
	{
		if(audio_pack_fifo_out==audio_pack_fifo_in && !(mp3_decoder_get_init_flag()))
		{
			//printf("mp3_decoder_get_pcm_data error, (mp3_decoder_get_init_flag()): %d\r\n",(mp3_decoder_get_init_flag()));
			return BH_FALSE;
		}
		
		buf_tmp[i]=audio_pcm_buf[audio_pack_fifo_out];
		audio_pack_fifo_out++;
		if(audio_pack_fifo_out >= (sizeof(audio_pcm_buf)/2))
		{
			audio_pack_fifo_out=0;
		}
	}	
	memcpy((uint8 *)buf,(uint8 *)buf_tmp,1024);
	return BH_TRUE;
}
#else
static void mp3_decoder_clear_pcm_data(void)
{
	printf("mp3_decoder_clear_pcm_data, sizeof(audio_pcm_buf): %d\r\n",sizeof(audio_pcm_buf));
	memset(audio_pcm_buf,0,sizeof(audio_pcm_buf));
	audio_pack_fifo_in=0;
	audio_pack_fifo_out=0;
}
void mp3_decoder_set_pcm_dataxxxxx(uint16 *buf, uint16 len)
{
	uint32 i=0;
	uint16 buf_tmp[10240]={0};
	memcpy(buf_tmp,buf,len*2);

	for(i=0;i<len;i++)
	{

#if 1	
		//if(audio_pack_fifo_in>=(1024*5))
		if(audio_pack_fifo_in>=(AUDIO_PACK_INIT_SIZE))		
		//if(audio_pack_fifo_in>=(BROADCAST_AUDIO_DATA_SIZE*AUDIO_PACK_INIT_SIZE))
		{
			if(!mp3_decoder_get_init_flag())
			{
				mp3_decoder_set_init_flag(1);
				//exit(0);
			}		
		}
#else
mp3_decoder_set_init_flag(1);

#endif
		audio_pcm_buf[audio_pack_fifo_in]=buf_tmp[i];
		audio_pack_fifo_in++;		
		if(audio_pack_fifo_in >= (sizeof(audio_pcm_buf)/2))
		{
			audio_pack_fifo_in=0;
		}
#if 0		
		while(audio_pack_fifo_in==audio_pack_fifo_out)
		{
			usleep(50*1000);
		}
#else
		while(1)
		{
			uint8 flag=0;
			//半满，等待
			if(audio_pack_fifo_out>audio_pack_fifo_in)
			{
				if((audio_pack_fifo_out-audio_pack_fifo_in)<(sizeof(audio_pcm_buf)/4))flag=1;
			}
			else
			{
				if((audio_pack_fifo_in-audio_pack_fifo_out)>(sizeof(audio_pcm_buf)/4))flag=1;
			}

			if(flag)
			{
				usleep(5*1000);
			}
			else
			{
				break;
			}
		}

#endif

	}
}
uint8 mp3_decoder_get_pcm_dataxxxx(uint16 *buf, uint16 len)
{
	uint32 i=0;
	uint16 buf_tmp[4096]={0};

	for(i=0;i<len;i++)
	{
		if(audio_pack_fifo_out==audio_pack_fifo_in && !(mp3_decoder_get_init_flag()))
		{
			//printf("mp3_decoder_get_pcm_data error, (mp3_decoder_get_init_flag()): %d\r\n",(mp3_decoder_get_init_flag()));
			return BH_FALSE;
		}
		
		buf_tmp[i]=audio_pcm_buf[audio_pack_fifo_out];
		audio_pack_fifo_out++;
		if(audio_pack_fifo_out >= (sizeof(audio_pcm_buf)/2))
		{
			audio_pack_fifo_out=0;
		}
	}	
	memcpy((uint8 *)buf,(uint8 *)buf_tmp,1024);
	return BH_TRUE;
}

#endif

//***********************************************************************************************************************
//函数作用:启动播放
//参数说明:channel 通道   msg_id  文件列表号
//注意事项:
//返回说明:无
//***********************************************************************************************************************
int mp3_decoder_play_list(uint8 channel,uint8 *path,uint16 list_num)
#if 1
{
	audio_channel_t *audio_channel=&audio_channel_left;
	int file_num=list_num;
	int file_index=0;
	uint8 tmp_buf[FILE_NAME_LENGTH_MAX]={0};
	uint8 *p=path;
	
	printf("mp3_decoder_play_list, file_num: %d\r\n",file_num);
	//printf("p[0]: %s\r\n",p[0]);
	//判断超过最多的文件数量 退出
	if(file_num>=FILE_LIST_NUM_MAX-1)return 0;

	//mp3_decoder_clear_pcm_data();

	//通道
	if(channel==CHANNEL_LEFT)audio_channel=&audio_channel_left;

	//拷贝播放列表
	for(file_index=0;file_index<list_num;file_index++)
	{
		memcpy(audio_channel->file_list[file_index],p,FILE_NAME_LENGTH_MAX);
		printf("audio_channel->file_list[%d]: %s\r\n",file_index,audio_channel->file_list[file_index]);
		p+=FILE_NAME_LENGTH_MAX;
	}

	//添加结束符
    audio_channel->file_list[file_num][0]=0;
    audio_channel->file_list[file_num+1][0]=0;
	//当前正在播放的文件回0  
    audio_channel->current_file_index=0;
	
    //送播放标志
    mp3_decode_set_is_playing(1);
    mp3_decode_set_current_file_eof(1);

	//play_begin_flag=1;

		
    return 0;
}

#else
{
	audio_channel_t *audio_channel=&audio_channel_left;
	int file_num=list_num;
	int file_index=0;
	printf("mp3_decoder_play_list, file_num: %d\r\n",file_num);
	//判断超过最多的文件数量 退出
	if(file_num>=FILE_LIST_NUM_MAX-1)return 0;

	//mp3_decoder_clear_pcm_data();

	//通道
	if(channel==CHANNEL_LEFT)audio_channel=&audio_channel_left;


	memcpy(audio_channel->file_list[file_index],path,FILE_NAME_LENGTH_MAX*list_num);

	//添加结束符
    audio_channel->file_list[file_num][0]=0;
    audio_channel->file_list[file_num+1][0]=0;
	//当前正在播放的文件回0  
    audio_channel->current_file_index=0;
	
    //送播放标志
    mp3_decode_set_is_playing(1);
    mp3_decode_set_current_file_eof(1);
        
    return 0;
}
#endif

static void mp3_decode_set_current_file_eof(uint8 flag)
{
	if(flag!=audio_channel_left.current_file_eof)
	{
		printf("current_file_eof: %d\r\n",flag);
		audio_channel_left.current_file_eof=flag;
	}
}
static uint8 mp3_decode_get_current_file_eof(void)
{
	return audio_channel_left.current_file_eof;
}
static void mp3_decode_set_is_playing(uint8 flag)
{
	if(flag!=audio_channel_left.is_playing)
	{
		printf("is_playing: %d\r\n",flag);
		audio_channel_left.is_playing=flag;
	}
}
uint8 mp3_decode_get_is_playing(void)
{
	return audio_channel_left.is_playing;
}

//***********************************************************************************************************************
//函数作用:停止播放
//参数说明:channel 通道
//注意事项:
//返回说明:无
//***********************************************************************************************************************
void mp3_decoder_stop_play(uint8 channel)
{
	audio_channel_t *audio_channel=&audio_channel_left;
	

	printf("mp3_decoder_stop_play\r\n");
	
	//通道
	if(channel==CHANNEL_LEFT)audio_channel=&audio_channel_left;


    //已经关闭了退出
    if(mp3_decode_get_is_playing()==0)return;
    
    //送关闭标志
    mp3_decode_set_is_playing(0);
    
    //等待关闭
    while(mp3_decode_get_current_file_eof()==0)usleep(10*1000);
        
}

static uint8 mp3_decoder_run_flag=0;
void mp3_decoder_set_run_flag(uint8 onoff)
{
	if(mp3_decoder_run_flag!=onoff)
	{
		printf("mp3_decoder_set_run_flag: %d\r\n",onoff);
		mp3_decoder_run_flag=onoff;
	}
}
static uint8 mp3_decoder_get_run_flag(void)
{
	return mp3_decoder_run_flag;
}

static void* mp3_decoder_thread(void* param)
{	
	printf("mp3_decoder_thread start....\r\n");

	pthread_detach(pthread_self());	//by zhq 20100527	
	mp3_decoder_clear_pcm_data();	
    ring_buf = kring_bufer_alloc_init(1024*1024);

	while(1)
	{
		while(1)
		{
			if(mp3_decoder_get_run_flag())
			{
				break;
			}

			usleep(10*1000);
		}
		while(1)
		{
			//判断全部播放完毕 退出
			if(audio_channel_left.file_list[audio_channel_left.current_file_index][0]==0)
			{
				break;
			}
			
			//如果有文件才能播放
			if(audio_channel_left.file_list[audio_channel_left.current_file_index][0])		
			{
				//判断打开文件
				if(mp3_decode_get_current_file_eof())
				{
					//打开媒体文件
					if(av_init(audio_channel_left.file_list[audio_channel_left.current_file_index], &audio_channel_left.pFormatCtx, &audio_channel_left.pAudioCodecCtx, &audio_channel_left.g_audioStream, &audio_channel_left.pVideoCodecCtx, &audio_channel_left.g_videoStream) < 0)
					{				
						//指向下一个文件
						audio_channel_left.current_file_index++;
						//判断播放完毕
						if(audio_channel_left.file_list[audio_channel_left.current_file_index][0]==0)
						{
							
							mp3_decode_set_is_playing(0);
						}
					}
					else
					{
						//清当前文件播放完毕标志
						mp3_decode_set_current_file_eof(0);
					}
				}
				//播放文件
				else
				{
					//播放文件	
					mp3_decode_set_current_file_eof(av_play(audio_channel_left.pFormatCtx,audio_channel_left.pAudioCodecCtx,audio_channel_left.g_audioStream,audio_channel_left.pVideoCodecCtx,audio_channel_left.g_videoStream));
					/*if(kring_buffer_len(ring_buf) >128*1024 )
					{
						usleep(30000);
					}*/
					//判断是否播放完毕 指向下一个文件
					if(mp3_decode_get_current_file_eof() || (mp3_decode_get_is_playing()==0))
					{
						//关闭解码器和媒体文件
						av_close(&audio_channel_left.pFormatCtx,audio_channel_left.pAudioCodecCtx,audio_channel_left.g_audioStream,audio_channel_left.pVideoCodecCtx,audio_channel_left.g_videoStream);
						//指向下一个文件
						audio_channel_left.current_file_index++;
						//如果是强制关闭文件 清除当前文件
						if(mp3_decode_get_is_playing()==0)audio_channel_left.file_list[audio_channel_left.current_file_index][0]=0;
						//判断播放完毕
						if(audio_channel_left.file_list[audio_channel_left.current_file_index][0]==0)
						{
							int exit_sleep =0;
							while((kring_buffer_len(ring_buf)!=0x00))
							{
								if(exit_sleep%10 == 0x00)
								printf("------%d,%d\r\n",exit_sleep,kring_buffer_len(ring_buf));
								usleep(100000);
								exit_sleep++;
								if(exit_sleep >500)//大约5秒退出
								{
									break;
								}
							}
							kring_buffer_reset(ring_buf);
							mp3_decode_set_is_playing(0);
							mp3_decode_set_current_file_eof(1);
							
							break;
						}
					}
				}
			}
		}

		if(BROADCAST_NONE==broadcast_get_broadcast_type())
		{
			mp3_decoder_set_init_flag(0);
			//清除缓冲区数据
			mp3_decoder_clear_pcm_data();
			mp3_decoder_set_run_flag(0);
		}
	}
	kring_bufer_alloc_exit(ring_buf);
}

void	mp3_decoder_thread_install(void)
{
	//创建线程
	if(!mp3_decoder_get_init_flag())
	{
		//pthread_create(&mp3_decoder_thread_id, NULL, (void*)mp3_decoder_thread, NULL); 	//PA内部协议数据采集线程创建
		ThreadCreate(&mp3_decoder_thread_id,  mp3_decoder_thread, NULL); 	//PA内部协议数据采集线程创建
	}
	return;
}




void mp3_decoder_set_pcm_data(uint16 *buf, uint16 len)
{
	#if 0
	uint32 i=0;
	uint16 buf_tmp[10240]={0};
	memcpy(buf_tmp,buf,len*2);

	for(i=0;i<len;i++)
	{
		if(audio_pack_fifo_in>=(AUDIO_PACK_INIT_SIZE))		
		{
			if(!mp3_decoder_get_init_flag())
			{
				mp3_decoder_set_init_flag(1);
				//exit(0);
			}		
		}
		audio_pcm_buf[audio_pack_fifo_in]=buf_tmp[i];
		audio_pack_fifo_in++;		

	}
	#else
    kring_buffer_put(ring_buf,buf,len*2);
	#endif
}
uint8 mp3_decoder_get_pcm_data(uint16 *buf, uint16 len)
{
	#if 0
	uint32 i=0;
	uint16 buf_tmp[4096]={0};
	if(audio_pack_fifo_out == audio_pack_fifo_in)
	return  BH_FALSE;

	for(i=0;i<len;i++)
	{
		if(audio_pack_fifo_out==audio_pack_fifo_in && !(mp3_decoder_get_init_flag()))
		{
			if(!mp3_decoder_get_init_flag())
			printf("mp3_decoder_get_pcm_data error, (mp3_decoder_get_init_flag()): %d\r\n",(mp3_decoder_get_init_flag()));
			return BH_FALSE;
		}
		
		buf_tmp[i]=audio_pcm_buf[audio_pack_fifo_out];
		audio_pack_fifo_out++;
		if(audio_pack_fifo_out==audio_pack_fifo_in)
		  break;
	}	
	//printf("audio_pack_fifo_out:%d,audio_pack_fifo_in:%d\r\n",audio_pack_fifo_out,audio_pack_fifo_in);
	memcpy((uint8 *)buf,(uint8 *)buf_tmp,1024);
	return BH_TRUE;
	#else
     if(kring_buffer_len(ring_buf)!=0)
	 {
		kring_buffer_get(ring_buf,buf,len*2);
		return BH_TRUE;
	 }else 
	    return BH_FALSE;
      

	#endif
}

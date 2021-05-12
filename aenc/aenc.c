#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "aenc.h"
//#include "g711.h"

typedef struct _T_AUDIO_INFO {
	snd_pcm_t *handle;
	snd_pcm_uframes_t frame_num;
	snd_pcm_format_t format;
	AUDIO_SAMPLE_RATE_E enSampleRate;
	AUDIO_BIT_WIDTH_E enBitWidth;
	uint16_t channels;                        //通道数
	size_t chunk_bytes;                       //周期字节数
	size_t bits_per_sample;                   //样本长度  8bit  16  bit等
	size_t bits_per_frame;                    //帧   记录了一个声音单元的长度   等于通道数乘以 样本长度
	uint8_t *data_buf;                        //数据的缓冲区
}T_AUDIO_INFO, *PT_AUDIO_INFO;

typedef struct _T_AUDIO_ENC
{
    AUDIO_CODEC_FORMAT_E enEncType;
    BOOL bFlag;
    uint8_t *pcEncBuf;
    size_t chunk_bytes;                       //周期字节数
}T_AUDIO_ENC, *PT_AUDIO_ENC;


#define AUDIO_CHANNEL_DOUBLE 2
#define AUDIO_CHANNEL_SINGLE 1

#define VOICE_PACAKGE_10MS 160000
//#define VOICE_PACAKGE_20MS 

static T_AUDIO_INFO g_tAudioInfo;
static T_AUDIO_ENC g_tAudioEnc = {AUDIO_CODEC_FORMAT_G711A, FALSE, NULL, 0};
static U32 g_u32FrameNum = 0;
static int g_iCaptureFlag = 0;
static int g_iExitThreadFlag = 0;
static char *g_pcBuf = NULL;


static int AIGetFormat(AUDIO_BIT_WIDTH_E eBitWidth,  snd_pcm_format_t *format)
{
    switch (eBitWidth)
    {
        case AUDIO_BIT_WIDTH_16:
            *format = SND_PCM_FORMAT_S16_LE;
            break;
        case AUDIO_BIT_WIDTH_8:
            *format = SND_PCM_FORMAT_U8;
            break;
        default:
            *format = SND_PCM_FORMAT_UNKNOWN;
            break;
    }
    
    return 0;
}

static int AIGetSampleRate(AUDIO_SAMPLE_RATE_E eSampleRate, U32 *u32SampleRate)
{
    switch(eSampleRate)
    {
        case AUDIO_SAMPLE_RATE_8:
            *u32SampleRate = 8000;
            break;
        case AUDIO_SAMPLE_RATE_11025:
            *u32SampleRate = 11025;
            break;
        case AUDIO_SAMPLE_RATE_16:
            *u32SampleRate = 16000;
            break;
        case AUDIO_SAMPLE_RATE_22050:
            *u32SampleRate = 22050;
            break;
        case AUDIO_SAMPLE_RATE_24:
            *u32SampleRate = 24000;
            break;
        case AUDIO_SAMPLE_RATE_32:
            *u32SampleRate = 32000;
            break;
        case AUDIO_SAMPLE_RATE_441:
            *u32SampleRate = 44100;
            break;
        case AUDIO_SAMPLE_RATE_48:
            *u32SampleRate = 48000;
            break;
        case AUDIO_SAMPLE_RATE_64:
            *u32SampleRate = 64000;
            break;
        default:
            *u32SampleRate = 8000;
            break;
    }
    
    return 0;
}

static ssize_t readAudioData(PT_AUDIO_INFO ptAudioInfo, size_t rcount)
{
    int r;
    int result = 0;
    int count = rcount;
    uint8_t *data = ptAudioInfo->data_buf;

    if (count != ptAudioInfo->frame_num) {
        count = ptAudioInfo->frame_num;
    }

    while (count > 0)
    {
        r = snd_pcm_readi(ptAudioInfo->handle, data, count);

        if (r == -EAGAIN || (r >= 0 && (size_t)r < count))
        {
            snd_pcm_wait(ptAudioInfo->handle, 1000);
        }
        else if (r == -EPIPE)
        {
            snd_pcm_prepare(ptAudioInfo->handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>/n");
        }
        else if (r == -ESTRPIPE)
        {
            fprintf(stderr, "<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>/n");
        }
        else if (r < 0)
        {
            fprintf(stderr, "Error snd_pcm_writei: [%s]", snd_strerror(r));
           // exit(-1);
        }
		
        if (r > 0)
        {
            result += r;
            count -= r;
            data += r * ptAudioInfo->bits_per_frame / 8;
        }
    }
    
    return rcount;
}

#if 0
void *AudioCaptureThread(void *arg)
{

    pthread_detach(pthread_self()); 
    while (g_iExitThreadFlag)
    {
        if (g_iCaptureFlag)
        {
  
            readAudioData(&g_tAudioInfo, g_tAudioInfo.frame_num);
            if (g_tAudioInfo.data_buf)
            {
                AUDIO_PostQueue(g_tAudioInfo.data_buf, g_tAudioInfo.chunk_bytes);
            }
        }
        else
        {
            usleep(1000);	
        }
    }
    
    return NULL;
}
#endif

int AENC_Open(void)
{
    char *devicename = "default";

    if (snd_pcm_open(&g_tAudioInfo.handle, devicename, SND_PCM_STREAM_CAPTURE, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_open [ %s]/n", devicename);
        return -1;
    }

    g_pcBuf = malloc(20 * 1024);
    if (NULL == g_pcBuf)
    {
        return -1;
    } 
    g_tAudioInfo.enBitWidth = AUDIO_BIT_WIDTH_16;
    g_tAudioInfo.enSampleRate = AUDIO_SAMPLE_RATE_441;//AUDIO_SAMPLE_RATE_8;
    g_tAudioInfo.channels = AUDIO_CHANNEL_DOUBLE;
    //AUDIO_InitQueue(10*1024);

    return 0;
}


void AENC_Close(void)
{
	//free(playback.data_buf);

    free(g_pcBuf);	
    snd_pcm_close(g_tAudioInfo.handle);
    //AUDIO_UninitQueue();
}

static int SetAudioChAttr(void)
{
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_format_t format;
    uint32_t u32SampleRate;
    uint32_t buffer_time, period_time;
    snd_pcm_uframes_t buffer_size;

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
	
    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(g_tAudioInfo.handle, hwparams) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_any/n");
        goto ERR_SET_PARAMS;
    }

    if (snd_pcm_hw_params_set_access(g_tAudioInfo.handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_set_access/n");
        goto ERR_SET_PARAMS;
    }

    /* Set sample format */
    if (AIGetFormat(g_tAudioInfo.enBitWidth,  &format) < 0)
    {
        fprintf(stderr, "Error get_snd_pcm_format/n");
        goto ERR_SET_PARAMS;
    }
    
	/*if (SNDWAV_P_GetFormat(wav, &format) < 0) {
		fprintf(stderr, "Error get_snd_pcm_format/n");
		goto ERR_SET_PARAMS;
	}*/

    if (snd_pcm_hw_params_set_format(g_tAudioInfo.handle, hwparams, format) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_set_format/n");
        goto ERR_SET_PARAMS;
    }
    g_tAudioInfo.format = format;

    /* Set number of channels */
   if (snd_pcm_hw_params_set_channels(g_tAudioInfo.handle, hwparams, g_tAudioInfo.channels) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_set_channels/n");
        goto ERR_SET_PARAMS;
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
    AIGetSampleRate(g_tAudioInfo.enSampleRate, &u32SampleRate);

    if (snd_pcm_hw_params_set_rate_near(g_tAudioInfo.handle, hwparams, &u32SampleRate, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_set_rate_near/n");
        goto ERR_SET_PARAMS;
    }
    /*if (LE_INT(wav->format.sample_rate) != exact_rate) {
		fprintf(stderr, "The rate %d Hz is not supported by your hardware./n ==> Using %d Hz instead./n", 
			LE_INT(wav->format.sample_rate), exact_rate);
	}*/


    if (snd_pcm_hw_params_get_buffer_time_max(hwparams, &buffer_time, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_get_buffer_time_max/n");
        goto ERR_SET_PARAMS;
    }
    
    buffer_time = VOICE_PACAKGE_10MS;
    if (buffer_time > 500000)
    {
        buffer_time = 500000;
    }
    period_time = buffer_time / 4;

    if (snd_pcm_hw_params_set_buffer_time_near(g_tAudioInfo.handle, hwparams, &buffer_time, 0) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params_set_buffer_time_near/n");
        goto ERR_SET_PARAMS;
    }

    if (snd_pcm_hw_params_set_period_time_near(g_tAudioInfo.handle, hwparams, &period_time, 0) < 0)
    {
		    fprintf(stderr, "Error snd_pcm_hw_params_set_period_time_near/n");
		    goto ERR_SET_PARAMS;
    }

    /* Set hw params */
    if (snd_pcm_hw_params(g_tAudioInfo.handle, hwparams) < 0)
    {
        fprintf(stderr, "Error snd_pcm_hw_params(handle, params)/n");
        goto ERR_SET_PARAMS;
    }
    
    snd_pcm_hw_params_get_period_size(hwparams, &g_tAudioInfo.frame_num, 0);
    snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
    if (g_tAudioInfo.frame_num == buffer_size)
    {		
        fprintf(stderr, ("Can't use period equal to buffer size (%lu == %lu)/n"), g_tAudioInfo.frame_num, buffer_size);		
        goto ERR_SET_PARAMS;
    }
    
    //获取样本长度
    g_tAudioInfo.bits_per_sample = snd_pcm_format_physical_width(format);
    g_tAudioInfo.bits_per_frame = g_tAudioInfo.bits_per_sample * g_tAudioInfo.channels;
    g_tAudioInfo.chunk_bytes = g_tAudioInfo.frame_num * g_tAudioInfo.bits_per_frame / 8;
	
	
  //3、通过snd_pcm_hw_params_get_period_size()取得peroid_size，注意在ALSA中peroid_size是以frame为单位的。
  //	The configured buffer and period sizes are stored in “frames” in the runtime. 1 frame = channels * sample_size. 
  //	所以要对peroid_size进行转换：chunk_bytes = peroid_size * sample_length / 8。chunk_bytes就是我们单次从WAV读PCM数据的大小。
  //  之后的过程就乏善可陈了。唯一要留意的是snd_pcm_writei()和snd_pcm_readi()的第三个参数size也是以frame为单位，不要忘记frames和bytes的转换。

    return 0;

ERR_SET_PARAMS:
    return -1;
}


int AENC_SetAIAttr(T_AUDIO_CH_ATTR *pAIAttr)
{
    g_tAudioInfo.enBitWidth = pAIAttr->enBitWidth;
    g_tAudioInfo.enSampleRate = pAIAttr->enSampleRate;
}

int AENC_GetAIAttr(T_AUDIO_CH_ATTR *pAIAttr)
{
    pAIAttr->enSampleRate = g_tAudioInfo.enSampleRate;
    pAIAttr->enBitWidth = g_tAudioInfo.enBitWidth;
}

int AENC_SetEncAttr(T_AUDIO_ENC_ATTR *pEncAttr)
{
    if (pEncAttr)
    {
        g_tAudioEnc.enEncType = pEncAttr->enEncType;
        g_tAudioEnc.bFlag = pEncAttr->bFlag;
    }
    return 0;
}

int AENC_GetEncAttr(T_AUDIO_ENC_ATTR *pEncAttr)
{
    if (pEncAttr)
    {
        pEncAttr->enEncType = g_tAudioEnc.enEncType;
        pEncAttr->bFlag =  g_tAudioEnc.bFlag;
    }
    return 0;
}

int AENC_StartEnc(void)
{
    pthread_t tid;
    int iRet = 0;
    iRet = SetAudioChAttr();
    printf("setAudioChAttr:iRet:%d\r\n",iRet);
    printf("g_tAudioInfo.chunk_bytes===%d\r\n",g_tAudioInfo.chunk_bytes);
        /* Allocate audio data buffer */
    g_tAudioInfo.data_buf = (uint8_t *)malloc(g_tAudioInfo.chunk_bytes);
    if (NULL == g_tAudioInfo.data_buf)
    {
        fprintf(stderr, "Error malloc: [data_buf]/n");
        return -1;
    }
    
    g_tAudioEnc.pcEncBuf = (uint8_t *)malloc(g_tAudioInfo.chunk_bytes);
    if (NULL == g_tAudioEnc.pcEncBuf)
    {
        fprintf(stderr, "Error malloc: [pcEncBuf]/n");
        return -1;
    }
    
    g_tAudioEnc.chunk_bytes = g_tAudioInfo.chunk_bytes;
    g_iExitThreadFlag = 1;
    g_iCaptureFlag = 1;
    //pthread_create(&tid, NULL, AudioCaptureThread, (void *)&g_tAudioInfo);
    
    return iRet;
}

void AENC_StopEnc(void)
{
    g_iCaptureFlag = 0;
    g_iExitThreadFlag = 0;
    usleep(100000);
    snd_pcm_drain(g_tAudioInfo.handle);
    free(g_tAudioInfo.data_buf);
    g_tAudioInfo.data_buf = NULL;
    free(g_tAudioEnc.pcEncBuf);
    g_tAudioEnc.pcEncBuf = NULL;
    
}

static int StartAudioEnc(AUDIO_CODEC_FORMAT_E eAudioEncType, void *pRawData, void *pEncAddr, int len)
{

    switch(g_tAudioEnc.enEncType)
    {
        case AUDIO_CODEC_FORMAT_G711A:
        {
            //G711_linear2alaw(len/2, pRawData, pEncAddr);
            return len/2;
        }
        case AUDIO_CODEC_FORMAT_G711MU:
        {
            //G711_linear2ulaw(len/2, pRawData, pEncAddr);
            return len/2;
        }
        case AUDIO_CODEC_FORMAT_G726:	
        default:
            memcpy(pEncAddr, pRawData, len);
            return len;
        	break;
    }
    return 0;
}

int AENC_GetStream_FromSnd(T_AENC_STREAM *pStream)
{
   
    int count = g_tAudioInfo.frame_num;
    int r =snd_pcm_readi(g_tAudioInfo.handle,g_tAudioEnc.pcEncBuf,count);
    pStream->ps16Addr =g_tAudioEnc.pcEncBuf;
    pStream->u32FrameNum =g_tAudioInfo.frame_num;
    if (r == -EAGAIN || (r >= 0 && (size_t)r < count))
        {
            snd_pcm_wait(g_tAudioInfo.handle, 1000);
        }
        else if (r == -EPIPE)
        {
            snd_pcm_prepare(g_tAudioInfo.handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>/n");
        }
        else if (r == -ESTRPIPE)
        {
            fprintf(stderr, "<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>/n");
        }
        else if (r < 0)
        {
            fprintf(stderr, "Error snd_pcm_writei: [%s]", snd_strerror(r));
          
        }
        pStream->u32Len =  g_tAudioEnc.chunk_bytes;
    return r;
}

#if 0
int AENC_GetStream(T_AENC_STREAM *pStream, int iBlockFlag)
{
    int iNum = 0;
    int iLen = 0;
    int iFrameSize = 0;
    char *pcFrameAddr = NULL;
    char *pcTmp = NULL;
    int iSize = 0;
    int i = 0;

    if (pStream == NULL)
    {
        return -1;	
    }
    iNum = AUDIO_GetQueueNum();
    if (iNum == 0)
    {
        if (TRUE == iBlockFlag)
        {
            AUDIO_GetQueueWait();
            iNum = AUDIO_GetQueueNum();
        }
        else
        {
            return -1;
        }
    }
    iFrameSize = iNum * g_tAudioEnc.chunk_bytes;
    pcFrameAddr = g_pcBuf;
/*    pcFrameAddr = malloc(iFrameSize);
    if (pcFrameAddr == NULL)
    {
        printf("[%s]:malloc memery failufe\n", __FUNCTION__);
        return -1;	
    }
*/
    pStream->ps16Addr = (S16 *)pcFrameAddr;
    for (i = 0; i < iNum; i++)
    {
        iLen = AUDIO_PopQueue(g_tAudioEnc.pcEncBuf,  g_tAudioInfo.chunk_bytes);
        iSize = StartAudioEnc(g_tAudioEnc.enEncType, (void *)g_tAudioEnc.pcEncBuf, (void *)(pcFrameAddr), iLen);
        pcFrameAddr += iSize;
       
    }
    pStream->u32FrameNum = g_u32FrameNum++;
    pStream->u32Len = (long)(pcFrameAddr) - (long)(pStream->ps16Addr);
    //pStream->u64TimeStamp = g_u32FrameNum++;

    return 0;
}

void AENC_ReleaseStream(T_AENC_STREAM *pStream)
{
    if (pStream)
    {

        /*if (pStream->ps16Addr)
        {
            free((void *)(pStream->ps16Addr));
            pStream->ps16Addr = NULL;
        }*/
    }
}
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "audioqueue.h"


static T_AUDIO_QUEUE	   s_tAudioQueue;
/*********************************************
* FUNCTION: AUDIO_InitQueue
* PURPUSE:  init audio queue
* INPUT:    iFrameLen: The audio frame length
* RETURN:   0:success, -1, failure
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
int AUDIO_InitQueue(int iFrameLen)
{
    int i;
    pthread_mutexattr_t mutexattr;

    memset(&s_tAudioQueue, 0, sizeof(s_tAudioQueue));
    s_tAudioQueue.iMaxFrameLen = iFrameLen;
    for (i = 0; i < MAX_ENC_QUEUE_NUM; i++ )
    {
        s_tAudioQueue.atFrame[i].pcData = malloc(iFrameLen);
        if (s_tAudioQueue.atFrame[i].pcData == NULL)
        {
            goto QUEUE_ERR;	
        }
        s_tAudioQueue.atFrame[i].iLen = 0;
    }

    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_TIMED_NP);
    pthread_mutex_init(&s_tAudioQueue.mutex,&mutexattr);
    pthread_mutexattr_destroy(&mutexattr);

    pthread_cond_init(&s_tAudioQueue.cond,NULL);

    return 0;
QUEUE_ERR:
    for (i = 0; i < MAX_ENC_QUEUE_NUM; i++ )
    {
        if (s_tAudioQueue.atFrame[i].pcData)
        {
            free(s_tAudioQueue.atFrame[i].pcData);	
            s_tAudioQueue.atFrame[i].pcData = NULL;
        }
    }
    
    return -1;
}

/**********************************************
* FUNCTION: AUDIO_UninitQueue
* PURPUSE:  uninit audio queue
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
void AUDIO_UninitQueue(void)
{
    int i = 0;

    s_tAudioQueue.iTotal = 0;
    pthread_mutex_destroy(&s_tAudioQueue.mutex);
    pthread_cond_destroy(&s_tAudioQueue.cond);

    for (i = 0; i < MAX_ENC_QUEUE_NUM; i++)
    {
        free(s_tAudioQueue.atFrame[i].pcData);
        s_tAudioQueue.atFrame[i].pcData = NULL;
    }

}

/**********************************************
* FUNCTION: AUDIO_PostQueue
* PURPUSE:  write data to  audio queue
* INPUT:    pcData: audio data
*           len:    audio data len
* RETURN:    0:success, -1, failure
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
int AUDIO_PostQueue(char *pcData, int len)
{
    int iHead		= 0;
    int iTail 		= 0;
    int iTotal  	= 0;
    
    if ((pcData == NULL) || (len == 0))
    {
        return 0;
    }
    pthread_mutex_lock(&s_tAudioQueue.mutex);
    iHead	 = s_tAudioQueue.iHead;
    iTail	 = s_tAudioQueue.iTail;
    iTotal = s_tAudioQueue.iTotal;

    memcpy(s_tAudioQueue.atFrame[iHead].pcData, pcData, len);
    s_tAudioQueue.atFrame[iHead].iLen = len;
    iHead++;

    if(iHead >= MAX_ENC_QUEUE_NUM)
    {
        iHead = 0;
    }
    s_tAudioQueue.iHead =  iHead;

    if (MAX_ENC_QUEUE_NUM == iTotal)
    {
        s_tAudioQueue.iTail++;
        if(s_tAudioQueue.iTail >= MAX_ENC_QUEUE_NUM)
        {
            s_tAudioQueue.iTail = 0;
        }
    }
    else
    {
        s_tAudioQueue.iTotal++;
    }
    //printf("s_tAudioQueue.iTail++%d\r\n",s_tAudioQueue.iTail);
	pthread_cond_signal(&s_tAudioQueue.cond);
	pthread_mutex_unlock(&s_tAudioQueue.mutex);

	return 0;
}

/**********************************************
* FUNCTION: AUDIO_PopQueue
* PURPUSE:  read data from audio queue
* OUTPUT:   pData: audio data       
* RETURN:   0:success, -1, timeout
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
int AUDIO_PopQueue(char *pcData, int len)
{
    int iRet = 0;
    int iTail = 0;

    if ((pcData == NULL) || (len == 0))
    {
        printf("[%s]%d, paramis err\n", __FUNCTION__, __LINE__);
        return 0;
    }

    pthread_mutex_lock(&s_tAudioQueue.mutex);
    if(s_tAudioQueue.iTotal > 0)
    {
        iTail = s_tAudioQueue.iTail;
        if (len > s_tAudioQueue.iMaxFrameLen)
        {
            printf("[%s]ERROR:audio frame length too small\n", __FUNCTION__);
            pthread_mutex_unlock(&s_tAudioQueue.mutex);
            return iRet;
        }
        s_tAudioQueue.iTotal--;
        memcpy(pcData, s_tAudioQueue.atFrame[iTail].pcData, 
                         s_tAudioQueue.atFrame[iTail].iLen);
        iRet = s_tAudioQueue.atFrame[iTail].iLen;
        s_tAudioQueue.iTail++;
        if(s_tAudioQueue.iTail >= MAX_ENC_QUEUE_NUM)
        {
            s_tAudioQueue.iTail = 0;
        }
    }
    pthread_mutex_unlock(&s_tAudioQueue.mutex);

    return iRet;
}

/**********************************************
* FUNCTION: AUDIO_GetQueueNum
* PURPUSE:  get frame num from audio queue    
* RETURN:   frame num
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
int AUDIO_GetQueueNum(void)
{
    int iNum = 0;

    pthread_mutex_lock(&s_tAudioQueue.mutex);
    iNum = s_tAudioQueue.iTotal;
    pthread_mutex_unlock(&s_tAudioQueue.mutex);

    return iNum;
}

/**********************************************
* FUNCTION: AUDIO_GetQueueWaittime
* PURPUSE:  waittime
* INPUT:   abstime: time      
* RETURN:   
* AUTHOR :  dingjq 
* DATE:     2013-04-24
* MODIFY:   
*********************************************/
int AUDIO_GetQueueWait(void)
{
    int iRet;

    /*struct timespes tTimeout;
    if (abstime == NULL)
    {
        tTimeout.tv_sec = 0;
        tTimeout.tv_nsec = 0;
    }
    else
    {
        tTimeout = *abstime;
    }*/

    pthread_cleanup_push(pthread_mutex_unlock, (void *)&(s_tAudioQueue.mutex));
    pthread_mutex_lock(&s_tAudioQueue.mutex);
   // iRet = pthread_cond_waittime(&s_tAudioQueue.cond, &s_tAudioQueue.mutex, &tTimeout);
    pthread_cond_wait(&s_tAudioQueue.cond, &s_tAudioQueue.mutex);
    pthread_mutex_unlock(&s_tAudioQueue.mutex);
    pthread_cleanup_pop(0);

    return 0;
}


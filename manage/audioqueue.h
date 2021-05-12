/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-06-04 15:57:18
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-04 16:53:20
 */ 
#ifndef _AUDIO_QUEUE_H_
#define _AUDIO_QUEUE_H_

#define MAX_ENC_QUEUE_NUM 200

typedef struct _T_AUDIO_DATA
{
    int iLen;
    char *pcData;
}T_AUDIO_DATA, *PT_AUDIO_DATA;


typedef struct _T_AUDIO_QUEUE
{
    T_AUDIO_DATA atFrame[MAX_ENC_QUEUE_NUM];
    int   iHead;
    int   iTail;
    int   iTotal;
    int   iMaxFrameLen;
    pthread_cond_t  cond;
    pthread_mutex_t	mutex;
}T_AUDIO_QUEUE, *PT_AUDIO_QUEUE;



int AUDIO_InitQueue(int iFrameLen);
void AUDIO_UninitQueue(void);
int AUDIO_PostQueue(char *pcData, int len);
int AUDIO_PopQueue(char *pcData, int len);
int AUDIO_GetQueueNum(void);
int AUDIO_GetQueueWait();

#endif

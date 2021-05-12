/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2014-07-11 15:57:44
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-05-27 11:55:06
 */ 
/*
** $Id: misc.h,v 1.13 2003/09/26 08:45:14 snig Exp $
**
** misc.h: the head file for Miscellous module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/03
*/

#ifndef GUI_MISC_H
#define GUI_MISC_H



//读取配置文件字段
extern int GetValueFromEtcFile(const char* pEtcFile, const char* pSection, const char* pKey, char* pValue, int iLen);
extern int SetValueToEtcFile(const char* pEtcFile, const char* pSection, const char* pKey, char* pValue);
int etc_LocateKeyValue(FILE* fp, const char* pKey, 
                               int bCurSection, char* pValue, int iLen,
                               FILE* bak_fp, char* nextSection);


#endif



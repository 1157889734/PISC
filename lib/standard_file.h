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

#ifndef STANDARD_FILE_H
#define STANDARD_FILE_H

#define MAX_FILE_SIZE 1024*64


int standard_file_read(const char *file, char *buf,int *file_size);


#endif



/******************************************************************
*
*	CyberIO for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: cfile.h
*
*	Revision:
*
*	02/1/05
*		- first revision
*
******************************************************************/

#ifndef _CG_IO_CFILE_H_
#define _CG_IO_CFILE_H_

#include <cybergarage/typedef.h>
#include <cybergarage/util/cstring.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#if defined(WIN32)
#define CG_FILE_SEPARATOR_CHAR '\\'
#define CG_FILE_PATH_SEPARATOR_CHAR ';'
#define CG_FILE_SEPARATOR "\\"
#define CG_FILE_PATH_SEPARATOR ";"
#else
#define CG_FILE_SEPARATOR_CHAR '/'
#define CG_FILE_PATH_SEPARATOR_CHAR ':'
#define CG_FILE_SEPARATOR "/"
#define CG_FILE_PATH_SEPARATOR ":"
#endif
	
#define CG_FILE_READ_CHUNK_SIZE 1024

/****************************************
* Data Type
****************************************/

typedef struct _CgFile {
	CgString *name;
	CgString *path;
	char *content;
} CgFile;

/****************************************
* Function
****************************************/

CgFile *cg_file_new();
void cg_file_delete(CgFile *file);

void cg_file_setname(CgFile *file, char *name);
char *cg_file_getname(CgFile *file);
long cg_file_getlength(CgFile *file);

long cg_file_getlastmodified(CgFile *file);
long cg_file_getlength(CgFile *file);
BOOL cg_file_exists(CgFile *file);
BOOL cg_file_remove(CgFile *file);

void cg_file_setcontent(CgFile *file, char *content);
char *cg_file_getcontent(CgFile *file);

char *cg_file_getfilename(CgFile *file);
char *cg_file_getpath(CgFile *file);

BOOL cg_file_load(CgFile *file);
BOOL cg_file_save(CgFile *file);

#ifdef  __cplusplus
}
#endif

#endif

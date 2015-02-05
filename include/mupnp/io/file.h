/******************************************************************
*
*	CyberIO for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <mupnp/typedef.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(WIN32) || defined(WINCE)
#define CG_USE_CFILE 1
#endif

#if defined(CG_USE_CFILE)

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

#define CG_FILE_OPEN_READ		0x01
#define CG_FILE_OPEN_WRITE		0x02
#define CG_FILE_OPEN_CREATE	0x04

#define CG_FILE_SEEK_SET			0x01
#define CG_FILE_SEEK_CUR			0x02
#define CG_FILE_SEEK_END			0x04

/****************************************
* Data Type
****************************************/

typedef struct _CgFile {
	BOOL headFlag;
	struct _CgFile *prev;
	struct _CgFile *next;
	CgString *name;
	CgString *path;
	char *content;
	FILE	*fp;
} CgFile, CgFileList;

/****************************************
* Function
****************************************/

CgFile *mupnp_file_new();
void mupnp_file_delete(CgFile *file);

#define mupnp_file_next(file) (CgFile *)mupnp_list_next((CgList *)file)

void mupnp_file_setname(CgFile *file, char *name);

#define mupnp_file_setpath(file, value) mupnp_file_setname(file, value)
void mupnp_file_setfilename(CgFile *file, char *value);
#define mupnp_file_addfilename(file, value) mupnp_file_setfilename(file, value)

char *mupnp_file_getname(CgFile *file);
long mupnp_file_getlength(CgFile *file);

long mupnp_file_getlastmodified(CgFile *file);
long mupnp_file_getlength(CgFile *file);
BOOL mupnp_file_exists(CgFile *file);
BOOL mupnp_file_remove(CgFile *file);

void mupnp_file_setcontent(CgFile *file, char *content);
char *mupnp_file_getcontent(CgFile *file);

char *mupnp_file_getfilename(CgFile *file);
char *mupnp_file_getpath(CgFile *file);

BOOL mupnp_file_load(CgFile *file);
BOOL mupnp_file_save(CgFile *file);

int mupnp_file_listfiles(CgFile *file, CgFileList *fileList);

#define mupnp_file_isseparatorchar(c) ((CG_FILE_SEPARATOR_CHAR == c) ? TRUE : FALSE)
#define mupnp_file_ispathseparatorchar(c) ((CG_FILE_PATH_SEPARATOR_CHAR == c) ? TRUE : FALSE)

/* Raw Functions */
BOOL mupnp_file_open(CgFile *file, int flag);
BOOL mupnp_file_close(CgFile *file);
BOOL mupnp_file_write(CgFile *file, CgByte *buf, int bufLen);
BOOL mupnp_file_read(CgFile *file, CgByte *buf, int bufLen);
BOOL mupnp_file_seek(CgFile *file, CgInt64, int whence); 

/****************************************
* Function (File List)
****************************************/

/**
 * Create a new file list
 *
 * \return File list
 */
CgFileList *mupnp_filelist_new();

/**
 * Destroy a file list
 *
 * \param fileList The file list in question
 */
void mupnp_filelist_delete(CgFileList *fileList);

/**
 * Clear the contents of a file list
 *
 * \param fileList File list in question
 */
#define mupnp_filelist_clear(fileList) mupnp_list_clear((CgList *)fileList, (CG_LIST_DESTRUCTORFUNC)mupnp_file_delete)

/**
 * Get the size of a file list
 *
 * \param fileList The file list in question
 */
#define mupnp_filelist_size(fileList) mupnp_list_size((CgList *)fileList)

/**
 * Get the first actual item from a file list to use as an iterator
 *
 * \param fileList The file list in question
 */
#define mupnp_filelist_gets(fileList) (CgFile *)mupnp_list_next((CgList *)fileList)

/**
 * Add a file into a file list
 *
 * \param fileList The file list in question
 * \param file The file to add to the list
 */
#define mupnp_filelist_add(fileList, file) mupnp_list_add((CgList *)fileList, (CgList *)file)

/**
 * Remove a file from file list
 *
 * \param fileList The file list in question
 * \param file The file to be removed 
 */
#define mupnp_filelist_remove(file) mupnp_list_remove((CgList *)file)

#endif

#ifdef  __cplusplus
}
#endif

#endif

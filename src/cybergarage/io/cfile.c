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
*	File: cfile.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*	03/19/06 Theo Beisch
*		- WINCE support (still untested)
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/io/cfile.h>
#include <cybergarage/util/clog.h>

#include <stdio.h>

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <windows.h>
#include <sys/stat.h>
#elif defined (_WIN32_WCE)
#include <windows.h>

#include <sys\types.h> //from PortSDK

struct stat {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };

#elif defined(BTRON) || defined(ITRON) || defined(TENGINE)
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <bsys/unixemu.h>
#else
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

/****************************************
* cg_file_new
****************************************/

CgFile *cg_file_new()
{
	CgFile *file;

	cg_log_debug_l4("Entering...\n");
	
	file = (CgFile *)malloc(sizeof(CgFile));

	if ( NULL != file )
	{
		file->name = cg_string_new();
		file->path = cg_string_new();
		file->content = NULL;
	}

	return file;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_delete
****************************************/

void cg_file_delete(CgFile *file)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_delete(file->name);
	cg_string_delete(file->path);
	if (file->content != NULL)
		free(file->content);
	free(file);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_setname
****************************************/

void cg_file_setname(CgFile *file, char *name)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(file->name, name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getname
****************************************/

char *cg_file_getname(CgFile *file)
{
	cg_log_debug_l4("Entering...\n");

	return cg_string_getvalue(file->name);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getname
****************************************/

long cg_file_getlastmodified(CgFile *file)
{
	char *fileName;
	struct stat buf ;

	cg_log_debug_l4("Entering...\n");

#if defined (_WIN32_WCE)
	buf.st_mtime = 0;
#endif

	fileName = cg_file_getname(file);
#if defined(BTRON) || defined(ITRON) || defined(TENGINE) 
	if(u_stat(fileName, &buf ) == -1)
		return 0;
	return buf.st_mtime;
#elif defined (_WIN32_WCE)
#pragma message ("WIN32_WCE - support for file I/O not provided by CSTL - fixmelater")
	return 0;
#else
	if(stat(fileName, &buf ) == -1)
		return 0;
	return buf.st_mtime;
#endif

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getname
****************************************/

long cg_file_getlength(CgFile *file)
{
	char *fileName;
	struct stat buf ;
	
	cg_log_debug_l4("Entering...\n");

	fileName = cg_file_getname(file);
#if defined(BTRON) || defined(ITRON) || defined(TENGINE) 
	if(u_stat(fileName, &buf ) == -1)
		return 0;
#elif defined (_WIN32_WCE)
#pragma message ("WIN32_WCE - support for file I/O not provided by CSTL - fixmelater")
	return 0;
#else
	if(stat(fileName, &buf ) == -1)
		return 0;
#endif
	return buf.st_size;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_exists
****************************************/

BOOL cg_file_exists(CgFile *file)
{
#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
	FILE *fp;
#else
	int fd;
#endif
	char *fileName;
	
	cg_log_debug_l4("Entering...\n");

	fileName = cg_file_getname(file);
	
#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
	fp = fopen(fileName, "r");
	if (fp == NULL)
		return FALSE;
	fclose(fp);
	return TRUE;
#else
	fd = open(fileName, O_RDONLY);
	if (fd == -1)
		return FALSE;
	close(fd);
	return TRUE;
#endif

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_remove
****************************************/

BOOL cg_file_remove(CgFile *file)
{
	char *fileName;
	int removeSuccess = 0;
	
	cg_log_debug_l4("Entering...\n");

	fileName = cg_file_getname(file);
#if defined(_WIN32_WCE)
	#pragma message("############################## FIXMELATER cfile.c - Verify File removal (untested on CE)")
	removeSuccess = DeleteFile((void*)fileName);
#else
	removeSuccess = remove(fileName);
#endif
	
	return (removeSuccess == 0) ? TRUE : FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_setcontent
****************************************/

void cg_file_setcontent(CgFile *file, char *content)
{
	cg_log_debug_l4("Entering...\n");

	if (file->content != NULL) {
		free(file->content);
		file->content = NULL;
	}
	file->content = cg_strdup(content);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getcontent
****************************************/

char *cg_file_getcontent(CgFile *file)
{
	cg_log_debug_l4("Entering...\n");

	return file->content;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getfilename
****************************************/

char *cg_file_getfilename(CgFile *file)
{
	char *fileName;
	int sepIdx;

	cg_log_debug_l4("Entering...\n");

	fileName = cg_file_getname(file);	
	sepIdx = cg_strrchr(fileName, CG_FILE_SEPARATOR, 1);
	if (0 < sepIdx)
		return (fileName + sepIdx + 1);
	return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_getpath
****************************************/

char *cg_file_getpath(CgFile *file)
{
	char *path;
	char *fileName;
	int sepIdx;

	cg_log_debug_l4("Entering...\n");

	path = cg_string_getvalue(file->path);
	if (path != NULL)
		return path;
		
	fileName = cg_file_getname(file);	
	sepIdx = cg_strrchr(fileName, CG_FILE_SEPARATOR, 1);
	
	if (0 < sepIdx) {
		path = (char *)malloc(sizeof(char)*(sepIdx+1));

		if  ( NULL == path )
		{
			cg_log_debug_s("Memory allocation failure!\n");
			return NULL;
		}
		
		cg_strncpy(path, fileName, sepIdx);
		path[sepIdx]='\0';
		cg_string_setpointervalue(file->path, path);
	}
	
	return cg_string_getvalue(file->path);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_load
****************************************/

BOOL cg_file_load(CgFile *file)
{
#if defined(WIN32) || defined(HAVE_FOPEN)
	FILE *fp;
	size_t nRead;
	size_t readCnt;
#else
	int fd;
	ssize_t nRead;
	ssize_t readCnt;
#endif
	char *fileName;
	long fileLen;
	
	cg_log_debug_l4("Entering...\n");

	if ( NULL == file )
		return FALSE;
	
	if (file->content != NULL) {
		free(file->content);
		file->content = NULL;
	}
	
	fileLen = cg_file_getlength(file);
	if (fileLen <= 0)
		return FALSE;
		
	fileName = cg_file_getname(file);
	file->content = (char *)malloc(fileLen + 1);		

	if ( NULL == file->content )
	{
		cg_log_debug("Memory allocation failure!\n");
		return FALSE;
	}
	
#if defined(WIN32) || defined(HAVE_FOPEN)
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		file->content[0] = '\0';
		return FALSE;
	}
	readCnt = 0;
	do {
		int remaining  = fileLen - readCnt;
		int chunkSize = remaining < CG_FILE_READ_CHUNK_SIZE ? remaining : CG_FILE_READ_CHUNK_SIZE;
		nRead = fread((file->content)+readCnt, sizeof(char), chunkSize, fp);
		readCnt += nRead;
	} while (0 < nRead);
	fclose(fp);
#else
	fd = open(fileName, O_RDONLY);
	if (fd == -1) {
		file->content[0] = '\0';
		return FALSE;
	}
	readCnt = 0;
	nRead = read(fd, (file->content)+readCnt, CG_FILE_READ_CHUNK_SIZE);
	while (0 < nRead) {
		readCnt += nRead;
		nRead = read(fd, (file->content)+readCnt, CG_FILE_READ_CHUNK_SIZE);
	}
	close(fd);
#endif

	file->content[readCnt] = '\0';
	
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_file_save
****************************************/

BOOL cg_file_save(CgFile *file)
{
#if defined(WIN32) || defined(HAVE_FOPEN)
	FILE *fp;
	size_t nWrite;
	size_t writeCnt;
	size_t contentLen;
#else
	int fd;
	ssize_t nWrite;
	ssize_t writeCnt;
	ssize_t contentLen;
#endif
	char *fileName;
	
	cg_log_debug_l4("Entering...\n");

	fileName = cg_file_getname(file);
	if (fileName == NULL)
		return FALSE;
		
	if (file->content == NULL)
		return FALSE;
	
	contentLen = cg_strlen(file->content);
	if (contentLen <= 0)
		return FALSE;
	
#if defined(WIN32) || defined(HAVE_FOPEN)
	fp = fopen(fileName, "w");
	if (fp == NULL)
		return FALSE;
	writeCnt = 0;
	nWrite = fwrite((file->content)+writeCnt, sizeof(char), contentLen-writeCnt, fp);
	while (0 < nWrite) {
		writeCnt += nWrite;
		if (contentLen <= writeCnt)
			break;		
		nWrite = fwrite((file->content)+writeCnt, sizeof(char), contentLen-writeCnt, fp);
	}
	fclose(fp);
#else
	fd = open(fileName, O_WRONLY);
	if (fd == -1)
		return FALSE;
	writeCnt = 0;
	nWrite = write(fd, (file->content)+writeCnt, contentLen-writeCnt);
	while (0 < nWrite) {
		writeCnt += nWrite;
		if (contentLen <= writeCnt)
			break;		
		nWrite = write(fd, (file->content)+writeCnt, contentLen-writeCnt);
	}
	close(fd);
#endif

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: cfile_list.c
*
*	Revision:
*
*	01/18/07
*		- first revision
*
******************************************************************/

#include <mupnp/io/file.h>

#if defined(CG_USE_CFILE)

#include <mupnp/util/log.h>

/****************************************
* mupnp_filelist_new
****************************************/

CgFileList *mupnp_filelist_new()
{
	CgFileList *fileList;

	mupnp_log_debug_l4("Entering...\n");

	fileList = (CgFileList *)malloc(sizeof(CgFileList));

	if ( NULL != fileList )
	{
		mupnp_list_header_init((CgList *)fileList);
		fileList->name = NULL;
		fileList->path = NULL;
		fileList->content = NULL;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return fileList;
}

/****************************************
* mupnp_filelist_delete
****************************************/

void mupnp_filelist_delete(CgFileList *fileList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_filelist_clear(fileList);
	free(fileList);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif


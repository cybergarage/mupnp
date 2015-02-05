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

mUpnpFileList *mupnp_filelist_new()
{
	mUpnpFileList *fileList;

	mupnp_log_debug_l4("Entering...\n");

	fileList = (mUpnpFileList *)malloc(sizeof(mUpnpFileList));

	if ( NULL != fileList )
	{
		mupnp_list_header_init((mUpnpList *)fileList);
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

void mupnp_filelist_delete(mUpnpFileList *fileList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_filelist_clear(fileList);
	free(fileList);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif


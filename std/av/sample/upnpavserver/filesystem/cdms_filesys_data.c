/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys_data.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"

/****************************************
* mupnp_upnpav_dms_filesys_content_data_new
****************************************/

mUpnpUpnpMediaFileSystemContentData *mupnp_upnpav_dms_filesys_content_data_new()
{
	mUpnpUpnpMediaFileSystemContentData *data;

	data = (mUpnpUpnpMediaFileSystemContentData *)malloc(sizeof(mUpnpUpnpMediaFileSystemContentData));
	data->pubdir = NULL;

	return data;
}

/****************************************
* mupnp_upnpav_dms_filesys_content_data_delete
****************************************/

void mupnp_upnpav_dms_filesys_content_data_delete(mUpnpUpnpMediaFileSystemContentData *data)
{
	if (!data)
		return;

	if (data->pubdir)
		free(data->pubdir);

	free(data);
}

/****************************************
* mupnp_upnpav_dms_filesys_content_getdata
****************************************/

mUpnpUpnpMediaFileSystemContentData *mupnp_upnpav_dms_filesys_content_getdata(mUpnpUpnpMediaContent *con)
{
	mUpnpUpnpMediaFileSystemContentData *data;

	data = mupnp_upnpav_content_getuserdata(con);
	if (!data) {
		data = mupnp_upnpav_dms_filesys_content_data_new();
		mupnp_upnpav_content_setuserdata(con, data);
	}

	return data;
}

/****************************************
* mupnp_upnpav_dms_filesys_content_setpubicdirectory
****************************************/

void mupnp_upnpav_dms_filesys_content_setpubicdirectory(mUpnpUpnpMediaContent *con, char *dir)
{
	mUpnpUpnpMediaFileSystemContentData *data;

	data = mupnp_upnpav_dms_filesys_content_getdata(con);
	if (data->pubdir)
		free(data->pubdir);
	data->pubdir = mupnp_strdup(dir);
}

/****************************************
* mupnp_upnpav_dms_filesys_content_getpubicdirectory
****************************************/

char *mupnp_upnpav_dms_filesys_content_getpubicdirectory(mUpnpUpnpMediaContent *con)
{
	mUpnpUpnpMediaFileSystemContentData *data;

	data = mupnp_upnpav_dms_filesys_content_getdata(con);
	return data->pubdir;
}

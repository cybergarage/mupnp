/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKC_MEDIASERVER_FILESYS_H_
#define _CG_CLINKC_MEDIASERVER_FILESYS_H_

#include <mupnp/std/av/cmediaserver.h>
#include <mupnp/std/av/ccontent.h>


#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _mUpnpUpnpMediaFileSystemContentData {
	char *pubdir;
} mUpnpUpnpMediaFileSystemContentData;


#define mUpnpUpnpMediaServer mUpnpUpnpAvServer
#define mUpnpUpnpMediaContent mUpnpUpnpAvContent
#define mUpnpUpnpMediaContentList mUpnpUpnpAvContentList
#define MUPNP_MEDIA_MIMETYPE_JPEG CG_UPNPAV_MIMETYPE_JPEG
#define MUPNP_MEDIA_MIMETYPE_MPEG CG_UPNPAV_MIMETYPE_MPEG
#define MUPNP_MEDIA_MIMETYPE_MP3  CG_UPNPAV_MIMETYPE_MP3
#define MUPNP_MEDIA_CONTENT_CONTAINER CG_UPNPAV_UPNPCLASS_CONTAINER
#define MUPNP_MEDIA_CONTENT_ITEM CG_UPNPAV_UPNPCLASS_ITEM
#define MUPNP_MEDIA_UPNPCLASS_NONE CG_UPNPAV_UPNPCLASS_NONE
#define MUPNP_MEDIA_UPNPCLASS_PHOTO CG_UPNPAV_UPNPCLASS_PHOTO
#define MUPNP_MEDIA_UPNPCLASS_MOVIE CG_UPNPAV_UPNPCLASS_MOVIE
#define MUPNP_MEDIA_UPNPCLASS_MUSIC CG_UPNPAV_UPNPCLASS_MUSIC
#define mUpnpUpnpMediaResource mUpnpUpnpAvResource
#define MUPNP_MEDIA_DLNAPN_MAXLEN CG_UPNPAV_DLNAATTR_MAXLEN
#define mupnp_upnp_media_resource_new mupnp_upnpav_resource_new
#define MUPNP_MEDIA_CONTENT_NONE 0
#define mupnp_upnp_dms_getdevice mupnp_upnpav_dms_getdevice
#define MUPNP_DMS_CONTENTDIRECTORY_BROWSE CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE
#define MUPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID
#define  CG_UPNPAV_FILESYS_RESURL_PATH "content"
#define  CG_UPNPAV_FILESYS_RESURL_MAXLEN (CG_NET_IPV6_ADDRSTRING_MAXSIZE + CG_MD5_STRING_BUF_SIZE + 64)

mUpnpUpnpAvServer *mupnp_upnpav_dms_filesys_new();
void mupnp_upnpav_dms_filesys_delete(mUpnpUpnpAvServer *dms);

#define mupnp_upnpav_dms_filesys_start(dms) mupnp_upnpav_dms_start(dms)
#define mupnp_upnpav_dms_filesys_stop(dms) mupnp_upnpav_dms_stop(dms)


void mupnp_upnpav_dms_filesys_setpublicationdirectory(mUpnpUpnpAvServer *dms, char *pubdir);
char *mupnp_upnpav_dms_filesys_getpublicationdirectory(mUpnpUpnpMediaServer *dms);

mUpnpUpnpMediaFileSystemContentData *mupnp_upnpav_dms_filesys_content_data_new();
void mupnp_upnpav_dms_filesys_content_data_delete(mUpnpUpnpMediaFileSystemContentData *data);
mUpnpUpnpMediaFileSystemContentData *mupnp_upnpav_dms_filesys_content_getdata(mUpnpUpnpAvContent *con);
void mupnp_upnpav_dms_filesys_content_setpubicdirectory(mUpnpUpnpMediaContent *con, char *dir);
char *mupnp_upnpav_dms_filesys_content_getpubicdirectory(mUpnpUpnpMediaContent *con);

void mupnp_upnpav_dms_filesys_http_listener(mUpnpHttpRequest *httpReq);

#ifdef  __cplusplus
}
#endif

#endif

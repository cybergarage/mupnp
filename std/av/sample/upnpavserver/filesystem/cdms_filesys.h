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

#include <cybergarage/upnp/std/av/cmediaserver.h>
#include <cybergarage/upnp/std/av/ccontent.h>


#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _CgUpnpMediaFileSystemContentData {
	char *pubdir;
} CgUpnpMediaFileSystemContentData;


#define CgUpnpMediaServer CgUpnpAvServer
#define CgUpnpMediaContent CgUpnpAvContent
#define CgUpnpMediaContentList CgUpnpAvContentList
#define CG_UPNP_MEDIA_MIMETYPE_JPEG CG_UPNPAV_MIMETYPE_JPEG
#define CG_UPNP_MEDIA_MIMETYPE_MPEG CG_UPNPAV_MIMETYPE_MPEG
#define CG_UPNP_MEDIA_MIMETYPE_MP3  CG_UPNPAV_MIMETYPE_MP3
#define CG_UPNP_MEDIA_CONTENT_CONTAINER CG_UPNPAV_UPNPCLASS_CONTAINER
#define CG_UPNP_MEDIA_CONTENT_ITEM CG_UPNPAV_UPNPCLASS_ITEM
#define CG_UPNP_MEDIA_UPNPCLASS_NONE CG_UPNPAV_UPNPCLASS_NONE
#define CG_UPNP_MEDIA_UPNPCLASS_PHOTO CG_UPNPAV_UPNPCLASS_PHOTO
#define CG_UPNP_MEDIA_UPNPCLASS_MOVIE CG_UPNPAV_UPNPCLASS_MOVIE
#define CG_UPNP_MEDIA_UPNPCLASS_MUSIC CG_UPNPAV_UPNPCLASS_MUSIC
#define CgUpnpMediaResource CgUpnpAvResource
#define CG_UPNP_MEDIA_DLNAPN_MAXLEN CG_UPNPAV_DLNAATTR_MAXLEN
#define cg_upnp_media_resource_new cg_upnpav_resource_new
#define CG_UPNP_MEDIA_CONTENT_NONE 0
#define cg_upnp_dms_getdevice cg_upnpav_dms_getdevice
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID
#define  CG_UPNPAV_FILESYS_RESURL_PATH "content"
#define  CG_UPNPAV_FILESYS_RESURL_MAXLEN (CG_NET_IPV6_ADDRSTRING_MAXSIZE + CG_MD5_STRING_BUF_SIZE + 64)

CgUpnpAvServer *cg_upnpav_dms_filesys_new();
void cg_upnpav_dms_filesys_delete(CgUpnpAvServer *dms);

#define cg_upnpav_dms_filesys_start(dms) cg_upnpav_dms_start(dms)
#define cg_upnpav_dms_filesys_stop(dms) cg_upnpav_dms_stop(dms)


void cg_upnpav_dms_filesys_setpublicationdirectory(CgUpnpAvServer *dms, char *pubdir);
char *cg_upnpav_dms_filesys_getpublicationdirectory(CgUpnpMediaServer *dms);

CgUpnpMediaFileSystemContentData *cg_upnpav_dms_filesys_content_data_new();
void cg_upnpav_dms_filesys_content_data_delete(CgUpnpMediaFileSystemContentData *data);
CgUpnpMediaFileSystemContentData *cg_upnpav_dms_filesys_content_getdata(CgUpnpAvContent *con);
void cg_upnpav_dms_filesys_content_setpubicdirectory(CgUpnpMediaContent *con, char *dir);
char *cg_upnpav_dms_filesys_content_getpubicdirectory(CgUpnpMediaContent *con);

void cg_upnpav_dms_filesys_http_listener(CgHttpRequest *httpReq);

#ifdef  __cplusplus
}
#endif

#endif

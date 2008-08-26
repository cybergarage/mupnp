/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontent.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKC_MEDIA_CONTENT_H_
#define _CG_CLINKC_MEDIA_CONTENT_H_

#include <cybergarage/upnp/std/av/cresource.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNP_MEDIA_CONTENT_NONE (0)
#define CG_UPNP_MEDIA_CONTENT_CONTAINER (1)
#define CG_UPNP_MEDIA_CONTENT_ITEM (2)

#define CG_UPNP_MEDIA_CONTENT_ID "id"
#define CG_UPNP_MEDIA_CONTENT_PARENTID "parentID"
#define CG_UPNP_MEDIA_CONTENT_CHILDCOUNT "childCount"
#define CG_UPNP_MEDIA_CONTENT_RESTRICTED "restricted"
#define CG_UPNP_MEDIA_CONTENT_TITLE "dc:title"
#define CG_UPNP_MEDIA_CONTENT_UPNPCLASS "upnp:class"

#define CG_UPNP_MEDIA_ROOT_CONTENT_ID "0"
#define CG_UPNP_MEDIA_ROOT_CONTENT_PARENTID "-1"

#define CG_UPNP_MEDIA_MIMETYPE_JPEG "image/jpeg"
#define CG_UPNP_MEDIA_MIMETYPE_MPEG "video/mpeg"
#define CG_UPNP_MEDIA_MIMETYPE_MP3 "audio/mpeg"

#define CG_UPNP_MEDIA_UPNPCLASS_PHOTO "object.item.imageItem.photo"
#define CG_UPNP_MEDIA_UPNPCLASS_MOVIE "object.item.videoItem.movie"
#define CG_UPNP_MEDIA_UPNPCLASS_MUSIC "object.item.audioItem.musicTrack"

#define CG_UPNP_MEDIA_DLNAPN_MAXLEN 128
#define CG_UPNP_MEDIA_DLNA_PN_JPEG_LRG "JPEG_LRG"
#define CG_UPNP_MEDIA_DLNA_PN_MPEG_PS_NTSC "MPEG_PS_NTSC"
#define CG_UPNP_MEDIA_DLNA_PN_MP3 "MP3"

/****************************************
* Struct
****************************************/

typedef CgXmlNode CgUpnpMediaContent;
typedef CgXmlNodeList CgUpnpMediaContentList;

/****************************************
* Content
****************************************/

#define cg_upnp_media_contentlist_new() cg_xml_nodelist_new()
#define cg_upnp_media_contentlist_delete(conList) cg_xml_nodelist_delete(conList)
#define cg_upnp_media_contentlist_clear(conList) cg_xml_nodelist_clear(conList)
#define cg_upnp_media_contentlist_size(conList) cg_xml_nodelist_size(conList)
#define cg_upnp_media_contentlist_gets(conList) cg_xml_nodelist_gets(conList)
#define cg_upnp_media_contentlist_add(conList, res) cg_xml_nodelist_add(conList, res)

#define cg_upnp_media_content_new() cg_xml_node_new()
#define cg_upnp_media_content_delete(con) cg_xml_node_delete(con)
CgUpnpMediaContent *cg_upnp_media_content_next(CgUpnpMediaContent *con);
#define cg_upnp_media_content_remove(con) cg_xml_node_remove(con)

void cg_upnp_media_content_copy(CgUpnpMediaContent *destContent, CgUpnpMediaContent *srcContent);

void cg_upnp_media_content_addchildcontent(CgUpnpMediaContent *con, CgUpnpMediaContent *childCon);
int cg_upnp_media_content_getnchildcontents(CgUpnpMediaContent *con);
CgUpnpMediaContent *cg_upnp_media_content_getchildcontents(CgUpnpMediaContent *con);
BOOL cg_upnp_media_content_haschildcontents(CgUpnpMediaContent *con);
#define cg_upnp_media_content_clearchildcontents(con) cg_xml_nodelist_clear(cg_xml_node_getchildnodelist(con))

#define cg_upnp_media_content_setid(con, val) cg_xml_node_setattribute(con, CG_UPNP_MEDIA_CONTENT_ID, val)
#define cg_upnp_media_content_getid(con) cg_xml_node_getattributevalue(con, CG_UPNP_MEDIA_CONTENT_ID)

#define cg_upnp_media_content_setparentid(con, val) cg_xml_node_setattribute(con, CG_UPNP_MEDIA_CONTENT_PARENTID, val)
#define cg_upnp_media_content_getparentid(con) cg_xml_node_getattributevalue(con, CG_UPNP_MEDIA_CONTENT_PARENTID)

#define cg_upnp_media_content_settitle(con, val) cg_xml_node_setchildnode(con, CG_UPNP_MEDIA_CONTENT_TITLE, val)
#define cg_upnp_media_content_gettitle(con) cg_xml_node_getchildnodevalue(con, CG_UPNP_MEDIA_CONTENT_TITLE)

#define cg_upnp_media_content_setupnpclass(con, val) cg_xml_node_setchildnode(con, CG_UPNP_MEDIA_CONTENT_UPNPCLASS, val)
#define cg_upnp_media_content_getupnpclass(con) cg_xml_node_getchildnodevalue(con, CG_UPNP_MEDIA_CONTENT_UPNPCLASS)

void cg_upnp_media_content_settype(CgUpnpMediaContent *con, int type);
int cg_upnp_media_content_gettype(CgUpnpMediaContent *con);

#define cg_upnp_media_content_isitemnode(node) (cg_upnp_media_content_gettype(node) == CG_UPNP_MEDIA_CONTENT_ITEM ? TRUE : FALSE)
#define cg_upnp_media_content_iscontainernode(node) (cg_upnp_media_content_gettype(node) == CG_UPNP_MEDIA_CONTENT_CONTAINER ? TRUE : FALSE)
BOOL cg_upnp_media_content_iscontentnode(CgXmlNode *node);

#define cg_upnp_media_content_setuserdata(con, data) cg_xml_node_setuserdata(con, data)
#define cg_upnp_media_content_getuserdata(con) cg_xml_node_getuserdata(con)

typedef BOOL (*CG_UPNP_MEDIA_CONTENT_COMPARE_FUNC)(CgUpnpMediaContent *con, void *userData);
CgUpnpMediaContent *cg_upnp_media_content_getbyid(CgUpnpMediaContent *con, char *id);
CgUpnpMediaContent *cg_upnp_media_content_getbytitle(CgUpnpMediaContent *con, char *title);

#define cg_upnp_media_content_addresource(con, res) cg_xml_node_addchildnode(con, res)
#define cg_upnp_media_content_getresources(con) cg_xml_node_getchildnode(con, CG_UPNP_MEDIA_RESOURCE_NAME)

#ifdef __cplusplus
}
#endif

#endif

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

#ifndef _CG_CLINKCAV_CONTENT_H_
#define _CG_CLINKCAV_CONTENT_H_

#include <mupnp/std/av/cresource.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_CONTENT_NONE (0)
#define CG_UPNPAV_CONTENT_CONTAINER (1)
#define CG_UPNPAV_CONTENT_ITEM (2)

#define CG_UPNPAV_CONTENT_ID "id"
#define CG_UPNPAV_CONTENT_PARENTID "parentID"
#define CG_UPNPAV_CONTENT_CHILDCOUNT "childCount"
#define CG_UPNPAV_CONTENT_RESTRICTED "restricted"
#define CG_UPNPAV_CONTENT_TITLE "dc:title"
#define CG_UPNPAV_CONTENT_UPNPCLASS "upnp:class"
#define CG_UPNPAV_CONTENT_UPNPALBUMARTURI "upnp:albumArtURI"

#define CG_UPNPAV_ROOT_CONTENT_TITLE "ROOT"
#define CG_UPNPAV_ROOT_CONTENT_ID "0"
#define CG_UPNPAV_ROOT_CONTENT_PARENTID "-1"

#define CG_UPNPAV_MIMETYPE_JPEG "image/jpeg"
#define CG_UPNPAV_MIMETYPE_PNG "image/png"
#define CG_UPNPAV_MIMETYPE_MPEG "video/mpeg"
#define CG_UPNPAV_MIMETYPE_MP3 "audio/mpeg"
#define CG_UPNPAV_MIMETYPE_AVC "video/mp4"

#define CG_UPNPAV_UPNPCLASS_CONTAINER "object.container"
#define CG_UPNPAV_UPNPCLASS_ITEM "object.item"
#define CG_UPNPAV_UPNPCLASS_PHOTO "object.item.imageItem.photo"
#define CG_UPNPAV_UPNPCLASS_MOVIE "object.item.videoItem.movie"
#define CG_UPNPAV_UPNPCLASS_MUSIC "object.item.audioItem.musicTrack"

#define CG_UPNPAV_DLNAATTR_MAXLEN 128
#define CG_UPNPAV_PROTOCOLINFO_MAXLEN (CG_UPNPAV_DLNAATTR_MAXLEN + 64)
#define CG_UPNPAV_DLNA_PN_JPEG_LRG "JPEG_LRG"
#define CG_UPNPAV_DLNA_PN_MPEG_PS_NTSC "MPEG_PS_NTSC"
#define CG_UPNPAV_DLNA_PN_MP3 "MP3"
#define CG_UPNPAV_DLNA_PN_AVC "AVC_MP4_ BL_CIF30C"

/****************************************
* Struct
****************************************/

typedef mUpnpXmlNode mUpnpAvContent;
typedef mUpnpXmlNodeList mUpnpAvContentList;

/****************************************
* Content
****************************************/

#define mupnp_upnpav_contentlist_new() mupnp_xml_nodelist_new()
#define mupnp_upnpav_contentlist_delete(conList) mupnp_xml_nodelist_delete(conList)
#define mupnp_upnpav_contentlist_clear(conList) mupnp_xml_nodelist_clear(conList)
#define mupnp_upnpav_contentlist_size(conList) mupnp_xml_nodelist_size(conList)
#define mupnp_upnpav_contentlist_gets(conList) mupnp_xml_nodelist_gets(conList)
#define mupnp_upnpav_contentlist_add(conList, res) mupnp_xml_nodelist_add(conList, res)

#define mupnp_upnpav_content_new() mupnp_xml_node_new()
#define mupnp_upnpav_content_delete(con) mupnp_xml_node_delete(con)
mUpnpAvContent* mupnp_upnpav_content_next(mUpnpAvContent* con);
#define mupnp_upnpav_content_remove(con) mupnp_xml_node_remove(con)

void mupnp_upnpav_content_copy(mUpnpAvContent* destContent, mUpnpAvContent* srcContent);

void mupnp_upnpav_content_addchildcontent(mUpnpAvContent* con, mUpnpAvContent* childCon);
int mupnp_upnpav_content_getnchildcontents(mUpnpAvContent* con);
mUpnpAvContent* mupnp_upnpav_content_getchildcontents(mUpnpAvContent* con);
BOOL mupnp_upnpav_content_haschildcontents(mUpnpAvContent* con);
#define mupnp_upnpav_content_clearchildcontents(con) mupnp_xml_nodelist_clear(mupnp_xml_node_getchildnodelist(con))

#define mupnp_upnpav_content_setid(con, val) mupnp_xml_node_setattribute(con, CG_UPNPAV_CONTENT_ID, val)
#define mupnp_upnpav_content_getid(con) mupnp_xml_node_getattributevalue(con, CG_UPNPAV_CONTENT_ID)

#define mupnp_upnpav_content_setparentid(con, val) mupnp_xml_node_setattribute(con, CG_UPNPAV_CONTENT_PARENTID, val)
#define mupnp_upnpav_content_getparentid(con) mupnp_xml_node_getattributevalue(con, CG_UPNPAV_CONTENT_PARENTID)

#define mupnp_upnpav_content_settitle(con, val) mupnp_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_TITLE, val)
#define mupnp_upnpav_content_gettitle(con) mupnp_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_TITLE)

#define mupnp_upnpav_content_setupnpclass(con, val) mupnp_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_UPNPCLASS, val)
#define mupnp_upnpav_content_getupnpclass(con) mupnp_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_UPNPCLASS)

#define mupnp_upnpav_content_setupnpalbumarturi(con, val) mupnp_xml_node_setchildnode(con, CG_UPNPAV_CONTENT_UPNPALBUMARTURI, val)
#define mupnp_upnpav_content_getupnpalbumarturi(con) mupnp_xml_node_getchildnodevalue(con, CG_UPNPAV_CONTENT_UPNPALBUMARTURI)

void mupnp_upnpav_content_settype(mUpnpAvContent* con, int type);
int mupnp_upnpav_content_gettype(mUpnpAvContent* con);

#define mupnp_upnpav_content_isitemnode(node) (mupnp_upnpav_content_gettype(node) == CG_UPNPAV_CONTENT_ITEM ? TRUE : FALSE)
#define mupnp_upnpav_content_iscontainernode(node) (mupnp_upnpav_content_gettype(node) == CG_UPNPAV_CONTENT_CONTAINER ? TRUE : FALSE)
BOOL mupnp_upnpav_content_iscontentnode(mUpnpXmlNode* node);

#define mupnp_upnpav_content_setuserdata(con, data) mupnp_xml_node_setuserdata(con, data)
#define mupnp_upnpav_content_getuserdata(con) mupnp_xml_node_getuserdata(con)

typedef BOOL (*CG_UPNPAV_CONTENT_COMPARE_FUNC)(mUpnpAvContent* con, void* userData);
mUpnpAvContent* mupnp_upnpav_content_getbyid(mUpnpAvContent* con, char* id);
mUpnpAvContent* mupnp_upnpav_content_getbytitle(mUpnpAvContent* con, char* title);

#define mupnp_upnpav_content_addresource(con, res) mupnp_xml_node_addchildnode(con, res)
#define mupnp_upnpav_content_getresources(con) mupnp_xml_node_getchildnode(con, CG_UPNPAV_RESOURCE_NAME)

#ifdef __cplusplus
}
#endif

#endif

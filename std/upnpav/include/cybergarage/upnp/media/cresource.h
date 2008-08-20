/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cresource.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKC_MEDIA_RESOURCE_H_
#define _CG_CLINKC_MEDIA_RESOURCE_H_

#include <cybergarage/util/clist.h>
#include <cybergarage/xml/cxml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNP_MEDIA_RESOURCE_NAME "res"
#define CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO "protocolInfo"
#define CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO_SIZE "size"

/****************************************
* Struct
****************************************/

typedef CgXmlNode CgUpnpMediaResource;
typedef CgXmlNodeList CgUpnpMediaResourceList;

typedef struct _CgUpnpMediaResourceData {
	CgString *mimeType;
	CgString *dlnaAttr;
} CgUpnpMediaResourceData;

/****************************************
* Resource
****************************************/

#define cg_upnp_media_resourcelist_new() cg_xml_nodelist_new()
#define cg_upnp_media_resourcelist_delete(resList) cg_xml_nodelist_delete(resList)
#define cg_upnp_media_resourcelist_clear(resList) cg_xml_nodelist_clear(resList)
#define cg_upnp_media_resourcelist_size(resList) cg_xml_nodelist_size(resList)
#define cg_upnp_media_resourcelist_gets(resList) cg_xml_nodelist_gets(resList)
#define cg_upnp_media_resourcelist_add(resList, res) cg_xml_nodelist_add(resList, res)

CgUpnpMediaResource *cg_upnp_media_resource_new();
void cg_upnp_media_resource_delete(CgUpnpMediaResource *res);
#define cg_upnp_media_resource_next(res) cg_xml_node_next(res)
#define cg_upnp_media_resource_remove(res) cg_xml_node_remove(res)

void cg_upnp_media_resource_copy(CgUpnpMediaResource *destRes, CgUpnpMediaResource *srcRes);

void cg_upnp_media_resource_setmimetype(CgUpnpMediaResource *res, char *mimeType);
void cg_upnp_media_resource_setdlnaattribute(CgUpnpMediaResource *res, char *attr);
#define cg_upnp_media_resource_seturl(res, val) cg_xml_node_setvalue(res, val)
void cg_upnp_media_resource_setsize(CgUpnpMediaResource *res, long value);

#define cg_upnp_media_resource_getmimetype(res) cg_string_getvalue(((CgUpnpMediaResourceData *)(cg_xml_node_getuserdata(res)))->mimeType)
#define cg_upnp_media_resource_getdlnaattribute(res) cg_string_getvalue(((CgUpnpMediaResourceData *)(cg_xml_node_getuserdata(res)))->dlnaAttr)
#define cg_upnp_media_resource_geturl(res) cg_xml_node_getvalue(res)
#define cg_upnp_media_resource_getsize(res) cg_str2long(cg_xml_node_getattributevalue(res, CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO_SIZE))

char *cg_upnp_media_resource_getdlnapnfrommimetype(CgUpnpMediaResource *res, char *dlnaAttr, int dlnaAttrSize);

CgUpnpMediaResourceData *cg_upnp_media_resource_data_new();
void cg_upnp_media_resource_data_delete(CgUpnpMediaResourceData *resData);
void cg_upnp_media_resource_data_copy(CgUpnpMediaResourceData *destResData, CgUpnpMediaResourceData *srcResData);

#ifdef __cplusplus
}
#endif

#endif

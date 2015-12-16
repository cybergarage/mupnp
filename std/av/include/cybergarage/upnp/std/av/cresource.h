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

#ifndef _CG_CLINKCAV_RESOURCE_H_
#define _CG_CLINKCAV_RESOURCE_H_

#include <mupnp/util/list.h>
#include <mupnp/xml/xml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_RESOURCE_NAME "res"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO "protocolInfo"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE "size"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_DURATION "duration"
#define CG_UPNPAV_RESOURCE_PROTOCOLINFO_RESOLUTION "resolution"

/****************************************
* Struct
****************************************/

typedef mUpnpXmlNode mUpnpAvResource;
typedef mUpnpXmlNodeList mUpnpAvResourceList;

typedef struct _mUpnpAvResourceData {
  mUpnpString* mimeType;
  mUpnpString* dlnaAttr;
} mUpnpAvResourceData;

/****************************************
* Resource
****************************************/

#define mupnp_upnpav_resourcelist_new() mupnp_xml_nodelist_new()
#define mupnp_upnpav_resourcelist_delete(resList) mupnp_xml_nodelist_delete(resList)
#define mupnp_upnpav_resourcelist_clear(resList) mupnp_xml_nodelist_clear(resList)
#define mupnp_upnpav_resourcelist_size(resList) mupnp_xml_nodelist_size(resList)
#define mupnp_upnpav_resourcelist_gets(resList) mupnp_xml_nodelist_gets(resList)
#define mupnp_upnpav_resourcelist_add(resList, res) mupnp_xml_nodelist_add(resList, res)

mUpnpAvResource* mupnp_upnpav_resource_new();
void mupnp_upnpav_resource_delete(mUpnpAvResource* res);
#define mupnp_upnpav_resource_next(res) mupnp_xml_node_next(res)
#define mupnp_upnpav_resource_remove(res) mupnp_xml_node_remove(res)

void mupnp_upnpav_resource_copy(mUpnpAvResource* destRes, mUpnpAvResource* srcRes);

void mupnp_upnpav_resource_setmimetype(mUpnpAvResource* res, char* mimeType);
void mupnp_upnpav_resource_setdlnaattribute(mUpnpAvResource* res, char* attr);
#define mupnp_upnpav_resource_seturl(res, val) mupnp_xml_node_setvalue(res, val)
void mupnp_upnpav_resource_setsize(mUpnpAvResource* res, long value);

#define mupnp_upnpav_resource_getmimetype(res) mupnp_string_getvalue(((mUpnpAvResourceData*)(mupnp_xml_node_getuserdata(res)))->mimeType)
#define mupnp_upnpav_resource_getdlnaattribute(res) mupnp_string_getvalue(((mUpnpAvResourceData*)(mupnp_xml_node_getuserdata(res)))->dlnaAttr)
#define mupnp_upnpav_resource_geturl(res) mupnp_xml_node_getvalue(res)
#define mupnp_upnpav_resource_getsize(res) mupnp_str2long(mupnp_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE))
#define mupnp_upnpav_resource_getduration(res) mupnp_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_DURATION)
#define mupnp_upnpav_resource_getresolution(res) mupnp_xml_node_getattributevalue(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_RESOLUTION)

mUpnpAvResourceData* mupnp_upnpav_resource_data_new();
void mupnp_upnpav_resource_data_delete(mUpnpAvResourceData* resData);
void mupnp_upnpav_resource_data_copy(mUpnpAvResourceData* destResData, mUpnpAvResourceData* srcResData);

char* mupnp_upnpav_resource_getdlnaattributesbymimetype(char* mimeType, char* dlnaAttr, int dlnaAttrSize);
char* mupnp_upnpav_resource_getdlnaattributes(mUpnpAvResource* res, char* dlnaAttr, int dlnaAttrSize);

#ifdef __cplusplus
}
#endif

#endif

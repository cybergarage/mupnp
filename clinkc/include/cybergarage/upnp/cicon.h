/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cicon.h
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CICON_H_
#define _CG_UPNP_CICON_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_ICON_ELEM_NAME "icon"
#define CG_UPNP_ICONLIST_ELEM_NAME "iconList"

#define CG_UPNP_ICON_MIME_TYPE "mimeType"
#define CG_UPNP_ICON_WIDTH "width"
#define CG_UPNP_ICON_HEIGHT "height"
#define CG_UPNP_ICON_DEPTH "depth"
#define CG_UPNP_ICON_URL "url"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpIcon {
	BOOL headFlag;
	struct _CgUpnpIcon *prev;
	struct _CgUpnpIcon *next;
	CgXmlNode *iconNode;
} CgUpnpIcon, CgUpnpIconList;

/****************************************
* Function (Icon)
****************************************/

CgUpnpIcon *cg_upnp_icon_new();
void cg_upnp_icon_delete(CgUpnpIcon *dev);

#define cg_upnp_icon_next(icon) (CgUpnpIcon *)cg_list_next((CgList *)icon)

#define cg_upnp_icon_isiconnode(node) cg_xml_node_isname(node, CG_UPNP_ICON_ELEM_NAME)

#define cg_upnp_icon_seticonnode(icon,node) (icon->iconNode = node)
#define cg_upnp_icon_geticonnode(icon) (icon->iconNode)

/**** mimeType ****/
#define cg_upnp_icon_setmimetype(icon, value) cg_xml_node_setchildnode(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_MIME_TYPE, value)
#define cg_upnp_icon_getmimetype(icon) cg_xml_node_getchildnodevalue(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_MIME_TYPE)

/**** width ****/
#define cg_upnp_icon_setwidth(icon, value) cg_xml_node_setchildnode(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_WIDTH, value)
#define cg_upnp_icon_getwidth(icon) cg_xml_node_getchildnodevalue(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_WIDTH)

/**** height ****/
#define cg_upnp_icon_setheight(icon, value) cg_xml_node_setchildnode(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_HEIGHT, value)
#define cg_upnp_icon_getheight(icon) cg_xml_node_getchildnodevalue(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_HEIGHT)

/**** depth ****/
#define cg_upnp_icon_setdepth(icon, value) cg_xml_node_setchildnode(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_DEPTH, value)
#define cg_upnp_icon_getdepth(icon) cg_xml_node_getchildnodevalue(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_DEPTH)

/**** URL ****/
#define cg_upnp_icon_seturl(icon, value) cg_xml_node_setchildnode(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_URL, value)
#define cg_upnp_icon_geturl(icon) cg_xml_node_getchildnodevalue(cg_upnp_icon_geticonnode(icon), CG_UPNP_ICON_URL)

/****************************************
* Function (IconList)
****************************************/

CgUpnpIconList *cg_upnp_iconlist_new();
void cg_upnp_iconlist_delete(CgUpnpIconList *iconList);

#define cg_upnp_iconlist_clear(iconList) cg_list_clear((CgList *)iconList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_icon_delete)
#define cg_upnp_iconlist_size(iconList) cg_list_size((CgList *)iconList)
#define cg_upnp_iconlist_gets(iconList) (CgUpnpIcon *)cg_list_next((CgList *)iconList)
#define cg_upnp_iconlist_add(iconList, icon) cg_list_add((CgList *)iconList, (CgList *)icon)

#ifdef  __cplusplus
}
#endif

#endif

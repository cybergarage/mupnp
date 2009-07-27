/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2009
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cprotocolinfo.h
*
*	Revision:
*
*	07/27/09
*		- first revision
*
******************************************************************/

#ifndef _CG_CLINKC_AV_PROTOCOLINFO_H_
#define _CG_CLINKC_AV_PROTOCOLINFO_H_

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/std/av/cupnpav.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Data Type
****************************************/
/**
 * The generic protocolinfo struct
 */
typedef struct _CgUpnpMediaProtocolInfo {
	BOOL headFlag;
	struct _CgUpnpMediaProtocolInfo *prev;
	struct _CgUpnpMediaProtocolInfo *next;
} CgUpnpMediaProtocolInfo, CgUpnpMediaProtocolInfoList;

/****************************************
* Function (ProtocolInfo)
****************************************/

/**
 * Create a new protocolinfo struct
 */
CgUpnpMediaProtocolInfo *cg_upnp_protocolinfo_new();

/**
 * Delete an protocolinfo struct
 *
 * \param dev ProtocolInfo to delete
 */
void cg_upnp_protocolinfo_delete(CgUpnpMediaProtocolInfo *dev);

/**
 * Get the next protocolinfo in list. Use as an iterator.
 * 
 * \param protocolinfo Node in a list of protocolinfos
 */
#define cg_upnp_protocolinfo_next(protocolinfo) (CgUpnpMediaProtocolInfo *)cg_list_next((CgList *)protocolinfo)

/**
 * Check whether the given node is a member of an protocolinfo list
 * 
 * \param node List node
 */
#define cg_upnp_protocolinfo_isprotocolinfonode(node) cg_xml_node_isname(node, CG_UPNP_ICON_ELEM_NAME)

/**
 * Set the XML protocolinfo node to an protocolinfo struct
 * 
 * \param protocolinfo The protocolinfo struct
 * \param node XML node
 */
#define cg_upnp_protocolinfo_setprotocolinfonode(protocolinfo,node) (protocolinfo->protocolinfoNode = node)

/**
 * Get the XML protocolinfo node from an protocolinfo struct
 * 
 * \param protocolinfo The protocolinfo struct
 */
#define cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo) (protocolinfo->protocolinfoNode)

/*****************************************************************************
 * mimeType
 *****************************************************************************/

/**
 * Set the protocolinfo's MIME type
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value MIME type
 */
#define cg_upnp_protocolinfo_setmimetype(protocolinfo, value) cg_xml_node_setchildnode(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_MIME_TYPE, value)

/**
 * Get the protocolinfo's MIME type
 * 
 */
#define cg_upnp_protocolinfo_getmimetype(protocolinfo) cg_xml_node_getchildnodevalue(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_MIME_TYPE)

/*****************************************************************************
 * width 
 *****************************************************************************/

/**
 * Set protocolinfo width
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value ProtocolInfo width
 */
#define cg_upnp_protocolinfo_setwidth(protocolinfo, value) cg_xml_node_setchildnode(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_WIDTH, value)

/**
 * Get protocolinfo width
 * 
 */
#define cg_upnp_protocolinfo_getwidth(protocolinfo) cg_xml_node_getchildnodeintvalue(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_WIDTH)

/*****************************************************************************
 * height 
 *****************************************************************************/

/**
 * Set protocolinfo height
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value ProtocolInfo height
 */
#define cg_upnp_protocolinfo_setheight(protocolinfo, value) cg_xml_node_setchildnode(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_HEIGHT, value)

/**
 * Get protocolinfo height
 * 
 */
#define cg_upnp_protocolinfo_getheight(protocolinfo) cg_xml_node_getchildnodeintvalue(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_HEIGHT)

/*****************************************************************************
 * Colour Depth 
 *****************************************************************************/

/**
 * Set protocolinfo's colour depth
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value Colour depth
 */
#define cg_upnp_protocolinfo_setdepth(protocolinfo, value) cg_xml_node_setchildnode(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_DEPTH, value)

/**
 * Get protocolinfo's colour depth
 * 
 */
#define cg_upnp_protocolinfo_getdepth(protocolinfo) cg_xml_node_getchildnodeintvalue(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_DEPTH)

/*****************************************************************************
 * URL
 *****************************************************************************/

/**
 * Set protocolinfo's URL
 * 
 * \param protocolinfo ProtocolInfo in question
 * \param value ProtocolInfo's URL
 */
#define cg_upnp_protocolinfo_seturl(protocolinfo, value) cg_xml_node_setchildnode(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_URL, value)

/**
 * Get protocolinfo's URL
 * 
 */
#define cg_upnp_protocolinfo_geturl(protocolinfo) cg_xml_node_getchildnodevalue(cg_upnp_protocolinfo_getprotocolinfonode(protocolinfo), CG_UPNP_ICON_URL)

/*****************************************************************************
 * Function (ProtocolInfoList)
 *****************************************************************************/

/**
 * Create a new list of protocolinfos
 * 
 */
CgUpnpMediaProtocolInfoList *cg_upnp_protocolinfolist_new();

/**
 * Delete a list of protocolinfos
 * 
 * \param protocolinfoList List of protocolinfos
 */
void cg_upnp_protocolinfolist_delete(CgUpnpMediaProtocolInfoList *protocolinfoList);

/**
 * Clear the contents of an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnp_protocolinfolist_clear(protocolinfoList) cg_list_clear((CgList *)protocolinfoList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_protocolinfo_delete)

/**
 * Get the size (number of elements) of an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnp_protocolinfolist_size(protocolinfoList) cg_list_size((CgList *)protocolinfoList)

/**
 * \todo Get the first protocolinfo from the list for iteration
 * 
 * \param protocolinfoList List of protocolinfos
 */
#define cg_upnp_protocolinfolist_gets(protocolinfoList) (CgUpnpMediaProtocolInfo *)cg_list_next((CgList *)protocolinfoList)

/**
 * Add an protocolinfo to an protocolinfo list
 * 
 * \param protocolinfoList List of protocolinfos
 * \param protocolinfo ProtocolInfo to add
 */
#define cg_upnp_protocolinfolist_add(protocolinfoList, protocolinfo) cg_list_add((CgList *)protocolinfoList, (CgList *)protocolinfo)

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif

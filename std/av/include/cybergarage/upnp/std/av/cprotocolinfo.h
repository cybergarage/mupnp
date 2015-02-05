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

#ifndef _CG_CLINKCAV_PROTOCOLINFO_H_
#define _CG_CLINKCAV_PROTOCOLINFO_H_

#include <mupnp/util/string.h>
#include <mupnp/util/list.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Constant
****************************************/

#define CG_UPNPAV_PROTOCOLINFO_MAXSIZE 512
#define CG_UPNPAV_PROTOCOLINFO_HTTPGET "http-get"

/****************************************
* Data Type
****************************************/
/**
 * The generic protocolinfo struct
 */
typedef struct _CgUpnpAvProtocolInfo {
	BOOL headFlag;
	struct _CgUpnpAvProtocolInfo *prev;
	struct _CgUpnpAvProtocolInfo *next;
	CgString *protocol;
	CgString *network;
	CgString *mimeType;
	CgString *additionalInfo;
	CgString *string;
} CgUpnpAvProtocolInfo, CgUpnpAvProtocolInfoList;

/****************************************
* Function (ProtocolInfo)
****************************************/

/**
 * Create a new protocolinfo struct
 */
CgUpnpAvProtocolInfo *mupnp_upnpav_protocolinfo_new();

/**
 * Delete an protocolinfo struct
 *
 * \param dev ProtocolInfo to delete
 */
void mupnp_upnpav_protocolinfo_delete(CgUpnpAvProtocolInfo *protocolinfo);

/**
 * Get the next protocolinfo in list. Use as an iterator.
 *
 * \param protocolinfo Node in a list of protocolinfos
 */
#define mupnp_upnpav_protocolinfo_next(protocolinfo) (CgUpnpAvProtocolInfo *)mupnp_list_next((CgList *)protocolinfo)

/*****************************************************************************
 * Protocol
 *****************************************************************************/
/**
 * Set the protocolinfo's protocol type
 *
 * \param protocolinfo ProtocolInfo in question
 * \param value Protocol
 */
#define mupnp_upnpav_protocolinfo_setprotocol(protocolinfo, value) mupnp_string_setvalue(protocolinfo->protocol, value)

/**
 * Get the protocolinfo's MIME type
 *
 */
#define mupnp_upnpav_protocolinfo_getprotocol(protocolinfo) mupnp_string_getvalue(protocolinfo->protocol)

/*****************************************************************************
 * Network
 *****************************************************************************/
/**
 * Set the protocolinfo's network type
 *
 * \param protocolinfo ProtocolInfo in question
 * \param value Network type
 */
#define mupnp_upnpav_protocolinfo_setnetwork(protocolinfo, value) mupnp_string_setvalue(protocolinfo->network, value)

/**
 * Get the protocolinfo's MIME type
 *
 */
#define mupnp_upnpav_protocolinfo_getnetwork(protocolinfo) mupnp_string_getvalue(protocolinfo->network)

/*****************************************************************************
 * mimeType
 *****************************************************************************/
/**
 * Set the protocolinfo's MIME type
 *
 * \param protocolinfo ProtocolInfo in question
 * \param value MIME type
 */
#define mupnp_upnpav_protocolinfo_setmimetype(protocolinfo, value) mupnp_string_setvalue(protocolinfo->mimeType, value)

/**
 * Get the protocolinfo's MIME type
 *
 */
#define mupnp_upnpav_protocolinfo_getmimetype(protocolinfo) mupnp_string_getvalue(protocolinfo->mimeType)

/*****************************************************************************
 * additionalInfo
 *****************************************************************************/
/**
 * Set the protocolinfo's additional info type
 *
 * \param protocolinfo ProtocolInfo in question
 * \param value Additiona lInfo
 */
#define mupnp_upnpav_protocolinfo_setadditionalinfo(protocolinfo, value) mupnp_string_setvalue(protocolinfo->additionalInfo, value)

/**
 * Get the protocolinfo's MIME type
 *
 */
#define mupnp_upnpav_protocolinfo_getadditionalinfo(protocolinfo) mupnp_string_getvalue(protocolinfo->additionalInfo)

    /* Due to the typo active in clinkcav-2.3 and 2.4, I leave the old
     * definitions here, to be backward compatible.  -lundman */
#define mupnp_upnpav_protocolinfo_setadditionainfo(protocolinfo, value) mupnp_string_setvalue(protocolinfo->additionalInfo, value)
#define mupnp_upnpav_protocolinfo_getadditionainfo(protocolinfo) mupnp_string_getvalue(protocolinfo->additionalInfo)


/*****************************************************************************
 * String
 *****************************************************************************/

/**
 * Get the protocolinfo's string
 *
 */
char *mupnp_upnpav_protocolinfo_getstring(CgUpnpAvProtocolInfo *protocolinfo);

/*****************************************************************************
 * Function (ProtocolInfoList)
 *****************************************************************************/

/**
 * Create a new list of protocolinfos
 *
 */
CgUpnpAvProtocolInfoList *mupnp_upnpav_protocolinfolist_new();

/**
 * Delete a list of protocolinfos
 *
 * \param protocolinfoList List of protocolinfos
 */
void mupnp_upnpav_protocolinfolist_delete(CgUpnpAvProtocolInfoList *protocolinfoList);

/**
 * Clear the contents of an protocolinfo list
 *
 * \param protocolinfoList List of protocolinfos
 */
#define mupnp_upnpav_protocolinfolist_clear(protocolinfoList) mupnp_list_clear((CgList *)protocolinfoList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnpav_protocolinfo_delete)

/**
 * Get the size (number of elements) of an protocolinfo list
 *
 * \param protocolinfoList List of protocolinfos
 */
#define mupnp_upnpav_protocolinfolist_size(protocolinfoList) mupnp_list_size((CgList *)protocolinfoList)

/**
 * \todo Get the first protocolinfo from the list for iteration
 *
 * \param protocolinfoList List of protocolinfos
 */
#define mupnp_upnpav_protocolinfolist_gets(protocolinfoList) (CgUpnpAvProtocolInfo *)mupnp_list_next((CgList *)protocolinfoList)

/**
 * Add an protocolinfo to an protocolinfo list
 *
 * \param protocolinfoList List of protocolinfos
 * \param protocolinfo ProtocolInfo to add
 */
#define mupnp_upnpav_protocolinfolist_add(protocolinfoList, protocolinfo) mupnp_list_add((CgList *)protocolinfoList, (CgList *)protocolinfo)

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif

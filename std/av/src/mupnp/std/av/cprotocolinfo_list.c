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
*	File: cprotocolinfolist.c
*
*	Revision:
*
*	07/27/09
*		- first revision
*
******************************************************************/

#include <mupnp/std/av/cprotocolinfo.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_protocolinflist_new
****************************************/

mUpnpAvProtocolInfoList *mupnp_upnpav_protocolinfolist_new()
{
	mUpnpAvProtocolInfo *protocolinfoList;

	mupnp_log_debug_l4("Entering...\n");

	protocolinfoList = (mUpnpAvProtocolInfo *)malloc(sizeof(mUpnpAvProtocolInfo));

	if ( NULL != protocolinfoList )
		mupnp_list_header_init((mUpnpList *)protocolinfoList);

	return protocolinfoList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnpav_protocolinfor_delete
****************************************/

void mupnp_upnpav_protocolinfolist_delete(mUpnpAvProtocolInfoList *protocolinfoList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)protocolinfoList);
	free(protocolinfoList);

	mupnp_log_debug_l4("Leaving...\n");
}

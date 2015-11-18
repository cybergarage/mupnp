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
*	File: cprotocolinfo.c
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
* mupnp_upnpav_protocolinfo_new
****************************************/

mUpnpAvProtocolInfo *mupnp_upnpav_protocolinfo_new()
{
	mUpnpAvProtocolInfo *info;

	mupnp_log_debug_l4("Entering...\n");

	info = (mUpnpAvProtocolInfo *)malloc(sizeof(mUpnpAvProtocolInfo));

	if  ( NULL != info ) {
		mupnp_list_node_init((mUpnpList *)info);

		info->protocol = mupnp_string_new();
		info->network = mupnp_string_new();
		info->mimeType = mupnp_string_new();
		info->additionalInfo = mupnp_string_new();
		info->string = mupnp_string_new();

		mupnp_upnpav_protocolinfo_setprotocol(info, "*");
		mupnp_upnpav_protocolinfo_setnetwork(info, "*");
		mupnp_upnpav_protocolinfo_setmimetype(info, "*");
		mupnp_upnpav_protocolinfo_setadditionainfo(info, "*");
    }
	mupnp_log_debug_l4("Leaving...\n");

	return info;
}

/****************************************
* mupnp_upnpav_protocolinfo_delete
****************************************/

void mupnp_upnpav_protocolinfo_delete(mUpnpAvProtocolInfo *info)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)info);

	if (info->protocol)
		mupnp_string_delete(info->protocol);
	if (info->network)
		mupnp_string_delete(info->network);
	if (info->mimeType)
		mupnp_string_delete(info->mimeType);
	if (info->additionalInfo)
		mupnp_string_delete(info->additionalInfo);
	if (info->string)
		mupnp_string_delete(info->string);

	free(info);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnpav_protocolinfo_getstring
****************************************/

char *mupnp_upnpav_protocolinfo_getstring(mUpnpAvProtocolInfo *info)
{
	char protocolInfoBuf[CG_UPNPAV_PROTOCOLINFO_MAXSIZE];

#if defined(WIN32)
	_snprintf(
#else
	snprintf(
#endif
			protocolInfoBuf,
			sizeof(protocolInfoBuf),
			"%s:%s:%s:%s",
		mupnp_upnpav_protocolinfo_getprotocol(info),
		mupnp_upnpav_protocolinfo_getnetwork(info),
		mupnp_upnpav_protocolinfo_getmimetype(info),
		mupnp_upnpav_protocolinfo_getadditionainfo(info));

	mupnp_string_setvalue(info->string, protocolInfoBuf);

	return mupnp_string_getvalue(info->string);
}

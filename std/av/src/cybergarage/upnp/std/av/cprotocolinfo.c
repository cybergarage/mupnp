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

#include <cybergarage/upnp/std/av/cprotocolinfo.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_protocolinfo_new
****************************************/

CgUpnpMediaProtocolInfo *cg_upnp_protocolinfo_new()
{
	CgUpnpMediaProtocolInfo *info;

	cg_log_debug_l4("Entering...\n");

	info = (CgUpnpMediaProtocolInfo *)malloc(sizeof(CgUpnpMediaProtocolInfo));

	if  ( NULL != info )
		cg_list_node_init((CgList *)info);

	cg_log_debug_l4("Leaving...\n");

	return info;
}

/****************************************
* cg_upnp_protocolinfo_delete
****************************************/

void cg_upnp_protocolinfo_delete(CgUpnpMediaProtocolInfo *info)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)info);
	free(info);

	cg_log_debug_l4("Leaving...\n");
}

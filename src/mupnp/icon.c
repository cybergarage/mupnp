/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cicon.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <mupnp/icon.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_icon_new
****************************************/

CgUpnpIcon *mupnp_upnp_icon_new()
{
	CgUpnpIcon *dev;

	mupnp_log_debug_l4("Entering...\n");

	dev = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));

	if  ( NULL != dev )
		mupnp_list_node_init((CgList *)dev);

	mupnp_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* mupnp_upnp_icon_delete
****************************************/

void mupnp_upnp_icon_delete(CgUpnpIcon *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)dev);
	free(dev);

	mupnp_log_debug_l4("Leaving...\n");
}

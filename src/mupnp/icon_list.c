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
*	File: cicon_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <mupnp/icon.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_iconr_new
****************************************/

CgUpnpIconList *mupnp_upnp_iconlist_new()
{
	CgUpnpIcon *iconList;

	mupnp_log_debug_l4("Entering...\n");

	iconList = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));

	if ( NULL != iconList )
		mupnp_list_header_init((CgList *)iconList);

	return iconList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_iconr_delete
****************************************/

void mupnp_upnp_iconlist_delete(CgUpnpIconList *iconList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)iconList);
	free(iconList);

	mupnp_log_debug_l4("Leaving...\n");
}

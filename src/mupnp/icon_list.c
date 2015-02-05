/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/icon.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_iconr_new
****************************************/

mUpnpUpnpIconList *mupnp_upnp_iconlist_new()
{
	mUpnpUpnpIcon *iconList;

	mupnp_log_debug_l4("Entering...\n");

	iconList = (mUpnpUpnpIcon *)malloc(sizeof(mUpnpUpnpIcon));

	if ( NULL != iconList )
		mupnp_list_header_init((mUpnpList *)iconList);

	return iconList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_iconr_delete
****************************************/

void mupnp_upnp_iconlist_delete(mUpnpUpnpIconList *iconList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)iconList);
	free(iconList);

	mupnp_log_debug_l4("Leaving...\n");
}

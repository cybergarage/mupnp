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
* mupnp_upnp_icon_new
****************************************/

mUpnpUpnpIcon *mupnp_upnp_icon_new()
{
	mUpnpUpnpIcon *dev;

	mupnp_log_debug_l4("Entering...\n");

	dev = (mUpnpUpnpIcon *)malloc(sizeof(mUpnpUpnpIcon));

	if  ( NULL != dev )
		mupnp_list_node_init((mUpnpList *)dev);

	mupnp_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* mupnp_upnp_icon_delete
****************************************/

void mupnp_upnp_icon_delete(mUpnpUpnpIcon *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)dev);
	free(dev);

	mupnp_log_debug_l4("Leaving...\n");
}

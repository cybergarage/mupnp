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

#include <mupnp/service.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_servicelist_new
****************************************/

mUpnpUpnpServiceList *mupnp_upnp_servicelist_new()
{
	mUpnpUpnpServiceList *serviceList;

	mupnp_log_debug_l4("Entering...\n");

	serviceList = (mUpnpUpnpServiceList *)malloc(sizeof(mUpnpUpnpServiceList));

	if ( NULL != serviceList )
		mupnp_list_header_init((mUpnpList *)serviceList);

	return serviceList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_servicelist_delete
****************************************/

void mupnp_upnp_servicelist_delete(mUpnpUpnpServiceList *serviceList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_servicelist_clear(serviceList);
	free(serviceList);

	mupnp_log_debug_l4("Leaving...\n");
}

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

#include <mupnp/argument.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_argument_new
****************************************/

mUpnpUpnpArgument *mupnp_upnp_argument_new()
{
	mUpnpUpnpArgument *arg;

	mupnp_log_debug_l4("Entering...\n");

	arg = (mUpnpUpnpArgument *)malloc(sizeof(mUpnpUpnpArgument));

	if ( NULL != arg )
	{
		mupnp_list_node_init((mUpnpList *)arg);
		
		arg->serviceNode = NULL;
		arg->argumentNode = NULL;
		
		arg->value = mupnp_string_new();
	}
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return arg;
}

/****************************************
* mupnp_upnp_argument_delete
****************************************/

void mupnp_upnp_argument_delete(mUpnpUpnpArgument *arg)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)arg);
	
	mupnp_string_delete(arg->value);
	
	mupnp_log_debug_l4("Leaving...\n");

	free(arg);
}

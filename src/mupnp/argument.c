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
*	File: cargument.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <mupnp/argument.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_argument_new
****************************************/

CgUpnpArgument *mupnp_upnp_argument_new()
{
	CgUpnpArgument *arg;

	mupnp_log_debug_l4("Entering...\n");

	arg = (CgUpnpArgument *)malloc(sizeof(CgUpnpArgument));

	if ( NULL != arg )
	{
		mupnp_list_node_init((CgList *)arg);
		
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

void mupnp_upnp_argument_delete(CgUpnpArgument *arg)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)arg);
	
	mupnp_string_delete(arg->value);
	
	mupnp_log_debug_l4("Leaving...\n");

	free(arg);
}

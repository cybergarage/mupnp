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
*	File: cargument_list.c
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

mUpnpUpnpArgumentList *mupnp_upnp_argumentlist_new()
{
	mUpnpUpnpArgument *argumentList;

	mupnp_log_debug_l4("Entering...\n");

	argumentList = (mUpnpUpnpArgument *)malloc(sizeof(mUpnpUpnpArgument));

	if ( NULL != argumentList )
		mupnp_list_header_init((mUpnpList *)argumentList);

	return argumentList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_argument_delete
****************************************/

void mupnp_upnp_argumentlist_delete(mUpnpUpnpArgumentList *argumentList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_clear((mUpnpList*)argumentList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_argument_delete);
	free(argumentList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_argumentlist_get
****************************************/

mUpnpUpnpArgument *mupnp_upnp_argumentlist_get(mUpnpUpnpArgumentList *argList, const char *name)
{
	mUpnpUpnpArgument *arg;
	const char *argName;
	
	mupnp_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (arg = mupnp_upnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_upnp_argument_next(arg)) {
		argName = mupnp_upnp_argument_getname(arg);
		if (argName == NULL)
			continue;
		if (mupnp_strcasecmp(argName, name) == 0)
			return arg;
	}

	return NULL;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_argumentlist_set
****************************************/

void mupnp_upnp_argumentlist_set(mUpnpUpnpArgumentList *argList, mUpnpUpnpArgumentList *srcArgList)
{
	mUpnpUpnpArgument *arg;
	mUpnpUpnpArgument *srcArg;
	const char *srcArgName;
	
	mupnp_log_debug_l4("Entering...\n");

	for (srcArg = mupnp_upnp_argumentlist_gets(srcArgList); srcArg != NULL; srcArg = mupnp_upnp_argument_next(srcArg)) {
		srcArgName = mupnp_upnp_argument_getname(srcArg);
		arg = mupnp_upnp_argumentlist_get(argList, srcArgName);
		if (arg == NULL)
			continue;
		mupnp_upnp_argument_setvalue(arg, mupnp_upnp_argument_getvalue(srcArg));
	}

	mupnp_log_debug_l4("Leaving...\n");
}

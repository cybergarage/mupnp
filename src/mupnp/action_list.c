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
*	File: caction_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <mupnp/action.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_action_new
****************************************/

CgUpnpActionList *mupnp_upnp_actionlist_new()
{
	CgUpnpAction *actionList;

	mupnp_log_debug_l4("Entering...\n");

	actionList = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));

	if ( NULL != actionList )
		mupnp_list_header_init((CgList *)actionList);

	return actionList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_action_delete
****************************************/

void mupnp_upnp_actionlist_delete(CgUpnpActionList *actionList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)actionList);
	free(actionList);

	mupnp_log_debug_l4("Leaving...\n");
}

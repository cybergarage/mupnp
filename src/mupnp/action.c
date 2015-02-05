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
*	File: caction.c
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
* prototype define for static functions
****************************************/

static void mupnp_upnp_action_initchildnodes(CgUpnpAction *action);
static void mupnp_upnp_action_initargumentlist(CgUpnpAction *action);

/****************************************
* mupnp_upnp_action_new
****************************************/

CgUpnpAction *mupnp_upnp_action_new()
{
	CgUpnpAction *action;

	mupnp_log_debug_l4("Entering...\n");

	action = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));

	if ( NULL != action )
	{
		mupnp_list_node_init((CgList *)action);
		
		action->parentService = NULL;
		action->actionNode = NULL;

		action->argumentList = mupnp_upnp_argumentlist_new();
		
		action->upnpStatus = mupnp_upnp_status_new();
		action->listener = NULL;
		mupnp_upnp_action_setuserdata(action, NULL);
	}

	mupnp_log_debug_l4("Leaving...\n");

	return action;
}

/****************************************
* mupnp_upnp_action_delete
****************************************/

void mupnp_upnp_action_delete(CgUpnpAction *action)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_argumentlist_delete(action->argumentList);
	
	mupnp_upnp_status_delete(action->upnpStatus);
	
	mupnp_list_remove((CgList *)action);
	free(action);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_action_setactionnode
****************************************/

void mupnp_upnp_action_setactionnode(CgUpnpAction *action, CgXmlNode *node)
{
	mupnp_log_debug_l4("Entering...\n");

	action->actionNode = node;
	mupnp_upnp_action_initchildnodes(action);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* mupnp_upnp_action_initchildnodes
****************************************/

static void mupnp_upnp_action_initchildnodes(CgUpnpAction *action)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_action_initargumentlist(action);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_upnp_action_initargumentlist
****************************************/

static void mupnp_upnp_action_initargumentlist(CgUpnpAction *action)
{
	CgXmlNode *actionNode;
	CgXmlNode *argumentListNode;
	CgXmlNode *childNode;
	CgUpnpArgument *arg;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_argumentlist_clear(action->argumentList);

	actionNode = mupnp_upnp_action_getactionnode(action);
	argumentListNode = mupnp_xml_node_getchildnode(actionNode, CG_UPNP_ARGUMENTLIST_ELEM_NAME);
	
	if (argumentListNode == NULL)
		return;

	for (childNode = mupnp_xml_node_getchildnodes(argumentListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
	
		if (mupnp_upnp_argument_isargumentnode(childNode) == FALSE)
			continue;
			
		arg = mupnp_upnp_argument_new();
		mupnp_upnp_argument_setargumentnode(arg, childNode);
		mupnp_upnp_argumentlist_add(action->argumentList, arg);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_action_getargumentbyname
****************************************/

CgUpnpArgument *mupnp_upnp_action_getargumentbyname(CgUpnpAction *action, const char *name)
{
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(name) <= 0)
		return NULL;
			
	argList = mupnp_upnp_action_getargumentlist(action);
	for (arg=mupnp_upnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_upnp_argument_next(arg)) {
		if (mupnp_upnp_argument_isname(arg, name) == TRUE)
			return arg;
	}
	
	return NULL;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_action_getargumentvaluebyname
****************************************/

char *mupnp_upnp_action_getargumentvaluebyname(CgUpnpAction *action, const char *name)
{
	CgUpnpArgument *arg;

	arg = mupnp_upnp_action_getargumentbyname(action, name);
	if (!arg)
		return NULL;
	return mupnp_upnp_argument_getvalue(arg);
}

/****************************************
* mupnp_upnp_action_setargumentvaluebyname
****************************************/

BOOL mupnp_upnp_action_setargumentvaluebyname(CgUpnpAction *action, const char *name, const char *value)
{
	CgUpnpArgument *arg;

	arg = mupnp_upnp_action_getargumentbyname(action, name);
	if (!arg)
		return FALSE;
	mupnp_upnp_argument_setvalue(arg, value);
	return TRUE;
}

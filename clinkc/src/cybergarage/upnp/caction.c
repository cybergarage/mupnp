/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: caction.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/caction.h>

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_action_initchildnodes(CgUpnpAction *action);
static void cg_upnp_action_initargumentlist(CgUpnpAction *action);

/****************************************
* cg_upnp_action_new
****************************************/

CgUpnpAction *cg_upnp_action_new()
{
	CgUpnpAction *action = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));
	cg_list_node_init((CgList *)action);
	
	action->parentService = NULL;
	action->actionNode = NULL;

	action->argumentList = cg_upnp_argumentlist_new();
	
	action->upnpStatus = cg_upnp_status_new();
	action->listener = NULL;
	cg_upnp_action_setuserdata(action, NULL);

	return action;
}

/****************************************
* cg_upnp_action_delete
****************************************/

void cg_upnp_action_delete(CgUpnpAction *action)
{
	cg_upnp_argumentlist_delete(action->argumentList);
	
	cg_upnp_status_delete(action->upnpStatus);
	
	cg_list_remove((CgList *)action);
	free(action);
}

/****************************************
* cg_upnp_action_setactionnode
****************************************/

void cg_upnp_action_setactionnode(CgUpnpAction *action, CgXmlNode *node)
{
	action->actionNode = node;
	cg_upnp_action_initchildnodes(action);
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_action_initchildnodes
****************************************/

static void cg_upnp_action_initchildnodes(CgUpnpAction *action)
{
	cg_upnp_action_initargumentlist(action);
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_action_initargumentlist
****************************************/

static void cg_upnp_action_initargumentlist(CgUpnpAction *action)
{
	CgXmlNode *actionNode;
	CgXmlNode *argumentListNode;
	CgXmlNode *childNode;
	CgUpnpArgument *arg;
	
	cg_upnp_argumentlist_clear(action->argumentList);

	actionNode = cg_upnp_action_getactionnode(action);
	argumentListNode = cg_xml_node_getchildnode(actionNode, CG_UPNP_ARGUMENTLIST_ELEM_NAME);
	
	if (argumentListNode == NULL)
		return;

	for (childNode = cg_xml_node_getchildnodes(argumentListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
	
		if (cg_upnp_argument_isargumentnode(childNode) == FALSE)
			continue;
			
		arg = cg_upnp_argument_new();
		cg_upnp_argument_setargumentnode(arg, childNode);
		cg_upnp_argumentlist_add(action->argumentList, arg);
	} 
}

/****************************************
* cg_upnp_action_getargumentbyname
****************************************/

CgUpnpArgument *cg_upnp_action_getargumentbyname(CgUpnpAction *action, char *name)
{
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
	argList = cg_upnp_action_getargumentlist(action);
	for (arg=cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isname(arg, name) == TRUE)
			return arg;
	}
	
	return NULL;
}


/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: caction_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/caction.h>

/****************************************
* cg_upnp_action_new
****************************************/

CgUpnpActionList *cg_upnp_actionlist_new()
{
	CgUpnpAction *actionList = (CgUpnpAction *)malloc(sizeof(CgUpnpAction));
	cg_list_header_init((CgList *)actionList);
	return actionList;
}

/****************************************
* cg_upnp_action_delete
****************************************/

void cg_upnp_actionlist_delete(CgUpnpActionList *actionList)
{
	cg_list_remove((CgList *)actionList);
	free(actionList);
}

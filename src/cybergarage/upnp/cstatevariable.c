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
*	File: cstatevariable.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_upnp_statevariable_new
****************************************/

CgUpnpStateVariable *cg_upnp_statevariable_new()
{
	CgUpnpStateVariable *statVar;

	cg_log_debug_l4("Entering...\n");

	statVar = (CgUpnpStateVariable *)malloc(sizeof(CgUpnpStateVariable));

	if ( NULL != statVar )
	{
		cg_list_node_init((CgList *)statVar);
		
		statVar->parentService = NULL;
		statVar->stateVarNode = NULL;
		
		statVar->value = cg_string_new();
		statVar->upnpStatus = cg_upnp_status_new();
		
		cg_upnp_statevariable_setlistener(statVar, NULL);
		cg_upnp_statevariable_setuserdata(statVar, NULL);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return statVar;
}

/****************************************
* cg_upnp_statevariable_delete
****************************************/

void cg_upnp_statevariable_delete(CgUpnpStateVariable *statVar)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)statVar);
	
	cg_string_delete(statVar->value);
	cg_upnp_status_delete(statVar->upnpStatus);
	
	free(statVar);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setstatevariablenode
****************************************/

void cg_upnp_statevariable_setstatevariablenode(CgUpnpStateVariable *statVar, CgXmlNode *node)
{
	cg_log_debug_l4("Entering...\n");

	statVar->stateVarNode = node;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvaluewithnotify
****************************************/

static void cg_upnp_statevariable_setvaluewithnotify(CgUpnpStateVariable *statVar, char *data, BOOL doNotify)
{
	CgUpnpService *service;

	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(statVar->value, data);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	/**** notify event ****/
	if (doNotify) {
		if (cg_upnp_statevariable_issendevents(statVar) == FALSE)
			return;

		service = cg_upnp_statevariable_getservice(statVar);
		if (service == NULL)
			return;

		cg_upnp_service_notify(service, statVar);
	}
#endif

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvalue
****************************************/

void cg_upnp_statevariable_setvalue(CgUpnpStateVariable *statVar, char *data)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_statevariable_setvaluewithnotify(statVar, data, TRUE);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_statevariable_setvaluewithoutnotify
****************************************/

void cg_upnp_statevariable_setvaluewithoutnotify(CgUpnpStateVariable *statVar, char *data)
{
	cg_log_debug_l4("Entering...\n");

	cg_upnp_statevariable_setvaluewithnotify(statVar, data, FALSE);

	cg_log_debug_l4("Leaving...\n");
}

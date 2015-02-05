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

#include <mupnp/statevariable.h>
#include <mupnp/service.h>
#include <mupnp/util/log.h>

#include <string.h>

/****************************************
* mupnp_upnp_statevariable_new
****************************************/

mUpnpUpnpStateVariable *mupnp_upnp_statevariable_new()
{
	mUpnpUpnpStateVariable *statVar;

	mupnp_log_debug_l4("Entering...\n");

	statVar = (mUpnpUpnpStateVariable *)malloc(sizeof(mUpnpUpnpStateVariable));

	if ( NULL != statVar )
	{
		mupnp_list_node_init((mUpnpList *)statVar);

		statVar->parentService = NULL;
		statVar->stateVarNode = NULL;

		statVar->value = mupnp_string_new();
		statVar->upnpStatus = mupnp_upnp_status_new();
		statVar->allowedValueList = NULL;


		mupnp_upnp_statevariable_setlistener(statVar, NULL);
		mupnp_upnp_statevariable_setuserdata(statVar, NULL);
	}

	mupnp_log_debug_l4("Leaving...\n");

	return statVar;
}

/****************************************
* mupnp_upnp_statevariable_delete
****************************************/

void mupnp_upnp_statevariable_delete(mUpnpUpnpStateVariable *statVar)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)statVar);

	mupnp_string_delete(statVar->value);
	mupnp_upnp_status_delete(statVar->upnpStatus);
	if (statVar->allowedValueList)
	{
		mupnp_list_remove((mUpnpList *)statVar->allowedValueList);
	}

	free(statVar);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setstatevariablenode
****************************************/

void mupnp_upnp_statevariable_setstatevariablenode(mUpnpUpnpStateVariable *statVar, mUpnpXmlNode *node)
{
	mupnp_log_debug_l4("Entering...\n");

	statVar->stateVarNode = node;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setvaluewithnotify
****************************************/

static void mupnp_upnp_statevariable_setvaluewithnotify(mUpnpUpnpStateVariable *statVar, const char *data, BOOL doNotify)
{
	mUpnpUpnpService *service;

	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(statVar->value, data);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	/**** notify event ****/
	if (doNotify) {
		if (mupnp_upnp_statevariable_issendevents(statVar) == FALSE)
			return;

		service = mupnp_upnp_statevariable_getservice(statVar);
		if (service == NULL)
			return;

		mupnp_upnp_service_notify(service, statVar);
	}
#endif

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setvalue
****************************************/

void mupnp_upnp_statevariable_setvalue(mUpnpUpnpStateVariable *statVar, const char *data)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_statevariable_setvaluewithnotify(statVar, data, TRUE);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setvaluewithoutnotify
****************************************/

void mupnp_upnp_statevariable_setvaluewithoutnotify(mUpnpUpnpStateVariable *statVar, const char *data)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_statevariable_setvaluewithnotify(statVar, data, FALSE);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_upnp_allowedvaluelist_new
 ****************************************/

mUpnpUpnpAllowedValuesList* mupnp_upnp_allowedvaluelist_new()
{
	mUpnpUpnpAllowedValuesList* allowedValueList = (mUpnpUpnpAllowedValuesList *)malloc(sizeof(mUpnpUpnpAllowedValuesList));


	if ( NULL != allowedValueList )
	{
		mupnp_list_header_init((mUpnpList *)allowedValueList);
		allowedValueList->value = mupnp_string_new();
		return allowedValueList;
	}

	return NULL;
}


/****************************************
 * mupnp_upnp_statevariable_getallowedvaluelist
 ****************************************/
mUpnpUpnpAllowedValuesList*  mupnp_upnp_statevariable_getallowedvaluelist(mUpnpUpnpStateVariable* statVar)
{
	if (!statVar->allowedValueList)
	{
		mUpnpXmlNode* allowedValuesNode = mupnp_xml_node_getchildnodebyname(mupnp_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_ALLOWEDVALUELIST);

		if (allowedValuesNode) {
			mUpnpXmlNode* cnode;
			for (cnode = mupnp_xml_node_getchildnodes(allowedValuesNode); cnode; cnode=mupnp_xml_node_next(cnode)) {
				if (!statVar->allowedValueList)
				{
					statVar->allowedValueList = (mUpnpUpnpAllowedValuesList *)malloc(sizeof(mUpnpUpnpAllowedValuesList));
					mupnp_list_header_init((mUpnpList *)statVar->allowedValueList);
					statVar->allowedValueList->value = mupnp_string_new();
					mupnp_string_setvalue(statVar->allowedValueList->value, mupnp_xml_node_getvalue(cnode));
				}
				else {
					mUpnpUpnpAllowedValue* allowed_value;
					allowed_value = (mUpnpUpnpAllowedValue *)malloc(sizeof(mUpnpUpnpAllowedValue));
					mupnp_list_node_init((mUpnpList*)allowed_value);
					allowed_value->value = mupnp_string_new();
					mupnp_string_setvalue(allowed_value->value, mupnp_xml_node_getvalue(cnode));
					mupnp_list_insert((mUpnpList *)statVar->allowedValueList, (mUpnpList *)allowed_value);
				}
			}
		}
	}
	return statVar->allowedValueList;
}

/****************************************
 * mupnp_upnp_statevariable_is_allowed_value
 ****************************************/
int mupnp_upnp_statevariable_is_allowed_value(mUpnpUpnpStateVariable* statVar, const char* value)
{
	if (!statVar->allowedValueList)
	{
		mupnp_upnp_statevariable_getallowedvaluelist(statVar);
	}

	if (statVar->allowedValueList)
	{
		mUpnpUpnpAllowedValue *allowedValue;
		for (allowedValue = (mUpnpUpnpAllowedValue*)mupnp_list_next((mUpnpList*)statVar->allowedValueList); allowedValue != NULL; allowedValue = (mUpnpUpnpAllowedValue*)mupnp_list_next((mUpnpList*)allowedValue))
		{
			if (mupnp_strcasecmp((char *)value, mupnp_string_getvalue(allowedValue->value)) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

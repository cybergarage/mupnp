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

CgUpnpStateVariable *mupnp_upnp_statevariable_new()
{
	CgUpnpStateVariable *statVar;

	mupnp_log_debug_l4("Entering...\n");

	statVar = (CgUpnpStateVariable *)malloc(sizeof(CgUpnpStateVariable));

	if ( NULL != statVar )
	{
		mupnp_list_node_init((CgList *)statVar);

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

void mupnp_upnp_statevariable_delete(CgUpnpStateVariable *statVar)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)statVar);

	mupnp_string_delete(statVar->value);
	mupnp_upnp_status_delete(statVar->upnpStatus);
	if (statVar->allowedValueList)
	{
		mupnp_list_remove((CgList *)statVar->allowedValueList);
	}

	free(statVar);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setstatevariablenode
****************************************/

void mupnp_upnp_statevariable_setstatevariablenode(CgUpnpStateVariable *statVar, CgXmlNode *node)
{
	mupnp_log_debug_l4("Entering...\n");

	statVar->stateVarNode = node;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setvaluewithnotify
****************************************/

static void mupnp_upnp_statevariable_setvaluewithnotify(CgUpnpStateVariable *statVar, const char *data, BOOL doNotify)
{
	CgUpnpService *service;

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

void mupnp_upnp_statevariable_setvalue(CgUpnpStateVariable *statVar, const char *data)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_statevariable_setvaluewithnotify(statVar, data, TRUE);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_statevariable_setvaluewithoutnotify
****************************************/

void mupnp_upnp_statevariable_setvaluewithoutnotify(CgUpnpStateVariable *statVar, const char *data)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_statevariable_setvaluewithnotify(statVar, data, FALSE);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_upnp_allowedvaluelist_new
 ****************************************/

CgUpnpAllowedValuesList* mupnp_upnp_allowedvaluelist_new()
{
	CgUpnpAllowedValuesList* allowedValueList = (CgUpnpAllowedValuesList *)malloc(sizeof(CgUpnpAllowedValuesList));


	if ( NULL != allowedValueList )
	{
		mupnp_list_header_init((CgList *)allowedValueList);
		allowedValueList->value = mupnp_string_new();
		return allowedValueList;
	}

	return NULL;
}


/****************************************
 * mupnp_upnp_statevariable_getallowedvaluelist
 ****************************************/
CgUpnpAllowedValuesList*  mupnp_upnp_statevariable_getallowedvaluelist(CgUpnpStateVariable* statVar)
{
	if (!statVar->allowedValueList)
	{
		CgXmlNode* allowedValuesNode = mupnp_xml_node_getchildnodebyname(mupnp_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_ALLOWEDVALUELIST);

		if (allowedValuesNode) {
			CgXmlNode* cnode;
			for (cnode = mupnp_xml_node_getchildnodes(allowedValuesNode); cnode; cnode=mupnp_xml_node_next(cnode)) {
				if (!statVar->allowedValueList)
				{
					statVar->allowedValueList = (CgUpnpAllowedValuesList *)malloc(sizeof(CgUpnpAllowedValuesList));
					mupnp_list_header_init((CgList *)statVar->allowedValueList);
					statVar->allowedValueList->value = mupnp_string_new();
					mupnp_string_setvalue(statVar->allowedValueList->value, mupnp_xml_node_getvalue(cnode));
				}
				else {
					CgUpnpAllowedValue* allowed_value;
					allowed_value = (CgUpnpAllowedValue *)malloc(sizeof(CgUpnpAllowedValue));
					mupnp_list_node_init((CgList*)allowed_value);
					allowed_value->value = mupnp_string_new();
					mupnp_string_setvalue(allowed_value->value, mupnp_xml_node_getvalue(cnode));
					mupnp_list_insert((CgList *)statVar->allowedValueList, (CgList *)allowed_value);
				}
			}
		}
	}
	return statVar->allowedValueList;
}

/****************************************
 * mupnp_upnp_statevariable_is_allowed_value
 ****************************************/
int mupnp_upnp_statevariable_is_allowed_value(CgUpnpStateVariable* statVar, const char* value)
{
	if (!statVar->allowedValueList)
	{
		mupnp_upnp_statevariable_getallowedvaluelist(statVar);
	}

	if (statVar->allowedValueList)
	{
		CgUpnpAllowedValue *allowedValue;
		for (allowedValue = (CgUpnpAllowedValue*)mupnp_list_next((CgList*)statVar->allowedValueList); allowedValue != NULL; allowedValue = (CgUpnpAllowedValue*)mupnp_list_next((CgList*)allowedValue))
		{
			if (mupnp_strcasecmp((char *)value, mupnp_string_getvalue(allowedValue->value)) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

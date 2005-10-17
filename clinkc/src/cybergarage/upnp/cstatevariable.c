/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
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

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_statevariable_initchildnodes(CgUpnpStateVariable *statVar);
static void cg_upnp_statevariable_initallowedvaluelist(CgUpnpStateVariable *statVar);
static void cg_upnp_statevariable_initallowedrange(CgUpnpStateVariable *statVar);

/****************************************
* cg_upnp_statevariable_new
****************************************/

CgUpnpStateVariable *cg_upnp_statevariable_new()
{
	CgUpnpStateVariable *statVar = (CgUpnpStateVariable *)malloc(sizeof(CgUpnpStateVariable));
	cg_list_node_init((CgList *)statVar);
	
	statVar->parentService = NULL;
	statVar->stateVarNode = NULL;
	
	statVar->allowedValueList = cg_upnp_allowedvaluelist_new();
	statVar->allowedValueRange = NULL;
	
	statVar->value = cg_string_new();
	statVar->upnpStatus = cg_upnp_status_new();
	
	cg_upnp_statevariable_setlistener(statVar, NULL);
	cg_upnp_statevariable_setuserdata(statVar, NULL);
	
	return statVar;
}

/****************************************
* cg_upnp_statevariable_delete
****************************************/

void cg_upnp_statevariable_delete(CgUpnpStateVariable *statVar)
{
	cg_list_remove((CgList *)statVar);
	
	cg_upnp_allowedvaluelist_delete(statVar->allowedValueList);
	if (statVar->allowedValueRange != NULL)
		cg_upnp_allowedvaluerange_delete(statVar->allowedValueRange);
		
	cg_string_delete(statVar->value);
	cg_upnp_status_delete(statVar->upnpStatus);
	
	free(statVar);
}

/****************************************
* cg_upnp_statevariable_setstatevariablenode
****************************************/

void cg_upnp_statevariable_setstatevariablenode(CgUpnpStateVariable *statVar, CgXmlNode *node)
{
	statVar->stateVarNode = node;
}

/****************************************
* cg_upnp_statevariable_setvalue
****************************************/

void cg_upnp_statevariable_setvalue(CgUpnpStateVariable *statVar, char *data)
{
	CgUpnpService *service;

	cg_string_setvalue(statVar->value, data);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	/**** notify event ****/
	if (cg_upnp_statevariable_issendevents(statVar) == FALSE)
		return;

	service = cg_upnp_statevariable_getservice(statVar);
	if (service == NULL)
		return;

	cg_upnp_service_notify(service, statVar);
#endif
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_statevariable_initchildnodes
****************************************/

static void cg_upnp_statevariable_initchildnodes(CgUpnpStateVariable *statVar)
{
	cg_upnp_statevariable_initallowedvaluelist(statVar);
	cg_upnp_statevariable_initallowedrange(statVar);
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_statevariable_initallowedvaluelist
****************************************/

static void cg_upnp_statevariable_initallowedvaluelist(CgUpnpStateVariable *statVar)
{
	CgXmlNode *statVarNode;
	CgXmlNode *allowedValueListNode;
	CgXmlNode *childNode;
	CgUpnpAllowedValue *allowedVal;
	
	cg_upnp_allowedvaluelist_clear(statVar->allowedValueList);
	
	statVarNode = cg_upnp_statevariable_getstatevariablenode(statVar);
	allowedValueListNode = cg_xml_node_getchildnode(statVarNode, CG_UPNP_ALLOWEDVALUELIST_ELEM_NAME);
	if (allowedValueListNode == NULL)
		return;

	for (childNode = cg_xml_node_getchildnodes(allowedValueListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_allowedvalue_isallowedvaluenode(childNode) == FALSE)
			continue;
		allowedVal = cg_upnp_allowedvalue_new();
		cg_upnp_allowedvalue_setallowedvaluenode(allowedVal, childNode);
		cg_upnp_allowedvaluelist_add(statVar->allowedValueList, allowedVal);
	}
}

/****************************************
* cg_upnp_statevariable_initallowedrange
****************************************/

static void cg_upnp_statevariable_initallowedrange(CgUpnpStateVariable *statVar)
{
	CgXmlNode *statVarNode;
	CgXmlNode *allowedValueRangeNode;
	
	if (statVar->allowedValueRange != NULL) {
		cg_upnp_allowedvaluerange_delete(statVar->allowedValueRange);
		statVar->allowedValueRange = NULL;
	}

	statVarNode = cg_upnp_statevariable_getstatevariablenode(statVar);
	allowedValueRangeNode = cg_xml_node_getchildnode(statVarNode, CG_UPNP_ALLOWEDVALUERANGE_ELEM_NAME);
	if (allowedValueRangeNode == NULL)
		return;
		
	statVar->allowedValueRange = cg_upnp_allowedvaluerange_new();
	cg_upnp_allowedvaluerange_setallowedvaluerangenode(statVar->allowedValueRange, allowedValueRangeNode);
}

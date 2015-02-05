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
*	File: caction_response.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
*	10/31/05
*		- Added mupnp_upnp_control_action_response_geterror
*
******************************************************************/

#include <mupnp/upnp_limit.h>
#include <mupnp/service.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* mupnp_upnp_control_action_response_new
****************************************/

mUpnpUpnpActionResponse *mupnp_upnp_control_action_response_new()
{
	mUpnpUpnpActionResponse *actionRes;
	 
	mupnp_log_debug_l4("Entering...\n");

	actionRes = (mUpnpUpnpActionResponse *)malloc(sizeof(mUpnpUpnpActionResponse));

	if ( NULL != actionRes )
	{	
		actionRes->soapRes = mupnp_soap_response_new();
		actionRes->isSoapResCreated = TRUE;

		actionRes->argList = mupnp_upnp_argumentlist_new();
	}
	
	return actionRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_delete
****************************************/

void mupnp_upnp_control_action_response_delete(mUpnpUpnpActionResponse *actionRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_control_action_response_clear(actionRes);
	
	if (actionRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(actionRes->soapRes);
		
	mupnp_upnp_argumentlist_delete(actionRes->argList);
	
	free(actionRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_clear
****************************************/

void mupnp_upnp_control_action_response_clear(mUpnpUpnpActionResponse *actionRes)
{
	mupnp_log_debug_l4("Entering...\n");

	if (actionRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = mupnp_soap_response_new();
	actionRes->isSoapResCreated = TRUE;
		
	mupnp_upnp_argumentlist_clear(actionRes->argList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_setsoapresponse
****************************************/

void mupnp_upnp_control_action_response_setsoapresponse(mUpnpUpnpActionResponse *actionRes, mUpnpSoapResponse *soapRes)
{
	mupnp_log_debug_l4("Entering...\n");

	if (actionRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = soapRes;
	actionRes->isSoapResCreated = FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_createresponsenode
****************************************/

static mUpnpXmlNode *mupnp_upnp_control_action_response_createresponsenode(mUpnpUpnpAction *action)
{
	mUpnpXmlNode *actionNameResNode;
	char nodeName[CG_UPNP_ACTOINNAME_LEN_MAX + sizeof(CG_SOAP_METHODNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_RESPONSE) + 1];
	char attrName[sizeof(CG_SOAP_ATTRIBUTE_XMLNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_METHODNS) + 1];
	const char *actionName;
	mUpnpXmlNode *serviceNode;
	mUpnpUpnpService *service;
	mUpnpUpnpArgumentList *argList;
	mUpnpUpnpArgument *arg;
	mUpnpXmlNode *argNode;
	
	mupnp_log_debug_l4("Entering...\n");

	actionNameResNode = mupnp_xml_node_new();

	/* action name */
	actionName = mupnp_upnp_action_getname(action);
	mupnp_strcpy(nodeName, CG_SOAP_METHODNS);
	mupnp_strcat(nodeName, CG_SOAP_DELIM);
	mupnp_strncat(nodeName, actionName, CG_UPNP_ACTOINNAME_LEN_MAX);
	mupnp_strcat(nodeName, CG_SOAP_RESPONSE);
	mupnp_xml_node_setname(actionNameResNode, nodeName);

	/* service attribute */
	serviceNode = mupnp_upnp_service_getservicenode(mupnp_upnp_action_getservice(action));
	if (serviceNode != NULL) {
		service = mupnp_upnp_service_new();
		mupnp_upnp_service_setservicenode(service, serviceNode);
		mupnp_strcpy(attrName, CG_SOAP_ATTRIBUTE_XMLNS);
		mupnp_strcat(attrName, CG_SOAP_DELIM);
		mupnp_strcat(attrName, CG_SOAP_METHODNS);
		mupnp_xml_node_setattribute(actionNameResNode, attrName, mupnp_upnp_service_getservicetype(service));
		mupnp_upnp_service_delete(service);
	}

	/* arguments */
	argList = mupnp_upnp_action_getargumentlist(action);
	for (arg = mupnp_upnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_upnp_argument_next(arg)) {
		if (mupnp_upnp_argument_isoutdirection(arg) == FALSE)
			continue;
		argNode = mupnp_xml_node_new();
		mupnp_xml_node_setname(argNode, mupnp_upnp_argument_getname(arg));		
		mupnp_xml_node_setvalue(argNode, mupnp_upnp_argument_getvalue(arg));
		mupnp_xml_node_addchildnode(actionNameResNode, argNode);
	}

	return actionNameResNode;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_setresponse
****************************************/

void mupnp_upnp_control_action_response_setresponse(mUpnpUpnpActionResponse *actionRes, mUpnpUpnpAction *action)
{
	mUpnpSoapResponse *soapRes;
	mUpnpHttpResponse *httpRes;
	mUpnpXmlNode *bodyNode;
	mUpnpXmlNode *resNode;
	mUpnpXmlNode *envNode;

	mupnp_log_debug_l4("Entering...\n");

	soapRes = mupnp_upnp_control_action_response_getsoapresponse(actionRes);
	httpRes = mupnp_soap_response_gethttpresponse(soapRes);

	mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	mupnp_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = mupnp_soap_response_getbodynode(soapRes);
	resNode = mupnp_upnp_control_action_response_createresponsenode(action);
	mupnp_xml_node_addchildnode(bodyNode, resNode);
	
	envNode = mupnp_soap_response_getenvelopenode(soapRes);
	mupnp_soap_response_setcontent(soapRes, envNode);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_getactionresponsenode
****************************************/

mUpnpXmlNode *mupnp_upnp_control_action_response_getactionresponsenode(mUpnpUpnpActionResponse *actionRes)
{
	mUpnpSoapResponse *soapRes;
	mUpnpXmlNode *bodyNode;
	
	mupnp_log_debug_l4("Entering...\n");

	soapRes = mupnp_upnp_control_action_response_getsoapresponse(actionRes);
	
	bodyNode = mupnp_soap_response_getbodynode(soapRes);

	if (bodyNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	return mupnp_xml_node_getchildnodes(bodyNode);		

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_getresult
****************************************/

BOOL mupnp_upnp_control_action_response_getresult(mUpnpUpnpActionResponse *actionRes, mUpnpUpnpAction *action)
{
	mUpnpXmlNode *resNode;
	mUpnpXmlNode *argNode;
	char *argName;
	mUpnpUpnpArgument *arg;
	
	mupnp_log_debug_l4("Entering...\n");

	resNode = mupnp_upnp_control_action_response_getactionresponsenode(actionRes);
	if (resNode == NULL)
		return FALSE;
		
	for (argNode = mupnp_xml_node_getchildnodes(resNode); argNode != NULL; argNode = mupnp_xml_node_next(argNode)) {
		argName = mupnp_xml_node_getname(argNode);
		arg = mupnp_upnp_action_getargumentbyname(action, argName);
		if (arg == NULL)
			continue;
		mupnp_upnp_argument_setvalue(arg, mupnp_xml_node_getvalue(argNode));
	}

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_action_response_geterror
****************************************/
BOOL mupnp_upnp_control_action_response_geterror(mUpnpUpnpActionResponse *actionRes, mUpnpUpnpAction *action)
{
	mUpnpXmlNode *resNode;
	mUpnpXmlNode *upnpErrorNode;
	mUpnpXmlNode *node;
	char *errDesc = NULL;
	char *errCode = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	resNode = mupnp_upnp_control_action_response_getactionresponsenode(actionRes);
	if (resNode == NULL)
	{
		return FALSE;
	}
		
	/* Response node is FAULT node, there will be no output args,
	   but set action status and description */
	upnpErrorNode = mupnp_xml_node_getchildnodewithnamespace(resNode, CG_SOAP_DETAIL, NULL, TRUE);

	if (upnpErrorNode == NULL) return FALSE;
		
	upnpErrorNode = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_FAULT_STRING, 
							      NULL, TRUE);

	if (upnpErrorNode == NULL) return FALSE;
		
	node = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_ERROR_DESCRIPTION, 
						     NULL, TRUE);
	if (node)
		errDesc = mupnp_xml_node_getvalue(node);

	node = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, CG_UPNP_CONTROL_ERROR_CODE, 
						     NULL, TRUE);
	if (node)
		errCode = mupnp_xml_node_getvalue(node);

	if (errCode == NULL) return FALSE;

	mupnp_upnp_action_setstatusdescription(action, errDesc);
	mupnp_upnp_action_setstatuscode(action, mupnp_str2int(errCode));
		
	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif


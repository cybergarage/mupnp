/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: caction_response.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/control/ccontrol.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* cg_upnp_control_action_response_new
****************************************/

CgUpnpActionResponse *cg_upnp_control_action_response_new()
{
	CgUpnpActionResponse *actionRes;
	 
	actionRes = (CgUpnpActionResponse *)malloc(sizeof(CgUpnpActionResponse));
	
	actionRes->soapRes = cg_soap_response_new();
	actionRes->isSoapResCreated = TRUE;

	actionRes->argList = cg_upnp_argumentlist_new();
	
	return actionRes;
}

/****************************************
* cg_upnp_control_action_response_delete
****************************************/

void cg_upnp_control_action_response_delete(CgUpnpActionResponse *actionRes)
{
	cg_upnp_control_action_response_clear(actionRes);
	
	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
		
	cg_upnp_argumentlist_delete(actionRes->argList);
	
	free(actionRes);
}

/****************************************
* cg_upnp_control_action_response_clear
****************************************/

void cg_upnp_control_action_response_clear(CgUpnpActionResponse *actionRes)
{
	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = cg_soap_response_new();
	actionRes->isSoapResCreated = TRUE;
		
	cg_upnp_argumentlist_clear(actionRes->argList);
}

/****************************************
* cg_upnp_control_action_response_setsoapresponse
****************************************/

void cg_upnp_control_action_response_setsoapresponse(CgUpnpActionResponse *actionRes, CgSoapResponse *soapRes)
{
	if (actionRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(actionRes->soapRes);
	actionRes->soapRes = soapRes;
	actionRes->isSoapResCreated = FALSE;
}

/****************************************
* cg_upnp_control_action_response_createresponsenode
****************************************/

static CgXmlNode *cg_upnp_control_action_response_createresponsenode(CgUpnpAction *action)
{
	CgXmlNode *actionNameResNode;
	char nodeName[CG_UPNP_ACTOINNAME_LEN_MAX + sizeof(CG_SOAP_METHODNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_RESPONSE) + 1];
	char attrName[sizeof(CG_SOAP_ATTRIBUTE_XMLNS) + sizeof(CG_SOAP_DELIM) + sizeof(CG_SOAP_METHODNS) + 1];
	char *actionName;
	CgXmlNode *serviceNode;
	CgUpnpService *service;
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	CgXmlNode *argNode;
	
	actionNameResNode = cg_xml_node_new();

	/* action name */
	actionName = cg_upnp_action_getname(action);
	cg_strcpy(nodeName, CG_SOAP_METHODNS);
	cg_strcat(nodeName, CG_SOAP_DELIM);
	cg_strncat(nodeName, actionName, CG_UPNP_ACTOINNAME_LEN_MAX);
	cg_strcat(nodeName, CG_SOAP_RESPONSE);
	cg_xml_node_setname(actionNameResNode, nodeName);

	/* service attribute */
	serviceNode = cg_upnp_service_getservicenode(cg_upnp_action_getservice(action));
	if (serviceNode != NULL) {
		service = cg_upnp_service_new();
		cg_upnp_service_setservicenode(service, serviceNode);
		cg_strcpy(attrName, CG_SOAP_ATTRIBUTE_XMLNS);
		cg_strcat(attrName, CG_SOAP_DELIM);
		cg_strcat(attrName, CG_SOAP_METHODNS);
		cg_xml_node_setattribute(actionNameResNode, attrName, cg_upnp_service_getservicetype(service));
		cg_upnp_service_delete(service);
	}

	/* arguments */
	argList = cg_upnp_action_getargumentlist(action);
	for (arg = cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isoutdirection(arg) == FALSE)
			continue;
		argNode = cg_xml_node_new();
		cg_xml_node_setname(argNode, cg_upnp_argument_getname(arg));		
		cg_xml_node_setvalue(argNode, cg_upnp_argument_getvalue(arg));
		cg_xml_node_addchildnode(actionNameResNode, argNode);
	}

	return actionNameResNode;
}

/****************************************
* cg_upnp_control_action_response_setresponse
****************************************/

void cg_upnp_control_action_response_setresponse(CgUpnpActionResponse *actionRes, CgUpnpAction *action)
{
	CgSoapResponse *soapRes;
	CgHttpResponse *httpRes;
	CgXmlNode *bodyNode;
	CgXmlNode *resNode;
	CgXmlNode *envNode;

	soapRes = cg_upnp_control_action_response_getsoapresponse(actionRes);
	httpRes = cg_soap_response_gethttpresponse(soapRes);

	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = cg_soap_response_getbodynode(soapRes);
	resNode = cg_upnp_control_action_response_createresponsenode(action);
	cg_xml_node_addchildnode(bodyNode, resNode);
	
	envNode = cg_soap_response_getenvelopenode(soapRes);
	cg_soap_response_setcontent(soapRes, envNode);
}

/****************************************
* cg_upnp_control_action_response_getactionresponsenode
****************************************/

CgXmlNode *cg_upnp_control_action_response_getactionresponsenode(CgUpnpActionResponse *actionRes)
{
	CgSoapResponse *soapRes;
	CgXmlNode *bodyNode;
	
	soapRes = cg_upnp_control_action_response_getsoapresponse(actionRes);
	
	bodyNode = cg_soap_response_getbodynode(soapRes);
	if (bodyNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	return cg_xml_node_getchildnodes(bodyNode);		
}

/****************************************
* cg_upnp_control_action_response_getresult
****************************************/

BOOL cg_upnp_control_action_response_getresult(CgUpnpActionResponse *actionRes, CgUpnpAction *action)
{
	CgXmlNode *resNode;
	CgXmlNode *argNode;
	char *argName;
	CgUpnpArgument *arg;
	
	resNode = cg_upnp_control_action_response_getactionresponsenode(actionRes);
	if (resNode == NULL)
		return FALSE;
		
	for (argNode = cg_xml_node_getchildnodes(resNode); argNode != NULL; argNode = cg_xml_node_next(argNode)) {
		argName = cg_xml_node_getname(argNode);
		arg = cg_upnp_action_getargumentbyname(action, argName);
		if (arg == NULL)
			continue;
		cg_upnp_argument_setvalue(arg, cg_xml_node_getvalue(argNode));
	}

	return TRUE;
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif


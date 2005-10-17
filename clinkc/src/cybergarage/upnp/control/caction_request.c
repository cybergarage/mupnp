/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: caction_request.c
*
*	Revision:
*
*	05/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* cg_upnp_control_action_request_new
****************************************/

CgUpnpActionRequest *cg_upnp_control_action_request_new()
{
	CgUpnpActionRequest *actionReq;
	 
	actionReq = (CgUpnpActionRequest *)malloc(sizeof(CgUpnpActionRequest));
	
	actionReq->soapReq = cg_soap_request_new();
	actionReq->isSoapReqCreated = TRUE;
	actionReq->actionRes = cg_upnp_control_action_response_new();
	
	actionReq->argList = cg_upnp_argumentlist_new();
	
	return actionReq;
}

/****************************************
* cg_upnp_control_action_request_delete
****************************************/

void cg_upnp_control_action_request_delete(CgUpnpActionRequest *actionReq)
{
	cg_upnp_control_action_request_clear(actionReq);

	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);

	cg_upnp_control_action_response_delete(actionReq->actionRes);
			
	cg_upnp_argumentlist_delete(actionReq->argList);
	
	free(actionReq);
}

/****************************************
* cg_upnp_control_action_request_clear
****************************************/

void cg_upnp_control_action_request_clear(CgUpnpActionRequest *actionReq)
{
	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);
	actionReq->soapReq = cg_soap_request_new();
	actionReq->isSoapReqCreated = TRUE;
	
	cg_upnp_argumentlist_clear(actionReq->argList);
}

/****************************************
* cg_upnp_control_action_request_setsoaprequest
****************************************/

void cg_upnp_control_action_request_setsoaprequest(CgUpnpActionRequest *actionReq, CgSoapRequest *soapReq)
{
	CgXmlNode *actionNode;
	CgXmlNode *argNode;
	CgUpnpArgument *arg;
	
	if (actionReq->isSoapReqCreated == TRUE)
		cg_soap_request_delete(actionReq->soapReq);
	actionReq->soapReq = soapReq;
	actionReq->isSoapReqCreated = FALSE;
	
	cg_upnp_argumentlist_clear(actionReq->argList);
	
	actionNode = cg_upnp_control_action_request_getactionnode(actionReq);
	if (actionNode == NULL)
		return;
	
	for (argNode = cg_xml_node_getchildnodes(actionNode); argNode != NULL; argNode = cg_xml_node_next(argNode)) {
		arg = cg_upnp_argument_new();
		cg_upnp_argument_setargumentnode(arg, argNode);
		cg_upnp_argumentlist_add(actionReq->argList, arg);
	}

	cg_soap_request_createcontent(soapReq);
}

/****************************************
* cg_upnp_control_action_request_getactionnode
****************************************/

CgXmlNode *cg_upnp_control_action_request_getactionnode(CgUpnpActionRequest *actionReq)
{
	CgSoapRequest *soapReq;	
	CgXmlNode *bodyNode;
	
	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	
	bodyNode = cg_soap_request_getbodynode(soapReq);
	if (bodyNode == NULL)
		return NULL;
	
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	return cg_xml_node_getchildnodes(bodyNode);		
}

/****************************************
* cg_upnp_control_action_request_getactionname
****************************************/

char *cg_upnp_control_action_request_getactionname(CgUpnpActionRequest *actionReq)
{
	CgXmlNode *node;
	char *name;
	int urnDelimIdx;
	
	node = cg_upnp_control_action_request_getactionnode(actionReq);
	if (node == NULL)
		return "";
	
	name = cg_xml_node_getname(node);
	if (name == NULL)
		return "";	
		
	urnDelimIdx = cg_strstr(name, CG_HTTP_SOAP_URN_DELIM);
	if (urnDelimIdx < 0)
		return "";
		
	return (name + urnDelimIdx + 1);
}

/****************************************
* cg_upnp_control_action_request_createactionnode
****************************************/

CgXmlNode *cg_upnp_control_action_request_createactionnode(CgUpnpAction *action)
{
	CgUpnpService *service;
	CgXmlNode *actionNode;
	CgUpnpArgument *arg;
	CgXmlNode *argNode;
		
	service = cg_upnp_action_getservice(action);
	
	actionNode = cg_xml_node_new();
	cg_xml_node_setname(actionNode, cg_upnp_action_getname(action));
	cg_xml_node_setnamespace(actionNode, CG_UPNP_CONTROL_NS, cg_upnp_service_getservicetype(service));
	
	for (arg = cg_upnp_action_getarguments(action); arg; arg = cg_upnp_argument_next(arg)) {
		argNode = cg_xml_node_new();
		cg_xml_node_setname(argNode, cg_upnp_argument_getname(arg));			
		cg_xml_node_setvalue(argNode, cg_upnp_argument_getvalue(arg));			
		cg_xml_node_addchildnode(actionNode, argNode);
	}
	
	return actionNode;
}

/****************************************
* cg_upnp_control_action_setrequest
****************************************/
	
void cg_upnp_control_action_request_setaction(CgUpnpActionRequest *actionReq, CgUpnpAction *action)
{
	CgUpnpService *service;
	CgSoapRequest *soapReq;
	CgString *soapAction;	
	CgXmlNode *bodyNode;
	CgXmlNode *contentNode;
	
	service = cg_upnp_action_getservice(action);
	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	
	soapAction = cg_string_new();
	cg_string_addvalue(soapAction, "\"");
	cg_string_addvalue(soapAction, cg_upnp_service_getservicetype(service));
	cg_string_addvalue(soapAction, "#");
	cg_string_addvalue(soapAction, cg_upnp_action_getname(action));
	cg_string_addvalue(soapAction, "\"");
	cg_soap_request_setsoapaction(soapReq, cg_string_getvalue(soapAction));
	cg_string_delete(soapAction);
		
	cg_upnp_control_request_sethostfromservice(soapReq, service);
	
	cg_upnp_control_soap_request_initializeenvelopenode(soapReq);
	bodyNode = cg_soap_request_getbodynode(soapReq);
	contentNode = cg_upnp_control_action_request_createactionnode(action);
	cg_xml_node_addchildnode(bodyNode, contentNode);

	cg_soap_request_createcontent(soapReq);
}

/****************************************
* cg_upnp_control_action_request_post
****************************************/

CgUpnpActionResponse *cg_upnp_control_action_request_post(CgUpnpActionRequest *actionReq)
{
	CgSoapRequest *soapReq;
	CgSoapResponse *soapRes;
	CgUpnpActionResponse *actionRes;
	CgHttpRequest *httpReq;
	CgNetURL *postURL;
	
	soapReq = cg_upnp_control_action_request_getsoaprequest(actionReq);
	soapRes = cg_soap_request_getsoapresponse(soapReq);
	actionRes = cg_upnp_control_action_request_getactionresponse(actionReq);
	httpReq = cg_soap_request_gethttprequest(soapReq);
	postURL = cg_http_request_getposturl(httpReq);
	
	cg_upnp_control_action_response_setsoapresponse(actionRes, soapRes);
	
	cg_soap_request_post(
		soapReq, 
		cg_net_url_gethost(postURL),
		cg_net_url_getport(postURL));
	
	return actionRes;
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif


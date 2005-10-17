/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cquery_response.c
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
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* cg_upnp_control_query_response_new
****************************************/

CgUpnpQueryResponse *cg_upnp_control_query_response_new()
{
	CgUpnpQueryResponse *queryRes;
	 
	queryRes = (CgUpnpQueryResponse *)malloc(sizeof(CgUpnpQueryResponse));
	
	queryRes->soapRes = cg_soap_response_new();
	queryRes->isSoapResCreated = TRUE;

	return queryRes;
}

/****************************************
* cg_upnp_control_query_response_delete
****************************************/

void cg_upnp_control_query_response_delete(CgUpnpQueryResponse *queryRes)
{
	cg_upnp_control_query_response_clear(queryRes);
	
	if (queryRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(queryRes->soapRes);
	
	free(queryRes);
}

/****************************************
* cg_upnp_control_query_response_clear
****************************************/

void cg_upnp_control_query_response_clear(CgUpnpQueryResponse *queryRes)
{
	if (queryRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(queryRes->soapRes);
	queryRes->soapRes = cg_soap_response_new();
	queryRes->isSoapResCreated = TRUE;
}

/****************************************
* cg_upnp_control_query_request_setsoaprequest
****************************************/

void cg_upnp_control_query_response_setsoapresponse(CgUpnpQueryResponse *queryRes, CgSoapResponse *soapRes)
{
	if (queryRes->isSoapResCreated == TRUE)
		cg_soap_response_delete(queryRes->soapRes);
	queryRes->soapRes = soapRes;
	queryRes->isSoapResCreated = FALSE;
}

/****************************************
* cg_upnp_control_query_response_createresponsenode
****************************************/

static CgXmlNode *cg_upnp_control_query_response_createresponsenode(CgUpnpStateVariable *statVar)
{
	CgXmlNode *queryResNode;
	CgXmlNode *returnNode;
	 
	queryResNode = cg_xml_node_new();
	cg_xml_node_setname(queryResNode, CG_UPNP_CONTROL_NS CG_SOAP_DELIM CG_UPNP_CONTROL_QUERY_STATE_VARIABLE_RESPONSE);
	cg_xml_node_setnamespace(queryResNode, CG_UPNP_CONTROL_NS, CG_UPNP_CONTROL_XMLNS);
	
	returnNode = cg_xml_node_new();
	cg_xml_node_setname(returnNode, CG_UPNP_CONTROL_RETURN);
	cg_xml_node_setvalue(returnNode, cg_upnp_statevariable_getvalue(statVar));
	cg_xml_node_addchildnode(queryResNode, returnNode);
			
	return queryResNode;
}

/****************************************
* cg_upnp_control_query_response_setresponse
****************************************/

void cg_upnp_control_query_response_setresponse(CgUpnpQueryResponse *queryRes, CgUpnpStateVariable *statVar)
{
	CgSoapResponse *soapRes;
	CgHttpResponse *httpRes;
	CgXmlNode *bodyNode;
	CgXmlNode *resNode;
	CgXmlNode *envNode;

	soapRes = cg_upnp_control_query_response_getsoapresponse(queryRes);
	httpRes = cg_soap_response_gethttpresponse(soapRes);

	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = cg_soap_response_getbodynode(soapRes);
	resNode = cg_upnp_control_query_response_createresponsenode(statVar);
	cg_xml_node_addchildnode(bodyNode, resNode);
	
	envNode = cg_soap_response_getenvelopenode(soapRes);
	cg_soap_response_setcontent(soapRes, envNode);
}

/****************************************
* cg_upnp_control_query_response_setresponse
****************************************/

CgXmlNode *cg_upnp_control_query_response_getreturnnode(CgUpnpQueryResponse *queryRes)
{
	CgSoapResponse *soapRes;	
	CgXmlNode *bodyNode;
	CgXmlNode *queryResNode;
	
	soapRes = cg_upnp_control_query_response_getsoapresponse(queryRes);
	
	bodyNode = cg_soap_response_getbodynode(soapRes);
	if (bodyNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	queryResNode = cg_xml_node_getchildnodes(bodyNode);		
	if (queryResNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(queryResNode) == FALSE)
		return NULL;
		
	return cg_xml_node_getchildnodes(queryResNode);		
}
	
/****************************************
* cg_upnp_control_query_response_setresponse
****************************************/

char *cg_upnp_control_query_response_getreturnvalue(CgUpnpQueryResponse *queryRes)
{
	CgXmlNode *returnNode;
	
	returnNode = cg_upnp_control_query_response_getreturnnode(queryRes);
	if (returnNode == NULL)
		return "";
	return cg_xml_node_getvalue(returnNode);
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif

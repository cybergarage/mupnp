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
*	File: cquery_response.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
******************************************************************/

#include <mupnp/upnp_limit.h>
#include <mupnp/service.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* mupnp_upnp_control_query_response_new
****************************************/

mUpnpUpnpQueryResponse *mupnp_upnp_control_query_response_new()
{
	mUpnpUpnpQueryResponse *queryRes;
	 
	mupnp_log_debug_l4("Entering...\n");

	queryRes = (mUpnpUpnpQueryResponse *)malloc(sizeof(mUpnpUpnpQueryResponse));

	if ( NULL != queryRes )
	{
		queryRes->soapRes = mupnp_soap_response_new();
		queryRes->isSoapResCreated = TRUE;
	}

	return queryRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_response_delete
****************************************/

void mupnp_upnp_control_query_response_delete(mUpnpUpnpQueryResponse *queryRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_control_query_response_clear(queryRes);
	
	if (queryRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(queryRes->soapRes);
	
	free(queryRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_response_clear
****************************************/

void mupnp_upnp_control_query_response_clear(mUpnpUpnpQueryResponse *queryRes)
{
	mupnp_log_debug_l4("Entering...\n");

	if (queryRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(queryRes->soapRes);
	queryRes->soapRes = mupnp_soap_response_new();
	queryRes->isSoapResCreated = TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_setsoaprequest
****************************************/

void mupnp_upnp_control_query_response_setsoapresponse(mUpnpUpnpQueryResponse *queryRes, mUpnpSoapResponse *soapRes)
{
	mupnp_log_debug_l4("Entering...\n");

	if (queryRes->isSoapResCreated == TRUE)
		mupnp_soap_response_delete(queryRes->soapRes);
	queryRes->soapRes = soapRes;
	queryRes->isSoapResCreated = FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_response_createresponsenode
****************************************/

static mUpnpXmlNode *mupnp_upnp_control_query_response_createresponsenode(mUpnpUpnpStateVariable *statVar)
{
	mUpnpXmlNode *queryResNode;
	mUpnpXmlNode *returnNode;
	 
	mupnp_log_debug_l4("Entering...\n");

	queryResNode = mupnp_xml_node_new();
	mupnp_xml_node_setname(queryResNode, CG_UPNP_CONTROL_NS CG_SOAP_DELIM CG_UPNP_CONTROL_QUERY_STATE_VARIABLE_RESPONSE);
	mupnp_xml_node_setnamespace(queryResNode, CG_UPNP_CONTROL_NS, CG_UPNP_CONTROL_XMLNS);
	
	returnNode = mupnp_xml_node_new();
	mupnp_xml_node_setname(returnNode, CG_UPNP_CONTROL_RETURN);
	mupnp_xml_node_setvalue(returnNode, mupnp_upnp_statevariable_getvalue(statVar));
	mupnp_xml_node_addchildnode(queryResNode, returnNode);
			
	return queryResNode;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_response_setresponse
****************************************/

void mupnp_upnp_control_query_response_setresponse(mUpnpUpnpQueryResponse *queryRes, mUpnpUpnpStateVariable *statVar)
{
	mUpnpSoapResponse *soapRes;
	mUpnpHttpResponse *httpRes;
	mUpnpXmlNode *bodyNode;
	mUpnpXmlNode *resNode;
	mUpnpXmlNode *envNode;

	mupnp_log_debug_l4("Entering...\n");

	soapRes = mupnp_upnp_control_query_response_getsoapresponse(queryRes);
	httpRes = mupnp_soap_response_gethttpresponse(soapRes);

	mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	mupnp_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = mupnp_soap_response_getbodynode(soapRes);
	resNode = mupnp_upnp_control_query_response_createresponsenode(statVar);
	mupnp_xml_node_addchildnode(bodyNode, resNode);
	
	envNode = mupnp_soap_response_getenvelopenode(soapRes);
	mupnp_soap_response_setcontent(soapRes, envNode);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_response_setresponse
****************************************/

mUpnpXmlNode *mupnp_upnp_control_query_response_getreturnnode(mUpnpUpnpQueryResponse *queryRes)
{
	mUpnpSoapResponse *soapRes;	
	mUpnpXmlNode *bodyNode;
	mUpnpXmlNode *queryResNode;
	
	mupnp_log_debug_l4("Entering...\n");

	soapRes = mupnp_upnp_control_query_response_getsoapresponse(queryRes);
	
	bodyNode = mupnp_soap_response_getbodynode(soapRes);
	if (bodyNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	queryResNode = mupnp_xml_node_getchildnodes(bodyNode);		
	if (queryResNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(queryResNode) == FALSE)
		return NULL;
		
	return mupnp_xml_node_getchildnodes(queryResNode);		

	mupnp_log_debug_l4("Leaving...\n");
}
	
/****************************************
* mupnp_upnp_control_query_response_setresponse
****************************************/

char *mupnp_upnp_control_query_response_getreturnvalue(mUpnpUpnpQueryResponse *queryRes)
{
	mUpnpXmlNode *returnNode;
	
	mupnp_log_debug_l4("Entering...\n");

	returnNode = mupnp_upnp_control_query_response_getreturnnode(queryRes);
	if (returnNode == NULL)
		return "";
	return mupnp_xml_node_getvalue(returnNode);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif

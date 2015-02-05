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
*	File: cquery_request.c
*
*	Revision:
*
*	06/09/05
*		- first revision
*
******************************************************************/

#include <mupnp/control/control.h>
#include <mupnp/service.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* mupnp_upnp_control_query_request_new
****************************************/

CgUpnpQueryRequest *mupnp_upnp_control_query_request_new()
{
	CgUpnpQueryRequest *queryReq;
	 
	mupnp_log_debug_l4("Entering...\n");

	queryReq = (CgUpnpQueryRequest *)malloc(sizeof(CgUpnpQueryRequest));

	if ( NULL != queryReq )
	{	
		queryReq->soapReq = mupnp_soap_request_new();
		if (queryReq->soapReq == NULL) 
		{
			/* Memory allocation failed */
			free(queryReq);
			return NULL;
		}

		queryReq->isSoapReqCreated = TRUE;
		
		queryReq->queryRes = mupnp_upnp_control_query_response_new();
		if (queryReq->queryRes == NULL)
		{
			/* Memory allocation failed */
			mupnp_soap_request_delete(queryReq->soapReq);
			free(queryReq);
			return NULL;
		}
	}
		
	return queryReq;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_delete
****************************************/

void mupnp_upnp_control_query_request_delete(CgUpnpQueryRequest *queryReq)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_control_query_request_clear(queryReq);

	if (queryReq->isSoapReqCreated == TRUE)
		mupnp_soap_request_delete(queryReq->soapReq);

	mupnp_upnp_control_query_response_delete(queryReq->queryRes);
			
	free(queryReq);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_clear
****************************************/

void mupnp_upnp_control_query_request_clear(CgUpnpQueryRequest *queryReq)
{
	mupnp_log_debug_l4("Entering...\n");

	if (queryReq->isSoapReqCreated == TRUE)
		mupnp_soap_request_delete(queryReq->soapReq);
	queryReq->soapReq = mupnp_soap_request_new();
	queryReq->isSoapReqCreated = TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_setsoaprequest
****************************************/

void mupnp_upnp_control_query_request_setsoaprequest(CgUpnpQueryRequest *queryReq, CgSoapRequest *soapReq)
{
	mupnp_log_debug_l4("Entering...\n");

	if (queryReq->isSoapReqCreated == TRUE)
		mupnp_soap_request_delete(queryReq->soapReq);
	queryReq->soapReq = soapReq;
	queryReq->isSoapReqCreated = FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_getvarnamenode
****************************************/

CgXmlNode *mupnp_upnp_control_query_request_getvarnamenode(CgUpnpQueryRequest *queryReq)
{
	CgSoapRequest *soapReq;	
	CgXmlNode *bodyNode;
	CgXmlNode *queryStateVarNode;
	
	mupnp_log_debug_l4("Entering...\n");

	soapReq = mupnp_upnp_control_query_request_getsoaprequest(queryReq);
	
	bodyNode = mupnp_soap_request_getbodynode(soapReq);
	if (bodyNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(bodyNode) == FALSE)
		return NULL;

	queryStateVarNode = mupnp_xml_node_getchildnodes(bodyNode);		
	if (queryStateVarNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(queryStateVarNode) == FALSE)
		return NULL;
		
	return mupnp_xml_node_getchildnodes(queryStateVarNode);		

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_getqueryname
****************************************/

char *mupnp_upnp_control_query_request_getvarname(CgUpnpQueryRequest *queryReq)
{
	CgXmlNode *node;
	
	mupnp_log_debug_l4("Entering...\n");

	node = mupnp_upnp_control_query_request_getvarnamenode(queryReq);
	if (node == NULL)
		return "";
	
	return mupnp_xml_node_getvalue(node);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_cratestatevariablenode
****************************************/

CgXmlNode *mupnp_upnp_control_query_request_cratestatevariablenode(CgUpnpStateVariable *statVar)
{
	CgXmlNode *queryVarNode;
	CgXmlNode *varNode;
	
	mupnp_log_debug_l4("Entering...\n");

	queryVarNode = mupnp_xml_node_new();
	mupnp_xml_node_setname(queryVarNode, CG_UPNP_CONTROL_NS  CG_SOAP_DELIM CG_UPNP_CONTROL_QUERY_STATE_VARIABLE);
	mupnp_xml_node_setnamespace(queryVarNode, CG_UPNP_CONTROL_NS, CG_UPNP_CONTROL_XMLNS);
	
	varNode = mupnp_xml_node_new();
	mupnp_xml_node_setname(varNode, CG_UPNP_CONTROL_NS CG_SOAP_DELIM CG_UPNP_CONTROL_VAR_NAME);
	mupnp_xml_node_setvalue(varNode, mupnp_upnp_statevariable_getname(statVar));
	
	mupnp_xml_node_addchildnode(queryVarNode, varNode);
	
	return queryVarNode;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_setstatevariable
****************************************/

void mupnp_upnp_control_query_request_setstatevariable(CgUpnpQueryRequest *queryReq, CgUpnpStateVariable *statVar)
{
	CgUpnpService *service;
	CgSoapRequest *soapReq;
	CgXmlNode *bodyNode;
	CgXmlNode *queryNode;
	
	mupnp_log_debug_l4("Entering...\n");

	service = mupnp_upnp_statevariable_getservice(statVar);
	soapReq = mupnp_upnp_control_query_request_getsoaprequest(queryReq);
	
	mupnp_soap_request_setsoapaction(soapReq, CG_UPNP_CONTROL_QUERY_SOAPACTION);
	
	mupnp_upnp_control_request_sethostfromservice(soapReq, service);
	
	mupnp_upnp_control_soap_request_initializeenvelopenode(soapReq);
	bodyNode = mupnp_soap_request_getbodynode(soapReq);
	queryNode = mupnp_upnp_control_query_request_cratestatevariablenode(statVar);
	mupnp_xml_node_addchildnode(bodyNode, queryNode);

	mupnp_soap_request_createcontent(soapReq);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_control_query_request_post
****************************************/

CgUpnpQueryResponse *mupnp_upnp_control_query_request_post(CgUpnpQueryRequest *queryReq)
{
	CgSoapRequest *soapReq;
	CgSoapResponse *soapRes;
	CgUpnpQueryResponse *queryRes;
	CgHttpRequest *httpReq;
	CgNetURL *postURL;
	
	mupnp_log_debug_l4("Entering...\n");

	soapReq = mupnp_upnp_control_query_request_getsoaprequest(queryReq);
	soapRes = mupnp_soap_request_getsoapresponse(soapReq);
	queryRes = mupnp_upnp_control_query_request_getqueryresponse(queryReq);
	httpReq = mupnp_soap_request_gethttprequest(soapReq);
	postURL = mupnp_http_request_getposturl(httpReq);
	
	mupnp_upnp_control_query_response_setsoapresponse(queryRes, soapRes);
	
	mupnp_soap_request_post(
		soapReq, 
		mupnp_net_url_gethost(postURL),
		mupnp_net_url_getport(postURL));
	
	return queryRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif

/******************************************************************
*
*	CyberSOAP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csoap_response.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <mupnp/soap/soap.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>

/****************************************
* mupnp_soap_response_new
****************************************/

CgSoapResponse *mupnp_soap_response_new()
{
	CgSoapResponse *soapRes;

	mupnp_log_debug_l4("Entering...\n");

	soapRes = (CgSoapResponse *)malloc(sizeof(CgSoapResponse));

	if ( NULL != soapRes )
	{
		soapRes->rootNodeList = mupnp_xml_nodelist_new();

		soapRes->httpRes = mupnp_http_response_new();
		soapRes->isHttpResCreated = TRUE;
	
		mupnp_soap_response_setuserdata(soapRes, NULL);
	}

	return soapRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_response_delete
****************************************/

void mupnp_soap_response_delete(CgSoapResponse *soapRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_soap_response_clear(soapRes);
	mupnp_xml_nodelist_delete(soapRes->rootNodeList);

	if (soapRes->isHttpResCreated == TRUE)
		mupnp_http_response_delete(soapRes->httpRes);

	free(soapRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_response_clear
****************************************/

void mupnp_soap_response_clear(CgSoapResponse *soapRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_xml_nodelist_clear(soapRes->rootNodeList);

	if (soapRes->isHttpResCreated == TRUE)
		mupnp_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = mupnp_http_response_new();
	soapRes->isHttpResCreated = TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_request_getbodynode
****************************************/

CgXmlNode *mupnp_soap_response_getbodynode(CgSoapResponse *soapRes)
{
	CgXmlNode *envNode;
	 
	mupnp_log_debug_l4("Entering...\n");

	envNode = mupnp_soap_response_getenvelopenode(soapRes);
	if (envNode == NULL)
		return NULL;
	if (mupnp_xml_node_haschildnodes(envNode) == FALSE)
		return NULL;
	return mupnp_xml_node_getchildnodes(envNode);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_response_sethttpresponse
****************************************/

void mupnp_soap_response_sethttpresponse(CgSoapResponse *soapRes, CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	if (soapRes->isHttpResCreated == TRUE)
		mupnp_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = httpRes;
	soapRes->isHttpResCreated = FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_response_setcontent
****************************************/

void mupnp_soap_response_setcontent(CgSoapResponse *soapReq, CgXmlNode *node)
{
	CgHttpResponse *httpRes;
		
	mupnp_log_debug_l4("Entering...\n");

	httpRes = mupnp_soap_response_gethttpresponse(soapReq);

	/**** content type ****/
	mupnp_http_response_setcontenttype(httpRes, CG_XML_CONTENT_TYPE);
	
	/**** content ****/
	mupnp_http_response_appendncontent(httpRes, CG_SOAP_VERSION_HEADER,
					mupnp_strlen(CG_SOAP_VERSION_HEADER));
	mupnp_http_response_appendncontent(httpRes, CG_XML_CONTENT_LF,
					mupnp_strlen(CG_XML_CONTENT_LF));
	mupnp_xml_node_tostring(node, TRUE, httpRes->content);
	
	/**** content length ****/
	mupnp_http_response_setcontentlength(httpRes, 
					  mupnp_string_length(httpRes->content));

	mupnp_log_debug_l4("Leaving...\n");
}

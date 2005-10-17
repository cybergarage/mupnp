/******************************************************************
*
*	CyberSOAP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csoap_response.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/soap/csoap.h>

/****************************************
* cg_soap_response_new
****************************************/

CgSoapResponse *cg_soap_response_new()
{
	CgSoapResponse *soapRes = (CgSoapResponse *)malloc(sizeof(CgSoapResponse));

	soapRes->rootNodeList = cg_xml_nodelist_new();

	soapRes->httpRes = cg_http_response_new();
	soapRes->isHttpResCreated = TRUE;

	cg_soap_response_setuserdata(soapRes, NULL);

	return soapRes;
}

/****************************************
* cg_soap_response_delete
****************************************/

void cg_soap_response_delete(CgSoapResponse *soapRes)
{
	cg_soap_response_clear(soapRes);

	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);

	free(soapRes);
}

/****************************************
* cg_soap_response_clear
****************************************/

void cg_soap_response_clear(CgSoapResponse *soapRes)
{
	cg_xml_nodelist_clear(soapRes->rootNodeList);

	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = cg_http_response_new();
	soapRes->isHttpResCreated = TRUE;
}

/****************************************
* cg_soap_request_getbodynode
****************************************/

CgXmlNode *cg_soap_response_getbodynode(CgSoapResponse *soapRes)
{
	CgXmlNode *envNode;
	 
	envNode = cg_soap_response_getenvelopenode(soapRes);
	if (envNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(envNode) == FALSE)
		return NULL;
	return cg_xml_node_getchildnodes(envNode);
}

/****************************************
* cg_soap_response_sethttpresponse
****************************************/

void cg_soap_response_sethttpresponse(CgSoapResponse *soapRes, CgHttpResponse *httpRes)
{
	if (soapRes->isHttpResCreated == TRUE)
		cg_http_response_delete(soapRes->httpRes);
	soapRes->httpRes = httpRes;
	soapRes->isHttpResCreated = FALSE;
}

/****************************************
* cg_soap_response_setcontent
****************************************/

void cg_soap_response_setcontent(CgSoapResponse *soapReq, CgXmlNode *node)
{
	CgString *contentStr;
	char *content;
	CgHttpResponse *httpRes;
		
	httpRes = cg_soap_response_gethttpresponse(soapReq);

	/**** content type ****/
	cg_http_response_setcontenttype(httpRes, CG_XML_CONTENT_TYPE);
	
	/**** content ****/	
	contentStr = cg_string_new();
	cg_string_addvalue(contentStr, CG_SOAP_VERSION_HEADER);
	cg_string_addvalue(contentStr, CG_XML_CONTENT_LF);
	cg_xml_node_tostring(node, TRUE, contentStr);
	content = cg_string_getvalue(contentStr);
	cg_http_response_setcontentpointer(httpRes, content);
	
	/**** content length ****/
	cg_http_response_setcontentlength(httpRes, cg_strlen(content));
		
	cg_string_setfreeflag(contentStr, FALSE);
	cg_string_delete(contentStr);
}

/******************************************************************
*
*	CyberSOAP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csoap_request.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/soap/csoap.h>

/****************************************
* cg_soap_request_new
****************************************/

CgSoapRequest *cg_soap_request_new()
{
	CgSoapRequest *soapReq = (CgSoapRequest *)malloc(sizeof(CgSoapRequest));

	soapReq->rootNodeList = cg_xml_nodelist_new();
	soapReq->soapRes = cg_soap_response_new();

	soapReq->httpReq = cg_http_request_new();
	soapReq->isHttpReqCreated = TRUE;
	cg_http_request_setcontenttype(soapReq->httpReq, CG_SOAP_CONTENT_TYPE);
	cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_POST);

	cg_soap_request_setuserdata(soapReq, NULL);

	return soapReq;
}

/****************************************
* cg_soap_request_delete
****************************************/

void cg_soap_request_delete(CgSoapRequest *soapReq)
{
	cg_soap_request_clear(soapReq);

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);

	cg_soap_response_delete(soapReq->soapRes);

	free(soapReq);
}

/****************************************
* cg_soap_request_clear
****************************************/

void cg_soap_request_clear(CgSoapRequest *soapReq)
{
	cg_xml_nodelist_clear(soapReq->rootNodeList);

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);

	soapReq->httpReq = cg_http_request_new();
	soapReq->isHttpReqCreated = TRUE;
	cg_http_request_setcontenttype(soapReq->httpReq, CG_SOAP_CONTENT_TYPE);
	cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_POST);
}

/****************************************
* cg_soap_request_getbodynode
****************************************/

CgXmlNode *cg_soap_request_getbodynode(CgSoapRequest *soapReq)
{
	CgXmlNode *envNode;
	 
	envNode = cg_soap_request_getenvelopenode(soapReq);
	if (envNode == NULL)
		return NULL;
	if (cg_xml_node_haschildnodes(envNode) == FALSE)
		return NULL;
	return cg_xml_node_getchildnodes(envNode);
}

/****************************************
* cg_soap_request_sethttprequest
****************************************/

BOOL cg_soap_request_sethttprequest(CgSoapRequest *soapReq, CgHttpRequest *httpReq)
{
	char *content;
	int contentLen;

	if (soapReq->isHttpReqCreated == TRUE)
		cg_http_request_delete(soapReq->httpReq);
	soapReq->httpReq = httpReq;
	soapReq->isHttpReqCreated = FALSE;

	content = cg_http_request_getcontent(httpReq);
	contentLen = cg_http_request_getcontentlength(httpReq);

	if (content == NULL || contentLen <=0)
		return FALSE;

	return cg_soap_request_parsemessage(soapReq, content, contentLen);
}

/****************************************
* cg_soap_request_parsemessage
****************************************/

BOOL cg_soap_request_parsemessage(CgSoapRequest *soapReq, char *msg, int msgLen)
{
	CgXmlParser *xmlParser;
	BOOL parseRet;

	if (msgLen<= 0)
		return FALSE;

	xmlParser = cg_xml_parser_new();
	parseRet = cg_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
	cg_xml_parser_delete(xmlParser);

	return parseRet;
}

/****************************************
* cg_soap_request_post
****************************************/

CgSoapResponse *cg_soap_request_post(CgSoapRequest *soapReq, char *ipaddr, int port)
{
	CgHttpResponse *httpRes;
	char *content;
	int contentLen;
	CgXmlParser *xmlParser;

	httpRes = cg_http_request_post(soapReq->httpReq, ipaddr, port);
	cg_soap_response_sethttpresponse(soapReq->soapRes,httpRes);

	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);
	if (content == NULL || contentLen <= 0)
		return soapReq->soapRes;

	xmlParser = cg_xml_parser_new();
	cg_xml_parse(xmlParser, soapReq->soapRes->rootNodeList, content, contentLen);
	cg_xml_parser_delete(xmlParser);

	return soapReq->soapRes;
}

/****************************************
* cg_soap_request_setcontent
****************************************/

void cg_soap_request_setcontent(CgSoapRequest *soapReq, CgXmlNode *node)
{
	CgString *contentStr;
	char *content;
	CgHttpRequest *httpReq;
		
	httpReq = cg_soap_request_gethttprequest(soapReq);
	
	/**** content type ****/
	cg_http_request_setcontenttype(httpReq, CG_XML_CONTENT_TYPE);
	
	/**** content ****/
	contentStr = cg_string_new();
	cg_string_addvalue(contentStr, CG_SOAP_VERSION_HEADER);
	cg_string_addvalue(contentStr, CG_XML_CONTENT_LF);
	cg_xml_node_tostring(node, TRUE, contentStr);
	content = cg_string_getvalue(contentStr);
	cg_http_request_setcontentpointer(httpReq, content);
	
	/**** content length ****/
	cg_http_request_setcontentlength(httpReq, cg_strlen(content));
		
	cg_string_setfreeflag(contentStr, FALSE);
	cg_string_delete(contentStr);
}

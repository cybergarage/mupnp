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
*	File: csoap.h
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#ifndef _CG_SOAP_CSOAP_H_
#define _CG_SOAP_CSOAP_H_

#include <mupnp/typedef.h>
#include <mupnp/http/http.h>
#include <mupnp/xml/xml.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_SOAP_VERSION_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

#define CG_HTTP_SOAP_ACTION "SOAPACTION"
#define CG_HTTP_SOAP_URN_DELIM ":"

#define CG_SOAP_ENVELOPE "Envelope"
#define CG_SOAP_BODY "Body"
#define CG_SOAP_RESPONSE "Response"
#define CG_SOAP_FAULT "Fault"
#define CG_SOAP_FAULT_CODE "faultcode"
#define CG_SOAP_FAULT_STRING "faultstring"
#define CG_SOAP_FAULTACTOR "faultactor"
#define CG_SOAP_DETAIL "detail"

#define CG_SOAP_ATTRIBUTE_XMLNS "xmlns"

#define CG_SOAP_XMLNS "s"
#define CG_SOAP_METHODNS "u"
#define CG_SOAP_DELIM ":"

#define CG_SOAP_XMLNS_URL "http://schemas.xmlsoap.org/soap/envelope/"
#define CG_SOAP_ENCSTYLE_URL "http://schemas.xmlsoap.org/soap/encoding/"
	
#define CG_SOAP_CONTENT_TYPE "text/xml; charset=\"utf-8\""
#define CG_SOAP_ENCORDING "encodingStyle"

/****************************************
* Data Type
****************************************/

typedef struct _CgSoapResponse {
	CgHttpResponse *httpRes;
	BOOL isHttpResCreated;
	CgXmlNodeList *rootNodeList;
	void *userData;
} CgSoapResponse;

typedef struct _CgSoapRequest{
	CgHttpRequest *httpReq;
	BOOL isHttpReqCreated;
	CgXmlNode *rootNodeList;
	CgSoapResponse *soapRes;
	void *userData;
} CgSoapRequest;

/****************************************
* Function (Request)
****************************************/

CgSoapRequest *mupnp_soap_request_new();
void mupnp_soap_request_delete(CgSoapRequest *soapReq);
void mupnp_soap_request_clear(CgSoapRequest *soapReq);

BOOL mupnp_soap_request_sethttprequest(CgSoapRequest *soapReq, CgHttpRequest *httpReq);
#define mupnp_soap_request_gethttprequest(soapReq) (soapReq->httpReq)

#define mupnp_soap_request_setsoapaction(soapReq, name) mupnp_http_packet_setheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SOAP_ACTION, name)
#define mupnp_soap_request_getsoapaction(soapReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SOAP_ACTION)
#define mupnp_soap_request_getsoapactionwithns(soapReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SOAP_ACTION_WITH_NS)

BOOL mupnp_soap_request_parsemessage(CgSoapRequest *soapReq, char *msg, size_t msgLen);
CgSoapResponse *mupnp_soap_request_post(CgSoapRequest *soapReq, const char *ipaddr, int port);
#define mupnp_soap_request_getsoapresponse(soapReq) (soapReq->soapRes)

#define mupnp_soap_request_getrootnoodelist(soapReq) (soapReq->rootNodeList)
#define mupnp_soap_request_getrootnoode(soapReq) mupnp_xml_nodelist_gets(soapReq->rootNodeList)
#define mupnp_soap_request_getenvelopenode(soapReq) mupnp_xml_nodelist_gets(soapReq->rootNodeList)
CgXmlNode *mupnp_soap_request_getbodynode(CgSoapRequest *soapReq);

void mupnp_soap_request_setcontent(CgSoapRequest *soapReq, CgXmlNode *node);
#define mupnp_soap_request_createcontent(soapReq) mupnp_soap_request_setcontent(soapReq, mupnp_soap_request_getenvelopenode(soapReq))

/**** User Data ****/
#define mupnp_soap_request_setuserdata(soapReq, value) (soapReq->userData = value)
#define mupnp_soap_request_getuserdata(soapReq) (soapReq->userData)

/****************************************
* Function (Response)
****************************************/

CgSoapResponse *mupnp_soap_response_new();
void mupnp_soap_response_delete(CgSoapResponse *soapRes);
void mupnp_soap_response_clear(CgSoapResponse *soapRes);

void mupnp_soap_response_sethttpresponse(CgSoapResponse *soapRes, CgHttpResponse *httpRes);
#define mupnp_soap_response_gethttpresponse(soapRes) (soapRes->httpRes)

#define mupnp_soap_response_getrootnoodelist(soapRes) (soapRes->rootNodeList);
#define mupnp_soap_response_getrootnoode(soapRes) mupnp_xml_nodelist_gets(soapRes->rootNodeList);
#define mupnp_soap_response_getenvelopenode(soapRes) mupnp_xml_nodelist_gets(soapRes->rootNodeList);
CgXmlNode *mupnp_soap_response_getbodynode(CgSoapResponse *soapRes);

void mupnp_soap_response_setcontent(CgSoapResponse *soapRes, CgXmlNode *node);
#define mupnp_soap_response_createcontent(soapRes) mupnp_soap_response_setcontent(soapRes, mupnp_soap_response_getenvelopenode(soapRes))

#define mupnp_soap_response_getstatuscode(soapRes) mupnp_http_response_getstatuscode(soapRes->httpRes)
#define mupnp_soap_response_issuccessful(soapRes) mupnp_http_response_issuccessful(soapRes->httpRes)

/**** User Data ****/
#define mupnp_soap_response_setuserdata(soapRes, value) (soapRes->userData = value)
#define mupnp_soap_response_getuserdata(soapRes) (soapRes->userData)

/****************************************
* Function
****************************************/

CgXmlNode *mupnp_soap_createenvelopebodynode();

#ifdef  __cplusplus
}
#endif

#endif

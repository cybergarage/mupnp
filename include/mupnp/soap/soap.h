/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifndef _MUPNP_SOAP_SOAP_H_
#define _MUPNP_SOAP_SOAP_H_

#include <mupnp/typedef.h>
#include <mupnp/http/http.h>
#include <mupnp/xml/xml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_SOAP_VERSION_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

#define MUPNP_HTTP_SOAP_ACTION "SOAPACTION"
#define MUPNP_HTTP_SOAP_URN_DELIM ":"

#define MUPNP_SOAP_ENVELOPE "Envelope"
#define MUPNP_SOAP_BODY "Body"
#define MUPNP_SOAP_RESPONSE "Response"
#define MUPNP_SOAP_FAULT "Fault"
#define MUPNP_SOAP_FAULT_CODE "faultcode"
#define MUPNP_SOAP_FAULT_STRING "faultstring"
#define MUPNP_SOAP_FAULTACTOR "faultactor"
#define MUPNP_SOAP_DETAIL "detail"

#define MUPNP_SOAP_ATTRIBUTE_XMLNS "xmlns"

#define MUPNP_SOAP_XMLNS "s"
#define MUPNP_SOAP_METHODNS "u"
#define MUPNP_SOAP_DELIM ":"

#define MUPNP_SOAP_XMLNS_URL "http://schemas.xmlsoap.org/soap/envelope/"
#define MUPNP_SOAP_ENCSTYLE_URL "http://schemas.xmlsoap.org/soap/encoding/"

#define MUPNP_SOAP_CONTENT_TYPE "text/xml; charset=\"utf-8\""
#define MUPNP_SOAP_ENCORDING "encodingStyle"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpSoapResponse {
  mUpnpHttpResponse* httpRes;
  bool isHttpResCreated;
  mUpnpXmlNodeList* rootNodeList;
  void* userData;
} mUpnpSoapResponse;

typedef struct _mUpnpSoapRequest {
  mUpnpHttpRequest* httpReq;
  bool isHttpReqCreated;
  mUpnpXmlNode* rootNodeList;
  mUpnpSoapResponse* soapRes;
  void* userData;
} mUpnpSoapRequest;

/****************************************
* Function (Request)
****************************************/

mUpnpSoapRequest* mupnp_soap_request_new();
void mupnp_soap_request_delete(mUpnpSoapRequest* soapReq);
void mupnp_soap_request_clear(mUpnpSoapRequest* soapReq);

bool mupnp_soap_request_sethttprequest(mUpnpSoapRequest* soapReq, mUpnpHttpRequest* httpReq);
#define mupnp_soap_request_gethttprequest(soapReq) (soapReq->httpReq)

#define mupnp_soap_request_setsoapaction(soapReq, name) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SOAP_ACTION, name)
#define mupnp_soap_request_getsoapaction(soapReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SOAP_ACTION)
#define mupnp_soap_request_getsoapactionwithns(soapReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SOAP_ACTION_WITH_NS)

bool mupnp_soap_request_parsemessage(mUpnpSoapRequest* soapReq, char* msg, size_t msgLen);
mUpnpSoapResponse* mupnp_soap_request_post(mUpnpSoapRequest* soapReq, const char* ipaddr, int port);
#define mupnp_soap_request_getsoapresponse(soapReq) (soapReq->soapRes)

#define mupnp_soap_request_getrootnoodelist(soapReq) (soapReq->rootNodeList)
#define mupnp_soap_request_getrootnoode(soapReq) mupnp_xml_nodelist_gets(soapReq->rootNodeList)
#define mupnp_soap_request_getenvelopenode(soapReq) mupnp_xml_nodelist_gets(soapReq->rootNodeList)
mUpnpXmlNode* mupnp_soap_request_getbodynode(mUpnpSoapRequest* soapReq);

void mupnp_soap_request_setcontent(mUpnpSoapRequest* soapReq, mUpnpXmlNode* node);
#define mupnp_soap_request_createcontent(soapReq) mupnp_soap_request_setcontent(soapReq, mupnp_soap_request_getenvelopenode(soapReq))

/**** User Data ****/
#define mupnp_soap_request_setuserdata(soapReq, value) (soapReq->userData = value)
#define mupnp_soap_request_getuserdata(soapReq) (soapReq->userData)

/****************************************
* Function (Response)
****************************************/

mUpnpSoapResponse* mupnp_soap_response_new();
void mupnp_soap_response_delete(mUpnpSoapResponse* soapRes);
void mupnp_soap_response_clear(mUpnpSoapResponse* soapRes);

void mupnp_soap_response_sethttpresponse(mUpnpSoapResponse* soapRes, mUpnpHttpResponse* httpRes);
#define mupnp_soap_response_gethttpresponse(soapRes) (soapRes->httpRes)

#define mupnp_soap_response_getrootnoodelist(soapRes) (soapRes->rootNodeList);
#define mupnp_soap_response_getrootnoode(soapRes) mupnp_xml_nodelist_gets(soapRes->rootNodeList);
#define mupnp_soap_response_getenvelopenode(soapRes) mupnp_xml_nodelist_gets(soapRes->rootNodeList);
mUpnpXmlNode* mupnp_soap_response_getbodynode(mUpnpSoapResponse* soapRes);

void mupnp_soap_response_setcontent(mUpnpSoapResponse* soapRes, mUpnpXmlNode* node);
#define mupnp_soap_response_createcontent(soapRes) mupnp_soap_response_setcontent(soapRes, mupnp_soap_response_getenvelopenode(soapRes))

#define mupnp_soap_response_getstatuscode(soapRes) mupnp_http_response_getstatuscode(soapRes->httpRes)
#define mupnp_soap_response_issuccessful(soapRes) mupnp_http_response_issuccessful(soapRes->httpRes)

/**** User Data ****/
#define mupnp_soap_response_setuserdata(soapRes, value) (soapRes->userData = value)
#define mupnp_soap_response_getuserdata(soapRes) (soapRes->userData)

/****************************************
* Function
****************************************/

mUpnpXmlNode* mupnp_soap_createenvelopebodynode();

#ifdef __cplusplus
}
#endif

#endif

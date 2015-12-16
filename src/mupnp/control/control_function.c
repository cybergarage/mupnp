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
#include <mupnp/control/control.h>

#include <mupnp/device.h>
#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_usn_getudn
****************************************/

const char* mupnp_usn_getudn(const char* usn, char* udnBuf, size_t udnBufLen)
{
  ssize_t idx;

  mupnp_log_debug_l4("Entering...\n");

  if (usn == NULL) {
    udnBuf[0] = '\0';
    return udnBuf;
  }

  idx = mupnp_strstr(usn, "::");
  if (idx < 0) {
    mupnp_strncpy(udnBuf, usn, udnBufLen);
    udnBuf[udnBufLen - 1] = '\0';
    mupnp_strtrim(udnBuf, " ", 1);
    return udnBuf;
  }

  if (idx < udnBufLen) {
    mupnp_strncpy(udnBuf, usn, (idx));
    udnBuf[idx] = '\0';
  }
  else {
    mupnp_strncpy(udnBuf, usn, udnBufLen);
    udnBuf[udnBufLen - 1] = '\0';
  }

  mupnp_strtrim(udnBuf, " ", 1);

  mupnp_log_debug("UDN: %s\n", udnBuf);

  return udnBuf;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_soap_response_initializeenvelopenode
****************************************/

void mupnp_control_soap_response_initializeenvelopenode(mUpnpSoapResponse* soapRes)
{
  mUpnpXmlNodeList* rootNodeList;
  mUpnpHttpResponse* httpRes;

  mupnp_log_debug_l4("Entering...\n");

  rootNodeList = mupnp_soap_response_getrootnoodelist(soapRes);
  mupnp_xml_nodelist_add(rootNodeList, mupnp_soap_createenvelopebodynode());

  httpRes = mupnp_soap_response_gethttpresponse(soapRes);
  mupnp_http_request_setcontenttype(httpRes, MUPNP_SOAP_CONTENT_TYPE);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_soap_request_initializeenvelopenode
****************************************/

void mupnp_control_soap_request_initializeenvelopenode(mUpnpSoapRequest* soapReq)
{
  mUpnpXmlNodeList* rootNodeList;
  mUpnpHttpRequest* httpReq;

  mupnp_log_debug_l4("Entering...\n");

  rootNodeList = mupnp_soap_request_getrootnoodelist(soapReq);
  mupnp_xml_nodelist_add(rootNodeList, mupnp_soap_createenvelopebodynode());

  httpReq = mupnp_soap_request_gethttprequest(soapReq);
  mupnp_http_request_setcontenttype(httpReq, MUPNP_SOAP_CONTENT_TYPE);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_soap_response_createfaultresponsenode
****************************************/

mUpnpXmlNode* mupnp_control_soap_response_createfaultresponsenode(int errCode, char* errDescr)
{
  mUpnpXmlNode* faultNode;
  mUpnpXmlNode* faultCodeNode;
  mUpnpXmlNode* faultStringNode;
  mUpnpXmlNode* detailNode;
  mUpnpXmlNode* upnpErrorNode;
  mUpnpXmlNode* errorCodeNode;
  mUpnpXmlNode* errorDesctiprionNode;

  mupnp_log_debug_l4("Entering...\n");

  /**** <s:Fault> ****/
  faultNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(faultNode, MUPNP_SOAP_XMLNS MUPNP_SOAP_DELIM MUPNP_SOAP_FAULT);

  /**** <faultcode>s:Client</faultcode> ****/
  faultCodeNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(faultCodeNode, MUPNP_SOAP_FAULT_CODE);
  mupnp_xml_node_setvalue(faultCodeNode, MUPNP_SOAP_XMLNS MUPNP_SOAP_DELIM MUPNP_CONTROL_FAULT_CODE);
  mupnp_xml_node_addchildnode(faultNode, faultCodeNode);

  /**** <faultstring>UPnPError</faultstring> ****/
  faultStringNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(faultStringNode, MUPNP_SOAP_FAULT_STRING);
  mupnp_xml_node_setvalue(faultStringNode, MUPNP_CONTROL_FAULT_STRING);
  mupnp_xml_node_addchildnode(faultNode, faultStringNode);

  /**** <detail> ****/
  detailNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(detailNode, MUPNP_SOAP_DETAIL);
  mupnp_xml_node_addchildnode(faultNode, detailNode);

  /**** <UPnPError xmlns="urn:schemas-upnp-org:control-1-0"> ****/
  upnpErrorNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(upnpErrorNode, MUPNP_CONTROL_FAULT_STRING);
  /**** Thanks for Visa Smolander (09/11/2005) ****/
  mupnp_xml_node_setattribute(upnpErrorNode, MUPNP_SOAP_ATTRIBUTE_XMLNS, MUPNP_CONTROL_XMLNS);
  mupnp_xml_node_addchildnode(detailNode, upnpErrorNode);

  /**** <errorCode>error code</errorCode> ****/
  errorCodeNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(errorCodeNode, MUPNP_CONTROL_ERROR_CODE);
  mupnp_xml_node_setintvalue(errorCodeNode, errCode);
  mupnp_xml_node_addchildnode(upnpErrorNode, errorCodeNode);

  /**** <errorDescription>error string</errorDescription> ****/
  errorDesctiprionNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(errorDesctiprionNode, MUPNP_CONTROL_ERROR_DESCRIPTION);
  mupnp_xml_node_setvalue(errorDesctiprionNode, errDescr);
  mupnp_xml_node_addchildnode(upnpErrorNode, errorDesctiprionNode);

  return faultNode;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_soap_response_setfaultresponse
****************************************/

void mupnp_control_soap_response_setfaultresponse(mUpnpSoapResponse* soapRes, int errCode, char* errDescr)
{
  mUpnpHttpResponse* httpRes;
  mUpnpXmlNode* bodyNode;
  mUpnpXmlNode* faultNode;
  mUpnpXmlNode* envNode;

  mupnp_log_debug_l4("Entering...\n");

  httpRes = mupnp_soap_response_gethttpresponse(soapRes);

  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_INTERNAL_SERVER_ERROR);
  mupnp_control_soap_response_initializeenvelopenode(soapRes);

  bodyNode = mupnp_soap_response_getbodynode(soapRes);
  faultNode = mupnp_control_soap_response_createfaultresponsenode(errCode, errDescr);
  mupnp_xml_node_addchildnode(bodyNode, faultNode);

  envNode = mupnp_soap_response_getenvelopenode(soapRes);
  mupnp_soap_response_setcontent(soapRes, envNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_request_sethostfromservice
****************************************/

void mupnp_control_request_sethostfromservice(mUpnpSoapRequest* soapReq, mUpnpService* service)
{
  mUpnpHttpRequest* httpReq;
  mUpnpNetURL* ctrlURL;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = mupnp_soap_request_gethttprequest(soapReq);

  ctrlURL = mupnp_service_getcontrolurl(service);

  mupnp_log_debug_s("Ctrl URL: %s - %d -%s",
      mupnp_net_url_gethost(ctrlURL),
      mupnp_net_url_getport(ctrlURL),
      mupnp_net_url_getpath(ctrlURL));

  //mupnp_http_request_seturi(httpReq, mupnp_xml_node_getchildnodevalue(mupnp_service_getservicenode(service), MUPNP_SERVICE_CONTROL_URL));
  mupnp_http_request_seturi(httpReq, mupnp_net_url_getrequest(ctrlURL));

  /**** Host ****/
  mupnp_net_url_delete(httpReq->postURL);
  httpReq->postURL = ctrlURL;

  mupnp_log_debug_s("Post URL: %s - %d -%s",
      mupnp_net_url_gethost(httpReq->postURL),
      mupnp_net_url_getport(httpReq->postURL),
      mupnp_net_url_getpath(httpReq->postURL));

  mupnp_log_debug_l4("Leaving...\n");
}

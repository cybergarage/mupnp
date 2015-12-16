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

#include <mupnp/upnp_limit.h>
#include <mupnp/service.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_QUERYCTRL)

/****************************************
* mupnp_control_query_response_new
****************************************/

mUpnpQueryResponse* mupnp_control_query_response_new()
{
  mUpnpQueryResponse* queryRes;

  mupnp_log_debug_l4("Entering...\n");

  queryRes = (mUpnpQueryResponse*)malloc(sizeof(mUpnpQueryResponse));

  if (NULL != queryRes) {
    queryRes->soapRes = mupnp_soap_response_new();
    queryRes->isSoapResCreated = true;
  }

  return queryRes;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_delete
****************************************/

void mupnp_control_query_response_delete(mUpnpQueryResponse* queryRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_control_query_response_clear(queryRes);

  if (queryRes->isSoapResCreated == true)
    mupnp_soap_response_delete(queryRes->soapRes);

  free(queryRes);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_clear
****************************************/

void mupnp_control_query_response_clear(mUpnpQueryResponse* queryRes)
{
  mupnp_log_debug_l4("Entering...\n");

  if (queryRes->isSoapResCreated == true)
    mupnp_soap_response_delete(queryRes->soapRes);
  queryRes->soapRes = mupnp_soap_response_new();
  queryRes->isSoapResCreated = true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_request_setsoaprequest
****************************************/

void mupnp_control_query_response_setsoapresponse(mUpnpQueryResponse* queryRes, mUpnpSoapResponse* soapRes)
{
  mupnp_log_debug_l4("Entering...\n");

  if (queryRes->isSoapResCreated == true)
    mupnp_soap_response_delete(queryRes->soapRes);
  queryRes->soapRes = soapRes;
  queryRes->isSoapResCreated = false;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_createresponsenode
****************************************/

static mUpnpXmlNode* mupnp_control_query_response_createresponsenode(mUpnpStateVariable* statVar)
{
  mUpnpXmlNode* queryResNode;
  mUpnpXmlNode* returnNode;

  mupnp_log_debug_l4("Entering...\n");

  queryResNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(queryResNode, MUPNP_CONTROL_NS MUPNP_SOAP_DELIM MUPNP_CONTROL_QUERY_STATE_VARIABLE_RESPONSE);
  mupnp_xml_node_setnamespace(queryResNode, MUPNP_CONTROL_NS, MUPNP_CONTROL_XMLNS);

  returnNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(returnNode, MUPNP_CONTROL_RETURN);
  mupnp_xml_node_setvalue(returnNode, mupnp_statevariable_getvalue(statVar));
  mupnp_xml_node_addchildnode(queryResNode, returnNode);

  return queryResNode;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_setresponse
****************************************/

void mupnp_control_query_response_setresponse(mUpnpQueryResponse* queryRes, mUpnpStateVariable* statVar)
{
  mUpnpSoapResponse* soapRes;
  mUpnpHttpResponse* httpRes;
  mUpnpXmlNode* bodyNode;
  mUpnpXmlNode* resNode;
  mUpnpXmlNode* envNode;

  mupnp_log_debug_l4("Entering...\n");

  soapRes = mupnp_control_query_response_getsoapresponse(queryRes);
  httpRes = mupnp_soap_response_gethttpresponse(soapRes);

  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_control_soap_response_initializeenvelopenode(soapRes);

  bodyNode = mupnp_soap_response_getbodynode(soapRes);
  resNode = mupnp_control_query_response_createresponsenode(statVar);
  mupnp_xml_node_addchildnode(bodyNode, resNode);

  envNode = mupnp_soap_response_getenvelopenode(soapRes);
  mupnp_soap_response_setcontent(soapRes, envNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_setresponse
****************************************/

mUpnpXmlNode* mupnp_control_query_response_getreturnnode(mUpnpQueryResponse* queryRes)
{
  mUpnpSoapResponse* soapRes;
  mUpnpXmlNode* bodyNode;
  mUpnpXmlNode* queryResNode;

  mupnp_log_debug_l4("Entering...\n");

  soapRes = mupnp_control_query_response_getsoapresponse(queryRes);

  bodyNode = mupnp_soap_response_getbodynode(soapRes);
  if (bodyNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(bodyNode) == false)
    return NULL;

  queryResNode = mupnp_xml_node_getchildnodes(bodyNode);
  if (queryResNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(queryResNode) == false)
    return NULL;

  return mupnp_xml_node_getchildnodes(queryResNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_query_response_setresponse
****************************************/

char* mupnp_control_query_response_getreturnvalue(mUpnpQueryResponse* queryRes)
{
  mUpnpXmlNode* returnNode;

  mupnp_log_debug_l4("Entering...\n");

  returnNode = mupnp_control_query_response_getreturnnode(queryRes);
  if (returnNode == NULL)
    return "";
  return mupnp_xml_node_getvalue(returnNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_QUERY (End)
****************************************/

#endif

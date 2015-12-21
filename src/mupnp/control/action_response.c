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
* MUPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

/****************************************
* mupnp_control_action_response_new
****************************************/

mUpnpActionResponse* mupnp_control_action_response_new()
{
  mUpnpActionResponse* actionRes;

  mupnp_log_debug_l4("Entering...\n");

  actionRes = (mUpnpActionResponse*)malloc(sizeof(mUpnpActionResponse));

  if (NULL != actionRes) {
    actionRes->soapRes = mupnp_soap_response_new();
    actionRes->isSoapResCreated = true;

    actionRes->argList = mupnp_argumentlist_new();
  }

  return actionRes;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_delete
****************************************/

void mupnp_control_action_response_delete(mUpnpActionResponse* actionRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_control_action_response_clear(actionRes);

  if (actionRes->isSoapResCreated == true)
    mupnp_soap_response_delete(actionRes->soapRes);

  mupnp_argumentlist_delete(actionRes->argList);

  free(actionRes);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_clear
****************************************/

void mupnp_control_action_response_clear(mUpnpActionResponse* actionRes)
{
  mupnp_log_debug_l4("Entering...\n");

  if (actionRes->isSoapResCreated == true)
    mupnp_soap_response_delete(actionRes->soapRes);
  actionRes->soapRes = mupnp_soap_response_new();
  actionRes->isSoapResCreated = true;

  mupnp_argumentlist_clear(actionRes->argList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_setsoapresponse
****************************************/

void mupnp_control_action_response_setsoapresponse(mUpnpActionResponse* actionRes, mUpnpSoapResponse* soapRes)
{
  mupnp_log_debug_l4("Entering...\n");

  if (actionRes->isSoapResCreated == true)
    mupnp_soap_response_delete(actionRes->soapRes);
  actionRes->soapRes = soapRes;
  actionRes->isSoapResCreated = false;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_createresponsenode
****************************************/

static mUpnpXmlNode* mupnp_control_action_response_createresponsenode(mUpnpAction* action)
{
  mUpnpXmlNode* actionNameResNode;
  char nodeName[MUPNP_ACTOINNAME_LEN_MAX + sizeof(MUPNP_SOAP_METHODNS) + sizeof(MUPNP_SOAP_DELIM) + sizeof(MUPNP_SOAP_RESPONSE) + 1];
  char attrName[sizeof(MUPNP_SOAP_ATTRIBUTE_XMLNS) + sizeof(MUPNP_SOAP_DELIM) + sizeof(MUPNP_SOAP_METHODNS) + 1];
  const char* actionName;
  mUpnpXmlNode* serviceNode;
  mUpnpService* service;
  mUpnpArgumentList* argList;
  mUpnpArgument* arg;
  mUpnpXmlNode* argNode;

  mupnp_log_debug_l4("Entering...\n");

  actionNameResNode = mupnp_xml_node_new();

  /* action name */
  actionName = mupnp_action_getname(action);
  mupnp_strcpy(nodeName, MUPNP_SOAP_METHODNS);
  mupnp_strcat(nodeName, MUPNP_SOAP_DELIM);
  mupnp_strncat(nodeName, actionName, MUPNP_ACTOINNAME_LEN_MAX);
  mupnp_strcat(nodeName, MUPNP_SOAP_RESPONSE);
  mupnp_xml_node_setname(actionNameResNode, nodeName);

  /* service attribute */
  serviceNode = mupnp_service_getservicenode(mupnp_action_getservice(action));
  if (serviceNode != NULL) {
    service = mupnp_service_new();
    mupnp_service_setservicenode(service, serviceNode);
    mupnp_strcpy(attrName, MUPNP_SOAP_ATTRIBUTE_XMLNS);
    mupnp_strcat(attrName, MUPNP_SOAP_DELIM);
    mupnp_strcat(attrName, MUPNP_SOAP_METHODNS);
    mupnp_xml_node_setattribute(actionNameResNode, attrName, mupnp_service_getservicetype(service));
    mupnp_service_delete(service);
  }

  /* arguments */
  argList = mupnp_action_getargumentlist(action);
  for (arg = mupnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isoutdirection(arg) == false)
      continue;
    argNode = mupnp_xml_node_new();
    mupnp_xml_node_setname(argNode, mupnp_argument_getname(arg));
    mupnp_xml_node_setvalue(argNode, mupnp_argument_getvalue(arg));
    mupnp_xml_node_addchildnode(actionNameResNode, argNode);
  }

  return actionNameResNode;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_setresponse
****************************************/

void mupnp_control_action_response_setresponse(mUpnpActionResponse* actionRes, mUpnpAction* action)
{
  mUpnpSoapResponse* soapRes;
  mUpnpHttpResponse* httpRes;
  mUpnpXmlNode* bodyNode;
  mUpnpXmlNode* resNode;
  mUpnpXmlNode* envNode;

  mupnp_log_debug_l4("Entering...\n");

  soapRes = mupnp_control_action_response_getsoapresponse(actionRes);
  httpRes = mupnp_soap_response_gethttpresponse(soapRes);

  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_control_soap_response_initializeenvelopenode(soapRes);

  bodyNode = mupnp_soap_response_getbodynode(soapRes);
  resNode = mupnp_control_action_response_createresponsenode(action);
  mupnp_xml_node_addchildnode(bodyNode, resNode);

  envNode = mupnp_soap_response_getenvelopenode(soapRes);
  mupnp_soap_response_setcontent(soapRes, envNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_getactionresponsenode
****************************************/

mUpnpXmlNode* mupnp_control_action_response_getactionresponsenode(mUpnpActionResponse* actionRes)
{
  mUpnpSoapResponse* soapRes;
  mUpnpXmlNode* bodyNode;

  mupnp_log_debug_l4("Entering...\n");

  soapRes = mupnp_control_action_response_getsoapresponse(actionRes);

  bodyNode = mupnp_soap_response_getbodynode(soapRes);

  if (bodyNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(bodyNode) == false)
    return NULL;

  return mupnp_xml_node_getchildnodes(bodyNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_getresult
****************************************/

bool mupnp_control_action_response_getresult(mUpnpActionResponse* actionRes, mUpnpAction* action)
{
  mUpnpXmlNode* resNode;
  mUpnpXmlNode* argNode;
  char* argName;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  resNode = mupnp_control_action_response_getactionresponsenode(actionRes);
  if (resNode == NULL)
    return false;

  for (argNode = mupnp_xml_node_getchildnodes(resNode); argNode != NULL; argNode = mupnp_xml_node_next(argNode)) {
    argName = mupnp_xml_node_getname(argNode);
    arg = mupnp_action_getargumentbyname(action, argName);
    if (arg == NULL)
      continue;
    mupnp_argument_setvalue(arg, mupnp_xml_node_getvalue(argNode));
  }

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_response_geterror
****************************************/
bool mupnp_control_action_response_geterror(mUpnpActionResponse* actionRes, mUpnpAction* action)
{
  mUpnpXmlNode* resNode;
  mUpnpXmlNode* upnpErrorNode;
  mUpnpXmlNode* node;
  char* errDesc = NULL;
  char* errCode = NULL;

  mupnp_log_debug_l4("Entering...\n");

  resNode = mupnp_control_action_response_getactionresponsenode(actionRes);
  if (resNode == NULL) {
    return false;
  }

  /* Response node is FAULT node, there will be no output args,
	   but set action status and description */
  upnpErrorNode = mupnp_xml_node_getchildnodewithnamespace(resNode, MUPNP_SOAP_DETAIL, NULL, true);

  if (upnpErrorNode == NULL)
    return false;

  upnpErrorNode = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, MUPNP_CONTROL_FAULT_STRING, NULL, true);

  if (upnpErrorNode == NULL)
    return false;

  node = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, MUPNP_CONTROL_ERROR_DESCRIPTION, NULL, true);
  if (node)
    errDesc = mupnp_xml_node_getvalue(node);

  node = mupnp_xml_node_getchildnodewithnamespace(upnpErrorNode, MUPNP_CONTROL_ERROR_CODE, NULL, true);
  if (node)
    errCode = mupnp_xml_node_getvalue(node);

  if (errCode == NULL)
    return false;

  mupnp_action_setstatusdescription(action, errDesc);
  mupnp_action_setstatuscode(action, mupnp_str2int(errCode));

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

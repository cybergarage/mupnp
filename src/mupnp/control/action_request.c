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
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

/****************************************
* mupnp_control_action_request_new
****************************************/

mUpnpActionRequest* mupnp_control_action_request_new()
{
  mUpnpActionRequest* actionReq;

  mupnp_log_debug_l4("Entering...\n");

  actionReq = (mUpnpActionRequest*)malloc(sizeof(mUpnpActionRequest));

  if (NULL != actionReq) {
    actionReq->soapReq = mupnp_soap_request_new();
    actionReq->isSoapReqCreated = true;
    actionReq->actionRes = mupnp_control_action_response_new();

    actionReq->argList = mupnp_argumentlist_new();
  }

  mupnp_log_debug_l4("Leaving...\n");

  return actionReq;
}

/****************************************
* mupnp_control_action_request_delete
****************************************/

void mupnp_control_action_request_delete(mUpnpActionRequest* actionReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_control_action_request_clear(actionReq);

  if (actionReq->isSoapReqCreated == true)
    mupnp_soap_request_delete(actionReq->soapReq);

  mupnp_control_action_response_delete(actionReq->actionRes);

  mupnp_argumentlist_delete(actionReq->argList);

  free(actionReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_request_clear
****************************************/

void mupnp_control_action_request_clear(mUpnpActionRequest* actionReq)
{
  mupnp_log_debug_l4("Entering...\n");

  if (actionReq->isSoapReqCreated == true)
    mupnp_soap_request_delete(actionReq->soapReq);
  actionReq->soapReq = mupnp_soap_request_new();
  actionReq->isSoapReqCreated = true;

  mupnp_argumentlist_clear(actionReq->argList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_request_setsoaprequest
****************************************/

void mupnp_control_action_request_setsoaprequest(mUpnpActionRequest* actionReq, mUpnpSoapRequest* soapReq)
{
  mUpnpXmlNode* actionNode;
  mUpnpXmlNode* argNode;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  if (actionReq->isSoapReqCreated == true)
    mupnp_soap_request_delete(actionReq->soapReq);
  actionReq->soapReq = soapReq;
  actionReq->isSoapReqCreated = false;

  mupnp_argumentlist_clear(actionReq->argList);

  actionNode = mupnp_control_action_request_getactionnode(actionReq);
  if (actionNode == NULL)
    return;

  for (argNode = mupnp_xml_node_getchildnodes(actionNode); argNode != NULL; argNode = mupnp_xml_node_next(argNode)) {
    arg = mupnp_argument_new();
    mupnp_argument_setargumentnode(arg, argNode);
    mupnp_argument_setname(arg, mupnp_xml_node_getname(argNode));
    mupnp_argument_setvalue(arg, mupnp_xml_node_getvalue(argNode));
    mupnp_argumentlist_add(actionReq->argList, arg);
  }

  mupnp_soap_request_createcontent(soapReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_request_getactionnode
****************************************/

mUpnpXmlNode* mupnp_control_action_request_getactionnode(mUpnpActionRequest* actionReq)
{
  mUpnpSoapRequest* soapReq;
  mUpnpXmlNode* bodyNode;

  mupnp_log_debug_l4("Entering...\n");

  soapReq = mupnp_control_action_request_getsoaprequest(actionReq);

  bodyNode = mupnp_soap_request_getbodynode(soapReq);
  if (bodyNode == NULL)
    return NULL;

  if (mupnp_xml_node_haschildnodes(bodyNode) == false)
    return NULL;

  return mupnp_xml_node_getchildnodes(bodyNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_request_getactionname
****************************************/

char* mupnp_control_action_request_getactionname(mUpnpActionRequest* actionReq)
{
  mUpnpXmlNode* node;
  char* name;
  ssize_t urnDelimIdx;

  mupnp_log_debug_l4("Entering...\n");

  node = mupnp_control_action_request_getactionnode(actionReq);
  if (node == NULL)
    return "";

  name = mupnp_xml_node_getname(node);
  if (name == NULL)
    return "";

  urnDelimIdx = mupnp_strstr(name, MUPNP_HTTP_SOAP_URN_DELIM);
  if (urnDelimIdx < 0)
    return "";

  mupnp_log_debug_l4("Leaving...\n");

  return (name + urnDelimIdx + 1);
}

/****************************************
* mupnp_control_action_request_createactionnode
****************************************/

mUpnpXmlNode* mupnp_control_action_request_createactionnode(mUpnpAction* action)
{
  mUpnpService* service;
  mUpnpXmlNode* actionNode;
  mUpnpArgument* arg;
  mUpnpXmlNode* argNode;
  mUpnpString* nameWithNamespace;

  mupnp_log_debug_l4("Entering...\n");

  service = mupnp_action_getservice(action);

  actionNode = mupnp_xml_node_new();
  /**** Thanks for Visa Smolander (10/31/2005) ****/
  nameWithNamespace = mupnp_string_new();
  mupnp_string_addvalue(nameWithNamespace, MUPNP_CONTROL_NS ":");
  mupnp_string_addvalue(nameWithNamespace, mupnp_action_getname(action));
  mupnp_xml_node_setname(actionNode, mupnp_string_getvalue(nameWithNamespace));
  mupnp_string_delete(nameWithNamespace);
  mupnp_xml_node_setnamespace(actionNode, MUPNP_CONTROL_NS, mupnp_service_getservicetype(service));

  for (arg = mupnp_action_getarguments(action); arg; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isindirection(arg) == false)
      continue;
    argNode = mupnp_xml_node_new();
    mupnp_xml_node_setname(argNode, mupnp_argument_getname(arg));
    mupnp_xml_node_setvalue(argNode, mupnp_argument_getvalue(arg));
    mupnp_xml_node_addchildnode(actionNode, argNode);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return actionNode;
}

/****************************************
* mupnp_control_action_setrequest
****************************************/

void mupnp_control_action_request_setaction(mUpnpActionRequest* actionReq, mUpnpAction* action)
{
  mUpnpService* service;
  mUpnpSoapRequest* soapReq;
  mUpnpString* soapAction;
  mUpnpXmlNode* bodyNode;
  mUpnpXmlNode* contentNode;

  mupnp_log_debug_l4("Entering...\n");

  service = mupnp_action_getservice(action);
  soapReq = mupnp_control_action_request_getsoaprequest(actionReq);

  soapAction = mupnp_string_new();
  mupnp_string_addvalue(soapAction, "\"");
  mupnp_string_addvalue(soapAction, mupnp_service_getservicetype(service));
  mupnp_string_addvalue(soapAction, "#");
  mupnp_string_addvalue(soapAction, mupnp_action_getname(action));
  mupnp_string_addvalue(soapAction, "\"");
  mupnp_soap_request_setsoapaction(soapReq, mupnp_string_getvalue(soapAction));
  mupnp_string_delete(soapAction);

  mupnp_control_request_sethostfromservice(soapReq, service);

  mupnp_control_soap_request_initializeenvelopenode(soapReq);
  bodyNode = mupnp_soap_request_getbodynode(soapReq);
  contentNode = mupnp_control_action_request_createactionnode(action);
  mupnp_xml_node_addchildnode(bodyNode, contentNode);

  mupnp_soap_request_createcontent(soapReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_control_action_request_post
****************************************/

mUpnpActionResponse* mupnp_control_action_request_post(mUpnpActionRequest* actionReq)
{
  mUpnpSoapRequest* soapReq;
  mUpnpSoapResponse* soapRes;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* httpReq;
  mUpnpNetURL* postURL;

  mupnp_log_debug_l4("Entering...\n");

  soapReq = mupnp_control_action_request_getsoaprequest(actionReq);
  soapRes = mupnp_soap_request_getsoapresponse(soapReq);
  actionRes = mupnp_control_action_request_getactionresponse(actionReq);
  httpReq = mupnp_soap_request_gethttprequest(soapReq);
  postURL = mupnp_http_request_getposturl(httpReq);

  mupnp_control_action_response_setsoapresponse(actionRes, soapRes);

  mupnp_soap_request_post(
      soapReq,
      mupnp_net_url_gethost(postURL),
      mupnp_net_url_getport(postURL));

  mupnp_log_debug_l4("Leaving...\n");

  return actionRes;
}

/****************************************
* MUPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

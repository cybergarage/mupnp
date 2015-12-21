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

#include <mupnp/event/notify.h>
#include <mupnp/event/event.h>
#include <mupnp/event/subscriber.h>
#include <mupnp/event/property.h>
#include <mupnp/control/control.h>
#include <mupnp/statevariable.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* Static Function Prototype
****************************************/

static mUpnpXmlNode* mupnp_event_notify_request_createpropertysetnode(mUpnpService* service, mUpnpStateVariable* statVar);

#define mupnp_event_notify_request_getpropertylistonly(notifyReq) ((mUpnpPropertyList*)mupnp_soap_request_getuserdata(notifyReq))

/****************************************
* mupnp_event_notify_request_new
****************************************/

mUpnpNotifyRequest* mupnp_event_notify_request_new()
{
  mUpnpNotifyRequest* notifyReq;
  mUpnpPropertyList* propList;

  mupnp_log_debug_l4("Entering...\n");

  notifyReq = mupnp_soap_request_new();

  propList = mupnp_propertylist_new();
  mupnp_event_notify_request_setpropertylist(notifyReq, propList);

  mupnp_log_debug_l4("Leaving...\n");

  return notifyReq;
}

/****************************************
* mupnp_event_notify_request_delete
****************************************/

void mupnp_event_notify_request_delete(mUpnpNotifyRequest* notifyReq)
{
  mUpnpPropertyList* propList;

  mupnp_log_debug_l4("Entering...\n");

  propList = mupnp_event_notify_request_getpropertylistonly(notifyReq);
  mupnp_propertylist_delete(propList);

  mupnp_soap_request_delete(notifyReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_notify_request_clear
****************************************/

void mupnp_event_notify_request_clear(mUpnpNotifyRequest* notifyReq)
{
  mUpnpPropertyList* propList;

  mupnp_log_debug_l4("Entering...\n");

  propList = mupnp_event_notify_request_getpropertylistonly(notifyReq);
  mupnp_propertylist_clear(propList);

  mupnp_soap_request_clear(notifyReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_notify_request_setsid
****************************************/

void mupnp_event_notify_request_setsid(mUpnpNotifyRequest* soapReq, char* sid)
{
  char buf[MUPNP_SUBSCRIPTION_SID_HEADER_SIZE];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SID, mupnp_event_subscription_tosidheaderstring(sid, buf, sizeof(buf)));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_notify_request_setpropertysetnode
****************************************/
bool mupnp_event_notify_request_setpropertysetnode(mUpnpNotifyRequest* notifyReq, mUpnpSubscriber* sub, /* mUpnpService */ void* pservice, mUpnpStateVariable* statVar)
{
  mUpnpHttpRequest* httpReq;
  mUpnpXmlNode* propSetNode;
  mUpnpService* service;
  char server[MUPNP_SEVERNAME_MAXLEN];

  mupnp_log_debug_l4("Entering...\n");

  service = (mUpnpService*)pservice;

  httpReq = mupnp_soap_request_gethttprequest(notifyReq);

  mupnp_http_request_setmethod(httpReq, MUPNP_HTTP_NOTIFY);
  mupnp_http_request_setconnection(httpReq, MUPNP_HTTP_CLOSE);
  mupnp_http_request_seturi(httpReq, mupnp_subscriber_getdeliverypath(sub));
  mupnp_http_request_sethost(httpReq, mupnp_subscriber_getdeliveryhost(sub), mupnp_subscriber_getdeliveryport(sub));
  mupnp_getservername(server, sizeof(server));
  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq,
      MUPNP_HTTP_SERVER,
      server);
  mupnp_event_notify_request_setnt(notifyReq, MUPNP_NT_EVENT);
  mupnp_event_notify_request_setnts(notifyReq, MUPNP_NTS_PROPCHANGE);
  mupnp_event_notify_request_setsid(notifyReq, mupnp_subscriber_getsid(sub));
  mupnp_event_notify_request_setseq(notifyReq, mupnp_subscriber_getnotifycount(sub));

  propSetNode = mupnp_event_notify_request_createpropertysetnode(service, statVar);
  mupnp_soap_request_setcontent(notifyReq, propSetNode);
  mupnp_xml_node_delete(propSetNode);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_event_notify_request_createpropertysetnode
****************************************/

static mUpnpXmlNode* mupnp_event_notify_request_createpropertysetnode(mUpnpService* service, mUpnpStateVariable* statVar)
{
  mUpnpXmlNode* propSetNode;
  mUpnpXmlNode* propNode;
  mUpnpXmlNode* varNode;

  mupnp_log_debug_l4("Entering...\n");

  propSetNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(propSetNode, MUPNP_NOTIFY_XMLNS MUPNP_SOAP_DELIM MUPNP_NOTIFY_PROPERTYSET);
  mupnp_xml_node_setnamespace(propSetNode, MUPNP_NOTIFY_XMLNS, MUPNP_SUBSCRIPTION_XMLNS);

  if (service) {
    for (statVar = mupnp_service_getstatevariables(service); statVar != NULL; statVar = mupnp_statevariable_next(statVar)) {
      if (!mupnp_statevariable_issendevents(statVar))
        continue;
      propNode = mupnp_xml_node_new();
      if (!propNode)
        continue;
      mupnp_xml_node_setname(propNode, MUPNP_NOTIFY_XMLNS MUPNP_SOAP_DELIM MUPNP_NOTIFY_PROPERTY);
      mupnp_xml_node_addchildnode(propSetNode, propNode);
      varNode = mupnp_xml_node_new();
      if (!varNode) {
        mupnp_xml_node_delete(propNode);
        continue;
      }
      mupnp_xml_node_setname(varNode, mupnp_statevariable_getname(statVar));
      mupnp_xml_node_setvalue(varNode, mupnp_statevariable_getvalue(statVar));
      mupnp_xml_node_addchildnode(propNode, varNode);
    }
  }
  else if (statVar) {
    propNode = mupnp_xml_node_new();
    if (propNode) {
      mupnp_xml_node_setname(propNode, MUPNP_NOTIFY_XMLNS MUPNP_SOAP_DELIM MUPNP_NOTIFY_PROPERTY);
      mupnp_xml_node_addchildnode(propSetNode, propNode);
      varNode = mupnp_xml_node_new();
      if (varNode) {
        mupnp_xml_node_setname(varNode, mupnp_statevariable_getname(statVar));
        mupnp_xml_node_setvalue(varNode, mupnp_statevariable_getvalue(statVar));
        mupnp_xml_node_addchildnode(propNode, varNode);
      }
      else
        mupnp_xml_node_delete(propNode);
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return propSetNode;
}

/****************************************
* mupnp_event_notify_request_getvariablenode
****************************************/

mUpnpXmlNode* mupnp_event_notify_request_getvariablenode(mUpnpNotifyRequest* nofityReq)
{
  mUpnpXmlNode* propSetNode;
  mUpnpXmlNode* propNode;

  mupnp_log_debug_l4("Entering...\n");

  propSetNode = mupnp_event_notify_request_getpropertysetnode(nofityReq);
  if (propSetNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(propSetNode) == false)
    return NULL;

  propNode = mupnp_xml_node_getchildnodes(propSetNode);
  if (propNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(propNode) == false)
    return NULL;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_xml_node_getchildnodes(propNode);
}

/****************************************
* mupnp_property_createfromnode
****************************************/

static mUpnpProperty* mupnp_property_createfromnode(mUpnpXmlNode* varNode)
{
  mUpnpProperty* prop;
  char* varName;
  char* varValue;
  ssize_t colonIdx;

  mupnp_log_debug_l4("Entering...\n");

  prop = mupnp_property_new();
  if (varNode == NULL)
    return prop;

  // remove the event namespace
  varName = mupnp_xml_node_getname(varNode);
  colonIdx = mupnp_strstr(varName, ":");
  if (0 <= colonIdx)
    varName = varName + colonIdx + 1;
  varValue = mupnp_xml_node_getvalue(varNode);
  mupnp_property_setname(prop, varName);
  mupnp_property_setvalue(prop, varValue);

  mupnp_log_debug_l4("Leaving...\n");

  return prop;
}

/****************************************
* mupnp_event_notify_request_getpropertylist
****************************************/

mUpnpPropertyList* mupnp_event_notify_request_getpropertylist(mUpnpNotifyRequest* notifyReq)
{
  mUpnpPropertyList* propList;
  mUpnpXmlNode* propSetNode;
  mUpnpXmlNode* propNode;
  mUpnpXmlNode* varNode;
  mUpnpProperty* prop;
  const char* sid;
  size_t seq;

  mupnp_log_debug_l4("Entering...\n");

  sid = mupnp_event_notify_request_getsid(notifyReq);
  seq = mupnp_event_notify_request_getseq(notifyReq);

  propList = mupnp_event_notify_request_getpropertylistonly(notifyReq);
  mupnp_propertylist_clear(propList);

  propSetNode = mupnp_soap_request_getrootnoode(notifyReq);
  if (propSetNode == NULL)
    return propList;

  for (propNode = mupnp_xml_node_getchildnodes(propSetNode); propNode != NULL; propNode = mupnp_xml_node_next(propNode)) {
    varNode = mupnp_xml_node_getchildnodes(propNode);
    prop = mupnp_property_createfromnode(varNode);
    mupnp_property_setsid(prop, sid);
    mupnp_property_setseq(prop, seq);
    mupnp_propertylist_add(propList, prop);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return propList;
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

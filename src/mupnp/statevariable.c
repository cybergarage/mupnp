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

#include <mupnp/statevariable.h>
#include <mupnp/service.h>
#include <mupnp/util/log.h>

#include <string.h>

/****************************************
* mupnp_statevariable_new
****************************************/

mUpnpStateVariable* mupnp_statevariable_new()
{
  mUpnpStateVariable* statVar;

  mupnp_log_debug_l4("Entering...\n");

  statVar = (mUpnpStateVariable*)malloc(sizeof(mUpnpStateVariable));

  if (NULL != statVar) {
    mupnp_list_node_init((mUpnpList*)statVar);

    statVar->parentService = NULL;
    statVar->stateVarNode = NULL;

    statVar->value = mupnp_string_new();
    statVar->upnpStatus = mupnp_status_new();
    statVar->allowedValueList = NULL;

    mupnp_statevariable_setlistener(statVar, NULL);
    mupnp_statevariable_setuserdata(statVar, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return statVar;
}

/****************************************
* mupnp_statevariable_delete
****************************************/

void mupnp_statevariable_delete(mUpnpStateVariable* statVar)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)statVar);

  mupnp_string_delete(statVar->value);
  mupnp_status_delete(statVar->upnpStatus);
  if (statVar->allowedValueList) {
    mupnp_list_remove((mUpnpList*)statVar->allowedValueList);
  }

  free(statVar);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_statevariable_setstatevariablenode
****************************************/

void mupnp_statevariable_setstatevariablenode(mUpnpStateVariable* statVar, mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  statVar->stateVarNode = node;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_statevariable_setvaluewithnotify
****************************************/

static void mupnp_statevariable_setvaluewithnotify(mUpnpStateVariable* statVar, const char* data, bool doNotify)
{
  mUpnpService* service;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(statVar->value, data);

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  /**** notify event ****/
  if (doNotify) {
    if (mupnp_statevariable_issendevents(statVar) == false)
      return;

    service = mupnp_statevariable_getservice(statVar);
    if (service == NULL)
      return;

    mupnp_service_notify(service, statVar);
  }
#endif

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_statevariable_setvalue
****************************************/

void mupnp_statevariable_setvalue(mUpnpStateVariable* statVar, const char* data)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_statevariable_setvaluewithnotify(statVar, data, true);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_statevariable_setvaluewithoutnotify
****************************************/

void mupnp_statevariable_setvaluewithoutnotify(mUpnpStateVariable* statVar, const char* data)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_statevariable_setvaluewithnotify(statVar, data, false);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_allowedvaluelist_new
 ****************************************/

mUpnpAllowedValuesList* mupnp_allowedvaluelist_new()
{
  mUpnpAllowedValuesList* allowedValueList = (mUpnpAllowedValuesList*)malloc(sizeof(mUpnpAllowedValuesList));

  if (NULL != allowedValueList) {
    mupnp_list_header_init((mUpnpList*)allowedValueList);
    allowedValueList->value = mupnp_string_new();
    return allowedValueList;
  }

  return NULL;
}

/****************************************
 * mupnp_statevariable_getallowedvaluelist
 ****************************************/
mUpnpAllowedValuesList* mupnp_statevariable_getallowedvaluelist(mUpnpStateVariable* statVar)
{
  if (!statVar->allowedValueList) {
    mUpnpXmlNode* allowedValuesNode = mupnp_xml_node_getchildnodebyname(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_ALLOWEDVALUELIST);

    if (allowedValuesNode) {
      mUpnpXmlNode* cnode;
      for (cnode = mupnp_xml_node_getchildnodes(allowedValuesNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        if (!statVar->allowedValueList) {
          statVar->allowedValueList = (mUpnpAllowedValuesList*)malloc(sizeof(mUpnpAllowedValuesList));
          mupnp_list_header_init((mUpnpList*)statVar->allowedValueList);
          statVar->allowedValueList->value = mupnp_string_new();
          mupnp_string_setvalue(statVar->allowedValueList->value, mupnp_xml_node_getvalue(cnode));
        }
        else {
          mUpnpAllowedValue* allowed_value;
          allowed_value = (mUpnpAllowedValue*)malloc(sizeof(mUpnpAllowedValue));
          mupnp_list_node_init((mUpnpList*)allowed_value);
          allowed_value->value = mupnp_string_new();
          mupnp_string_setvalue(allowed_value->value, mupnp_xml_node_getvalue(cnode));
          mupnp_list_insert((mUpnpList*)statVar->allowedValueList, (mUpnpList*)allowed_value);
        }
      }
    }
  }
  return statVar->allowedValueList;
}

/****************************************
 * mupnp_statevariable_is_allowed_value
 ****************************************/
int mupnp_statevariable_is_allowed_value(mUpnpStateVariable* statVar, const char* value)
{
  if (!statVar->allowedValueList) {
    mupnp_statevariable_getallowedvaluelist(statVar);
  }

  if (statVar->allowedValueList) {
    mUpnpAllowedValue* allowedValue;
    for (allowedValue = (mUpnpAllowedValue*)mupnp_list_next((mUpnpList*)statVar->allowedValueList); allowedValue != NULL; allowedValue = (mUpnpAllowedValue*)mupnp_list_next((mUpnpList*)allowedValue)) {
      if (mupnp_strcasecmp((char*)value, mupnp_string_getvalue(allowedValue->value)) == 0) {
        return 1;
      }
    }
  }
  return 0;
}

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

#include <mupnp/action.h>
#include <mupnp/util/log.h>

/****************************************
* prototype define for static functions
****************************************/

static void mupnp_action_initchildnodes(mUpnpAction* action);
static void mupnp_action_initargumentlist(mUpnpAction* action);

/****************************************
* mupnp_action_new
****************************************/

mUpnpAction* mupnp_action_new()
{
  mUpnpAction* action;

  mupnp_log_debug_l4("Entering...\n");

  action = (mUpnpAction*)malloc(sizeof(mUpnpAction));

  if (NULL != action) {
    mupnp_list_node_init((mUpnpList*)action);

    action->parentService = NULL;
    action->actionNode = NULL;

    action->argumentList = mupnp_argumentlist_new();

    action->upnpStatus = mupnp_status_new();
    action->listener = NULL;
    mupnp_action_setuserdata(action, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return action;
}

/****************************************
* mupnp_action_delete
****************************************/

void mupnp_action_delete(mUpnpAction* action)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_argumentlist_delete(action->argumentList);

  mupnp_status_delete(action->upnpStatus);

  mupnp_list_remove((mUpnpList*)action);
  free(action);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_action_setactionnode
****************************************/

void mupnp_action_setactionnode(mUpnpAction* action, mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  action->actionNode = node;
  mupnp_action_initchildnodes(action);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* mupnp_action_initchildnodes
****************************************/

static void mupnp_action_initchildnodes(mUpnpAction* action)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_action_initargumentlist(action);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_action_initargumentlist
****************************************/

static void mupnp_action_initargumentlist(mUpnpAction* action)
{
  mUpnpXmlNode* actionNode;
  mUpnpXmlNode* argumentListNode;
  mUpnpXmlNode* childNode;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_argumentlist_clear(action->argumentList);

  actionNode = mupnp_action_getactionnode(action);
  argumentListNode = mupnp_xml_node_getchildnode(actionNode, MUPNP_ARGUMENTLIST_ELEM_NAME);

  if (argumentListNode == NULL)
    return;

  for (childNode = mupnp_xml_node_getchildnodes(argumentListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {

    if (mupnp_argument_isargumentnode(childNode) == false)
      continue;

    arg = mupnp_argument_new();
    mupnp_argument_setargumentnode(arg, childNode);
    mupnp_argumentlist_add(action->argumentList, arg);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_action_getargumentbyname
****************************************/

mUpnpArgument* mupnp_action_getargumentbyname(mUpnpAction* action, const char* name)
{
  mUpnpArgumentList* argList;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(name) <= 0)
    return NULL;

  argList = mupnp_action_getargumentlist(action);
  for (arg = mupnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isname(arg, name) == true)
      return arg;
  }

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_action_getargumentvaluebyname
****************************************/

char* mupnp_action_getargumentvaluebyname(mUpnpAction* action, const char* name)
{
  mUpnpArgument* arg;

  arg = mupnp_action_getargumentbyname(action, name);
  if (!arg)
    return NULL;
  return mupnp_argument_getvalue(arg);
}

/****************************************
* mupnp_action_setargumentvaluebyname
****************************************/

bool mupnp_action_setargumentvaluebyname(mUpnpAction* action, const char* name, const char* value)
{
  mUpnpArgument* arg;

  arg = mupnp_action_getargumentbyname(action, name);
  if (!arg)
    return false;
  mupnp_argument_setvalue(arg, value);
  return true;
}

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

#ifndef _MUPNP_ACTION_H_
#define _MUPNP_ACTION_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#include <mupnp/argument.h>
#include <mupnp/upnp_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_ACTION_ELEM_NAME "action"
#define MUPNP_ACTIONLIST_ELEM_NAME "actionList"

#define MUPNP_ACTION_NAME "name"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpAction {
  MUPNP_LIST_STRUCT_MEMBERS

  void* parentService;
  mUpnpXmlNode* actionNode;
  mUpnpArgumentList* argumentList;
  /**** Execution Data ****/
  bool (*listener)(struct _mUpnpAction*);
  mUpnpStatus* upnpStatus;
  void* userData;
} mUpnpAction, mUpnpActionList;

typedef bool (*MUPNP_ACTION_LISTNER)(mUpnpAction*);

/****************************************************************************
 * Function (Action)
 ****************************************************************************/

/**
 * Create a new action
 *
 * @return mUpnpAction*
 */
mUpnpAction* mupnp_action_new();

/**
 * Destroy an action
 *
 * @param action The action to destroy
 */
void mupnp_action_delete(mUpnpAction* action);

/**
 * Get the next action in a list of actions. Use as an iterator.
 *
 * @param action The current action
 */
#define mupnp_action_next(action) (mUpnpAction*) mupnp_list_next((mUpnpList*)action)

/**
 * Check, whether the given XML node is an action node
 * 
 * @param node The mUpnpXmlNode*
 */
#define mupnp_action_isactionnode(node) mupnp_xml_node_isname(node, MUPNP_ACTION_ELEM_NAME)

/****************************************************************************
 * XML Node
 ****************************************************************************/

/**
 * Set the action's XML node (i.e. the XML representation of the action)
 *
 * @param action The action in question
 * @param node The mUpnpXmlNode*
 */
void mupnp_action_setactionnode(mUpnpAction* action, mUpnpXmlNode* node);

/**
 * Get the action's XML node (i.e. the XML representation of the action)
 *
 * @param action The action in question
 * @return mUpnpXmlNode*
 */
#define mupnp_action_getactionnode(action) (action->actionNode)

/****************************************************************************
 * Service
 ****************************************************************************/

/**
 * Set the action's parent service
 *
 * @param action The action in question
 * @param service The action's parent service
 */
#define mupnp_action_setservice(action, service) (action->parentService = service)

/**
 * Get the action's parent service
 *
 * @param action The action in question
 * @return mUpnpService*
 */
#define mupnp_action_getservice(action) ((mUpnpService*)action->parentService)

/****************************************************************************
 * Name
 ****************************************************************************/

/**
 * Set the action's name
 *
 * @param action The action in question
 * @param value The action's name
 */
#define mupnp_action_setname(action, value) mupnp_xml_node_setchildnode(mupnp_action_getactionnode(action), MUPNP_ACTION_NAME, value)

/**
 * Get the action's name
 *
 * @param action The action in question
 * @return char*
 */
#define mupnp_action_getname(action) mupnp_xml_node_getchildnodevalue(mupnp_action_getactionnode(action), MUPNP_ACTION_NAME)

/**
 * Check, whether the action's name matches the one given as parameter.
 *
 * @param action The action in question
 * @param name The name string to compare
 */
#define mupnp_action_isname(action, name) mupnp_streq(mupnp_action_getname(action), name)

/****************************************************************************
 * Argument
 ****************************************************************************/

/**
 * Get the action's list of arguments
 *
 * @param action The action in question
 * @return mUpnpArgumentList*
 */
#define mupnp_action_getargumentlist(action) (action->argumentList)

/**
 * Get the first argument from the action's list of arguments. Use as the
 * starting point in iteration loops.
 *
 * @param action The action in question
 * @return mUpnpArgument*
 */
#define mupnp_action_getarguments(action) ((mUpnpArgument*)mupnp_list_next((mUpnpList*)action->argumentList))

/**
 * Find an argument from the action by the argument's name.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @return mUpnpArgument* if successfull; otherwise NULL
 */
mUpnpArgument* mupnp_action_getargumentbyname(mUpnpAction* action, const char* name);

/**
 * Find out, whether the action has an argument by the given name
 *
 * @param action The action in question
 * @param name The name to look for
 * @return true if the action has the argument; otherwise false
 */
#define mupnp_action_hasargumentbyname(action, name) ((mupnp_action_getargumentbyname(action, name) != NULL) ? true : false)

/**
 * Get a value of the specified argument from the action by the argument's name directly.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @return char* if successfull; otherwise NULL
 */
char* mupnp_action_getargumentvaluebyname(mUpnpAction* action, const char* name);

/**
 * Set a value of the specified argument from the action by the argument's name directly.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @param value The value to set
 * @return true if successfull; otherwise NO
 */
bool mupnp_action_setargumentvaluebyname(mUpnpAction* action, const char* name, const char* value);

/****************************************************************************
 * Function (Action - Execution Data)
 ****************************************************************************/

/****************************************************************************
 * UPnPStatus
 ****************************************************************************/

/**
 * Set the action's latest status code
 *
 * @param action The action in question
 * @param code The status code (int)
 */
#define mupnp_action_setstatuscode(action, code) mupnp_status_setcode(action->upnpStatus, code)

/**
 * Get the action's latest status code
 *
 * @param action The action in question
 * @return int
 */
#define mupnp_action_getstatuscode(action) mupnp_status_getcode(action->upnpStatus)

/**
 * Set the action's latest status description
 *
 * @param action The action in question
 * @param value The status description string
 */
#define mupnp_action_setstatusdescription(action, value) mupnp_status_setdescription(action->upnpStatus, value)

/**
 * Get the action's latest status description
 *
 * @param action The action in question
 * @return char*
 */
#define mupnp_action_getstatusdescription(action) mupnp_status_getdescription(action->upnpStatus)

/****************************************************************************
 * Listener
 ****************************************************************************/

/**
 * Set the action's listener function
 *
 * @param action The action in question
 * @param func The listener function of type: bool (*MUPNP_ACTION_LISTNER)(mUpnpAction *)
 */
#define mupnp_action_setlistener(action, func) (action->listener = func)

/**
 * Get the action's listener function
 *
 * @param action The action in question
 * @return bool (*MUPNP_ACTION_LISTNER)(mUpnpAction *)
 */
#define mupnp_action_getlistener(action) (action->listener)

/**
 * @deprecated Compatibility macro for typo correction
 */
#define mupnp_action_setlistner mupnp_action_setlistener

/**
 * @deprecated Compatibility macro for typo correction
 */
#define mupnp_action_getlistner mupnp_action_getlistener

/****************************************************************************
 * User Data
 ****************************************************************************/

/**
 * Set the action's arbitrary user data pointer
 *
 * @param action The action in question
 * @param value The user data pointer (void*)
 */
#define mupnp_action_setuserdata(action, value) (action->userData = value)

/**
 * Get the action's arbitrary user data pointer
 *
 * @param action The action in question
 * @return void*
 */
#define mupnp_action_getuserdata(action) (action->userData)

/****************************************************************************
 * Function (ActionList)
 ****************************************************************************/

/**
 * Create a new list of actions
 *
 * @return mUpnpActionList*
 */
mUpnpActionList* mupnp_actionlist_new();

/**
 * Destroy a list of actions
 *
 * @param actionList The list of actions to destroy
 */
void mupnp_actionlist_delete(mUpnpActionList* actionList);

/**
 * Clear the contents of a list of actions
 *
 * @param actionList The list of actions to clear
 */
#define mupnp_actionlist_clear(actionList) mupnp_list_clear((mUpnpList*)actionList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_action_delete)

/**
 * Get the number of actions in a list of actions
 *
 * @param actionList The list in question
 * @return int
 */
#define mupnp_actionlist_size(actionList) mupnp_list_size((mUpnpList*)actionList)

/**
 * Get the first action in a list of actions. Use as the starting point in
 * iteration loops.
 *
 * @param actionList The list in question
 * @return mUpnpAction*
 */
#define mupnp_actionlist_gets(actionList) (mUpnpAction*) mupnp_list_next((mUpnpList*)actionList)

/**
 * Add an action to a list of actions
 *
 * @param actionList The list in question
 * @param action The action to add to the list
 */
#define mupnp_actionlist_add(actionList, action) mupnp_list_add((mUpnpList*)actionList, (mUpnpList*)action)

#ifdef __cplusplus
}
#endif

#endif

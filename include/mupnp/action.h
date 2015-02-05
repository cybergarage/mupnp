/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: caction.h
*
*	Revision:
*
*	02/23/05
*		- first revision
*
*	18-Jan-06 Heikki Junnila
*		- Added API comments
*
******************************************************************/

#ifndef _CG_UPNP_CACTION_H_
#define _CG_UPNP_CACTION_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#include <mupnp/argument.h>
#include <mupnp/upnp_status.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_ACTION_ELEM_NAME "action"
#define CG_UPNP_ACTIONLIST_ELEM_NAME "actionList"

#define CG_UPNP_ACTION_NAME "name"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpUpnpAction {
	BOOL headFlag;
	struct _mUpnpUpnpAction *prev;
	struct _mUpnpUpnpAction *next;
	void *parentService;
	mUpnpXmlNode *actionNode;
	mUpnpUpnpArgumentList *argumentList;
	/**** Execution Data ****/
	BOOL (*listener)(struct _mUpnpUpnpAction *);
	mUpnpUpnpStatus *upnpStatus;
	void *userData;
} mUpnpUpnpAction, mUpnpUpnpActionList;

typedef BOOL (*CG_UPNP_ACTION_LISTNER)(mUpnpUpnpAction *);

/****************************************************************************
 * Function (Action)
 ****************************************************************************/

/**
 * Create a new action
 *
 * @return mUpnpUpnpAction*
 */
mUpnpUpnpAction *mupnp_upnp_action_new();

/**
 * Destroy an action
 *
 * @param action The action to destroy
 */
void mupnp_upnp_action_delete(mUpnpUpnpAction *action);

/**
 * Get the next action in a list of actions. Use as an iterator.
 *
 * @param action The current action
 */
#define mupnp_upnp_action_next(action) (mUpnpUpnpAction *)mupnp_list_next((mUpnpList *)action)

/**
 * Check, whether the given XML node is an action node
 * 
 * @param node The mUpnpXmlNode*
 */
#define mupnp_upnp_action_isactionnode(node) mupnp_xml_node_isname(node, CG_UPNP_ACTION_ELEM_NAME)

/****************************************************************************
 * XML Node
 ****************************************************************************/

/**
 * Set the action's XML node (i.e. the XML representation of the action)
 *
 * @param action The action in question
 * @param node The mUpnpXmlNode*
 */
void mupnp_upnp_action_setactionnode(mUpnpUpnpAction *action, mUpnpXmlNode *node);

/**
 * Get the action's XML node (i.e. the XML representation of the action)
 *
 * @param action The action in question
 * @return mUpnpXmlNode*
 */
#define mupnp_upnp_action_getactionnode(action) (action->actionNode)

/****************************************************************************
 * Service
 ****************************************************************************/

/**
 * Set the action's parent service
 *
 * @param action The action in question
 * @param service The action's parent service
 */
#define mupnp_upnp_action_setservice(action, service) (action->parentService = service)

/**
 * Get the action's parent service
 *
 * @param action The action in question
 * @return mUpnpUpnpService*
 */
#define mupnp_upnp_action_getservice(action) ((mUpnpUpnpService *)action->parentService)

/****************************************************************************
 * Name
 ****************************************************************************/

/**
 * Set the action's name
 *
 * @param action The action in question
 * @param value The action's name
 */
#define mupnp_upnp_action_setname(action, value) mupnp_xml_node_setchildnode(mupnp_upnp_action_getactionnode(action), CG_UPNP_ACTION_NAME, value)

/**
 * Get the action's name
 *
 * @param action The action in question
 * @return char*
 */
#define mupnp_upnp_action_getname(action) mupnp_xml_node_getchildnodevalue(mupnp_upnp_action_getactionnode(action), CG_UPNP_ACTION_NAME)

/**
 * Check, whether the action's name matches the one given as parameter.
 *
 * @param action The action in question
 * @param name The name string to compare
 */
#define mupnp_upnp_action_isname(action, name) mupnp_streq(mupnp_upnp_action_getname(action), name)

/****************************************************************************
 * Argument
 ****************************************************************************/

/**
 * Get the action's list of arguments
 *
 * @param action The action in question
 * @return mUpnpUpnpArgumentList*
 */
#define mupnp_upnp_action_getargumentlist(action) (action->argumentList)

/**
 * Get the first argument from the action's list of arguments. Use as the
 * starting point in iteration loops.
 *
 * @param action The action in question
 * @return mUpnpUpnpArgument*
 */
#define mupnp_upnp_action_getarguments(action) ((mUpnpUpnpArgument *)mupnp_list_next((mUpnpList *)action->argumentList))

/**
 * Find an argument from the action by the argument's name.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @return mUpnpUpnpArgument* if successfull; otherwise NULL
 */
mUpnpUpnpArgument *mupnp_upnp_action_getargumentbyname(mUpnpUpnpAction *action, const char *name);

/**
 * Find out, whether the action has an argument by the given name
 *
 * @param action The action in question
 * @param name The name to look for
 * @return TRUE if the action has the argument; otherwise FALSE
 */
#define mupnp_upnp_action_hasargumentbyname(action,name) ((mupnp_upnp_action_getargumentbyname(action,name) != NULL) ? TRUE : FALSE)

/**
 * Get a value of the specified argument from the action by the argument's name directly.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @return char* if successfull; otherwise NULL
 */
char *mupnp_upnp_action_getargumentvaluebyname(mUpnpUpnpAction *action, const char *name);

/**
 * Set a value of the specified argument from the action by the argument's name directly.
 *
 * @param action The action in question
 * @param name The name of the argument to look for
 * @param value The value to set
 * @return TRUE if successfull; otherwise NO
 */
BOOL mupnp_upnp_action_setargumentvaluebyname(mUpnpUpnpAction *action, const char *name, const char *value);

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
#define mupnp_upnp_action_setstatuscode(action, code) mupnp_upnp_status_setcode(action->upnpStatus, code)

/**
 * Get the action's latest status code
 *
 * @param action The action in question
 * @return int
 */
#define mupnp_upnp_action_getstatuscode(action) mupnp_upnp_status_getcode(action->upnpStatus)

/**
 * Set the action's latest status description
 *
 * @param action The action in question
 * @param value The status description string
 */
#define mupnp_upnp_action_setstatusdescription(action, value) mupnp_upnp_status_setdescription(action->upnpStatus, value)

/**
 * Get the action's latest status description
 *
 * @param action The action in question
 * @return char*
 */
#define mupnp_upnp_action_getstatusdescription(action) mupnp_upnp_status_getdescription(action->upnpStatus)

/****************************************************************************
 * Listener
 ****************************************************************************/

/**
 * Set the action's listener function
 *
 * @param action The action in question
 * @param func The listener function of type: BOOL (*CG_UPNP_ACTION_LISTNER)(mUpnpUpnpAction *)
 */
#define mupnp_upnp_action_setlistener(action, func) (action->listener = func)

/**
 * Get the action's listener function
 *
 * @param action The action in question
 * @return BOOL (*CG_UPNP_ACTION_LISTNER)(mUpnpUpnpAction *)
 */
#define mupnp_upnp_action_getlistener(action) (action->listener)

/**
 * @deprecated Compatibility macro for typo correction
 */
#define mupnp_upnp_action_setlistner mupnp_upnp_action_setlistener

/**
 * @deprecated Compatibility macro for typo correction
 */
#define mupnp_upnp_action_getlistner mupnp_upnp_action_getlistener

/****************************************************************************
 * User Data
 ****************************************************************************/

/**
 * Set the action's arbitrary user data pointer
 *
 * @param action The action in question
 * @param value The user data pointer (void*)
 */
#define mupnp_upnp_action_setuserdata(action, value) (action->userData = value)

/**
 * Get the action's arbitrary user data pointer
 *
 * @param action The action in question
 * @return void*
 */
#define mupnp_upnp_action_getuserdata(action) (action->userData)

/****************************************************************************
 * Function (ActionList)
 ****************************************************************************/

/**
 * Create a new list of actions
 *
 * @return mUpnpUpnpActionList*
 */
mUpnpUpnpActionList *mupnp_upnp_actionlist_new();

/**
 * Destroy a list of actions
 *
 * @param actionList The list of actions to destroy
 */
void mupnp_upnp_actionlist_delete(mUpnpUpnpActionList *actionList);

/**
 * Clear the contents of a list of actions
 *
 * @param actionList The list of actions to clear
 */
#define mupnp_upnp_actionlist_clear(actionList) mupnp_list_clear((mUpnpList *)actionList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_action_delete)

/**
 * Get the number of actions in a list of actions
 *
 * @param actionList The list in question
 * @return int
 */
#define mupnp_upnp_actionlist_size(actionList) mupnp_list_size((mUpnpList *)actionList)

/**
 * Get the first action in a list of actions. Use as the starting point in
 * iteration loops.
 *
 * @param actionList The list in question
 * @return mUpnpUpnpAction*
 */
#define mupnp_upnp_actionlist_gets(actionList) (mUpnpUpnpAction *)mupnp_list_next((mUpnpList *)actionList)

/**
 * Add an action to a list of actions
 *
 * @param actionList The list in question
 * @param action The action to add to the list
 */
#define mupnp_upnp_actionlist_add(actionList, action) mupnp_list_add((mUpnpList *)actionList, (mUpnpList *)action)

#ifdef  __cplusplus
}
#endif

#endif

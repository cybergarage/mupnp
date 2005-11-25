/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: caction.h
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CACTION_H_
#define _CG_UPNP_CACTION_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>

#include <cybergarage/upnp/cargument.h>
#include <cybergarage/upnp/cupnp_status.h>

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

typedef struct _CgUpnpAction {
	BOOL headFlag;
	struct _CgUpnpAction *prev;
	struct _CgUpnpAction *next;
	void *parentService;
	CgXmlNode *actionNode;
	CgUpnpArgumentList *argumentList;
	/**** Execution Data ****/
	BOOL (*listener)(struct _CgUpnpAction *);
	CgUpnpStatus *upnpStatus;
	void *userData;
} CgUpnpAction, CgUpnpActionList;

typedef BOOL (*CG_UPNP_ACTION_LISTNER)(CgUpnpAction *);

/****************************************
* Function (Action)
****************************************/

CgUpnpAction *cg_upnp_action_new();
void cg_upnp_action_delete(CgUpnpAction *action);

#define cg_upnp_action_next(action) (CgUpnpAction *)cg_list_next((CgList *)action)

#define cg_upnp_action_isactionnode(node) cg_xml_node_isname(node, CG_UPNP_ACTION_ELEM_NAME)

/**** XML Node ****/
void cg_upnp_action_setactionnode(CgUpnpAction *action, CgXmlNode *node);
#define cg_upnp_action_getactionnode(action) (action->actionNode)

/**** Service ****/
#define cg_upnp_action_setservice(action, service) (action->parentService = service)
#define cg_upnp_action_getservice(action) ((CgUpnpService *)action->parentService)

/**** name ****/
#define cg_upnp_action_setname(action, value) cg_xml_node_setchildnode(cg_upnp_action_getactionnode(action), CG_UPNP_ACTION_NAME, value)
#define cg_upnp_action_getname(action) cg_xml_node_getchildnodevalue(cg_upnp_action_getactionnode(action), CG_UPNP_ACTION_NAME)
#define cg_upnp_action_isname(action, name) cg_streq(cg_upnp_action_getname(action), name)

/**** argument ****/
#define cg_upnp_action_getargumentlist(action) (action->argumentList)
#define cg_upnp_action_getarguments(action) ((CgUpnpArgument *)cg_list_next((CgList *)action->argumentList))
CgUpnpArgument *cg_upnp_action_getargumentbyname(CgUpnpAction *action, char *name);
#define cg_upnp_action_hasargumentbyname(action,name) ((cg_upnp_action_getargumentbyname(action,name) != NULL) ? TRUE : FALSE)

/****************************************
* Function (Action - Execution Data)
****************************************/

/**** UPnPStatus ****/
#define cg_upnp_action_setstatuscode(action, code) cg_upnp_status_setcode(action->upnpStatus, code)
#define cg_upnp_action_getstatuscode(action) cg_upnp_status_getcode(action->upnpStatus)
#define cg_upnp_action_setstatusdescription(action, value) cg_upnp_status_setdescription(action->upnpStatus, value)
#define cg_upnp_action_getstatusdescription(action) cg_upnp_status_getdescription(action->upnpStatus)

/**** Listner ****/
#define cg_upnp_action_setlistner(action, func) (action->listener = func)
#define cg_upnp_action_getlistner(action) (action->listener)

/**** User Data****/
#define cg_upnp_action_setuserdata(action, value) (action->userData = value)
#define cg_upnp_action_getuserdata(action) (action->userData)

/****************************************
* Function (ActionList)
****************************************/

CgUpnpActionList *cg_upnp_actionlist_new();
void cg_upnp_actionlist_delete(CgUpnpActionList *actionList);

#define cg_upnp_actionlist_clear(actionList) cg_list_clear((CgList *)actionList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_action_delete)
#define cg_upnp_actionlist_size(actionList) cg_list_size((CgList *)actionList)
#define cg_upnp_actionlist_gets(actionList) (CgUpnpAction *)cg_list_next((CgList *)actionList)
#define cg_upnp_actionlist_add(actionList, action) cg_list_add((CgList *)actionList, (CgList *)action)

#ifdef  __cplusplus
}
#endif

#endif

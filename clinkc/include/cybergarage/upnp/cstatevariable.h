/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cstatevariable.h
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CSTATEVARIABLE_H_
#define _CG_UPNP_CSTATEVARIABLE_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>

#include <cybergarage/upnp/callowedvalue.h>
#include <cybergarage/upnp/cupnp_status.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_STATEVARIABLE_ELEM_NAME "stateVariable"
#define CG_UPNP_SERVICESTATETALBE_ELEM_NAME "serviceStateTable"

#define CG_UPNP_STATEVARIABLE_NAME "name"
#define CG_UPNP_STATEVARIABLE_DATATYPE "dataType"
#define CG_UPNP_STATEVARIABLE_SENDEVENTS "sendEvents"
#define CG_UPNP_STATEVARIABLE_SENDEVENTS_YES "yes"
#define CG_UPNP_STATEVARIABLE_SENDEVENTS_NO "no"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpStateVariable {
	BOOL headFlag;
	struct _CgUpnpStateVariable *prev;
	struct _CgUpnpStateVariable *next;
	void *parentService;
	CgXmlNode *stateVarNode;
	/**** Child nodes ****/
	CgUpnpAllowedValueList *allowedValueList;
	CgUpnpAllowedValueRange *allowedValueRange;
	/**** Execution Data ****/
	CgString *value;
	BOOL (*listener)(struct _CgUpnpStateVariable *);
	CgUpnpStatus *upnpStatus;
	void *userData;
} CgUpnpStateVariable, CgUpnpServiceStateTable;

typedef BOOL (*CG_UPNP_STATEVARIABLE_LISTNER)(CgUpnpStateVariable *);

/****************************************
* Function (StateVariable)
****************************************/

CgUpnpStateVariable *cg_upnp_statevariable_new();
void cg_upnp_statevariable_delete(CgUpnpStateVariable *statVar);

#define cg_upnp_statevariable_next(statVar) (CgUpnpStateVariable *)cg_list_next((CgList *)statVar)

#define cg_upnp_statevariable_isstatevariablenode(node) cg_xml_node_isname(node, CG_UPNP_STATEVARIABLE_ELEM_NAME)

/**** XML Node ****/
void cg_upnp_statevariable_setstatevariablenode(CgUpnpStateVariable *statVar, CgXmlNode *node);
#define cg_upnp_statevariable_getstatevariablenode(statVar) (statVar->stateVarNode)

/**** Service ****/
#define cg_upnp_statevariable_setservice(statVar, service) (statVar->parentService = service)
#define cg_upnp_statevariable_getservice(statVar) ((CgUpnpService *)statVar->parentService)

/**** allowedValueList ****/
#define cg_upnp_statevariable_getallowedvaluelist(statVar) (statVar->allowedValueList)
#define cg_upnp_statevariable_hasallowedvaluelist(statVar) cg_upnp_allowedvaluelist_size(statVar->allowedValueList)

/**** allowedValueRange ****/
#define cg_upnp_statevariable_getallowedvaluerange(statVar) (statVar->allowedValueRange)
#define cg_upnp_statevariable_hasallowedvaluerange(statVar) ((statVar->allowedValueRange != NULL) ? TRUE : FALSE)

/**** name ****/
#define cg_upnp_statevariable_setname(statVar, value) cg_xml_node_setchildnode(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_NAME, value)
#define cg_upnp_statevariable_getname(statVar) cg_xml_node_getchildnodevalue(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_NAME)
#define  cg_upnp_statevariable_isname(statVar, name) cg_streq(cg_upnp_statevariable_getname(statVar), name)

/**** dataType ****/
#define cg_upnp_statevariable_setdatatype(statVar, value) cg_xml_node_setchildnode(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_DATATYPE, value)
#define cg_upnp_statevariable_getdatatype(statVar) cg_xml_node_getchildnodevalue(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_DATATYPE)

/**** sendEvents ****/
#define cg_upnp_statevariable_setsendevents(statVar, value) cg_xml_node_setattribute(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_SENDEVENTS, value)
#define cg_upnp_statevariable_getsendevents(statVar) cg_xml_node_getattributevalue(cg_upnp_statevariable_getstatevariablenode(statVar), CG_UPNP_STATEVARIABLE_SENDEVENTS)
#define cg_upnp_statevariable_issendevents(statVar) (cg_streq(cg_upnp_statevariable_getsendevents(statVar), CG_UPNP_STATEVARIABLE_SENDEVENTS_YES))

/****************************************
* Function (StateVariable - Execution Data)
****************************************/

/**** value ****/
void cg_upnp_statevariable_setvalue(CgUpnpStateVariable *statVar, char *data);
#define cg_upnp_statevariable_getvalue(statVar) cg_string_getvalue(statVar->value)

/*** listener ****/
#define cg_upnp_statevariable_setlistener(statVar, func) (statVar->listener = func)
#define cg_upnp_statevariable_getlistener(statVar) (statVar->listener)

/**** UPnPStatus ****/
#define cg_upnp_statevariable_setstatuscode(statVar, code) cg_upnp_status_setcode(statVar->upnpStatus, code)
#define cg_upnp_statevariable_getstatuscode(statVar) cg_upnp_status_getcode(statVar->upnpStatus)
#define cg_upnp_statevariable_setstatusdescription(statVar, value) cg_upnp_status_setdescription(statVar->upnpStatus, value)
#define cg_upnp_statevariable_getstatusdescription(statVar) cg_upnp_status_getdescription(statVar->upnpStatus)

/**** User Data****/
#define cg_upnp_statevariable_setuserdata(statVar, value) (statVar->userData = value)
#define cg_upnp_statevariable_getuserdata(statVar) (statVar->userData)

/****************************************
* Function (ServiceStateTable)
****************************************/

CgUpnpServiceStateTable *cg_upnp_servicestatetable_new();
void cg_upnp_servicestatetable_delete(CgUpnpServiceStateTable *servicestatetable);

#define cg_upnp_servicestatetable_clear(stateTable) cg_list_clear((CgList *)stateTable, (CG_LIST_DESTRUCTORFUNC)cg_upnp_statevariable_delete)
#define cg_upnp_servicestatetable_size(stateTable) cg_list_size((CgList *)stateTable)
#define cg_upnp_servicestatetable_gets(stateTable) (CgUpnpStateVariable *)cg_list_next((CgList *)stateTable)
#define cg_upnp_servicestatetable_add(stateTable, statVar) cg_list_add((CgList *)stateTable, (CgList *)statVar)

#ifdef  __cplusplus
}
#endif

#endif

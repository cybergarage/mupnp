/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cargument.h
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CARGUMENT_H_
#define _CG_UPNP_CARGUMENT_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_ARGUMENT_ELEM_NAME "argument"
#define CG_UPNP_ARGUMENTLIST_ELEM_NAME "argumentList"

#define CG_UPNP_ARGUMENT_IN_DIR "in"
#define CG_UPNP_ARGUMENT_OUT_DIR "out"
#define CG_UPNP_ARGUMENT_NAME "name"
#define CG_UPNP_ARGUMENT_DIRECTION "direction"
#define CG_UPNP_ARGUMENT_RELATED_STATE_VARIABLE "relatedStateVariable"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpArgument {
	BOOL headFlag;
	struct _CgUpnpArgument *prev;
	struct _CgUpnpArgument *next;
	CgXmlNode *serviceNode;
	CgXmlNode *argumentNode;
	/**** Execution Data ****/
	CgString *value;
} CgUpnpArgument, CgUpnpArgumentList;

/****************************************
* Function (Argument)
****************************************/

CgUpnpArgument *cg_upnp_argument_new();
void cg_upnp_argument_delete(CgUpnpArgument *dev);

#define cg_upnp_argument_next(arg) (CgUpnpArgument *)cg_list_next((CgList *)arg)

#define cg_upnp_argument_isargumentnode(node) cg_xml_node_isname(node, CG_UPNP_ARGUMENT_ELEM_NAME)

#define cg_upnp_argument_setservicenode(arg,node) (arg->serviceNode = node)
#define cg_upnp_argument_getservicenode(arg) (arg->serviceNode)
#define cg_upnp_argument_setargumentnode(arg,node) (arg->argumentNode = node)
#define cg_upnp_argument_getargumentnode(arg) (arg->argumentNode)

/**** name ****/
#define cg_upnp_argument_setname(arg, value) cg_xml_node_setchildnode(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_NAME, value)
#define cg_upnp_argument_getname(arg) cg_xml_node_getchildnodevalue(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_NAME)
#define cg_upnp_argument_isname(arg, name) cg_streq(cg_upnp_argument_getname(arg), name)

/**** direction ****/
#define cg_upnp_argument_setdirection(arg, value) cg_xml_node_setchildnode(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_DIRECTION, value)
#define cg_upnp_argument_getdirection(arg) cg_xml_node_getchildnodevalue(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_DIRECTION)
#define cg_upnp_argument_isindirection(arg) cg_streq(cg_upnp_argument_getdirection(arg), CG_UPNP_ARGUMENT_IN_DIR)
#define cg_upnp_argument_isoutdirection(arg) cg_streq(cg_upnp_argument_getdirection(arg), CG_UPNP_ARGUMENT_OUT_DIR)

/**** relatedStateVariable ****/
#define cg_upnp_argument_setrelatedstatevariable(arg, value) cg_xml_node_setchildnode(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_RELATED_STATE_VARIABLE, value)
#define cg_upnp_argument_getrelatedstatevariable(arg) cg_xml_node_getchildnodevalue(cg_upnp_argument_getargumentnode(arg), CG_UPNP_ARGUMENT_RELATED_STATE_VARIABLE)

/****************************************
* Function (Argument - Execution Data)
****************************************/

/**** value ****/
#define cg_upnp_argument_setvalue(arg,data) cg_string_setvalue(arg->value, data)
#define cg_upnp_argument_getvalue(arg) cg_string_getvalue(arg->value)

/****************************************
* Function (ArgumentList)
****************************************/

CgUpnpArgumentList *cg_upnp_argumentlist_new();
void cg_upnp_argumentlist_delete(CgUpnpArgumentList *argumentList);

#define cg_upnp_argumentlist_clear(argList) cg_list_clear((CgList *)argList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_argument_delete)
#define cg_upnp_argumentlist_size(argList) cg_list_size((CgList *)argList)
#define cg_upnp_argumentlist_gets(argList) (CgUpnpArgument *)cg_list_next((CgList *)argList)
#define cg_upnp_argumentlist_add(argList, arg) cg_list_add((CgList *)argList, (CgList *)arg)

CgUpnpArgument *cg_upnp_argumentlist_get(CgUpnpArgumentList *argumentList, char *name);

void cg_upnp_argumentlist_set(CgUpnpArgumentList *argumentList, CgUpnpArgumentList *srcArgumentList);

#ifdef  __cplusplus
}
#endif

#endif

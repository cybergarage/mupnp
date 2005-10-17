/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cproperty.h
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CPROPERTY_H_
#define _CG_UPNP_CPROPERTY_H_

#include <cybergarage/typedef.h>

#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/net/curl.h>

#include <cybergarage/upnp/cstatevariable.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpProperty {
	BOOL headFlag;
	struct _CgUpnpProperty *prev;
	struct _CgUpnpProperty *next;

	CgString *name;
	CgString *value;
	CgString *sid;
	long seq;
} CgUpnpProperty, CgUpnpPropertyList;

/****************************************
* Function (Property)
****************************************/

CgUpnpProperty *cg_upnp_property_new();
void cg_upnp_property_delete(CgUpnpProperty *prop);
void cg_upnp_property_clear(CgUpnpProperty *prop);

#define cg_upnp_property_next(prop) (CgUpnpProperty *)cg_list_next((CgList *)prop)

/**** name ****/
#define cg_upnp_property_setname(prop, value) cg_string_setvalue(prop->name, value)
#define cg_upnp_property_getname(prop) cg_string_getvalue(prop->name)

/**** value ****/
#define cg_upnp_property_setvalue(prop, nvalue) cg_string_setvalue(prop->value, nvalue)
#define cg_upnp_property_getvalue(prop) cg_string_getvalue(prop->value)

/**** sid ****/
#define cg_upnp_property_setsid(prop, value) cg_string_setvalue(prop->sid, value)
#define cg_upnp_property_getsid(prop) cg_string_getvalue(prop->sid)

/**** seq ****/
#define cg_upnp_property_setseq(prop, value) (prop->seq = value)
#define cg_upnp_property_getseq(prop) (prop->seq)

/****************************************
* Function (PropertyList)
****************************************/

CgUpnpPropertyList *cg_upnp_propertylist_new();
void cg_upnp_propertylist_delete(CgUpnpPropertyList *propertylist);

#define cg_upnp_propertylist_clear(propList) cg_list_clear((CgList *)propList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_property_delete)
#define cg_upnp_propertylist_size(propList) cg_list_size((CgList *)propList)
#define cg_upnp_propertylist_gets(propList) (CgUpnpProperty *)cg_list_next((CgList *)propList)
#define cg_upnp_propertylist_add(propList, prop) cg_list_add((CgList *)propList, (CgList *)prop)

#ifdef  __cplusplus
}
#endif

#endif

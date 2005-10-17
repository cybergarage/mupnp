/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cproperty_list.c
*
*	Revision:
*
*	07/08/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cproperty.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_propertylist_new
****************************************/

CgUpnpPropertyList *cg_upnp_propertylist_new()
{
	CgUpnpPropertyList *propertyList = (CgUpnpPropertyList *)malloc(sizeof(CgUpnpPropertyList));
	cg_list_header_init((CgList *)propertyList);
	return propertyList;
}

/****************************************
* cg_upnp_propertylist_delete
****************************************/

void cg_upnp_propertylist_delete(CgUpnpPropertyList *propertyList)
{
	cg_upnp_propertylist_clear(propertyList);
	free(propertyList);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

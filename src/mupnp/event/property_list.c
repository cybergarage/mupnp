/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cproperty_list.c
*
*	Revision:
*
*	07/08/05
*		- first revision
*
******************************************************************/

#include <mupnp/event/property.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_propertylist_new
****************************************/

CgUpnpPropertyList *mupnp_upnp_propertylist_new()
{
	CgUpnpPropertyList *propertyList;

	mupnp_log_debug_l4("Entering...\n");

	propertyList = (CgUpnpPropertyList *)malloc(sizeof(CgUpnpPropertyList));

	if ( NULL != propertyList )
		mupnp_list_header_init((CgList *)propertyList);

	return propertyList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_propertylist_delete
****************************************/

void mupnp_upnp_propertylist_delete(CgUpnpPropertyList *propertyList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_propertylist_clear(propertyList);
	free(propertyList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

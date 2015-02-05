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
*	File: cproperty.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*
******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/event/notify.h>
#include <mupnp/event/property.h>
#include <mupnp/util/log.h>

#include <time.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_property_new
****************************************/

CgUpnpProperty *mupnp_upnp_property_new()
{
	CgUpnpProperty *prop;

	mupnp_log_debug_l4("Entering...\n");

	prop = (CgUpnpProperty *)malloc(sizeof(CgUpnpProperty));

	if ( NULL != prop )
	{
		mupnp_list_node_init((CgList *)prop);
		
		prop->name = mupnp_string_new();
		prop->value = mupnp_string_new();
		prop->sid = mupnp_string_new();
		
		mupnp_upnp_property_setseq(prop, 0);
	}
	
	return prop;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_property_delete
****************************************/

void mupnp_upnp_property_delete(CgUpnpProperty *prop)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_property_clear(prop);
	mupnp_list_remove((CgList *)prop);

	mupnp_string_delete(prop->name);
	mupnp_string_delete(prop->value);
	mupnp_string_delete(prop->sid);
	
	free(prop);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_property_clear
****************************************/

void mupnp_upnp_property_clear(CgUpnpProperty *prop)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(prop->name, NULL);
	mupnp_string_setvalue(prop->value, NULL);
	mupnp_string_setvalue(prop->sid, NULL);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

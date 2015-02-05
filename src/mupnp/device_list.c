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
*	File: cdevice_list.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <mupnp/device.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_devicelist_new
****************************************/

CgUpnpDeviceList *mupnp_upnp_devicelist_new()
{
	CgUpnpDeviceList *devList;

	mupnp_log_debug_l4("Entering...\n");

	devList = (CgUpnpDeviceList *)malloc(sizeof(CgUpnpDeviceList));

	if ( NULL != devList )
		mupnp_list_header_init((CgList *)devList);

	return devList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_devicelist_delete
****************************************/

void mupnp_upnp_devicelist_delete(CgUpnpDeviceList *devList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_devicelist_clear(devList);
	free(devList);

	mupnp_log_debug_l4("Leaving...\n");
}

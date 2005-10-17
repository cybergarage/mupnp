/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdevice_list.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>

/****************************************
* cg_upnp_devicelist_new
****************************************/

CgUpnpDeviceList *cg_upnp_devicelist_new()
{
	CgUpnpDeviceList *devList = (CgUpnpDeviceList *)malloc(sizeof(CgUpnpDeviceList));
	cg_list_header_init((CgList *)devList);
	return devList;
}

/****************************************
* cg_upnp_devicelist_delete
****************************************/

void cg_upnp_devicelist_delete(CgUpnpDeviceList *devList)
{
	cg_upnp_devicelist_clear(devList);
	free(devList);
}

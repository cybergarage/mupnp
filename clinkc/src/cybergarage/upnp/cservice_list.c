/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cservice_list.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>

/****************************************
* cg_upnp_servicelist_new
****************************************/

CgUpnpServiceList *cg_upnp_servicelist_new()
{
	CgUpnpServiceList *serviceList = (CgUpnpServiceList *)malloc(sizeof(CgUpnpServiceList));
	cg_list_header_init((CgList *)serviceList);
	return serviceList;
}

/****************************************
* cg_upnp_servicelist_delete
****************************************/

void cg_upnp_servicelist_delete(CgUpnpServiceList *serviceList)
{
	cg_upnp_servicelist_clear(serviceList);
	free(serviceList);
}

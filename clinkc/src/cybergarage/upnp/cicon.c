/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cicon.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cicon.h>

/****************************************
* cg_upnp_icon_new
****************************************/

CgUpnpIcon *cg_upnp_icon_new()
{
	CgUpnpIcon *dev = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));
	cg_list_node_init((CgList *)dev);
	return dev;
}

/****************************************
* cg_upnp_icon_delete
****************************************/

void cg_upnp_icon_delete(CgUpnpIcon *dev)
{
	cg_list_remove((CgList *)dev);
	free(dev);
}

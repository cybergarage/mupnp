/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cicon_list.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cicon.h>

/****************************************
* cg_upnp_iconr_new
****************************************/

CgUpnpIconList *cg_upnp_iconlist_new()
{
	CgUpnpIcon *iconList = (CgUpnpIcon *)malloc(sizeof(CgUpnpIcon));
	cg_list_header_init((CgList *)iconList);
	return iconList;
}

/****************************************
* cg_upnp_iconr_delete
****************************************/

void cg_upnp_iconlist_delete(CgUpnpIconList *iconList)
{
	cg_list_remove((CgList *)iconList);
	free(iconList);
}

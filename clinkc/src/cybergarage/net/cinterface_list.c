/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cinterface_list.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/net/cinterface.h>

/****************************************
* cg_net_interfacelist_new
****************************************/

CgNetworkInterfaceList *cg_net_interfacelist_new()
{
	CgNetworkInterfaceList *netIfList = (CgNetworkInterfaceList *)malloc(sizeof(CgNetworkInterfaceList));
	cg_list_header_init((CgList *)netIfList);
	netIfList->name = NULL;
	netIfList->ipaddr = NULL;
	netIfList->index = 0;
	return netIfList;
}

/****************************************
* cg_net_interfacelist_delete
****************************************/

void cg_net_interfacelist_delete(CgNetworkInterfaceList *netIfList)
{
	cg_net_interfacelist_clear(netIfList);
	free(netIfList);
}

/****************************************
* cg_net_interfacelist_getinterface
****************************************/

CgNetworkInterface *cg_net_interfacelist_get(CgNetworkInterfaceList *netIfList, char *name)
{
	CgNetworkInterface *netIf;
	char *ifName;
		
	if (name == NULL)
		return NULL;
		
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf != NULL; netIf = cg_net_interface_next(netIf)) {
		ifName = cg_net_interface_getname(netIf);
		if (ifName == NULL)
			continue;
		if (cg_strcasecmp(ifName, name) == 0)
			return netIf;
	}
	
	return NULL;
}

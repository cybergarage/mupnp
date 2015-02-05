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
*	File: cinterface_list.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#include <mupnp/util/list.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_net_interfacelist_new
****************************************/

CgNetworkInterfaceList *mupnp_net_interfacelist_new()
{
	CgNetworkInterfaceList *netIfList;

	mupnp_log_debug_l4("Entering...\n");

	netIfList = (CgNetworkInterfaceList *)malloc(sizeof(CgNetworkInterfaceList));

	if ( NULL != netIfList )
	{
		mupnp_list_header_init((CgList *)netIfList);
		netIfList->name = NULL;
		netIfList->ipaddr = NULL;
	}

	return netIfList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_delete
****************************************/

void mupnp_net_interfacelist_delete(CgNetworkInterfaceList *netIfList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_net_interfacelist_clear(netIfList);
	free(netIfList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_getinterface
****************************************/

CgNetworkInterface *mupnp_net_interfacelist_get(CgNetworkInterfaceList *netIfList, char *name)
{
	CgNetworkInterface *netIf;
	char *ifName;
		
	mupnp_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf != NULL; netIf = mupnp_net_interface_next(netIf)) {
		ifName = mupnp_net_interface_getname(netIf);
		if (ifName == NULL)
			continue;
		if (mupnp_strcasecmp(ifName, name) == 0)
			return netIf;
	}
	
	return NULL;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_getchanges
****************************************/

void mupnp_net_interfacelist_getchanges(CgNetworkInterfaceList *netIfListOld,
				     CgNetworkInterfaceList *netIfListNew,
				     CgNetworkInterfaceList *netIfListAdded,
				     CgNetworkInterfaceList *netIfListRemoved)
{
	CgNetworkInterface *netIfOld, *netIfNew, *tmp;
	BOOL found;
	
	mupnp_log_debug_l4("Entering...\n");

	/* Browse through old interfaces and check, if they are in the new */
	tmp = mupnp_net_interfacelist_gets(netIfListOld);
	while (tmp != NULL)
	{
		netIfOld = tmp; tmp = mupnp_net_interface_next(netIfOld);
		
		found = FALSE;
		for (netIfNew = mupnp_net_interfacelist_gets(netIfListNew); netIfNew != NULL;
		     netIfNew = mupnp_net_interface_next(netIfNew))
		{
			if (mupnp_net_interface_cmp(netIfOld, netIfNew) == 0)
			{
				found = TRUE;
				break;
			}
		}
		
		/* Old interface was not found in new ones, so it's removed */
		if (found == FALSE)
		{
			mupnp_net_interface_remove(netIfOld);
			if (netIfListRemoved != NULL)
				mupnp_net_interfacelist_add(netIfListRemoved, 
							 netIfOld);
		}
	}
	
	/* Browse through new interfaces and check, if they are in the 
	   remaining old interfaces */
	tmp = mupnp_net_interfacelist_gets(netIfListNew);
	while (tmp != NULL)
	{
		netIfNew = tmp; tmp = mupnp_net_interface_next(netIfNew);
		
		found = FALSE;
		for (netIfOld = mupnp_net_interfacelist_gets(netIfListOld); netIfOld != NULL;
		     netIfOld = mupnp_net_interface_next(netIfOld))
		{
			if (mupnp_net_interface_cmp(netIfOld, netIfNew) == 0)
			{
				found = TRUE;
				break;
			}
		}
		
		/* New interface was not found in old ones, so it's added */
		if (found == FALSE)
		{
			mupnp_net_interface_remove(netIfNew);
			if (netIfListAdded != NULL)
				mupnp_net_interfacelist_add(netIfListAdded, 
							 netIfNew);
		}
	}

	mupnp_log_debug_l4("Leaving...\n");
}

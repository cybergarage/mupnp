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
*	File: cssdp_notify_server_list.c
*
*	Revision:
*
*	05/31/05
*		- first revision
*	02/13/06 Theo Beisch
*		- mupnp_upnp_ssdp_serverlist_open now returns FALSE
*		  if no interface had successful open
*
******************************************************************/

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_ssdp_serverlist_new
****************************************/

CgUpnpSSDPServerList *mupnp_upnp_ssdp_serverlist_new()
{
	CgUpnpSSDPServerList *ssdpServerList;

	mupnp_log_debug_l4("Entering...\n");

	ssdpServerList = (CgUpnpSSDPServerList *)malloc(sizeof(CgUpnpSSDPServerList));

	if ( NULL != ssdpServerList )
	{
		mupnp_list_header_init((CgList *)ssdpServerList);
		
		ssdpServerList->httpmuSock = NULL;
		ssdpServerList->recvThread = NULL;
		ssdpServerList->listener = NULL;
	}
	
	return ssdpServerList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_serverlist_delete
****************************************/

void mupnp_upnp_ssdp_serverlist_delete(CgUpnpSSDPServerList *ssdpServerList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdp_serverlist_clear(ssdpServerList);
	
	free(ssdpServerList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_serverlist_open
****************************************/

BOOL mupnp_upnp_ssdp_serverlist_open(CgUpnpSSDPServerList *ssdpServerList)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	CgUpnpSSDPServer *ssdpServer;
	char *bindAddr;
	
	mupnp_log_debug_l4("Entering...\n");

	netIfList = mupnp_net_interfacelist_new();
#ifndef CG_NET_USE_ANYADDR
	mupnp_net_gethostinterfaces(netIfList);
#else
	netIf = mupnp_net_interface_getany();
	mupnp_net_interfacelist_add(netIfList, netIf);
#endif
	for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
		bindAddr = mupnp_net_interface_getaddress(netIf);
		if (mupnp_strlen(bindAddr) <= 0)
			continue;
		ssdpServer = mupnp_upnp_ssdp_server_new();
		if (mupnp_upnp_ssdp_server_open(ssdpServer, bindAddr) == FALSE) {
			mupnp_upnp_ssdp_server_delete(ssdpServer);
			continue;
		}
		mupnp_upnp_ssdp_serverlist_add(ssdpServerList, ssdpServer);
	}

	mupnp_net_interfacelist_delete(netIfList);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;	
}

/****************************************
* mupnp_upnp_ssdp_serverlist_close
****************************************/

BOOL mupnp_upnp_ssdp_serverlist_close(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (ssdpServer = mupnp_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_upnp_ssdp_server_next(ssdpServer))
		mupnp_upnp_ssdp_server_close(ssdpServer);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdp_serverlist_start
****************************************/

BOOL mupnp_upnp_ssdp_serverlist_start(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (ssdpServer = mupnp_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_upnp_ssdp_server_next(ssdpServer))
		mupnp_upnp_ssdp_server_start(ssdpServer);
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdp_serverlist_stop
****************************************/

BOOL mupnp_upnp_ssdp_serverlist_stop(CgUpnpSSDPServerList *ssdpServerList)
{
	CgUpnpSSDPServer *ssdpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (ssdpServer = mupnp_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_upnp_ssdp_server_next(ssdpServer))
		mupnp_upnp_ssdp_server_stop(ssdpServer);
		
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdp_serverlist_setlistener
****************************************/

void mupnp_upnp_ssdp_serverlist_setlistener(CgUpnpSSDPServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener)
{
	CgUpnpSSDPServer *ssdpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (ssdpServer = mupnp_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_upnp_ssdp_server_next(ssdpServer))
		mupnp_upnp_ssdp_server_setlistener(ssdpServer, listener);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_serverlist_setuserdata
****************************************/

void mupnp_upnp_ssdp_serverlist_setuserdata(CgUpnpSSDPServerList *ssdpServerList, void *data)
{
	CgUpnpSSDPServer *ssdpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (ssdpServer = mupnp_upnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_upnp_ssdp_server_next(ssdpServer))
		mupnp_upnp_ssdp_server_setuserdata(ssdpServer, data);

	mupnp_log_debug_l4("Leaving...\n");
}


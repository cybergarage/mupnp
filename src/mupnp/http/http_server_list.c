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
*	File: chttp_server_list.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*	12/14/05
*		- fixed to fail if opening assigned port fails
*
******************************************************************/

#include <mupnp/http/http.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_serverlist_new
****************************************/

CgHttpServerList *mupnp_http_serverlist_new()
{
	CgHttpServerList *httpServerList;

	mupnp_log_debug_l4("Entering...\n");

	httpServerList = (CgHttpServerList *)malloc(sizeof(CgHttpServerList));

	if ( NULL != httpServerList )
	{
		mupnp_list_header_init((CgList *)httpServerList);
		
		httpServerList->sock = NULL;
		httpServerList->acceptThread = NULL;
		httpServerList->listener = NULL;
		httpServerList->userData = NULL;
		httpServerList->mutex = NULL;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return httpServerList;
}

/****************************************
* mupnp_http_serverlist_delete
****************************************/

void mupnp_http_serverlist_delete(CgHttpServerList *httpServerList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_serverlist_clear(httpServerList);
	free(httpServerList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_open
****************************************/

BOOL mupnp_http_serverlist_open(CgHttpServerList *httpServerList, int port)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	CgHttpServer *httpServer;
	char *bindAddr;
	BOOL result =FALSE;
	
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
		httpServer = mupnp_http_server_new();

		if (mupnp_http_server_open(httpServer, port, bindAddr) == FALSE) {
			/* Theo Beisch - why break off, 
			   we may be running ok on at least some IF??? 
			   (at least WINCE does...) */
			continue;
		}
		mupnp_http_serverlist_add(httpServerList, httpServer);
		result = TRUE; //at least one IF found ok
	}

	
	if (result==FALSE) mupnp_http_serverlist_clear(httpServerList);
	mupnp_net_interfacelist_delete(netIfList);

	return result;	
}

/****************************************
* mupnp_http_serverlist_close
****************************************/

BOOL mupnp_http_serverlist_close(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
		mupnp_http_server_close(httpServer);

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_start
****************************************/

BOOL mupnp_http_serverlist_start(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
		mupnp_http_server_start(httpServer);
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_http_serverlist_stop
****************************************/

BOOL mupnp_http_serverlist_stop(CgHttpServerList *httpServerList)
{
	CgHttpServer *httpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
		mupnp_http_server_stop(httpServer);
		
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_http_serverlist_setlistener
****************************************/

void mupnp_http_serverlist_setlistener(CgHttpServerList *httpServerList, CG_HTTP_LISTENER listener)
{
	CgHttpServer *httpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
		mupnp_http_server_setlistener(httpServer, listener);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_setuserdata
****************************************/

void mupnp_http_serverlist_setuserdata(CgHttpServerList *httpServerList, void *value)
{
	CgHttpServer *httpServer;
	
	mupnp_log_debug_l4("Entering...\n");

	for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
		mupnp_http_server_setuserdata(httpServer, value);

	mupnp_log_debug_l4("Leaving...\n");
}

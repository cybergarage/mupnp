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
*	File: cssdp_socket.c
*
*	Revision:
*
*	02/18/05
*		- first revision
*
******************************************************************/

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_ssdp_server_new
****************************************/

CgUpnpSSDPServer *mupnp_upnp_ssdp_server_new()
{
	CgUpnpSSDPServer *server;

	mupnp_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPServer *)malloc(sizeof(CgUpnpSSDPServer));

	if ( NULL != server )
	{
		mupnp_list_node_init((CgList *)server);

		server->httpmuSock = NULL;
		server->recvThread = NULL;

		mupnp_upnp_ssdp_server_setlistener(server, NULL);
		mupnp_upnp_ssdp_server_setuserdata(server, NULL);
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return server;
}

/****************************************
* mupnp_upnp_ssdp_server_delete
****************************************/

void mupnp_upnp_ssdp_server_delete(CgUpnpSSDPServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdp_server_stop(server);
	mupnp_upnp_ssdp_server_close(server);
	
	mupnp_list_remove((CgList *)server);

	free(server);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_server_open
****************************************/

BOOL mupnp_upnp_ssdp_server_open(CgUpnpSSDPServer *server, char *bindAddr)
{
	const char *ssdpAddr = CG_UPNP_SSDP_ADDRESS;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_ssdp_server_isopened(server) == TRUE)
		return FALSE;
		
	if (mupnp_net_isipv6address(bindAddr) == TRUE)
		ssdpAddr = mupnp_upnp_ssdp_getipv6address();
	
	server->httpmuSock = mupnp_upnp_httpmu_socket_new();
	if (mupnp_upnp_httpmu_socket_bind(server->httpmuSock, ssdpAddr, CG_UPNP_SSDP_PORT, bindAddr) == FALSE) {
		mupnp_upnp_httpmu_socket_delete(server->httpmuSock);
		server->httpmuSock = NULL;
		return FALSE;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdp_server_close
****************************************/

BOOL mupnp_upnp_ssdp_server_close(CgUpnpSSDPServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdp_server_stop(server);
	
	if (server->httpmuSock != NULL) {
		mupnp_upnp_httpmu_socket_close(server->httpmuSock);
		mupnp_upnp_httpmu_socket_delete(server->httpmuSock);
		server->httpmuSock = NULL;
	}
	
	mupnp_log_debug_l4("Leaving...\n");
	return TRUE;
}

/****************************************
* mupnp_upnp_ssdp_server_performlistener
****************************************/

void mupnp_upnp_ssdp_server_performlistener(CgUpnpSSDPServer *server, CgUpnpSSDPPacket *ssdpPkt)
{
	CG_UPNP_SSDP_LISTNER listener;

	mupnp_log_debug_l4("Entering...\n");

	listener = mupnp_upnp_ssdp_server_getlistener(server);
	if (listener == NULL)
		return;
	listener(ssdpPkt);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_server_thread
****************************************/

static void mupnp_upnp_ssdp_server_thread(CgThread *thread)
{
	CgUpnpSSDPServer *server;
	CgUpnpSSDPPacket *ssdpPkt;
	void *userData;
	
	mupnp_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPServer *)mupnp_thread_getuserdata(thread);
	userData = mupnp_upnp_ssdp_server_getuserdata(server);
	
	if (mupnp_upnp_ssdp_server_isopened(server) == FALSE)
		return;

	ssdpPkt = mupnp_upnp_ssdp_packet_new();
	mupnp_upnp_ssdp_packet_setuserdata(ssdpPkt, userData);
	
	while (mupnp_thread_isrunnable(thread) == TRUE) {
		if (mupnp_upnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
			break;

		mupnp_upnp_ssdp_server_performlistener(server, ssdpPkt);
		mupnp_upnp_ssdp_packet_clear(ssdpPkt);
	}
	
	mupnp_upnp_ssdp_packet_delete(ssdpPkt);

    mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_server_start
****************************************/

BOOL mupnp_upnp_ssdp_server_start(CgUpnpSSDPServer *server)
{
	if (server->recvThread != NULL)
		return FALSE;
		
	mupnp_log_debug_l4("Entering...\n");

	server->recvThread = mupnp_thread_new();
	mupnp_thread_setaction(server->recvThread, mupnp_upnp_ssdp_server_thread);
	mupnp_thread_setuserdata(server->recvThread, server);
	if (mupnp_thread_start(server->recvThread) == FALSE) {	
		mupnp_thread_delete(server->recvThread);
		server->recvThread = NULL;
		return FALSE;
	}

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_server_stop
****************************************/

BOOL mupnp_upnp_ssdp_server_stop(CgUpnpSSDPServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	if (server->recvThread != NULL) {
		mupnp_thread_stop(server->recvThread);
		mupnp_thread_delete(server->recvThread);
		server->recvThread = NULL;
	}
	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}


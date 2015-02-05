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
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
* mupnp_upnp_ssdpresponse_server_new
****************************************/

CgUpnpSSDPResponseServer *mupnp_upnp_ssdpresponse_server_new()
{
	CgUpnpSSDPResponseServer *server;

	mupnp_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPResponseServer *)malloc(sizeof(CgUpnpSSDPResponseServer));

	if ( NULL != server )
	{
		mupnp_list_node_init((CgList *)server);

		server->httpuSock = NULL;
		server->recvThread = NULL;

		mupnp_upnp_ssdpresponse_server_setlistener(server, NULL);
		mupnp_upnp_ssdpresponse_server_setuserdata(server, NULL);
	}
		
	mupnp_log_debug_l4("Leaving...\n");

	return server;
}

/****************************************
* mupnp_upnp_ssdpresponse_server_delete
****************************************/

void mupnp_upnp_ssdpresponse_server_delete(CgUpnpSSDPResponseServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdpresponse_server_stop(server);
	mupnp_upnp_ssdpresponse_server_close(server);
	
	mupnp_list_remove((CgList *)server);

	free(server);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdpresponse_server_open
****************************************/

BOOL mupnp_upnp_ssdpresponse_server_open(CgUpnpSSDPResponseServer *server, int bindPort, char *bindAddr)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_ssdpresponse_server_isopened(server) == TRUE)
		return FALSE;
		
	server->httpuSock = mupnp_upnp_httpu_socket_new();
	if (mupnp_upnp_httpu_socket_bind(server->httpuSock, bindPort, bindAddr) == FALSE) {
		mupnp_upnp_httpu_socket_delete(server->httpuSock);
		server->httpuSock = NULL;
		return FALSE;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdpresponse_server_close
****************************************/

BOOL mupnp_upnp_ssdpresponse_server_close(CgUpnpSSDPResponseServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdpresponse_server_stop(server);
	
	if (server->httpuSock != NULL) {
		mupnp_upnp_httpu_socket_close(server->httpuSock);
		mupnp_upnp_httpu_socket_delete(server->httpuSock);
		server->httpuSock = NULL;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdpresponse_server_performlistener
****************************************/

void mupnp_upnp_ssdpresponse_server_performlistener(CgUpnpSSDPResponseServer *server, CgUpnpSSDPPacket *ssdpPkt)
{
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;

	mupnp_log_debug_l4("Entering...\n");

	listener = mupnp_upnp_ssdpresponse_server_getlistener(server);
	if (listener == NULL)
		return;
	listener(ssdpPkt);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdpresponse_server_thread
****************************************/

static void mupnp_upnp_ssdpresponse_server_thread(CgThread *thread)
{
	CgUpnpSSDPResponseServer *server;
	CgUpnpSSDPPacket *ssdpPkt;
	void *userData;
	
	mupnp_log_debug_l4("Entering...\n");

	server = (CgUpnpSSDPResponseServer *)mupnp_thread_getuserdata(thread);
	userData = mupnp_upnp_ssdpresponse_server_getuserdata(server);
	
	if (mupnp_upnp_ssdpresponse_server_isopened(server) == FALSE)
		return;

	ssdpPkt = mupnp_upnp_ssdp_packet_new();
	mupnp_upnp_ssdp_packet_setuserdata(ssdpPkt, userData);
	
	while (mupnp_thread_isrunnable(thread) == TRUE) {
		if (mupnp_upnp_httpu_socket_recv(server->httpuSock, ssdpPkt) <= 0)
			break;

		mupnp_upnp_ssdp_packet_print(ssdpPkt);
		
		mupnp_upnp_ssdpresponse_server_performlistener(server, ssdpPkt);
		mupnp_upnp_ssdp_packet_clear(ssdpPkt);
	}
	
	mupnp_upnp_ssdp_packet_delete(ssdpPkt);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdpresponse_server_start
****************************************/

BOOL mupnp_upnp_ssdpresponse_server_start(CgUpnpSSDPResponseServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	if (server->recvThread != NULL)
		return FALSE;
		
	server->recvThread = mupnp_thread_new();
	mupnp_thread_setaction(server->recvThread, mupnp_upnp_ssdpresponse_server_thread);
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
* mupnp_upnp_ssdpresponse_server_stop
****************************************/

BOOL mupnp_upnp_ssdpresponse_server_stop(CgUpnpSSDPResponseServer *server)
{
	mupnp_log_debug_l4("Entering...\n");

	if (server->recvThread != NULL) {
		mupnp_thread_stop(server->recvThread);
		mupnp_thread_delete(server->recvThread);
		server->recvThread = NULL;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_ssdpresponse_server_post
****************************************/

BOOL mupnp_upnp_ssdpresponse_server_post(CgUpnpSSDPResponseServer *server, CgUpnpSSDPRequest *ssdpReq)
{
	CgUpnpHttpUSocket *httpuSock;
	char *ifAddr;
	const char *ssdpAddr;
	CgString *ssdpMsg;
	size_t sentLen = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	httpuSock = mupnp_upnp_ssdpresponse_server_getsocket(server);
	
	ifAddr = mupnp_socket_getaddress(httpuSock);
	ssdpAddr = mupnp_upnp_ssdp_gethostaddress(ifAddr);
	mupnp_upnp_ssdprequest_sethost(ssdpReq, ssdpAddr, CG_UPNP_SSDP_PORT);
		
	ssdpMsg = mupnp_string_new();
	mupnp_upnp_ssdprequest_tostring(ssdpReq, ssdpMsg);

	sentLen = mupnp_socket_sendto(httpuSock, ssdpAddr, CG_UPNP_SSDP_PORT, mupnp_string_getvalue(ssdpMsg), mupnp_string_length(ssdpMsg));
	mupnp_string_delete(ssdpMsg);
	
	mupnp_log_debug_l4("Leaving...\n");

	return (sentLen > 0);
}

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

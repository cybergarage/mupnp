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

#include <mupnp/service.h>
#include <mupnp/upnp_function.h>
#include <mupnp/device.h>
#include <mupnp/ssdp/ssdp.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_ssdp_socket_notify
****************************************/

static BOOL mupnp_upnp_ssdp_socket_notify(mUpnpUpnpSSDPSocket *ssdpSock, mUpnpUpnpSSDPRequest *ssdpReq, const char *ssdpAddr)
{
	mUpnpString *ssdpMsg;
	size_t sentLen;

	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdprequest_setmethod(ssdpReq, CG_HTTP_NOTIFY);
	
	ssdpMsg = mupnp_string_new();
	mupnp_upnp_ssdprequest_tostring(ssdpReq, ssdpMsg);

	sentLen = mupnp_socket_sendto(ssdpSock, ssdpAddr, CG_UPNP_SSDP_PORT, mupnp_string_getvalue(ssdpMsg), mupnp_string_length(ssdpMsg));
	mupnp_string_delete(ssdpMsg);
	
	mupnp_log_debug_l4("Leaving...\n");

	return (0 < sentLen) ? TRUE : FALSE;
}

/****************************************
* mupnp_upnp_ssdp_socket_notifyfrom
****************************************/

BOOL mupnp_upnp_ssdp_socket_notifyfrom(mUpnpUpnpSSDPSocket *ssdpSock, mUpnpUpnpSSDPRequest *ssdpReq, const char *bindAddr)
{
	const char *ssdpAddr;
	
	mupnp_log_debug_l4("Entering...\n");

	ssdpAddr = mupnp_upnp_ssdp_gethostaddress(bindAddr);
	mupnp_upnp_ssdprequest_sethost(ssdpReq, ssdpAddr, CG_UPNP_SSDP_PORT);

	mupnp_log_debug_l4("Leaving...\n");
	
  return mupnp_upnp_ssdp_socket_notify(ssdpSock, ssdpReq, ssdpAddr);
}

/****************************************
* mupnp_upnp_ssdp_socket_postresponse
****************************************/

BOOL mupnp_upnp_ssdp_socket_postresponse(mUpnpUpnpSSDPSocket *ssdpSock, mUpnpUpnpSSDPResponse *ssdpRes, const char *host, int port)
{
	mUpnpString *ssdpMsg;
  size_t ssdpMsgLen;
	BOOL postSuccess;

	mupnp_log_debug_l4("Entering...\n");

	ssdpMsg = mupnp_string_new();
	mupnp_upnp_ssdpresponse_tostring(ssdpRes, ssdpMsg);
  ssdpMsgLen = mupnp_string_length(ssdpMsg);
	postSuccess = (mupnp_socket_sendto(ssdpSock, host, port, mupnp_string_getvalue(ssdpMsg), ssdpMsgLen) == ssdpMsgLen) ? TRUE : FALSE;
	mupnp_string_delete(ssdpMsg);

	mupnp_log_debug_l4("Leaving...\n");

	return postSuccess;
}

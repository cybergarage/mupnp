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
*	File: chttpmu_socket.c
*
*	Revision:
*
*	02/18/05
*		- first revision
*
******************************************************************/

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_httpmu_socket_bind
****************************************/

BOOL mupnp_upnp_httpmu_socket_bind(CgUpnpHttpMuSocket *sock, const char *mcastAddr, int port, const char *bindAddr)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_socket_bind(sock, port, bindAddr, FALSE, TRUE) == FALSE)
		return FALSE;
		
	if (mupnp_socket_joingroup(sock, mcastAddr, bindAddr) == FALSE) {
		mupnp_socket_close(sock);
		return FALSE;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_httpmu_socket_recv
****************************************/

ssize_t mupnp_upnp_httpmu_socket_recv(CgUpnpHttpMuSocket *sock, CgUpnpSSDPPacket *ssdpPkt)
{
	CgDatagramPacket *dgmPkt;
	char *ssdpData;
	ssize_t recvLen;
	
	mupnp_log_debug_l4("Entering...\n");

	dgmPkt = mupnp_upnp_ssdp_packet_getdatagrampacket(ssdpPkt);
	recvLen = mupnp_socket_recv(sock, dgmPkt);

	if (recvLen <= 0)
		return recvLen;

	ssdpData = mupnp_socket_datagram_packet_getdata(dgmPkt);

	/* set header information to the packets headerlist,
	   this will leave only the request line in the datagram packet 
	   which is need to verify the message */
	mupnp_upnp_ssdp_packet_setheader(ssdpPkt, ssdpData);
	
	mupnp_log_debug_l4("Leaving...\n");

	return recvLen;
}

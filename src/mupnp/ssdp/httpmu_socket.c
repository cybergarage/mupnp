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
* cg_upnp_httpmu_socket_bind
****************************************/

BOOL cg_upnp_httpmu_socket_bind(CgUpnpHttpMuSocket *sock, const char *mcastAddr, int port, const char *bindAddr)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_socket_bind(sock, port, bindAddr, FALSE, TRUE) == FALSE)
		return FALSE;
		
	if (cg_socket_joingroup(sock, mcastAddr, bindAddr) == FALSE) {
		cg_socket_close(sock);
		return FALSE;
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_httpmu_socket_recv
****************************************/

ssize_t cg_upnp_httpmu_socket_recv(CgUpnpHttpMuSocket *sock, CgUpnpSSDPPacket *ssdpPkt)
{
	CgDatagramPacket *dgmPkt;
	char *ssdpData;
	ssize_t recvLen;
	
	cg_log_debug_l4("Entering...\n");

	dgmPkt = cg_upnp_ssdp_packet_getdatagrampacket(ssdpPkt);
	recvLen = cg_socket_recv(sock, dgmPkt);

	if (recvLen <= 0)
		return recvLen;

	ssdpData = cg_socket_datagram_packet_getdata(dgmPkt);

	/* set header information to the packets headerlist,
	   this will leave only the request line in the datagram packet 
	   which is need to verify the message */
	cg_upnp_ssdp_packet_setheader(ssdpPkt, ssdpData);
	
	cg_log_debug_l4("Leaving...\n");

	return recvLen;
}

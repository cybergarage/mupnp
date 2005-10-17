/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdatagram_packet.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/net/csocket.h>

/****************************************
* cg_socket_datagram_packet_new
****************************************/

CgDatagramPacket *cg_socket_datagram_packet_new()
{
	CgDatagramPacket *dgmPkt = (CgDatagramPacket *)malloc(sizeof(CgDatagramPacket));

	dgmPkt->data = cg_string_new();
	dgmPkt->localAddress = cg_string_new();
	dgmPkt->remoteAddress = cg_string_new();

	cg_socket_datagram_packet_setlocalport(dgmPkt, 0);
	cg_socket_datagram_packet_setremoteport(dgmPkt, 0);
	
	return dgmPkt;
}

/****************************************
* cg_socket_datagram_packet_delete
****************************************/

void cg_socket_datagram_packet_delete(CgDatagramPacket *dgmPkt)
{
	cg_string_delete(dgmPkt->data);
	cg_string_delete(dgmPkt->localAddress);
	cg_string_delete(dgmPkt->remoteAddress);

	free(dgmPkt);
}

/****************************************
* cg_socket_datagram_packet_copy
****************************************/

void cg_socket_datagram_packet_copy(CgDatagramPacket *dstDgmPkt, CgDatagramPacket *srcDgmPkt)
{
	cg_socket_datagram_packet_setdata(dstDgmPkt, cg_socket_datagram_packet_getdata(srcDgmPkt));
	cg_socket_datagram_packet_setlocaladdress(dstDgmPkt, cg_socket_datagram_packet_getlocaladdress(srcDgmPkt));
	cg_socket_datagram_packet_setlocalport(dstDgmPkt, cg_socket_datagram_packet_getlocalport(srcDgmPkt));
	cg_socket_datagram_packet_setremoteaddress(dstDgmPkt, cg_socket_datagram_packet_getremoteaddress(srcDgmPkt));
	cg_socket_datagram_packet_setremoteport(dstDgmPkt, cg_socket_datagram_packet_getremoteport(srcDgmPkt));
}

/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_httpmu_socket_bind
****************************************/

bool mupnp_httpmu_socket_bind(mUpnpHttpMuSocket* sock, const char* mcastAddr, int port, const char* bindAddr)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_socket_bind(sock, port, bindAddr, false, true) == false)
    return false;

  if (mupnp_socket_joingroup(sock, mcastAddr, bindAddr) == false) {
    mupnp_socket_close(sock);
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_httpmu_socket_recv
****************************************/

ssize_t mupnp_httpmu_socket_recv(mUpnpHttpMuSocket* sock, mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpDatagramPacket* dgmPkt;
  char* ssdpData;
  ssize_t recvLen;

  mupnp_log_debug_l4("Entering...\n");

  dgmPkt = mupnp_ssdp_packet_getdatagrampacket(ssdpPkt);
  recvLen = mupnp_socket_recv(sock, dgmPkt);

  if (recvLen <= 0)
    return recvLen;

  ssdpData = mupnp_socket_datagram_packet_getdata(dgmPkt);

  /* set header information to the packets headerlist,
	   this will leave only the request line in the datagram packet 
	   which is need to verify the message */
  mupnp_ssdp_packet_setheader(ssdpPkt, ssdpData);

  mupnp_log_debug_l4("Leaving...\n");

  return recvLen;
}

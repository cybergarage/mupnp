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
* mupnp_httpu_socket_recv
****************************************/

ssize_t mupnp_httpu_socket_recv(mUpnpHttpMuSocket* sock, mUpnpSSDPPacket* ssdpPkt)
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
  mupnp_ssdp_packet_setheader(ssdpPkt, ssdpData);
  mupnp_socket_datagram_packet_setdata(dgmPkt, NULL);

  mupnp_log_debug_l4("Leaving...\n");

  return recvLen;
}

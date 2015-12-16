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

#include <mupnp/net/socket.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_socket_datagram_packet_new
****************************************/

mUpnpDatagramPacket* mupnp_socket_datagram_packet_new()
{
  mUpnpDatagramPacket* dgmPkt;

  mupnp_log_debug_l4("Entering...\n");

  dgmPkt = (mUpnpDatagramPacket*)malloc(sizeof(mUpnpDatagramPacket));

  if (NULL != dgmPkt) {
    dgmPkt->data = mupnp_string_new();
    dgmPkt->localAddress = mupnp_string_new();
    dgmPkt->remoteAddress = mupnp_string_new();

    mupnp_socket_datagram_packet_setlocalport(dgmPkt, 0);
    mupnp_socket_datagram_packet_setremoteport(dgmPkt, 0);
  }

  return dgmPkt;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_datagram_packet_delete
****************************************/

void mupnp_socket_datagram_packet_delete(mUpnpDatagramPacket* dgmPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_delete(dgmPkt->data);
  mupnp_string_delete(dgmPkt->localAddress);
  mupnp_string_delete(dgmPkt->remoteAddress);

  free(dgmPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_datagram_packet_copy
****************************************/

void mupnp_socket_datagram_packet_copy(mUpnpDatagramPacket* dstDgmPkt, mUpnpDatagramPacket* srcDgmPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_socket_datagram_packet_setdata(dstDgmPkt, mupnp_socket_datagram_packet_getdata(srcDgmPkt));
  mupnp_socket_datagram_packet_setlocaladdress(dstDgmPkt, mupnp_socket_datagram_packet_getlocaladdress(srcDgmPkt));
  mupnp_socket_datagram_packet_setlocalport(dstDgmPkt, mupnp_socket_datagram_packet_getlocalport(srcDgmPkt));
  mupnp_socket_datagram_packet_setremoteaddress(dstDgmPkt, mupnp_socket_datagram_packet_getremoteaddress(srcDgmPkt));
  mupnp_socket_datagram_packet_setremoteport(dstDgmPkt, mupnp_socket_datagram_packet_getremoteport(srcDgmPkt));

  mupnp_log_debug_l4("Leaving...\n");
}

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

#include <mupnp/service.h>
#include <mupnp/upnp_function.h>
#include <mupnp/device.h>
#include <mupnp/ssdp/ssdp.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_ssdp_socket_notify
****************************************/

static bool mupnp_ssdp_socket_notify(mUpnpSSDPSocket* ssdpSock, mUpnpSSDPRequest* ssdpReq, const char* ssdpAddr)
{
  mUpnpString* ssdpMsg;
  size_t sentLen;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdprequest_setmethod(ssdpReq, MUPNP_HTTP_NOTIFY);

  ssdpMsg = mupnp_string_new();
  mupnp_ssdprequest_tostring(ssdpReq, ssdpMsg);

  sentLen = mupnp_socket_sendto(ssdpSock, ssdpAddr, MUPNP_SSDP_PORT, mupnp_string_getvalue(ssdpMsg), mupnp_string_length(ssdpMsg));
  mupnp_string_delete(ssdpMsg);

  mupnp_log_debug_l4("Leaving...\n");

  return (0 < sentLen) ? true : false;
}

/****************************************
* mupnp_ssdp_socket_notifyfrom
****************************************/

bool mupnp_ssdp_socket_notifyfrom(mUpnpSSDPSocket* ssdpSock, mUpnpSSDPRequest* ssdpReq, const char* bindAddr)
{
  const char* ssdpAddr;

  mupnp_log_debug_l4("Entering...\n");

  ssdpAddr = mupnp_ssdp_gethostaddress(bindAddr);
  mupnp_ssdprequest_sethost(ssdpReq, ssdpAddr, MUPNP_SSDP_PORT);

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_ssdp_socket_notify(ssdpSock, ssdpReq, ssdpAddr);
}

/****************************************
* mupnp_ssdp_socket_postresponse
****************************************/

bool mupnp_ssdp_socket_postresponse(mUpnpSSDPSocket* ssdpSock, mUpnpSSDPResponse* ssdpRes, const char* host, int port)
{
  mUpnpString* ssdpMsg;
  size_t ssdpMsgLen;
  bool postSuccess;

  mupnp_log_debug_l4("Entering...\n");

  ssdpMsg = mupnp_string_new();
  mupnp_ssdpresponse_tostring(ssdpRes, ssdpMsg);
  ssdpMsgLen = mupnp_string_length(ssdpMsg);
  postSuccess = (mupnp_socket_sendto(ssdpSock, host, port, mupnp_string_getvalue(ssdpMsg), ssdpMsgLen) == ssdpMsgLen) ? true : false;
  mupnp_string_delete(ssdpMsg);

  mupnp_log_debug_l4("Leaving...\n");

  return postSuccess;
}

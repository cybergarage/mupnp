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
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_ssdp_server_new
****************************************/

mUpnpSSDPServer* mupnp_ssdp_server_new()
{
  mUpnpSSDPServer* server;

  mupnp_log_debug_l4("Entering...\n");

  server = (mUpnpSSDPServer*)malloc(sizeof(mUpnpSSDPServer));

  if (NULL != server) {
    mupnp_list_node_init((mUpnpList*)server);

    server->httpmuSock = NULL;
    server->recvThread = NULL;

    mupnp_ssdp_server_setlistener(server, NULL);
    mupnp_ssdp_server_setuserdata(server, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return server;
}

/****************************************
* mupnp_ssdp_server_delete
****************************************/

void mupnp_ssdp_server_delete(mUpnpSSDPServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdp_server_stop(server);
  mupnp_ssdp_server_close(server);

  mupnp_list_remove((mUpnpList*)server);

  free(server);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_server_open
****************************************/

bool mupnp_ssdp_server_open(mUpnpSSDPServer* server, char* bindAddr)
{
  const char* ssdpAddr = MUPNP_SSDP_ADDRESS;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_ssdp_server_isopened(server) == true)
    return false;

  if (mupnp_net_isipv6address(bindAddr) == true)
    ssdpAddr = mupnp_ssdp_getipv6address();

  server->httpmuSock = mupnp_httpmu_socket_new();
  if (mupnp_httpmu_socket_bind(server->httpmuSock, ssdpAddr, MUPNP_SSDP_PORT, bindAddr) == false) {
    mupnp_httpmu_socket_delete(server->httpmuSock);
    server->httpmuSock = NULL;
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdp_server_close
****************************************/

bool mupnp_ssdp_server_close(mUpnpSSDPServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdp_server_stop(server);

  if (server->httpmuSock != NULL) {
    mupnp_httpmu_socket_close(server->httpmuSock);
    mupnp_httpmu_socket_delete(server->httpmuSock);
    server->httpmuSock = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");
  return true;
}

/****************************************
* mupnp_ssdp_server_performlistener
****************************************/

void mupnp_ssdp_server_performlistener(mUpnpSSDPServer* server, mUpnpSSDPPacket* ssdpPkt)
{
  MUPNP_SSDP_LISTNER listener;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_ssdp_server_getlistener(server);
  if (listener == NULL)
    return;
  listener(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_server_thread
****************************************/

static void mupnp_ssdp_server_thread(mUpnpThread* thread)
{
  mUpnpSSDPServer* server;
  mUpnpSSDPPacket* ssdpPkt;
  void* userData;

  mupnp_log_debug_l4("Entering...\n");

  server = (mUpnpSSDPServer*)mupnp_thread_getuserdata(thread);
  userData = mupnp_ssdp_server_getuserdata(server);

  if (mupnp_ssdp_server_isopened(server) == false)
    return;

  ssdpPkt = mupnp_ssdp_packet_new();
  mupnp_ssdp_packet_setuserdata(ssdpPkt, userData);

  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
  }

  mupnp_ssdp_packet_delete(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_server_start
****************************************/

bool mupnp_ssdp_server_start(mUpnpSSDPServer* server)
{
  if (server->recvThread != NULL)
    return false;

  mupnp_log_debug_l4("Entering...\n");

  server->recvThread = mupnp_thread_new();
  mupnp_thread_setaction(server->recvThread, mupnp_ssdp_server_thread);
  mupnp_thread_setuserdata(server->recvThread, server);
  if (mupnp_thread_start(server->recvThread) == false) {
    mupnp_thread_delete(server->recvThread);
    server->recvThread = NULL;
    return false;
  }

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_server_stop
****************************************/

bool mupnp_ssdp_server_stop(mUpnpSSDPServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  if (server->recvThread != NULL) {
    mupnp_thread_stop(server->recvThread);
    mupnp_thread_delete(server->recvThread);
    server->recvThread = NULL;
  }
  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

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
* MUPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_CONTROLPOINT)

/****************************************
* mupnp_ssdpresponse_server_new
****************************************/

mUpnpSSDPResponseServer* mupnp_ssdpresponse_server_new()
{
  mUpnpSSDPResponseServer* server;

  mupnp_log_debug_l4("Entering...\n");

  server = (mUpnpSSDPResponseServer*)malloc(sizeof(mUpnpSSDPResponseServer));

  if (NULL != server) {
    mupnp_list_node_init((mUpnpList*)server);

    server->httpuSock = NULL;
    server->recvThread = NULL;

    mupnp_ssdpresponse_server_setlistener(server, NULL);
    mupnp_ssdpresponse_server_setuserdata(server, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return server;
}

/****************************************
* mupnp_ssdpresponse_server_delete
****************************************/

void mupnp_ssdpresponse_server_delete(mUpnpSSDPResponseServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdpresponse_server_stop(server);
  mupnp_ssdpresponse_server_close(server);

  mupnp_list_remove((mUpnpList*)server);

  free(server);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_server_open
****************************************/

bool mupnp_ssdpresponse_server_open(mUpnpSSDPResponseServer* server, int bindPort, char* bindAddr)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_ssdpresponse_server_isopened(server) == true)
    return false;

  server->httpuSock = mupnp_httpu_socket_new();
  if (mupnp_httpu_socket_bind(server->httpuSock, bindPort, bindAddr) == false) {
    mupnp_httpu_socket_delete(server->httpuSock);
    server->httpuSock = NULL;
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdpresponse_server_close
****************************************/

bool mupnp_ssdpresponse_server_close(mUpnpSSDPResponseServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdpresponse_server_stop(server);

  if (server->httpuSock != NULL) {
    mupnp_httpu_socket_close(server->httpuSock);
    mupnp_httpu_socket_delete(server->httpuSock);
    server->httpuSock = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdpresponse_server_performlistener
****************************************/

void mupnp_ssdpresponse_server_performlistener(mUpnpSSDPResponseServer* server, mUpnpSSDPPacket* ssdpPkt)
{
  MUPNP_SSDP_RESPONSE_LISTNER listener;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_ssdpresponse_server_getlistener(server);
  if (listener == NULL)
    return;
  listener(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_server_thread
****************************************/

static void mupnp_ssdpresponse_server_thread(mUpnpThread* thread)
{
  mUpnpSSDPResponseServer* server;
  mUpnpSSDPPacket* ssdpPkt;
  void* userData;

  mupnp_log_debug_l4("Entering...\n");

  server = (mUpnpSSDPResponseServer*)mupnp_thread_getuserdata(thread);
  userData = mupnp_ssdpresponse_server_getuserdata(server);

  if (mupnp_ssdpresponse_server_isopened(server) == false)
    return;

  ssdpPkt = mupnp_ssdp_packet_new();
  mupnp_ssdp_packet_setuserdata(ssdpPkt, userData);

  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpu_socket_recv(server->httpuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_packet_print(ssdpPkt);

    mupnp_ssdpresponse_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
  }

  mupnp_ssdp_packet_delete(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_server_start
****************************************/

bool mupnp_ssdpresponse_server_start(mUpnpSSDPResponseServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  if (server->recvThread != NULL)
    return false;

  server->recvThread = mupnp_thread_new();
  mupnp_thread_setaction(server->recvThread, mupnp_ssdpresponse_server_thread);
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
* mupnp_ssdpresponse_server_stop
****************************************/

bool mupnp_ssdpresponse_server_stop(mUpnpSSDPResponseServer* server)
{
  mupnp_log_debug_l4("Entering...\n");

  if (server->recvThread != NULL) {
    mupnp_thread_stop(server->recvThread);
    mupnp_thread_delete(server->recvThread);
    server->recvThread = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdpresponse_server_post
****************************************/

bool mupnp_ssdpresponse_server_post(mUpnpSSDPResponseServer* server, mUpnpSSDPRequest* ssdpReq)
{
  mUpnpHttpUSocket* httpuSock;
  char* ifAddr;
  const char* ssdpAddr;
  mUpnpString* ssdpMsg;
  size_t sentLen = 0;

  mupnp_log_debug_l4("Entering...\n");

  httpuSock = mupnp_ssdpresponse_server_getsocket(server);

  ifAddr = mupnp_socket_getaddress(httpuSock);
  ssdpAddr = mupnp_ssdp_gethostaddress(ifAddr);
  mupnp_ssdprequest_sethost(ssdpReq, ssdpAddr, MUPNP_SSDP_PORT);

  ssdpMsg = mupnp_string_new();
  mupnp_ssdprequest_tostring(ssdpReq, ssdpMsg);

  sentLen = mupnp_socket_sendto(httpuSock, ssdpAddr, MUPNP_SSDP_PORT, mupnp_string_getvalue(ssdpMsg), mupnp_string_length(ssdpMsg));
  mupnp_string_delete(ssdpMsg);

  mupnp_log_debug_l4("Leaving...\n");

  return (sentLen > 0);
}

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

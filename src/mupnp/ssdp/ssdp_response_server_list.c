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
* mupnp_ssdpresponse_serverlist_new
****************************************/

mUpnpSSDPResponseServerList* mupnp_ssdpresponse_serverlist_new()
{
  mUpnpSSDPResponseServerList* ssdpServerList;

  mupnp_log_debug_l4("Entering...\n");

  ssdpServerList = (mUpnpSSDPResponseServerList*)malloc(sizeof(mUpnpSSDPResponseServerList));

  if (NULL != ssdpServerList) {
    mupnp_list_header_init((mUpnpList*)ssdpServerList);

    ssdpServerList->httpuSock = NULL;
    ssdpServerList->recvThread = NULL;
    ssdpServerList->listener = NULL;
  }

  return ssdpServerList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_delete
****************************************/

void mupnp_ssdpresponse_serverlist_delete(mUpnpSSDPResponseServerList* ssdpServerList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdpresponse_serverlist_clear(ssdpServerList);

  free(ssdpServerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_open
****************************************/

bool mupnp_ssdpresponse_serverlist_open(mUpnpSSDPResponseServerList* ssdpServerList, int bindPort)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  mUpnpSSDPResponseServer* ssdpServer;
  char* bindAddr;
  bool result = false;

  mupnp_log_debug_l4("Entering...\n");

  netIfList = mupnp_net_interfacelist_new();
#ifndef MUPNP_NET_USE_ANYADDR
  mupnp_net_gethostinterfaces(netIfList);
#else
  netIf = mupnp_net_interface_getany();
  mupnp_net_interfacelist_add(netIfList, netIf);
#endif
  for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    bindAddr = mupnp_net_interface_getaddress(netIf);
    if (mupnp_strlen(bindAddr) <= 0)
      continue;
    ssdpServer = mupnp_ssdpresponse_server_new();
    if (mupnp_ssdpresponse_server_open(ssdpServer, bindPort, bindAddr) == false) {
      mupnp_ssdpresponse_server_delete(ssdpServer);
      continue;
    }
    mupnp_ssdpresponse_serverlist_add(ssdpServerList, ssdpServer);
    result = true;
  }

  if (result == false)
    mupnp_ssdpresponse_serverlist_clear(ssdpServerList);

  mupnp_net_interfacelist_delete(netIfList);

  return result;
}

/****************************************
* mupnp_ssdpresponse_serverlist_close
****************************************/

bool mupnp_ssdpresponse_serverlist_close(mUpnpSSDPResponseServerList* ssdpServerList)
{
  mUpnpSSDPResponseServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    mupnp_ssdpresponse_server_close(ssdpServer);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_start
****************************************/

bool mupnp_ssdpresponse_serverlist_start(mUpnpSSDPResponseServerList* ssdpServerList)
{
  mUpnpSSDPResponseServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    mupnp_ssdpresponse_server_start(ssdpServer);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_stop
****************************************/

bool mupnp_ssdpresponse_serverlist_stop(mUpnpSSDPResponseServerList* ssdpServerList)
{
  mUpnpSSDPResponseServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    mupnp_ssdpresponse_server_stop(ssdpServer);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_setlistener
****************************************/

void mupnp_ssdpresponse_serverlist_setlistener(mUpnpSSDPResponseServerList* ssdpServerList, MUPNP_SSDP_LISTNER listener)
{
  mUpnpSSDPResponseServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    mupnp_ssdpresponse_server_setlistener(ssdpServer, listener);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_setuserdata
****************************************/

void mupnp_ssdpresponse_serverlist_setuserdata(mUpnpSSDPResponseServerList* ssdpServerList, void* data)
{
  mUpnpSSDPResponseServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    mupnp_ssdpresponse_server_setuserdata(ssdpServer, data);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdpresponse_serverlist_post
****************************************/

bool mupnp_ssdpresponse_serverlist_post(mUpnpSSDPResponseServerList* ssdpServerList, mUpnpSSDPRequest* ssdpReq)
{
  mUpnpSSDPResponseServer* ssdpServer;
  bool success = true;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdpresponse_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdpresponse_server_next(ssdpServer))
    success &= mupnp_ssdpresponse_server_post(ssdpServer, ssdpReq);

  return success;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

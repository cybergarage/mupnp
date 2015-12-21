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
* mupnp_ssdp_serverlist_new
****************************************/

mUpnpSSDPServerList* mupnp_ssdp_serverlist_new()
{
  mUpnpSSDPServerList* ssdpServerList;

  mupnp_log_debug_l4("Entering...\n");

  ssdpServerList = (mUpnpSSDPServerList*)malloc(sizeof(mUpnpSSDPServerList));

  if (NULL != ssdpServerList) {
    mupnp_list_header_init((mUpnpList*)ssdpServerList);

    ssdpServerList->httpmuSock = NULL;
    ssdpServerList->recvThread = NULL;
    ssdpServerList->listener = NULL;
  }

  return ssdpServerList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_serverlist_delete
****************************************/

void mupnp_ssdp_serverlist_delete(mUpnpSSDPServerList* ssdpServerList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdp_serverlist_clear(ssdpServerList);

  free(ssdpServerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_serverlist_open
****************************************/

bool mupnp_ssdp_serverlist_open(mUpnpSSDPServerList* ssdpServerList)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  mUpnpSSDPServer* ssdpServer;
  char* bindAddr;

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
    ssdpServer = mupnp_ssdp_server_new();
    if (mupnp_ssdp_server_open(ssdpServer, bindAddr) == false) {
      mupnp_ssdp_server_delete(ssdpServer);
      continue;
    }
    mupnp_ssdp_serverlist_add(ssdpServerList, ssdpServer);
  }

  mupnp_net_interfacelist_delete(netIfList);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdp_serverlist_close
****************************************/

bool mupnp_ssdp_serverlist_close(mUpnpSSDPServerList* ssdpServerList)
{
  mUpnpSSDPServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdp_server_next(ssdpServer))
    mupnp_ssdp_server_close(ssdpServer);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdp_serverlist_start
****************************************/

bool mupnp_ssdp_serverlist_start(mUpnpSSDPServerList* ssdpServerList)
{
  mUpnpSSDPServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdp_server_next(ssdpServer))
    mupnp_ssdp_server_start(ssdpServer);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdp_serverlist_stop
****************************************/

bool mupnp_ssdp_serverlist_stop(mUpnpSSDPServerList* ssdpServerList)
{
  mUpnpSSDPServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdp_server_next(ssdpServer))
    mupnp_ssdp_server_stop(ssdpServer);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_ssdp_serverlist_setlistener
****************************************/

void mupnp_ssdp_serverlist_setlistener(mUpnpSSDPServerList* ssdpServerList, MUPNP_SSDP_LISTNER listener)
{
  mUpnpSSDPServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdp_server_next(ssdpServer))
    mupnp_ssdp_server_setlistener(ssdpServer, listener);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_serverlist_setuserdata
****************************************/

void mupnp_ssdp_serverlist_setuserdata(mUpnpSSDPServerList* ssdpServerList, void* data)
{
  mUpnpSSDPServer* ssdpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (ssdpServer = mupnp_ssdp_serverlist_gets(ssdpServerList); ssdpServer != NULL; ssdpServer = mupnp_ssdp_server_next(ssdpServer))
    mupnp_ssdp_server_setuserdata(ssdpServer, data);

  mupnp_log_debug_l4("Leaving...\n");
}

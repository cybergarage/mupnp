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

#include <mupnp/http/http.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_serverlist_new
****************************************/

mUpnpHttpServerList* mupnp_http_serverlist_new()
{
  mUpnpHttpServerList* httpServerList;

  mupnp_log_debug_l4("Entering...\n");

  httpServerList = (mUpnpHttpServerList*)malloc(sizeof(mUpnpHttpServerList));

  if (NULL != httpServerList) {
    mupnp_list_header_init((mUpnpList*)httpServerList);

    httpServerList->sock = NULL;
    httpServerList->acceptThread = NULL;
    httpServerList->listener = NULL;
    httpServerList->userData = NULL;
    httpServerList->mutex = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return httpServerList;
}

/****************************************
* mupnp_http_serverlist_delete
****************************************/

void mupnp_http_serverlist_delete(mUpnpHttpServerList* httpServerList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_serverlist_clear(httpServerList);
  free(httpServerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_open
****************************************/

bool mupnp_http_serverlist_open(mUpnpHttpServerList* httpServerList, int port)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  mUpnpHttpServer* httpServer;
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
    httpServer = mupnp_http_server_new();

    if (mupnp_http_server_open(httpServer, port, bindAddr) == false) {
      /* Theo Beisch - why break off, 
			   we may be running ok on at least some IF??? 
			   (at least WINCE does...) */
      continue;
    }
    mupnp_http_serverlist_add(httpServerList, httpServer);
    result = true; //at least one IF found ok
  }

  if (result == false)
    mupnp_http_serverlist_clear(httpServerList);
  mupnp_net_interfacelist_delete(netIfList);

  return result;
}

/****************************************
* mupnp_http_serverlist_close
****************************************/

bool mupnp_http_serverlist_close(mUpnpHttpServerList* httpServerList)
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
    mupnp_http_server_close(httpServer);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_start
****************************************/

bool mupnp_http_serverlist_start(mUpnpHttpServerList* httpServerList)
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
    mupnp_http_server_start(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_serverlist_stop
****************************************/

bool mupnp_http_serverlist_stop(mUpnpHttpServerList* httpServerList)
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
    mupnp_http_server_stop(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_serverlist_setlistener
****************************************/

void mupnp_http_serverlist_setlistener(mUpnpHttpServerList* httpServerList, MUPNP_HTTP_LISTENER listener)
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
    mupnp_http_server_setlistener(httpServer, listener);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_serverlist_setuserdata
****************************************/

void mupnp_http_serverlist_setuserdata(mUpnpHttpServerList* httpServerList, void* value)
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  for (httpServer = mupnp_http_serverlist_gets(httpServerList); httpServer != NULL; httpServer = mupnp_http_server_next(httpServer))
    mupnp_http_server_setuserdata(httpServer, value);

  mupnp_log_debug_l4("Leaving...\n");
}

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

#if defined(MUPNP_NET_USE_SOCKET_LIST)

/****************************************
* mupnp_socketlist_new
****************************************/

mUpnpSocketList* mupnp_socketlist_new()
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpSocketList* socketList = (mUpnpSocketList*)malloc(sizeof(mUpnpSocketList));

  if (NULL != socketList)
    mupnp_list_header_init((mUpnpList*)socketList);

  return socketList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socketlist_delete
****************************************/

void mupnp_socketlist_delete(mUpnpSocketList* socketList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_socketlist_clear(socketList);
  free(socketList);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

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

#include <mupnp/icon.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_iconr_new
****************************************/

mUpnpIconList* mupnp_iconlist_new()
{
  mUpnpIcon* iconList;

  mupnp_log_debug_l4("Entering...\n");

  iconList = (mUpnpIcon*)malloc(sizeof(mUpnpIcon));

  if (NULL != iconList)
    mupnp_list_header_init((mUpnpList*)iconList);

  return iconList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_iconr_delete
****************************************/

void mupnp_iconlist_delete(mUpnpIconList* iconList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)iconList);
  free(iconList);

  mupnp_log_debug_l4("Leaving...\n");
}

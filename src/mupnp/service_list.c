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
#include <mupnp/util/log.h>

/****************************************
* mupnp_servicelist_new
****************************************/

mUpnpServiceList* mupnp_servicelist_new()
{
  mUpnpServiceList* serviceList;

  mupnp_log_debug_l4("Entering...\n");

  serviceList = (mUpnpServiceList*)malloc(sizeof(mUpnpServiceList));

  if (NULL != serviceList)
    mupnp_list_header_init((mUpnpList*)serviceList);

  return serviceList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_servicelist_delete
****************************************/

void mupnp_servicelist_delete(mUpnpServiceList* serviceList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_servicelist_clear(serviceList);
  free(serviceList);

  mupnp_log_debug_l4("Leaving...\n");
}

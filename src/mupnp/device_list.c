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

#include <mupnp/device.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_devicelist_new
****************************************/

mUpnpDeviceList* mupnp_devicelist_new()
{
  mUpnpDeviceList* devList;

  mupnp_log_debug_l4("Entering...\n");

  devList = (mUpnpDeviceList*)malloc(sizeof(mUpnpDeviceList));

  if (NULL != devList)
    mupnp_list_header_init((mUpnpList*)devList);

  return devList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_devicelist_delete
****************************************/

void mupnp_devicelist_delete(mUpnpDeviceList* devList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_devicelist_clear(devList);
  free(devList);

  mupnp_log_debug_l4("Leaving...\n");
}

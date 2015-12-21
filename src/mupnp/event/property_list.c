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

#include <mupnp/event/property.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_propertylist_new
****************************************/

mUpnpPropertyList* mupnp_propertylist_new()
{
  mUpnpPropertyList* propertyList;

  mupnp_log_debug_l4("Entering...\n");

  propertyList = (mUpnpPropertyList*)malloc(sizeof(mUpnpPropertyList));

  if (NULL != propertyList)
    mupnp_list_header_init((mUpnpList*)propertyList);

  return propertyList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_propertylist_delete
****************************************/

void mupnp_propertylist_delete(mUpnpPropertyList* propertyList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_propertylist_clear(propertyList);
  free(propertyList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

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

#include <mupnp/statevariable.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_servicestatetable_new
****************************************/

mUpnpServiceStateTable* mupnp_servicestatetable_new()
{
  mUpnpServiceStateTable* statevariableTable;

  mupnp_log_debug_l4("Entering...\n");

  statevariableTable = (mUpnpServiceStateTable*)malloc(sizeof(mUpnpServiceStateTable));

  if (NULL != statevariableTable)
    mupnp_list_header_init((mUpnpList*)statevariableTable);

  mupnp_log_debug_l4("Leaving...\n");

  return statevariableTable;
}

/****************************************
* mupnp_servicestatetable_delete
****************************************/

void mupnp_servicestatetable_delete(mUpnpServiceStateTable* statevariableTable)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)statevariableTable);
  free(statevariableTable);

  mupnp_log_debug_l4("Leaving...\n");
}

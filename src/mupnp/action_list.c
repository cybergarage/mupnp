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

#include <mupnp/action.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_action_new
****************************************/

mUpnpActionList* mupnp_actionlist_new()
{
  mUpnpAction* actionList;

  mupnp_log_debug_l4("Entering...\n");

  actionList = (mUpnpAction*)malloc(sizeof(mUpnpAction));

  if (NULL != actionList)
    mupnp_list_header_init((mUpnpList*)actionList);

  return actionList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_action_delete
****************************************/

void mupnp_actionlist_delete(mUpnpActionList* actionList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)actionList);
  free(actionList);

  mupnp_log_debug_l4("Leaving...\n");
}

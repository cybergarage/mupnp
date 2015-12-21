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

#include <mupnp/upnp_status.h>
#include <mupnp/util/list.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_status_new
****************************************/

mUpnpStatus* mupnp_status_new()
{
  mUpnpStatus* upnpStat;

  mupnp_log_debug_l4("Entering...\n");

  upnpStat = (mUpnpStatus*)malloc(sizeof(mUpnpStatus));

  if (NULL != upnpStat) {
    upnpStat->code = 0;
    upnpStat->description = mupnp_string_new();
  }

  return upnpStat;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_status_delete
****************************************/

void mupnp_status_delete(mUpnpStatus* upnpStat)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_delete(upnpStat->description);
  free(upnpStat);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_status_code2string
****************************************/

char* mupnp_status_code2string(int code)
{
  mupnp_log_debug_l4("Entering...\n");

  switch (code) {
  case MUPNP_STATUS_INVALID_ACTION:
    return "Invalid Action";
  case MUPNP_STATUS_INVALID_ARGS:
    return "Invalid Args";
  case MUPNP_STATUS_OUT_OF_SYNC:
    return "Out of Sync";
  case MUPNP_STATUS_INVALID_VAR:
    return "Invalid Var";
  case MUPNP_STATUS_ACTION_FAILED:
    return "Action Failed";
  }
  return "";

  mupnp_log_debug_l4("Leaving...\n");
}

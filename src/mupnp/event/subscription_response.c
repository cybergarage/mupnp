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

#include <mupnp/event/event.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_event_subscription_subscriberesponse_setresponse
****************************************/

void mupnp_event_subscription_subscriberesponse_setresponse(mUpnpSubscriptionResponse* subRes, int code)
{
  char server[MUPNP_SEVERNAME_MAXLEN];
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_response_setstatuscode(subRes, code);
  mupnp_getservername(server, sizeof(server));
  mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subRes),
      MUPNP_HTTP_SERVER,
      server);
  mupnp_http_response_setcontentlength(subRes, 0);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_response_setsid
****************************************/

void mupnp_event_subscription_response_setsid(mUpnpSubscriptionResponse* subRes, const char* sid)
{
  mUpnpString* headerSID;
  ssize_t uuidIdx;

  mupnp_log_debug_l4("Entering...\n");

  headerSID = mupnp_string_new();

  uuidIdx = mupnp_strstr(sid, MUPNP_ST_UUID_DEVICE);
  if (uuidIdx < 0)
    mupnp_string_addvalue(headerSID, MUPNP_ST_UUID_DEVICE ":");
  mupnp_string_addvalue(headerSID, sid);

  mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subRes), MUPNP_HTTP_SID, mupnp_string_getvalue(headerSID));

  mupnp_string_delete(headerSID);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_response_settimeout
****************************************/

void mupnp_event_subscription_response_settimeout(mUpnpSubscriptionResponse* subRes, long value)
{
  mUpnpString* buf;

  mupnp_log_debug_l4("Entering...\n");

  buf = mupnp_string_new();
  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)subRes, MUPNP_HTTP_TIMEOUT, mupnp_event_subscription_totimeoutheaderstring(value, buf));
  mupnp_string_delete(buf);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/event/event.h>
#include <mupnp/util/log.h>
#include <mupnp/upnp_function.h>

#include <stdio.h>
#include <stdlib.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_event_subscription_totimeoutheaderstring
****************************************/

const char* mupnp_event_subscription_totimeoutheaderstring(mUpnpTime time, mUpnpString* buf)
{
  char timeBuf[MUPNP_STRING_LONG_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  if (time != MUPNP_SUBSCRIPTION_INFINITE_VALUE) {
    mupnp_string_setvalue(buf, MUPNP_SUBSCRIPTION_TIMEOUT_HEADER);
    mupnp_string_addvalue(buf, mupnp_long2str(time, timeBuf, sizeof(timeBuf)));
  }
  else
    mupnp_string_setvalue(buf, MUPNP_SUBSCRIPTION_INFINITE_STRING);
  return mupnp_string_getvalue(buf);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_gettimeout
****************************************/

mUpnpTime mupnp_event_subscription_gettimeout(const char* headerValue)
{
  ssize_t minusIdx;
  long timeout;

  mupnp_log_debug_l4("Entering...\n");

  minusIdx = mupnp_strstr(headerValue, "-");
  if (minusIdx < 0)
    return MUPNP_SUBSCRIPTION_INFINITE_VALUE;
  timeout = mupnp_str2long(headerValue + minusIdx + 1);
  if (timeout == 0)
    return MUPNP_SUBSCRIPTION_INFINITE_VALUE;
  if (timeout < 0)
    timeout = -timeout;
  return timeout;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_createsid
****************************************/

const char* mupnp_event_subscription_createsid(char* buf, size_t bufSize)
{
  mupnp_log_debug_l4("Entering...\n");
  mupnp_createuuid(buf, bufSize);
  mupnp_log_debug_l4("Leaving...\n");
  return buf;
}

/****************************************
* mupnp_event_subscription_tosidheaderstring
****************************************/

const char* mupnp_event_subscription_tosidheaderstring(const char* sid, char* buf, size_t bufSize)
{
  ssize_t colonIdx;

  mupnp_log_debug_l4("Entering...\n");

  colonIdx = mupnp_strstr(sid, MUPNP_SUBSCRIPTION_UUID);
  if (0 <= colonIdx) {
    mupnp_strncpy(buf, sid, bufSize);
    buf[bufSize - 1] = '\0';
    return buf;
  }

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize,
#else
  sprintf(buf,
#endif
      "%s%s",
      MUPNP_SUBSCRIPTION_UUID,
      sid);
  return buf;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_getsid
****************************************/

const char* mupnp_event_subscription_getsid(const char* headerValue)
{
  ssize_t colonIdx;

  mupnp_log_debug_l4("Entering...\n");

  colonIdx = mupnp_strstr(headerValue, ":");
  if (colonIdx < 0)
    return headerValue;

  mupnp_log_debug_l4("Leaving...\n");

  return (headerValue + colonIdx + 1);
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

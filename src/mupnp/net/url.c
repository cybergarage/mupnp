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

#include <mupnp/net/interface.h>
#include <mupnp/net/uri.h>
#include <mupnp/util/log.h>

#include <stdio.h>
#include <string.h>

/****************************************
 * mupnp_net_getmodifierhosturl
 ****************************************/

const char* mupnp_net_getmodifierhosturl(const char* host, int port, const char* uri, const char* begin, const char* end, char* buf, size_t bufSize)
{
  bool isIPv6Host;
  const char* zone = NULL;
  int hostLen;

  mupnp_log_debug_l4("Entering...\n");

  isIPv6Host = mupnp_net_isipv6address(host);
  hostLen = (int)mupnp_strlen(host);
  if (isIPv6Host == true) {
    /* A zone index ("fe80::1%2") must be written as "%25" inside a URL
       (RFC 6874); mupnp_net_uri_set() decodes it again. */
    zone = strchr(host, '%');
    if (zone) {
      hostLen = (int)(zone - host);
      zone++;
    }
  }

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize,
#else
  sprintf(buf,
#endif
      "%shttp://%s%.*s%s%s%s:%d%s%s",
      begin,
      ((isIPv6Host == true) ? "[" : ""),
      hostLen,
      host,
      (zone ? "%25" : ""),
      (zone ? zone : ""),
      ((isIPv6Host == true) ? "]" : ""),
      port,
      uri,
      end);

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
 * mupnp_net_gethosturl
 ****************************************/

const char* mupnp_net_gethosturl(const char* host, int port, const char* uri, char* buf, size_t bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_net_getmodifierhosturl(host, port, uri, "", "", buf, bufSize);

  mupnp_log_debug_l4("Leaving...\n");
}

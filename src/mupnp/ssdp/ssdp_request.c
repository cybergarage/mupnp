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

#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_ssdprequest_new
****************************************/

mUpnpSSDPRequest* mupnp_ssdprequest_new()
{
  mUpnpSSDPRequest* ssdpReq;

  mupnp_log_debug_l4("Entering...\n");

  ssdpReq = mupnp_http_request_new();

  mupnp_http_request_seturi(ssdpReq, "*");
  mupnp_http_request_setversion(ssdpReq, MUPNP_HTTP_VER11);
  mupnp_http_request_setcontentlength(ssdpReq, 0);

  mupnp_log_debug_l4("Leaving...\n");

  return ssdpReq;
}

/****************************************
* mupnp_ssdprequest_delete
****************************************/

void mupnp_ssdprequest_delete(mUpnpSSDPRequest* ssdpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_delete(ssdpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdprequest_setleasetime
****************************************/

void mupnp_ssdprequest_setleasetime(mUpnpSSDPRequest* ssdpReq, mUpnpTime value)
{
  char buf[sizeof(MUPNP_HTTP_MAX_AGE) + 1 + MUPNP_STRING_INTEGER_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  sprintf(buf, "%s=%ld", MUPNP_HTTP_MAX_AGE, (long)value);
  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_CACHE_CONTROL, buf);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdprequest_getleasetime
****************************************/

mUpnpTime mupnp_ssdprequest_getleasetime(mUpnpSSDPRequest* ssdpReq)
{
  const char* cacheCtrl;

  mupnp_log_debug_l4("Entering...\n");

  cacheCtrl = mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_CACHE_CONTROL);

  return mupnp_ssdp_getleasetime(cacheCtrl);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdprequest_tostring
****************************************/

char* mupnp_ssdprequest_tostring(mUpnpSSDPRequest* ssdpReq, mUpnpString* ssdpMsg)
{
  mUpnpHttpHeader* header;
  const char* name;
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_addvalue(ssdpMsg, mupnp_http_request_getmethod(ssdpReq));
  mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_SP);
  mupnp_string_addvalue(ssdpMsg, mupnp_http_request_geturi(ssdpReq));
  mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_SP);
  mupnp_string_addvalue(ssdpMsg, mupnp_http_request_getversion(ssdpReq));
  mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_CRLF);

  for (header = mupnp_http_packet_getheaders((mUpnpHttpPacket*)ssdpReq); header != NULL; header = mupnp_http_header_next(header)) {
    name = mupnp_http_header_getname(header);
    value = mupnp_http_header_getvalue(header);
    mupnp_string_addvalue(ssdpMsg, name);
    mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_COLON);
    mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_SP);
    mupnp_string_addvalue(ssdpMsg, value);
    mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_CRLF);
  }
  mupnp_string_addvalue(ssdpMsg, MUPNP_HTTP_CRLF);

  return mupnp_string_getvalue(ssdpMsg);

  mupnp_log_debug_l4("Leaving...\n");
}

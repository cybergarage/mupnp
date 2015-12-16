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

#include <mupnp/util/list.h>
#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_header_new
****************************************/

mUpnpHttpHeader* mupnp_http_header_new()
{
  mUpnpHttpHeader* header;

  mupnp_log_debug_l4("Entering...\n");

  header = (mUpnpHttpHeader*)malloc(sizeof(mUpnpHttpHeader));

  if (NULL != header) {
    mupnp_list_node_init((mUpnpList*)header);
    header->name = mupnp_string_new();
    header->value = mupnp_string_new();
  }

  return header;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_delete
****************************************/

void mupnp_http_header_delete(mUpnpHttpHeader* header)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)header);
  mupnp_string_delete(header->name);
  mupnp_string_delete(header->value);
  free(header);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_setname
****************************************/

void mupnp_http_header_setname(mUpnpHttpHeader* header, const char* name)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(header->name, name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_getname
****************************************/

const char* mupnp_http_header_getname(mUpnpHttpHeader* header)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(header->name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_setvalue
****************************************/

void mupnp_http_header_setvalue(mUpnpHttpHeader* header, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(header->value, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_getvalue
****************************************/

const char* mupnp_http_header_getvalue(mUpnpHttpHeader* header)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(header->value);

  mupnp_log_debug_l4("Leaving...\n");
}

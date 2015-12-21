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

#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_headerlist_new
****************************************/

mUpnpHttpHeaderList* mupnp_http_headerlist_new()
{
  mUpnpHttpHeaderList* headerList;

  mupnp_log_debug_l4("Entering...\n");

  headerList = (mUpnpHttpHeaderList*)malloc(sizeof(mUpnpHttpHeaderList));

  if (NULL != headerList) {
    mupnp_list_header_init((mUpnpList*)headerList);
    headerList->name = NULL;
    headerList->value = NULL;
  }

  return headerList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_delete
****************************************/

void mupnp_http_headerlist_delete(mUpnpHttpHeaderList* headerList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_headerlist_clear(headerList);
  free(headerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_getheader
****************************************/

mUpnpHttpHeader* mupnp_http_headerlist_get(mUpnpHttpHeaderList* headerList, const char* name)
{
  mUpnpHttpHeader* header;
  const char* headerName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (header = mupnp_http_headerlist_gets(headerList); header != NULL; header = mupnp_http_header_next(header)) {
    headerName = mupnp_http_header_getname(header);
    if (headerName == NULL)
      continue;
    if (mupnp_strcasecmp(headerName, name) == 0)
      return header;
  }

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_setheader
****************************************/

void mupnp_http_headerlist_set(mUpnpHttpHeaderList* headerList, const char* name, const char* value)
{
  mUpnpHttpHeader* header;

  mupnp_log_debug_l4("Entering...\n");

  header = mupnp_http_headerlist_get(headerList, name);
  if (header == NULL) {
    header = mupnp_http_header_new();
    mupnp_http_headerlist_add(headerList, header);
    mupnp_http_header_setname(header, name);
  }

  mupnp_http_header_setvalue(header, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_setheader
****************************************/

const char* mupnp_http_headerlist_getvalue(mUpnpHttpHeaderList* headerList, const char* name)
{
  mUpnpHttpHeader* header;

  mupnp_log_debug_l4("Entering...\n");

  header = mupnp_http_headerlist_get(headerList, name);
  if (header != NULL)
    return mupnp_http_header_getvalue(header);

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

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

#include <mupnp/xml/xml.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_escapechars
****************************************/

char* mupnp_xml_escapechars(mUpnpString* str)
{
  char* fromChars[5];
  char* toChars[5];

  mupnp_log_debug_l4("Entering...\n");

  fromChars[0] = "&";
  fromChars[1] = ">";
  fromChars[2] = "<";
  fromChars[3] = "\"";
  fromChars[4] = "'";

  toChars[0] = "&amp;";
  toChars[1] = "&gt;";
  toChars[2] = "&lt;";
  toChars[3] = "&quot;";
  toChars[4] = "&apos;";

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_replace(str, fromChars, toChars, (sizeof(fromChars) / sizeof(fromChars[0])));
}

char* mupnp_xml_unescapechars(mUpnpString* str)
{
  char* fromChars[5];
  char* toChars[5];

  mupnp_log_debug_l4("Entering...\n");

  fromChars[0] = "&amp;";
  fromChars[1] = "&gt;";
  fromChars[2] = "&lt;";
  fromChars[3] = "&quot;";
  fromChars[4] = "&apos;";

  toChars[0] = "&";
  toChars[1] = ">";
  toChars[2] = "<";
  toChars[3] = "\"";
  toChars[4] = "'";

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_replace(str, fromChars, toChars, (sizeof(fromChars) / sizeof(fromChars[0])));
}

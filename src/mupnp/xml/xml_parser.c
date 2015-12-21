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
#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_parser_new
****************************************/

mUpnpXmlParser* mupnp_xml_parser_new()
{
  mUpnpXmlParser* parser;

  mupnp_log_debug_l4("Entering...\n");

  parser = (mUpnpXmlParser*)malloc(sizeof(mUpnpXmlParser));

  if (NULL != parser) {
    parser->parseResult = false;
  }
  return parser;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_parser_delete
****************************************/

void mupnp_xml_parser_delete(mUpnpXmlParser* parser)
{
  mupnp_log_debug_l4("Entering...\n");

  free(parser);

  mupnp_log_debug_l4("Leaving...\n");
}

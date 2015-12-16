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
* mupnp_xml_attribute_new
****************************************/

mUpnpXmlAttribute* mupnp_xml_attribute_new()
{
  mUpnpXmlAttribute* attr;

  mupnp_log_debug_l4("Entering...\n");

  attr = (mUpnpXmlAttribute*)malloc(sizeof(mUpnpXmlAttribute));

  if (NULL != attr) {
    mupnp_list_node_init((mUpnpList*)attr);
    attr->name = mupnp_string_new();
    attr->value = mupnp_string_new();
  }
  return attr;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_delete
****************************************/

void mupnp_xml_attribute_delete(mUpnpXmlAttribute* attr)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)attr);
  mupnp_string_delete(attr->name);
  mupnp_string_delete(attr->value);
  free(attr);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setname
****************************************/

void mupnp_xml_attribute_setname(mUpnpXmlAttribute* attr, const char* name)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(attr->name, name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_getname
****************************************/

const char* mupnp_xml_attribute_getname(mUpnpXmlAttribute* attr)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(attr->name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_isname
****************************************/

bool mupnp_xml_attribute_isname(mUpnpXmlAttribute* attr, const char* name)
{
  mupnp_log_debug_l4("Entering...\n");

  return ((mupnp_strcmp(mupnp_xml_attribute_getname(attr), name) == 0) ? true : false);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setvalue
****************************************/

void mupnp_xml_attribute_setvalue(mUpnpXmlAttribute* attr, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(attr->value, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setvalue
****************************************/

const char* mupnp_xml_attribute_getvalue(mUpnpXmlAttribute* attr)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(attr->value);

  mupnp_log_debug_l4("Leaving...\n");
}

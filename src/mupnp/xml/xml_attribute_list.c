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

#include <mupnp/util/log.h>
#include <mupnp/xml/xml.h>

/****************************************
 * mupnp_xml_attributelist_new
 ****************************************/

mUpnpXmlAttributeList* mupnp_xml_attributelist_new(void)
{
  mUpnpXmlAttributeList* attrList;

  mupnp_log_debug_l4("Entering...\n");

  attrList = (mUpnpXmlAttributeList*)malloc(sizeof(mUpnpXmlAttributeList));

  if (NULL != attrList) {
    mupnp_list_header_init((mUpnpList*)attrList);
    attrList->name = NULL;
    attrList->value = NULL;
  }

  return attrList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_attributelist_delete
 ****************************************/

void mupnp_xml_attributelist_delete(mUpnpXmlAttributeList* attrList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_xml_attributelist_clear(attrList);
  free(attrList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_attributelist_getattribute
 ****************************************/

mUpnpXmlAttribute* mupnp_xml_attributelist_get(mUpnpXmlAttributeList* attrList, const char* name)
{
  mUpnpXmlAttribute* attr;
  const char* attrName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (attr = mupnp_xml_attributelist_gets(attrList); attr != NULL; attr = mupnp_xml_attribute_next(attr)) {
    attrName = mupnp_xml_attribute_getname(attr);
    if (attrName == NULL)
      continue;
    if (mupnp_strcasecmp(attrName, name) == 0)
      return attr;
  }

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_attributelist_setattribute
 ****************************************/

bool mupnp_xml_attributelist_set(mUpnpXmlAttributeList* attrList, const char* name, const char* value)
{
  mUpnpXmlAttribute* attr;
  bool isNewAttr = false;

  mupnp_log_debug_l4("Entering...\n");

  if ((NULL == attrList) || (NULL == name))
    return false;

  attr = mupnp_xml_attributelist_get(attrList, name);
  if (attr == NULL) {
    attr = mupnp_xml_attribute_new();
    if (attr == NULL) {
      mupnp_log_debug_s("Memory allocation failure (XML attribute)\n");
      return false;
    }
    mupnp_xml_attribute_setname(attr, name);
    if (mupnp_xml_attribute_getname(attr) == NULL) {
      mupnp_log_debug_s("Memory allocation failure (XML attribute name)\n");
      mupnp_xml_attribute_delete(attr);
      return false;
    }
    mupnp_xml_attributelist_add(attrList, attr);
    isNewAttr = true;
  }

  mupnp_xml_attribute_setvalue(attr, value);
  if ((NULL != value) && (mupnp_xml_attribute_getvalue(attr) == NULL)) {
    mupnp_log_debug_s("Memory allocation failure (XML attribute value)\n");
    if (isNewAttr)
      mupnp_xml_attribute_delete(attr);
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_xml_attributelist_getvalue
 ****************************************/

const char* mupnp_xml_attributelist_getvalue(mUpnpXmlAttributeList* attrList, const char* name)
{
  mUpnpXmlAttribute* attr;

  mupnp_log_debug_l4("Entering...\n");

  attr = mupnp_xml_attributelist_get(attrList, name);
  if (attr == NULL)
    return NULL;
  return mupnp_xml_attribute_getvalue(attr);

  mupnp_log_debug_l4("Leaving...\n");
}

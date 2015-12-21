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
* mupnp_xml_nodelist_new
****************************************/

mUpnpXmlNodeList* mupnp_xml_nodelist_new()
{
  mUpnpXmlNodeList* nodeList;

  mupnp_log_debug_l4("Entering...\n");

  nodeList = (mUpnpXmlNodeList*)malloc(sizeof(mUpnpXmlNodeList));

  if (NULL != nodeList) {
    mupnp_list_header_init((mUpnpList*)nodeList);

    nodeList->name = NULL;
    nodeList->value = NULL;
    nodeList->attrList = NULL;
    nodeList->nodeList = NULL;
    nodeList->parentNode = NULL;
    nodeList->userData = NULL;
    nodeList->userDataDestructorFunc = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return nodeList;
}

/****************************************
* mupnp_xml_nodelist_delete
****************************************/

void mupnp_xml_nodelist_delete(mUpnpXmlNodeList* nodeList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_xml_nodelist_clear(nodeList);
  free(nodeList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_nodelist_getbyname
****************************************/

mUpnpXmlNode* mupnp_xml_nodelist_getbyname(mUpnpXmlNodeList* nodeList, const char* name)
{
  mUpnpXmlNode* node;
  char* nodeName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (node = mupnp_xml_nodelist_gets(nodeList); node != NULL; node = mupnp_xml_node_next(node)) {
    nodeName = mupnp_xml_node_getname(node);
    if (nodeName == NULL)
      continue;
    if (mupnp_strcasecmp(nodeName, name) == 0)
      return node;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_xml_nodelist_getbyxpath
****************************************/

mUpnpXmlNode* mupnp_xml_nodelist_getbyxpath(mUpnpXmlNodeList* nodeList, const char* name)
{
  mUpnpXmlNode* node;
  char* nodeName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (node = mupnp_xml_nodelist_gets(nodeList); node != NULL; node = mupnp_xml_node_next(node)) {
    nodeName = mupnp_xml_node_getname(node);
    if (nodeName == NULL)
      continue;
    if (mupnp_strcasecmp(nodeName, name) == 0)
      return node;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

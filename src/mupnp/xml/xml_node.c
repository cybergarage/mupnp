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
* mupnp_xml_node_new
****************************************/

mUpnpXmlNode* mupnp_xml_node_new()
{
  mUpnpXmlNode* node;

  mupnp_log_debug_l4("Entering...\n");

  node = (mUpnpXmlNode*)malloc(sizeof(mUpnpXmlNode));

  if (NULL != node) {
    mupnp_list_node_init((mUpnpList*)node);

    node->name = mupnp_string_new();
    node->value = mupnp_string_new();
    node->attrList = mupnp_xml_attributelist_new();
    node->nodeList = mupnp_xml_nodelist_new();
    node->parentNode = NULL;
    node->userData = NULL;
    node->userDataDestructorFunc = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return node;
}

/****************************************
* mupnp_xml_node_delete
****************************************/

void mupnp_xml_node_delete(mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)node);
  mupnp_string_delete(node->name);
  mupnp_string_delete(node->value);
  mupnp_xml_attributelist_delete(node->attrList);
  mupnp_xml_nodelist_delete(node->nodeList);
  if (node->userDataDestructorFunc != NULL)
    node->userDataDestructorFunc(node->userData);
  free(node);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_node_setparentnode
****************************************/

mUpnpXmlNode* mupnp_xml_node_getrootnode(mUpnpXmlNode* node)
{
  mUpnpXmlNode* rootNode;
  mUpnpXmlNode* parentNode;

  mupnp_log_debug_l4("Entering...\n");

  rootNode = NULL;
  parentNode = mupnp_xml_node_getparentnode(node);
  while (parentNode != NULL) {
    rootNode = parentNode;
    parentNode = mupnp_xml_node_getparentnode(rootNode);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return rootNode;
}

/****************************************
* mupnp_xml_node_addchildnode
****************************************/

void mupnp_xml_node_addchildnode(mUpnpXmlNode* node, mUpnpXmlNode* cnode)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_xml_nodelist_add(node->nodeList, cnode);
  mupnp_xml_node_setparentnode(cnode, node);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_node_setchildnode
****************************************/

void mupnp_xml_node_setchildnode(mUpnpXmlNode* node, const char* name, const char* value)
{
  mUpnpXmlNode* childNode;

  mupnp_log_debug_l4("Entering...\n");

  childNode = mupnp_xml_node_getchildnode(node, name);
  if (childNode != NULL) {
    mupnp_xml_node_setvalue(childNode, value);
    return;
  }
  childNode = mupnp_xml_node_new();
  mupnp_xml_node_setname(childNode, name);
  mupnp_xml_node_setvalue(childNode, value);

  mupnp_xml_node_addchildnode(node, childNode);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_node_removechildnode
 ****************************************/

bool mupnp_xml_node_removechildnode(mUpnpXmlNode* node, const char* name)
{
  mUpnpXmlNode* childNode;

  mupnp_log_debug_l4("Entering...\n");

  childNode = mupnp_xml_node_getchildnode(node, name);
  if (!childNode)
    return false;

  mupnp_xml_node_remove(childNode);

  return true;
}

/***************************************************************************** 
* Get childnode with some specific namespace prefix, or ignore namespace prefix.
* If ns is NULL, name string must match completely
*****************************************************************************/

mUpnpXmlNode* mupnp_xml_node_getchildnodewithnamespace(mUpnpXmlNode* node, const char* name, const char* ns, bool ignoreNs)
{
  char* nameWithPrefix = NULL;
  size_t nameLen = 0;
  ssize_t nameIdx;
  mUpnpXmlNode* result = NULL;
  mUpnpXmlNode* child = NULL;
  char* nodeName = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (node == NULL)
    return NULL;

  if (name == NULL)
    return NULL;

  if (ignoreNs == true) {
    for (child = mupnp_xml_node_getchildnodelist(node); child != NULL; child = mupnp_xml_node_next(child)) {
      nodeName = mupnp_xml_node_getname(child);
      if (nodeName == NULL)
        continue;

      nameIdx = mupnp_strstr(nodeName, ":");
      if (nameIdx < 0) {
        /* No prefix (no ':') */
        if (mupnp_strcasecmp(nodeName, name) == 0)
          return child;
      }
      else {
        if (mupnp_strcasecmp(&(nodeName[nameIdx + 1]), name) == 0)
          return child;
      }
    }

    return NULL;
  }
  else {
    if (ns == NULL) {
      /* When ns is NULL, this works like normal ..._getchildnode */
      return mupnp_xml_node_getchildnode(node, name);
    }

    nameLen = mupnp_strlen(name) + mupnp_strlen(ns) + 1; /* Not including the terminating \0 */
    nameWithPrefix = (char*)malloc(nameLen + 1);
    if (nameWithPrefix == NULL)
      return NULL;

#if defined(HAVE_SNPRINTF)
    snprintf(nameWithPrefix, nameLen + 1, "%s:%s", ns, name);
#else
    sprintf(nameWithPrefix, "%s:%s", ns, name);
#endif

    result = mupnp_xml_node_getchildnode(node, nameWithPrefix);

    free(nameWithPrefix);
    return result;
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_node_getchildnodevalue
****************************************/

const char* mupnp_xml_node_getchildnodevalue(mUpnpXmlNode* node, const char* name)
{
  mUpnpXmlNode* childNode;

  mupnp_log_debug_l4("Entering...\n");

  childNode = mupnp_xml_node_getchildnode(node, name);
  if (childNode != NULL)
    return mupnp_xml_node_getvalue(childNode);

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_xml_node_removeattribute
****************************************/

bool mupnp_xml_node_removeattribute(mUpnpXmlNode* node, const char* name)
{
  mUpnpXmlAttribute* attr;

  if (!node || !name)
    return false;

  attr = mupnp_xml_node_getattribute(node, name);
  if (!attr)
    return false;

  mupnp_xml_attribute_remove(attr);

  return true;
}

/****************************************
 * mupnp_xml_node_copy
 ****************************************/

void mupnp_xml_node_copy(mUpnpXmlNode* dstNode, mUpnpXmlNode* srcNode)
{
  mUpnpXmlAttribute* attr;
  mUpnpXmlNode* dstChildNode;
  mUpnpXmlNode* srcChildNode;

  mupnp_log_debug_l4("Entering...\n");

  if (!dstNode || !srcNode)
    return;

  mupnp_xml_node_setname(dstNode, mupnp_xml_node_getname(srcNode));
  mupnp_xml_node_setvalue(dstNode, mupnp_xml_node_getvalue(srcNode));

  for (attr = mupnp_xml_node_getattributes(srcNode); attr != NULL; attr = mupnp_xml_attribute_next(attr))
    mupnp_xml_node_setattribute(dstNode, mupnp_xml_attribute_getname(attr), mupnp_xml_attribute_getvalue(attr));

  for (srcChildNode = mupnp_xml_node_getchildnodes(srcNode); srcChildNode != NULL; srcChildNode = mupnp_xml_node_next(srcChildNode)) {
    dstChildNode = mupnp_xml_node_new();
    mupnp_xml_node_copy(dstChildNode, srcChildNode);
    mupnp_xml_node_addchildnode(dstNode, dstChildNode);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_node_attribute_tostring
****************************************/

static char* mupnp_xml_node_attribute_tostring(mUpnpXmlNode* node, mUpnpString* str)
{
  mUpnpXmlAttribute* attr;
  const char* name;
  const char* value;
  mUpnpString* valueStr;

  mupnp_log_debug_l4("Entering...\n");

  valueStr = mupnp_string_new();
  if (valueStr == NULL)
    return NULL;

  for (attr = mupnp_xml_node_getattributes(node); attr != NULL; attr = mupnp_xml_attribute_next(attr)) {
    name = mupnp_xml_attribute_getname(attr);
    value = mupnp_xml_attribute_getvalue(attr);

    mupnp_string_setvalue(valueStr, value);
    mupnp_xml_escapechars(valueStr);

    /* All the following functions return NULL only when memory 
		   allocation fails, so we can check them all */
    if (!mupnp_string_naddvalue(str, " ", 1) || !mupnp_string_addvalue(str, name) || !mupnp_string_naddvalue(str, "=\"", 2) || !mupnp_string_addvalue(str, mupnp_string_getvalue(valueStr)) || !mupnp_string_naddvalue(str, "\"", 1)) {
      /* Memory allocation failed */
      mupnp_string_delete(valueStr);
      return NULL;
    }
  }
  mupnp_string_delete(valueStr);

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_xml_node_tostring_indent
****************************************/

static char* mupnp_xml_node_tostring_indent(mUpnpXmlNode* node, int indentLevel, bool withChildNode, mUpnpString* str)
{
  char* name;
  char* value;
  mUpnpString* valueStr;
  mUpnpXmlNode* childNode;

  mupnp_log_debug_l4("Entering...\n");

  name = mupnp_xml_node_getname(node);
  value = mupnp_xml_node_getvalue(node);

  if (mupnp_xml_node_haschildnodes(node) == false || withChildNode == false) {
    mupnp_string_addrepvalue(str, MUPNP_XML_INDENT_STRING, indentLevel);
    if (!mupnp_string_naddvalue(str, "<", 1) || !mupnp_string_addvalue(str, name) || !mupnp_xml_node_attribute_tostring(node, str))
      /* Memory allocation failed */
      return NULL;

    valueStr = mupnp_string_new();
    if (!valueStr)
      /* Memory allocation failed */
      return NULL;

    mupnp_string_setvalue(valueStr, value);
    mupnp_xml_escapechars(valueStr);

    if (!mupnp_string_naddvalue(str, ">", 1) || !mupnp_string_addvalue(str, mupnp_string_getvalue(valueStr)) || !mupnp_string_naddvalue(str, "</", 2) || !mupnp_string_addvalue(str, name) || !mupnp_string_naddvalue(str, ">", 1) || !mupnp_string_addvalue(str, "\n")) {
      /* Memory allocation failed */
      mupnp_string_delete(valueStr);
      return NULL;
    }

    mupnp_string_delete(valueStr);

    return mupnp_string_getvalue(str);
  }

  mupnp_string_addrepvalue(str, MUPNP_XML_INDENT_STRING, indentLevel);
  if (!mupnp_string_naddvalue(str, "<", 1) || !mupnp_string_addvalue(str, name) || !mupnp_xml_node_attribute_tostring(node, str) || !mupnp_string_naddvalue(str, ">", 1) || !mupnp_string_addvalue(str, "\n"))
    /* Memory allocation failed */
    return NULL;

  for (childNode = mupnp_xml_node_getchildnodes(node); childNode != NULL; childNode = mupnp_xml_node_next(childNode))
    if (!mupnp_xml_node_tostring_indent(childNode, indentLevel + 1, true, str))
      /* Memory allocation failed */
      return NULL;

  mupnp_string_addrepvalue(str, MUPNP_XML_INDENT_STRING, indentLevel);
  if (!mupnp_string_naddvalue(str, "</", 2) || !mupnp_string_addvalue(str, name) || !mupnp_string_naddvalue(str, ">", 1) || !mupnp_string_addvalue(str, "\n"))
    /* Memory allocation failed */
    return NULL;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_xml_node_tostring
****************************************/

char* mupnp_xml_node_tostring(mUpnpXmlNode* node, bool withChildNode, mUpnpString* str)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_xml_node_tostring_indent(node, 0, withChildNode, str);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_node_print
 ****************************************/

void mupnp_xml_node_print(mUpnpXmlNode* node)
{
  mUpnpString* str = mupnp_string_new();
  mupnp_xml_node_tostring(node, true, str);
  printf("%s", mupnp_string_getvalue(str));
  mupnp_string_delete(str);
}

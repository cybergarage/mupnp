/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cxml_node.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*	12/13/07 Aapo Makela
*		- Fixes to work in out-of-memory situations
*	05/22/08
*		- Added mupnp_xml_nodelist_getbyname() instead of mupnp_xml_nodelist_get().
*		- Added mupnp_xml_node_getchildnodebyname() instead of mupnp_xml_node_getchildnode().
*		- Added mupnp_xml_nodelist_getbyxpath() and mupnp_xml_node_getchildnodebyxpath() to get a node simply.
*		- Deprecated mupnp_xml_nodelist_get() and mupnp_xml_node_getchildnode().*
*
******************************************************************/

#include <mupnp/xml/xml.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_node_new
****************************************/

CgXmlNode *mupnp_xml_node_new()
{
	CgXmlNode *node;
	
	mupnp_log_debug_l4("Entering...\n");

	node = (CgXmlNode *)malloc(sizeof(CgXmlNode));

	if ( NULL != node )
	{
		mupnp_list_node_init((CgList *)node);

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

void mupnp_xml_node_delete(CgXmlNode *node)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)node);
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

CgXmlNode *mupnp_xml_node_getrootnode(CgXmlNode *node)
{
	CgXmlNode *rootNode;
	CgXmlNode *parentNode;
	
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

void mupnp_xml_node_addchildnode(CgXmlNode *node, CgXmlNode *cnode)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_xml_nodelist_add(node->nodeList, cnode);
	mupnp_xml_node_setparentnode(cnode, node);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_node_setchildnode
****************************************/

void mupnp_xml_node_setchildnode(CgXmlNode *node, const char *name, const char *value)
{
	CgXmlNode *childNode;

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

BOOL mupnp_xml_node_removechildnode(CgXmlNode *node, const char *name)
{
	CgXmlNode *childNode;
  
	mupnp_log_debug_l4("Entering...\n");
  
	childNode = mupnp_xml_node_getchildnode(node, name);
	if (!childNode)
    return FALSE;

  mupnp_xml_node_remove(childNode);
  
  return TRUE;
}

/***************************************************************************** 
* Get childnode with some specific namespace prefix, or ignore namespace prefix.
* If ns is NULL, name string must match completely
*****************************************************************************/

CgXmlNode *mupnp_xml_node_getchildnodewithnamespace(CgXmlNode *node, const char *name, const char *ns, BOOL ignoreNs)
{
	char *nameWithPrefix = NULL;
	size_t nameLen = 0;
	ssize_t nameIdx;
	CgXmlNode *result = NULL;
	CgXmlNode *child = NULL;
	char *nodeName = NULL;

	mupnp_log_debug_l4("Entering...\n");

	if (node == NULL)
		return NULL;

	if (name == NULL)
		return NULL;

	if (ignoreNs == TRUE)
	{
		for (child = mupnp_xml_node_getchildnodelist(node); child != NULL; child = mupnp_xml_node_next(child))
		{
			nodeName = mupnp_xml_node_getname(child);
			if (nodeName == NULL)
				continue;

			nameIdx = mupnp_strstr(nodeName, ":");
			if (nameIdx < 0)
			{
				/* No prefix (no ':') */
				if (mupnp_strcasecmp(nodeName, name) == 0)
					return child;
			}
			else
			{
				if (mupnp_strcasecmp(&(nodeName[nameIdx+1]), name) == 0)
					return child;
			}
		}

		return NULL;
	}
	else
	{
		if (ns == NULL)
		{
			/* When ns is NULL, this works like normal ..._getchildnode */
			return mupnp_xml_node_getchildnode(node, name);
		}

		nameLen = mupnp_strlen(name) + mupnp_strlen(ns) + 1; /* Not including the terminating \0 */
		nameWithPrefix = (char*) malloc(nameLen + 1);
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

const char *mupnp_xml_node_getchildnodevalue(CgXmlNode *node, const char *name)
{
	CgXmlNode *childNode;

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

BOOL mupnp_xml_node_removeattribute(CgXmlNode *node, const char *name)
{
	CgXmlAttribute *attr;

	if (!node || !name)
		return FALSE;

	attr = mupnp_xml_node_getattribute(node, name);
	if (!attr)
		return FALSE;

	mupnp_xml_attribute_remove(attr);
  
  return TRUE;
}

/****************************************
 * mupnp_xml_node_copy
 ****************************************/

void mupnp_xml_node_copy(CgXmlNode *dstNode, CgXmlNode *srcNode)
{
	CgXmlAttribute *attr;
	CgXmlNode *dstChildNode;
	CgXmlNode *srcChildNode;
	
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

static char *mupnp_xml_node_attribute_tostring(CgXmlNode *node, CgString *str)
{
	CgXmlAttribute *attr;
	const char *name;
	const char *value;
	CgString *valueStr;
	
	mupnp_log_debug_l4("Entering...\n");

	valueStr = mupnp_string_new();
	if (valueStr == NULL) return NULL;

	for (attr = mupnp_xml_node_getattributes(node); attr != NULL; attr = mupnp_xml_attribute_next(attr)) {
		name = mupnp_xml_attribute_getname(attr);
		value = mupnp_xml_attribute_getvalue(attr);
		
		mupnp_string_setvalue(valueStr, value);
		mupnp_xml_escapechars(valueStr);

		/* All the following functions return NULL only when memory 
		   allocation fails, so we can check them all */
		if (!mupnp_string_naddvalue(str, " ", 1) || 
		    !mupnp_string_addvalue(str, name) ||
		    !mupnp_string_naddvalue(str, "=\"", 2) ||
		    !mupnp_string_addvalue(str, mupnp_string_getvalue(valueStr)) ||
		    !mupnp_string_naddvalue(str, "\"", 1))
		{
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

static char *mupnp_xml_node_tostring_indent(CgXmlNode *node, int indentLevel, BOOL withChildNode, CgString *str)
{
	char *name;
	char *value;
	CgString *valueStr;
	CgXmlNode *childNode;
	
	mupnp_log_debug_l4("Entering...\n");

	name = mupnp_xml_node_getname(node);
	value = mupnp_xml_node_getvalue(node);

	if (mupnp_xml_node_haschildnodes(node) == FALSE || withChildNode == FALSE) {
		mupnp_string_addrepvalue(str, CG_XML_INDENT_STRING, indentLevel);
		if (!mupnp_string_naddvalue(str, "<", 1) ||
		    !mupnp_string_addvalue(str, name) ||
		    !mupnp_xml_node_attribute_tostring(node, str))
			/* Memory allocation failed */
			return NULL;
		
		valueStr = mupnp_string_new();
		if (!valueStr)
			/* Memory allocation failed */
			return NULL;
		
		mupnp_string_setvalue(valueStr, value);
		mupnp_xml_escapechars(valueStr);

		if (!mupnp_string_naddvalue(str, ">", 1) ||
		    !mupnp_string_addvalue(str, mupnp_string_getvalue(valueStr)) ||
		    !mupnp_string_naddvalue(str, "</", 2) ||
		    !mupnp_string_addvalue(str, name) ||
		    !mupnp_string_naddvalue(str, ">", 1) ||
		    !mupnp_string_addvalue(str, "\n"))
		{
			/* Memory allocation failed */
			mupnp_string_delete(valueStr);
			return NULL;
		}

		mupnp_string_delete(valueStr);
		
		return mupnp_string_getvalue(str);
	}

	mupnp_string_addrepvalue(str, CG_XML_INDENT_STRING, indentLevel);
	if (!mupnp_string_naddvalue(str, "<", 1) ||
	    !mupnp_string_addvalue(str, name) ||
	    !mupnp_xml_node_attribute_tostring(node, str) ||
	    !mupnp_string_naddvalue(str, ">", 1) ||
	    !mupnp_string_addvalue(str, "\n"))
		/* Memory allocation failed */
		return NULL;

	for (childNode = mupnp_xml_node_getchildnodes(node); childNode != NULL; childNode = mupnp_xml_node_next(childNode))
		if (!mupnp_xml_node_tostring_indent(childNode, indentLevel+1, TRUE, str))
			/* Memory allocation failed */
			return NULL;

	mupnp_string_addrepvalue(str, CG_XML_INDENT_STRING, indentLevel);
	if (!mupnp_string_naddvalue(str, "</", 2) ||
	    !mupnp_string_addvalue(str, name) ||
	    !mupnp_string_naddvalue(str, ">", 1) ||
	    !mupnp_string_addvalue(str, "\n"))
		/* Memory allocation failed */
		return NULL;

	mupnp_log_debug_l4("Leaving...\n");
	
	return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_xml_node_tostring
****************************************/

char *mupnp_xml_node_tostring(CgXmlNode *node, BOOL withChildNode, CgString *str)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_xml_node_tostring_indent(node, 0, withChildNode, str);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_xml_node_print
 ****************************************/

void  mupnp_xml_node_print(CgXmlNode *node)
{
	CgString *str = mupnp_string_new();
	mupnp_xml_node_tostring(node, TRUE, str);
	printf("%s", mupnp_string_getvalue(str));
	mupnp_string_delete(str);
}



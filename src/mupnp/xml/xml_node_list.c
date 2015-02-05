/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_node_list.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <mupnp/xml/xml.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_nodelist_new
****************************************/

CgXmlNodeList *mupnp_xml_nodelist_new()
{
	CgXmlNodeList *nodeList;

	mupnp_log_debug_l4("Entering...\n");

	nodeList = (CgXmlNodeList *)malloc(sizeof(CgXmlNodeList));

	if ( NULL != nodeList )
	{
		mupnp_list_header_init((CgList *)nodeList);

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

void mupnp_xml_nodelist_delete(CgXmlNodeList *nodeList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_xml_nodelist_clear(nodeList);
	free(nodeList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_nodelist_getbyname
****************************************/

CgXmlNode *mupnp_xml_nodelist_getbyname(CgXmlNodeList *nodeList, const char *name)
{
	CgXmlNode *node;
	char *nodeName;
		
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

CgXmlNode *mupnp_xml_nodelist_getbyxpath(CgXmlNodeList *nodeList, const char *name)
{
	CgXmlNode *node;
	char *nodeName;
		
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

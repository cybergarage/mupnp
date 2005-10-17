/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cxml_parser_expat.c
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/****************************************
*	Compiler Switch (BEGIN)
****************************************/

#if !defined(CG_XMLPARSER_LIBXML2)

/****************************************
* Header Files
****************************************/

#include <cybergarage/xml/cxml.h>

#if defined(WIN32)
#define XML_STATIC
#endif

#if !defined(WIN32)
#define XMLCALL
#endif

#include <expat.h>

/****************************************
* cg_xml_parse (Expat)
****************************************/

typedef struct _CgExpatData {
	CgXmlNode *rootNode;
	CgXmlNode *currNode;
} CgExpatData;

static void XMLCALL cg_expat_element_start(void *userData, const char *el, const char **attr)
{
	CgExpatData *expatData;
	CgXmlNode *node;
	int n;

	expatData = (CgExpatData *)userData;

	node = cg_xml_node_new();
	cg_xml_node_setname(node, (char *)el);

	for (n = 0; attr[n]; n += 2)
		cg_xml_node_setattribute(node, (char *)attr[n], (char *)attr[n+1]);

	if (expatData->rootNode != NULL) {
		if (expatData->currNode != NULL)
			cg_xml_node_addchildnode(expatData->currNode, node);
		else
			cg_xml_node_addchildnode(expatData->rootNode, node);
	}
	else
		expatData->rootNode = node;

	expatData->currNode = node;
}

static void XMLCALL cg_expat_element_end(void *userData, const char *el)
{
	CgExpatData *expatData = (CgExpatData *)userData;
	if (expatData->currNode != NULL)
		expatData->currNode = cg_xml_node_getparentnode(expatData->currNode);
}

static void XMLCALL cg_expat_character_data(void *userData, const XML_Char *s, int len)
{
	CgExpatData *expatData;

	expatData = (CgExpatData *)userData;

	if (expatData->currNode != NULL)
		cg_xml_node_naddvalue(expatData->currNode, (char *)s, len);
}

BOOL cg_xml_parse(CgXmlParser *parser, CgXmlNodeList *nodeList, char *data, int len)
{
	XML_Parser p;
	CgExpatData expatData;

	p = XML_ParserCreate(NULL);
	if (!p)
		return FALSE;

	expatData.rootNode = NULL;
	expatData.currNode = NULL;
	XML_SetUserData(p, &expatData);
	XML_SetElementHandler(p, cg_expat_element_start, cg_expat_element_end);
	XML_SetCharacterDataHandler(p, cg_expat_character_data);

	parser->parseResult = XML_Parse(p, data, len, 1);
	XML_ParserFree(p);

	if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
		if (expatData.rootNode != NULL)
			cg_xml_node_delete(expatData.rootNode);
		return FALSE;
	}

	cg_xml_nodelist_add(nodeList, expatData.rootNode);

	return TRUE;
}

/****************************************
* Compiler Switch (END)
****************************************/

#endif

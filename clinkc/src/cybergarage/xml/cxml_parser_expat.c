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
*	10/31/05
*		- Added performance measurement functionality under
*		  CG_SHOW_TIMINGS macro (not enabled by default)
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

#ifdef CG_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

extern long int cg_total_elapsed_time;
#endif

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
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
	
	gettimeofday(&start_time, NULL);
#endif	
	p = XML_ParserCreate(NULL);
	if (!p)
		return FALSE;
	/* Fix to get expat parser to work with DLink-routers */
	if (data[len-1] == 0) len--;
	
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

#ifdef CG_SHOW_TIMINGS
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &start_time, &elapsed_time);
	printf("Parsing XML completed. Elapsed time: "
	       "%ld msec\n", ((elapsed_time.tv_sec*1000) + 
			      (elapsed_time.tv_usec/1000)));
	cg_total_elapsed_time += (elapsed_time.tv_sec*1000000)+
				 (elapsed_time.tv_usec);
	printf("Total elapsed time: %ld msec\n", cg_total_elapsed_time / 1000);
#endif	
	return TRUE;
}

/****************************************
* Compiler Switch (END)
****************************************/

#endif

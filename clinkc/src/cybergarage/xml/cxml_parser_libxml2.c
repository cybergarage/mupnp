/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File:	cxml_parser_libxml2.c
*
*	Revision;
*
*	08/19/05
*		- first version
*		- The functions are created using a contribution from
*		  Smolander Visa <visa.smolander@nokia.com> to CyberLink for C++.
*       09/07/05
*               - Thanks for  Smolander Visa <visa.smolander@nokia.com>
*               - Fixed some bugs to parser name spaces.
*
*       10/31/05
*               - Added performance measurement functionality under
*                 CG_SHOW_TIMINGS macro (not enabled by default)
*
*	11/18/05
*		- Fixed to use SAX parser and parse predefined XML entities
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/****************************************
* Compiler Switch (BEGIN)
****************************************/

#if defined(CG_XMLPARSER_LIBXML2)

/****************************************
* Header Files
****************************************/

#include <cybergarage/xml/cxml.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef CG_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

extern long int cg_total_elapsed_time;
#endif


static xmlEntityPtr cg_libxml2_get_entity(void *user_data, 
					  const xmlChar *name);
static void cg_libxml2_characters(void *user_data,
				  const xmlChar *ch,
				  int len);
static void cg_libxml2_start_element(void *user_data,
				     const xmlChar *name,
				     const xmlChar **attrs);
static void cg_libxml2_end_element(void *user_data,
				   const xmlChar *name);			     

typedef struct _CgLibxml2Data {
	CgXmlNode *rootNode;
	CgXmlNode *currNode;
} CgLibxml2Data;

static xmlSAXHandler cg_libxml2_handler =
{
	.startElement = cg_libxml2_start_element,
	.endElement = cg_libxml2_end_element,
	.getEntity = cg_libxml2_get_entity,
	.characters = cg_libxml2_characters
};
	

static void cg_libxml2_start_element(void *user_data,
				     const xmlChar *name,
				     const xmlChar **attrs)
{
	CgLibxml2Data *libxml2Data;
	CgXmlNode *node;
	int n;

	libxml2Data = (CgLibxml2Data *)user_data;

	node = cg_xml_node_new();
	cg_xml_node_setname(node, (char *)name);

	if (attrs != NULL)
	{
		for (n = 0; attrs[n]; n += 2)
			cg_xml_node_setattribute(node, (char *)attrs[n], (char *)attrs[n+1]);
	}

	if (libxml2Data->rootNode != NULL) {
		if (libxml2Data->currNode != NULL)
			cg_xml_node_addchildnode(libxml2Data->currNode, node);
		else
			cg_xml_node_addchildnode(libxml2Data->rootNode, node);
	}
	else
		libxml2Data->rootNode = node;

	libxml2Data->currNode = node;
}

static void cg_libxml2_end_element(void *user_data,
				   const xmlChar *name)
{
	CgLibxml2Data *libxml2Data = (CgLibxml2Data *)user_data;
	if (libxml2Data->currNode != NULL)
		libxml2Data->currNode = cg_xml_node_getparentnode(libxml2Data->currNode);
}

static void cg_libxml2_characters(void *user_data,
				  const xmlChar *ch,
				  int len)
{
	CgLibxml2Data *libxml2Data;

	libxml2Data = (CgLibxml2Data *)user_data;

	if (libxml2Data->currNode != NULL)
		cg_xml_node_naddvalue(libxml2Data->currNode, (char *)ch, len);
}

static xmlEntityPtr cg_libxml2_get_entity(void *user_data, const xmlChar *name) 
{
	return xmlGetPredefinedEntity(name);
}

/****************************************
* cg_xml_parse
****************************************/

BOOL cg_xml_parse(CgXmlParser *parser, CgXmlNodeList *nodeList, char *data, int len)
{
	CgLibxml2Data libxml2Data;
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
	
	gettimeofday(&start_time, NULL);
#endif	
	
	libxml2Data.rootNode = NULL;
	libxml2Data.currNode = NULL;
	
	if (xmlSAXUserParseMemory(&cg_libxml2_handler, &libxml2Data, data, len) < 0)
	{
		if (libxml2Data.rootNode != NULL)
			cg_xml_node_delete(libxml2Data.rootNode);
		
		return FALSE;
	}
	
	cg_xml_nodelist_add(nodeList, libxml2Data.rootNode);
	
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

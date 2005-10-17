/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cxml_parser.c
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <cybergarage/xml/cxml.h>
#include <cybergarage/http/chttp.h>

/****************************************
* cg_xml_parser_new
****************************************/

CgXmlParser *cg_xml_parser_new()
{
	CgXmlParser *parser = (CgXmlParser *)malloc(sizeof(CgXmlParser));
	parser->parseResult = FALSE;
	return parser;
}

/****************************************
* cg_xml_parser_delete
****************************************/

void cg_xml_parser_delete(CgXmlParser *parser)
{
	free(parser);
}

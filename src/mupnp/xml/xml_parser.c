/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cxml_parser.c
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <mupnp/xml/xml.h>
#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_parser_new
****************************************/

CgXmlParser *mupnp_xml_parser_new()
{
	CgXmlParser *parser;

	mupnp_log_debug_l4("Entering...\n");

	parser = (CgXmlParser *)malloc(sizeof(CgXmlParser));

	if ( NULL != parser )
	{
		parser->parseResult = FALSE;
	}
	return parser;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_parser_delete
****************************************/

void mupnp_xml_parser_delete(CgXmlParser *parser)
{
	mupnp_log_debug_l4("Entering...\n");

	free(parser);

	mupnp_log_debug_l4("Leaving...\n");
}

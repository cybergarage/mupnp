/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cxml_function.cpp
*
*	Revision:
*
*	04/11/05
*		- first revision
*
******************************************************************/

#include <cybergarage/xml/cxml.h>

/****************************************
* cg_xml_escapechars
****************************************/

char *cg_xml_escapechars(CgString *str)
{
	char *fromChars[5];
	char *toChars[5];
	
	fromChars[0] = "<";
	fromChars[1] = ">";
	fromChars[2] = "&";
	fromChars[3] = "\"";
	fromChars[4] = "'";
	
	toChars[0] = "&lt;";
	toChars[1] = "&gt;";
	toChars[2] = "&amp;";
	toChars[3] = "&quot;";
	toChars[4] = "&apos;";
	
	return cg_string_replace(str, fromChars, toChars, (sizeof(fromChars)/sizeof(fromChars[0])));
}


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
*	10/31/05
*		- Added cg_xml_unescapechars() function for DIDL-lite parsing
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
	
	fromChars[0] = "&";
	fromChars[1] = ">";
	fromChars[2] = "<";
	fromChars[3] = "\"";
	fromChars[4] = "'";
	
	toChars[0] = "&amp;";
	toChars[1] = "&gt;";
	toChars[2] = "&lt;";
	toChars[3] = "&quot;";
	toChars[4] = "&apos;";
	
	return cg_string_replace(str, fromChars, toChars, (sizeof(fromChars)/sizeof(fromChars[0])));
}

char *cg_xml_unescapechars(CgString *str)
{
	char *fromChars[5];
	char *toChars[5];
	
	fromChars[0] = "&amp;";
	fromChars[1] = "&gt;";
	fromChars[2] = "&lt;";
	fromChars[3] = "&dquot;";
	fromChars[4] = "&apos;";
	
	toChars[0] = "&";
	toChars[1] = ">";
	toChars[2] = "<";
	toChars[3] = "\"";
	toChars[4] = "'";
	
	return cg_string_replace(str, fromChars, toChars, (sizeof(fromChars)/sizeof(fromChars[0])));
}



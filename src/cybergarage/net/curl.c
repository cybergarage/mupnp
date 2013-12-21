/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: curl.c
*
*	Revision:
*
*	03/16/05
*		- first revision
*
*
*	10/31/05
*		- cg_net_getmodifierhosturl set the tag begin mark "<" before port
*		  while it must come before "http://"
*
*	19-Jan-06 Aapo Makela
*		- Fixed cg_net_gethosturl to call cg_net_getmodifierhosturl
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/clog.h>

#include <stdio.h>

/****************************************
* cg_net_getmodifierhosturl
****************************************/

const char *cg_net_getmodifierhosturl(const char *host, int port, const char *uri, const char *begin, const char *end, char *buf, size_t bufSize)
{
	BOOL isIPv6Host;
	
	cg_log_debug_l4("Entering...\n");

	isIPv6Host = cg_net_isipv6address(host);
	
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize,
#else
	sprintf(buf,
#endif
		"%shttp://%s%s%s:%d%s%s",
		begin,
		((isIPv6Host == TRUE) ? "[" : ""),
		host,
		((isIPv6Host == TRUE) ? "]" : ""),
		port,
		uri,
		end);
	
  cg_log_debug_l4("Leaving...\n");
	
  return buf;
}

/****************************************
* cg_net_gethosturl
****************************************/

const char *cg_net_gethosturl(const char *host, int port, const char *uri, char *buf, size_t bufSize)
{
	cg_log_debug_l4("Entering...\n");

	return cg_net_getmodifierhosturl(host, port, uri, "", "", buf, bufSize);

	cg_log_debug_l4("Leaving...\n");
}

/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
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
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/net/cinterface.h>

#include <stdio.h>

/****************************************
* cg_net_gethosturl
****************************************/

char *cg_net_gethosturl(char *host, int port, char *uri, char *buf, int bufSize)
{
	BOOL isIPv6Host;
	isIPv6Host = cg_net_isipv6address(host);
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize,
#else
	sprintf(buf,
#endif
		"http://%s%s%s:%d%s",
		((isIPv6Host == TRUE) ? "[" : ""),
		host,
		((isIPv6Host == TRUE) ? "]" : ""),
		port,
		uri);
	return buf;
}

/****************************************
* cg_net_getmodifierhosturl
****************************************/

char *cg_net_getmodifierhosturl(char *host, int port, char *uri, char *buf, int bufSize, char *begin, char *end)
{
	BOOL isIPv6Host;
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
	return buf;
}

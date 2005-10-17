/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cinterface_function.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>

/****************************************
* global variables
****************************************/

static BOOL CG_NET_USE_LOOPBACK_ADDR = FALSE;
static BOOL CG_NET_USE_ONLY_IPV4_ADDR = FALSE;
static BOOL CG_NET_USE_ONLY_IPV6_ADDR = FALSE;

////////////////////////////////////////////////
//	cg_net_isuseaddress
////////////////////////////////////////////////

BOOL cg_net_isuseaddress(char *addr)
{
	if (CG_NET_USE_ONLY_IPV6_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == FALSE)
			return FALSE;
	}
	if (CG_NET_USE_ONLY_IPV4_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == TRUE)
			return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

BOOL cg_net_isipv6address(char *addr)
{
	if (addr == NULL)
		return FALSE;
	if (0 < cg_strchr(addr, ":", 1))
		return TRUE;
	return FALSE;
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

int cg_net_getipv6scopeid(char *addr)
{
	int addrLen;
	int perIdx;
	char scopeIDBuf[8];
	if (cg_net_isipv6address(addr) == FALSE)
		return 0;
	addrLen = cg_strlen(addr);
	perIdx = cg_strchr(addr, "%", 1);
	if (perIdx < 0)
		return 0;
	cg_strncpy(scopeIDBuf, (addr+perIdx+1), sizeof(scopeIDBuf));
	return atoi(scopeIDBuf);
}

/*
////////////////////////////////////////////////
//	GetHostURL
////////////////////////////////////////////////

const char *CyberNet::GetHostURL(const char *host, int port, const char *uri, std::string &buf)
{
	std::string hostStr = host;
	if (IsIPv6Address(host) == true) {
		StripIPv6ScopeID(host, hostStr);
		hostStr = "[";
		hostStr += hostStr.c_str();
		hostStr += "]";
	}
#ifndef NO_USE_OSTRINGSTREAM
	std::ostringstream sbuf;
	sbuf << "http://" << hostStr << ":" << port << uri;
	buf = sbuf.str();
#else
	buf = "http://";
	buf += hostStr;
	buf += ":";
	buf += port;
	buf += uri;
#endif
	return buf.c_str();
}

////////////////////////////////////////////////
//	GetHostURL
////////////////////////////////////////////////

static string ifAddress;

void CyberNet::SetHostInterface(const char *ifaddr)
{
	ifAddress = ifaddr;
}

const char *CyberNet::GetHostInterface()
{
	return ifAddress.c_str();
}

bool CyberNet::HasAssignedHostInterface()
{
	return (0 < ifAddress.length()) ? true : false;
}
*/

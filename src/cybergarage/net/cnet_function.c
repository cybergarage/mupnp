/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cnet_function.c
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
#include <cybergarage/util/clog.h>

/****************************************
* global variables
****************************************/

static BOOL CG_NET_USE_ONLY_IPV4_ADDR = FALSE;
static BOOL CG_NET_USE_ONLY_IPV6_ADDR = FALSE;

////////////////////////////////////////////////
//	cg_net_isuseaddress
////////////////////////////////////////////////

BOOL cg_net_isuseaddress(char *addr)
{
	cg_log_debug_l4("Entering...\n");

	if (CG_NET_USE_ONLY_IPV6_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == FALSE)
			return FALSE;
	}
	if (CG_NET_USE_ONLY_IPV4_ADDR == TRUE) {
		if (cg_net_isipv6address(addr) == TRUE)
			return FALSE;
	}
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

BOOL cg_net_isipv6address(char *addr)
{
	cg_log_debug_l4("Entering...\n");

	if (addr == NULL)
		return FALSE;
	if (0 < cg_strchr(addr, ":", 1))
		return TRUE;
	return FALSE;

	cg_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	cg_net_getipv6address
////////////////////////////////////////////////

int cg_net_getipv6scopeid(char *addr)
{
	size_t addrLen;
	ssize_t perIdx;
	char scopeIDBuf[8+1];

	cg_log_debug_l4("Entering...\n");

	if (cg_net_isipv6address(addr) == FALSE)
		return 0;
	addrLen = cg_strlen(addr);
	perIdx = cg_strchr(addr, "%", 1);
	if (perIdx < 0)
		return 0;
	cg_strncpy(scopeIDBuf, (addr+perIdx+1), sizeof(scopeIDBuf)-1);
	scopeIDBuf[sizeof(scopeIDBuf)-1] = '\0';
	
	cg_log_debug_l4("Leaving...\n");
	
	return atoi(scopeIDBuf);
}

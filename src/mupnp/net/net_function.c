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

#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* global variables
****************************************/

static BOOL CG_NET_USE_ONLY_IPV4_ADDR = FALSE;
static BOOL CG_NET_USE_ONLY_IPV6_ADDR = FALSE;

////////////////////////////////////////////////
//	mupnp_net_isuseaddress
////////////////////////////////////////////////

BOOL mupnp_net_isuseaddress(char *addr)
{
	mupnp_log_debug_l4("Entering...\n");

	if (CG_NET_USE_ONLY_IPV6_ADDR == TRUE) {
		if (mupnp_net_isipv6address(addr) == FALSE)
			return FALSE;
	}
	if (CG_NET_USE_ONLY_IPV4_ADDR == TRUE) {
		if (mupnp_net_isipv6address(addr) == TRUE)
			return FALSE;
	}
	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	mupnp_net_getipv6address
////////////////////////////////////////////////

BOOL mupnp_net_isipv6address(const char *addr)
{
	mupnp_log_debug_l4("Entering...\n");

	if (addr == NULL)
		return FALSE;
	if (0 < mupnp_strchr(addr, ":", 1))
		return TRUE;
	return FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

////////////////////////////////////////////////
//	mupnp_net_getipv6address
////////////////////////////////////////////////

int mupnp_net_getipv6scopeid(const char *addr)
{
	size_t addrLen;
	ssize_t perIdx;
	char scopeIDBuf[8+1];

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_net_isipv6address(addr) == FALSE)
		return 0;
	addrLen = mupnp_strlen(addr);
	perIdx = mupnp_strchr(addr, "%", 1);
	if (perIdx < 0)
		return 0;
	mupnp_strncpy(scopeIDBuf, (addr+perIdx+1), sizeof(scopeIDBuf)-1);
	scopeIDBuf[sizeof(scopeIDBuf)-1] = '\0';
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return atoi(scopeIDBuf);
}

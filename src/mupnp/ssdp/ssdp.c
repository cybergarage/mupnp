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
*	File: cssdp.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/string.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

static char *ssdpIPv6Address = CG_UPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS;
static int ssdpAnnounceCount = CG_UPNP_SSDP_DEFAULT_ANNOUNCE_COUNT;

/****************************************
* mupnp_upnp_ssdp_setipv6address
****************************************/

void mupnp_upnp_ssdp_setipv6address(const char *addr)
{
	mupnp_log_debug_l4("Entering...\n");

	ssdpIPv6Address = CG_UPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_getipv6address
****************************************/

const char *mupnp_upnp_ssdp_getipv6address()
{
	mupnp_log_debug_l4("Entering...\n");
	mupnp_log_debug_l4("Leaving...\n");

	return ssdpIPv6Address;
}

/****************************************
* mupnp_upnp_ssdp_setannouncecount
****************************************/

void mupnp_upnp_ssdp_setannouncecount(int count)
{
	mupnp_log_debug_l4("Entering...\n");

	ssdpAnnounceCount = count;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_getannouncecount
****************************************/

int mupnp_upnp_ssdp_getannouncecount()
{
	mupnp_log_debug_l4("Entering...\n");

	return ssdpAnnounceCount;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_getleasetime
****************************************/

int mupnp_upnp_ssdp_getleasetime(const char *cacheCnt)
{
	ssize_t eqIdx;

	mupnp_log_debug_l4("Entering...\n");

	if (cacheCnt == NULL)
		return 0;
	eqIdx = mupnp_strchr(cacheCnt, "=", 1);
	if (eqIdx < 0)
		return 0;
	return atoi((cacheCnt + eqIdx + 1));

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_gethostaddress
****************************************/

const char *mupnp_upnp_ssdp_gethostaddress(const char *ifAddr)
{
	const char *ssdpAddr = CG_UPNP_SSDP_ADDRESS;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_net_isipv6address(ifAddr) == TRUE)
		ssdpAddr = mupnp_upnp_ssdp_getipv6address();
	return ssdpAddr;

	mupnp_log_debug_l4("Leaving...\n");
}

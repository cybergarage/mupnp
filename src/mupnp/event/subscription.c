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
*	File: csubscription.c
*
*	Revision:
*
*	06/20/05
*		- first revision
*	02/12/06 Theo Beisch
*		- changed time(NULL) to mupnp_getcurrentsystemtime()
*	18-Jan-06 Aapo Makela
*		- Modified to fix negative timeout value
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <mupnp/event/event.h>
#include <mupnp/util/log.h>
#include <mupnp/upnp_function.h>

#include <stdio.h>
#include <stdlib.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_event_subscription_totimeoutheaderstring
****************************************/

const char *mupnp_upnp_event_subscription_totimeoutheaderstring(mUpnpTime time, mUpnpString *buf)
{
	char timeBuf[CG_STRING_LONG_BUFLEN];
	
	mupnp_log_debug_l4("Entering...\n");

	if (time != CG_UPNP_SUBSCRIPTION_INFINITE_VALUE) {
		mupnp_string_setvalue(buf, CG_UPNP_SUBSCRIPTION_TIMEOUT_HEADER);
		mupnp_string_addvalue(buf, mupnp_long2str(time, timeBuf, sizeof(timeBuf)));
	}
	else
		mupnp_string_setvalue(buf, CG_UPNP_SUBSCRIPTION_INFINITE_STRING);
	return mupnp_string_getvalue(buf);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_gettimeout
****************************************/

mUpnpTime mupnp_upnp_event_subscription_gettimeout(const char *headerValue)
{
	ssize_t minusIdx;
	long timeout;
	 
	mupnp_log_debug_l4("Entering...\n");

	minusIdx = mupnp_strstr(headerValue, "-");
	if (minusIdx < 0)
		return CG_UPNP_SUBSCRIPTION_INFINITE_VALUE;
	timeout = mupnp_str2long(headerValue + minusIdx + 1);
	if (timeout == 0)
		return CG_UPNP_SUBSCRIPTION_INFINITE_VALUE;
	if (timeout < 0)
		timeout =-timeout;
	return timeout;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_createsid
****************************************/

const char *mupnp_upnp_event_subscription_createsid(char *buf, size_t bufSize)
{
	mupnp_log_debug_l4("Entering...\n");
	mupnp_upnp_createuuid(buf, bufSize);
	mupnp_log_debug_l4("Leaving...\n");
	return buf;	
}

/****************************************
* mupnp_upnp_event_subscription_tosidheaderstring
****************************************/

const char *mupnp_upnp_event_subscription_tosidheaderstring(const char *sid, char *buf, size_t bufSize)
{
	ssize_t colonIdx;

	mupnp_log_debug_l4("Entering...\n");

	colonIdx = mupnp_strstr(sid, CG_UPNP_SUBSCRIPTION_UUID);
	if (0 <= colonIdx) {
		mupnp_strncpy(buf, sid, bufSize);
		buf[bufSize-1] = '\0';
		return buf;
	}

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize,
#else
	sprintf(buf,
#endif
		"%s%s",
		CG_UPNP_SUBSCRIPTION_UUID,
		sid);
	return buf;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_getsid
****************************************/

const char *mupnp_upnp_event_subscription_getsid(const char *headerValue)
{
	ssize_t colonIdx;
	
	mupnp_log_debug_l4("Entering...\n");

	colonIdx = mupnp_strstr(headerValue, ":");
	if (colonIdx < 0)
		return headerValue;

	mupnp_log_debug_l4("Leaving...\n");

	return (headerValue + colonIdx + 1);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif


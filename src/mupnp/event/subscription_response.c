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
*	File: csubscription_response.c
*
*	Revision:
*
*	06/20/05
*		- first revision
*
******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_event_subscription_subscriberesponse_setresponse
****************************************/

void mupnp_upnp_event_subscription_subscriberesponse_setresponse(CgUpnpSubscriptionResponse *subRes, int code)
{
  char server[CG_UPNP_SEVERNAME_MAXLEN];
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_response_setstatuscode(subRes, code);
  mupnp_upnp_getservername(server, sizeof(server));
  mupnp_http_packet_setheadervalue(((CgHttpPacket*)subRes),
                                  CG_HTTP_SERVER,
                                  server);
	mupnp_http_response_setcontentlength(subRes, 0);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_response_setsid
****************************************/

void mupnp_upnp_event_subscription_response_setsid(CgUpnpSubscriptionResponse *subRes, const char *sid)
{
	CgString *headerSID;
	ssize_t uuidIdx;

	mupnp_log_debug_l4("Entering...\n");

	headerSID = mupnp_string_new();

	uuidIdx = mupnp_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (uuidIdx < 0)
		mupnp_string_addvalue(headerSID, CG_UPNP_ST_UUID_DEVICE ":");
	mupnp_string_addvalue(headerSID, sid);

	mupnp_http_packet_setheadervalue(((CgHttpPacket*)subRes), CG_HTTP_SID, mupnp_string_getvalue(headerSID));

	mupnp_string_delete(headerSID);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_response_settimeout
****************************************/

void mupnp_upnp_event_subscription_response_settimeout(CgUpnpSubscriptionResponse *subRes, long value)
{
	CgString *buf;

	mupnp_log_debug_l4("Entering...\n");

	buf = mupnp_string_new();
	mupnp_http_packet_setheadervalue((CgHttpPacket*)subRes, CG_HTTP_TIMEOUT, mupnp_upnp_event_subscription_totimeoutheaderstring(value, buf));
	mupnp_string_delete(buf);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

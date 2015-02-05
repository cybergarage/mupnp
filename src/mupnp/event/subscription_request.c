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
*	File: csubscription_request.c
*
*	Revision:
*
*	07/13/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change the following functions to set HOTS header according to UPnP Spec HOST.
*		  mupnp_upnp_event_subscription_request_setnewsubscription()
*		  mupnp_upnp_event_subscription_request_setrenewsubscription()
*		  mupnp_upnp_event_subscription_request_setunsubscription()
*		- Change mupnp_upnp_event_subscription_request_setservice() to add / to preceede relative path statement (required e.g. for intel devices).
*
*	10/31/05
*		- mupnp_upnp_event_subscription_request_setservice:
*		  changed eventSubURLStr[1] to [0]
*
*	04/24/07 Aapo Makela
*		- Do not set host in subscription request (it is handled automatically when sending request)
******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/control/control.h>
#include <mupnp/device.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_event_subscription_request_setsid
****************************************/

void mupnp_upnp_event_subscription_request_setsid(CgUpnpSubscriptionRequest *subReq, const char *sid)
{
	CgString *headerSID;
	ssize_t uuidIdx;
	
	mupnp_log_debug_l4("Entering...\n");

	headerSID = mupnp_string_new();

	uuidIdx = mupnp_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (uuidIdx < 0)
		mupnp_string_addvalue(headerSID, CG_UPNP_ST_UUID_DEVICE ":");
	mupnp_string_addvalue(headerSID, sid);
	
	mupnp_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_SID, mupnp_string_getvalue(headerSID));

	mupnp_string_delete(headerSID);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_request_settimeout
****************************************/

void mupnp_upnp_event_subscription_request_settimeout(CgUpnpSubscriptionRequest *subReq, long timeout)
{
	CgString *timeoutBuf;

	mupnp_log_debug_l4("Entering...\n");

	timeoutBuf = mupnp_string_new();
	mupnp_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_TIMEOUT, mupnp_upnp_event_subscription_totimeoutheaderstring(timeout, timeoutBuf));
	mupnp_string_delete(timeoutBuf);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_request_setservice
****************************************/

static void mupnp_upnp_event_subscription_request_setservice(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service)
{
	CgNetURL *eventSubURL;

	mupnp_log_debug_l4("Entering...\n");

	eventSubURL = mupnp_upnp_service_geteventsuburl(service); 
	mupnp_http_request_seturi(subReq, mupnp_net_url_getrequest(eventSubURL));

	mupnp_net_url_delete(subReq->postURL);

	subReq->postURL = eventSubURL;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_request_setnewsubscription
****************************************/

void mupnp_upnp_event_subscription_request_setnewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, const char *callback, CgTime timeout)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	mupnp_upnp_event_subscription_request_setservice(subReq, service);
	mupnp_upnp_event_subscription_request_setcallback(subReq, callback);
	mupnp_upnp_event_subscription_request_setnt(subReq, CG_UPNP_NT_EVENT);
	mupnp_upnp_event_subscription_request_settimeout(subReq, timeout);
	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_request_setrenewsubscription
****************************************/

void mupnp_upnp_event_subscription_request_setrenewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, const char *uuid, CgTime timeout)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	mupnp_upnp_event_subscription_request_setservice(subReq, service);
	mupnp_upnp_event_subscription_request_setsid(subReq, uuid);
	mupnp_upnp_event_subscription_request_settimeout(subReq, timeout);
	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_event_subscription_request_setunsubscription
****************************************/

void mupnp_upnp_event_subscription_request_setunsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_request_setmethod(subReq, CG_HTTP_UNSUBSCRIBE);
	mupnp_upnp_event_subscription_request_setservice(subReq, service);
	mupnp_upnp_event_subscription_request_setsid(subReq, mupnp_upnp_service_getsubscriptionsid(service));
	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

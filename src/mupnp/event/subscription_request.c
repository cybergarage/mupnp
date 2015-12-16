/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/control/control.h>
#include <mupnp/device.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_event_subscription_request_setsid
****************************************/

void mupnp_event_subscription_request_setsid(mUpnpSubscriptionRequest* subReq, const char* sid)
{
  mUpnpString* headerSID;
  ssize_t uuidIdx;

  mupnp_log_debug_l4("Entering...\n");

  headerSID = mupnp_string_new();

  uuidIdx = mupnp_strstr(sid, MUPNP_ST_UUID_DEVICE);
  if (uuidIdx < 0)
    mupnp_string_addvalue(headerSID, MUPNP_ST_UUID_DEVICE ":");
  mupnp_string_addvalue(headerSID, sid);

  mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_SID, mupnp_string_getvalue(headerSID));

  mupnp_string_delete(headerSID);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_request_settimeout
****************************************/

void mupnp_event_subscription_request_settimeout(mUpnpSubscriptionRequest* subReq, long timeout)
{
  mUpnpString* timeoutBuf;

  mupnp_log_debug_l4("Entering...\n");

  timeoutBuf = mupnp_string_new();
  mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_TIMEOUT, mupnp_event_subscription_totimeoutheaderstring(timeout, timeoutBuf));
  mupnp_string_delete(timeoutBuf);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_request_setservice
****************************************/

static void mupnp_event_subscription_request_setservice(mUpnpSubscriptionRequest* subReq, mUpnpService* service)
{
  mUpnpNetURL* eventSubURL;

  mupnp_log_debug_l4("Entering...\n");

  eventSubURL = mupnp_service_geteventsuburl(service);
  mupnp_http_request_seturi(subReq, mupnp_net_url_getrequest(eventSubURL));

  mupnp_net_url_delete(subReq->postURL);

  subReq->postURL = eventSubURL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_request_setnewsubscription
****************************************/

void mupnp_event_subscription_request_setnewsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service, const char* callback, mUpnpTime timeout)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_setmethod(subReq, MUPNP_HTTP_SUBSCRIBE);
  mupnp_event_subscription_request_setservice(subReq, service);
  mupnp_event_subscription_request_setcallback(subReq, callback);
  mupnp_event_subscription_request_setnt(subReq, MUPNP_NT_EVENT);
  mupnp_event_subscription_request_settimeout(subReq, timeout);
  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_request_setrenewsubscription
****************************************/

void mupnp_event_subscription_request_setrenewsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service, const char* uuid, mUpnpTime timeout)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_setmethod(subReq, MUPNP_HTTP_SUBSCRIBE);
  mupnp_event_subscription_request_setservice(subReq, service);
  mupnp_event_subscription_request_setsid(subReq, uuid);
  mupnp_event_subscription_request_settimeout(subReq, timeout);
  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_event_subscription_request_setunsubscription
****************************************/

void mupnp_event_subscription_request_setunsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_setmethod(subReq, MUPNP_HTTP_UNSUBSCRIBE);
  mupnp_event_subscription_request_setservice(subReq, service);
  mupnp_event_subscription_request_setsid(subReq, mupnp_service_getsubscriptionsid(service));
  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

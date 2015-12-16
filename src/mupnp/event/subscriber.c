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
#include <mupnp/event/notify.h>
#include <mupnp/upnp_limit.h>
#include <mupnp/util/log.h>
#include <mupnp/util/time.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/**
 * Create a new event subscriber
 */
mUpnpSubscriber* mupnp_subscriber_new()
{
  mUpnpSubscriber* sub;

  mupnp_log_debug_l4("Entering...\n");

  sub = (mUpnpSubscriber*)malloc(sizeof(mUpnpSubscriber));

  if (NULL != sub) {
    mupnp_list_node_init((mUpnpList*)sub);

    sub->sid = mupnp_string_new();
    sub->ifAddr = mupnp_string_new();
    sub->deliveryURL = mupnp_net_url_new();

    mupnp_subscriber_settimeout(sub, 0);
    mupnp_subscriber_renew(sub);
    mupnp_subscriber_setnotifycount(sub, 0);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return sub;
}

/**
 * Destroy an event subscriber
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_delete(mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_subscriber_clear(sub);
  mupnp_list_remove((mUpnpList*)sub);

  mupnp_string_delete(sub->sid);
  mupnp_string_delete(sub->ifAddr);
  mupnp_net_url_delete(sub->deliveryURL);

  free(sub);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Clear the contents of an event subscriber
 *
 * @todo Lacks implementation (is it necessary?)
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_clear(mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Renew a subscription. Essentially sets subscription time (duration) 
 * to zero and resets notify count (== event key).
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_renew(mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  //Theo Beisch use clinkc time
  mupnp_subscriber_setsubscriptiontime(sub, mupnp_getcurrentsystemtime());

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Increment the event notify count by one
 *
 * @param sub The event subscriber
 * @return The new notify count
 */
long mupnp_subscriber_incrementnotifycount(mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  if (MUPNP_NOTIFY_COUNT_MAX <= sub->notifyCount)
    sub->notifyCount = 0;
  sub->notifyCount++;

  mupnp_log_debug_l4("Leaving...\n");

  return sub->notifyCount;
}

/**
 * Check, whether a subscriber's event subscription has been expired
 *
 * @param sub The subscriber
 * @return true if the subscription has been expired; otherwise false
 */
bool mupnp_subscriber_isexpired(mUpnpSubscriber* sub)
{
  mUpnpTime currTime;
  mUpnpTime timeout;
  mUpnpTime expiredTime;

  timeout = mupnp_subscriber_gettimeout(sub);
  if (timeout == MUPNP_SUBSCRIPTION_INFINITE_VALUE)
    return false;

  //Theo Beisch - use clinkc function
  currTime = mupnp_getcurrentsystemtime(); //returns time in s
  expiredTime = mupnp_subscriber_getsubscriptiontime(sub) + (timeout); //tb: removed( *1000);
  if (expiredTime < currTime)
    return true;

  return false;

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return true if succesful; otherwise false
 */
static bool mupnp_subscriber_notifymain(mUpnpSubscriber* sub, mUpnpService* service, mUpnpStateVariable* statVar)
{
  char* host;
  int port;
  mUpnpNotifyRequest* notifyReq;
  mUpnpNotifyResponse* notifyRes;
  bool notifySuccess;

  mupnp_log_debug_l4("Entering...\n");

  host = mupnp_subscriber_getdeliveryhost(sub);
  port = mupnp_subscriber_getdeliveryport(sub);

  notifyReq = mupnp_event_notify_request_new();
  mupnp_event_notify_request_setpropertysetnode(notifyReq, sub, service, statVar);
  notifyRes = mupnp_event_notify_request_post(notifyReq, host, port);
  notifySuccess = mupnp_event_notify_response_issuccessful(notifyRes);

  mupnp_http_request_print(notifyReq->httpReq);
  mupnp_http_response_print(notifyRes->httpRes);

  mupnp_event_notify_request_delete(notifyReq);

  if (notifySuccess == false)
    return false;

  mupnp_subscriber_incrementnotifycount(sub);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return true if succesful; otherwise false
 */
bool mupnp_subscriber_notify(mUpnpSubscriber* sub, mUpnpStateVariable* statVar)
{
  return mupnp_subscriber_notifymain(sub, NULL, statVar);
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param service The evented service
 * @return true if succesful; otherwise false
 */
bool mupnp_subscriber_notifyall(mUpnpSubscriber* sub, /* mUpnpService */ void* service)
{
  return mupnp_subscriber_notifymain(sub, (mUpnpService*)service, NULL);
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

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

#ifndef _MUPNP_SUBSCRIBER_H_
#define _MUPNP_SUBSCRIBER_H_

#include <mupnp/typedef.h>

#include <mupnp/util/list.h>
#include <mupnp/util/string.h>
#include <mupnp/net/url.h>
#include <mupnp/util/time.h>

#include <mupnp/statevariable.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

/**
 * The device-side structure for an event subscriber
 */
typedef struct _mUpnpSubscriber {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* sid;
  mUpnpString* ifAddr;
  mUpnpNetURL* deliveryURL;

  mUpnpTime timeout;
  long notifyCount;
  mUpnpTime subscriptionTime;
} mUpnpSubscriber, mUpnpSubscriberList;

/****************************************
* Function (Subscriber)
****************************************/

/**
 * Create a new event subscriber
 */
mUpnpSubscriber* mupnp_subscriber_new();

/**
 * Destroy an event subscriber
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_delete(mUpnpSubscriber* sub);

/**
 * Clear the contents of an event subscriber
 *
 * @todo Lacks implementation (is it necessary?)
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_clear(mUpnpSubscriber* sub);

/**
 * Renew a subscription. Essentially sets subscription time (duration) 
 * to zero and resets notify count (== event key).
 *
 * @param sub The event subscriber
 */
void mupnp_subscriber_renew(mUpnpSubscriber* sub);

/**
 * Get the next subscriber in a subscriber list. Use as an iterator.
 *
 * @param sub The current event subscriber
 * @return Next subscriber or NULL
 */
#define mupnp_subscriber_next(sub) (mUpnpSubscriber*) mupnp_list_next((mUpnpList*)sub)

/**
 * Remove the event subscriber from a subscriber list.
 *
 * @param sub The event subscriber to remove
 */
#define mupnp_subscriber_remove(sub) mupnp_list_remove((mUpnpList*)sub)

/****************************************************************************
 * sid
 ****************************************************************************/

/**
 * Set the subscriber's SID (Subscription IDentifier) value
 *
 * @param sub The event subscriber
 * @param value The SID value
 */
#define mupnp_subscriber_setsid(sub, value) mupnp_string_setvalue(sub->sid, value)

/**
 * Get the subscriber's SID (Subscription IDentifier) value
 *
 * @param sub The current event subscriber
 * @return Subscriber's SID
 */
#define mupnp_subscriber_getsid(sub) mupnp_string_getvalue(sub->sid)

/****************************************************************************
 * timeout
 ****************************************************************************/

/**
 * Set the subscription timeout (expiration time) to a certain value.
 *
 * @param sub The event subscriber
 * @param value Timeout
 */
#define mupnp_subscriber_settimeout(sub, value) (sub->timeout = value)

/**
 * Get the subscription timeout (expiration time)
 *
 * @param sub The event subscriber
 * @return Timeout
 */
#define mupnp_subscriber_gettimeout(sub) (sub->timeout)

/****************************************************************************
 * subscriptionTime
 ****************************************************************************/

/**
 * Set the subscription time (duration) to a certain value.
 *
 * @param sub The event subscriber
 * @param value Subscription time
 */
#define mupnp_subscriber_setsubscriptiontime(sub, value) (sub->subscriptionTime = value)

/**
 * Get the subscription time (duration)
 *
 * @param sub The event subscriber
 * @return Subscription time
 */
#define mupnp_subscriber_getsubscriptiontime(sub) (sub->subscriptionTime)

/****************************************************************************
 * notifycount
 ****************************************************************************/

/**
 * Set the notify count to a certain value.
 *
 * @param sub The event subscriber
 * @param value Notify count
 */
#define mupnp_subscriber_setnotifycount(sub, value) (sub->notifyCount = value)

/**
 * Get the notify count
 *
 * @param sub The event subscriber
 * @return Notify count
 */
#define mupnp_subscriber_getnotifycount(sub) (sub->notifyCount)

/**
 * Increment the event notify count by one
 *
 * @param sub The event subscriber
 * @return The new notify count
 */
long mupnp_subscriber_incrementnotifycount(mUpnpSubscriber* sub);

/****************************************************************************
 * Delivery URL
 ****************************************************************************/

/**
 * Set the subscriber's delivery URL
 *
 * @param sub The event subscriber
 * @param value The delivery URL
 */
#define mupnp_subscriber_setdeliveryurl(sub, value) mupnp_net_url_set(sub->deliveryURL, value)

/**
 * Set the subscriber's delivery host
 *
 * @param sub The event subscriber
 */
#define mupnp_subscriber_getdeliveryhost(sub) mupnp_net_url_gethost(sub->deliveryURL)

/**
 * Set the subscriber's delivery path
 *
 * @param sub The event subscriber
 */
#define mupnp_subscriber_getdeliverypath(sub) mupnp_net_url_getpath(sub->deliveryURL)

/**
 * Set the subscriber's delivery port
 *
 * @param sub The event subscriber
 */
#define mupnp_subscriber_getdeliveryport(sub) mupnp_net_url_getport(sub->deliveryURL)

/**
 * Check, whether a subscriber's event subscription has been expired
 *
 * @param sub The subscriber
 * @return true if the subscription has been expired; otherwise false
 */
bool mupnp_subscriber_isexpired(mUpnpSubscriber* sub);

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return true if succesful; otherwise false
 */
bool mupnp_subscriber_notify(mUpnpSubscriber* sub, mUpnpStateVariable* statVar);

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param service The evented service
 * @return true if succesful; otherwise false
 */
bool mupnp_subscriber_notifyall(mUpnpSubscriber* sub, void* service);

/****************************************
* Function (SubscriberList)
****************************************/

/**
 * Create a new event subscriber list
 */
mUpnpSubscriberList* mupnp_subscriberlist_new();

/**
 * Destroy an event subscriber list
 *
 * @param subscriberlist The list to destroy
 */
void mupnp_subscriberlist_delete(mUpnpSubscriberList* subscriberlist);

/**
 * Clear the contents of a subscriber list
 *
 * @param subList The subscriber list to clear
 */
#define mupnp_subscriberlist_clear(subList) mupnp_list_clear((mUpnpList*)subList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_subscriber_delete)

/**
 * Get the size (node count) of a subscriber list
 *
 * @param subList The subscriber list to evaluate
 */
#define mupnp_subscriberlist_size(subList) mupnp_list_size((mUpnpList*)subList)

/**
 * Get the first item in a subscriber list (use as the first step in iteration).
 *
 * @param subList The subscriber list to evaluate
 */
#define mupnp_subscriberlist_gets(subList) (mUpnpSubscriber*) mupnp_list_next((mUpnpList*)subList)

/**
 * Add a new event subscriber to a subscriber list
 *
 * @param subList The subscriber list to add to
 * @param sub The subscriber to add
 */
#define mupnp_subscriberlist_add(subList, sub) mupnp_list_add((mUpnpList*)subList, (mUpnpList*)sub)

#ifdef __cplusplus
}
#endif

#endif

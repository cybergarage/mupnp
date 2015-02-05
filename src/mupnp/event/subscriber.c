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
*	File: csubscriber.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*	12-Jan-06 Heikki Junnila
*		- Added API comments
*	01/12/06 Theo Beisch
*		- modified time() to come from util/ctime
*		  for WINCE OS support		
*	04/04/06 Theo Beisch
*		- subscriber renew does not reset notifyCount
*		- subscriber clear does reset notifyCount
*		- expiry check is by [s] (removed *1000 factor)
*	03/13/08
*		- Changed mupnp_upnp_subscriber_notifyall() using void parameter instead of mUpnpService not to conflict the prototype defines.
*
******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/event/notify.h>
#include <mupnp/upnp_limit.h>
#include <mupnp/util/log.h>
#include <mupnp/util/time.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/**
 * Create a new event subscriber
 */
mUpnpUpnpSubscriber *mupnp_upnp_subscriber_new()
{
	mUpnpUpnpSubscriber *sub;

	mupnp_log_debug_l4("Entering...\n");

	sub = (mUpnpUpnpSubscriber *)malloc(sizeof(mUpnpUpnpSubscriber));

	if ( NULL != sub )
	{
		mupnp_list_node_init((mUpnpList *)sub);
		
		sub->sid = mupnp_string_new();
		sub->ifAddr = mupnp_string_new();
		sub->deliveryURL = mupnp_net_url_new();
		
		mupnp_upnp_subscriber_settimeout(sub, 0);
		mupnp_upnp_subscriber_renew(sub);
		mupnp_upnp_subscriber_setnotifycount(sub, 0);
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return sub;
}

/**
 * Destroy an event subscriber
 *
 * @param sub The event subscriber
 */
void mupnp_upnp_subscriber_delete(mUpnpUpnpSubscriber *sub)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_subscriber_clear(sub);
	mupnp_list_remove((mUpnpList *)sub);

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
void mupnp_upnp_subscriber_clear(mUpnpUpnpSubscriber *sub)
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
void mupnp_upnp_subscriber_renew(mUpnpUpnpSubscriber *sub)
{
	mupnp_log_debug_l4("Entering...\n");

	//Theo Beisch use clinkc time
	mupnp_upnp_subscriber_setsubscriptiontime(sub, mupnp_getcurrentsystemtime());

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Increment the event notify count by one
 *
 * @param sub The event subscriber
 * @return The new notify count
 */
long mupnp_upnp_subscriber_incrementnotifycount(mUpnpUpnpSubscriber *sub)
{
	mupnp_log_debug_l4("Entering...\n");

	if (CG_UPNP_NOTIFY_COUNT_MAX <= sub->notifyCount)
		sub->notifyCount = 0;
	sub->notifyCount++;

	mupnp_log_debug_l4("Leaving...\n");

	return sub->notifyCount;
}

/**
 * Check, whether a subscriber's event subscription has been expired
 *
 * @param sub The subscriber
 * @return TRUE if the subscription has been expired; otherwise FALSE
 */
BOOL mupnp_upnp_subscriber_isexpired(mUpnpUpnpSubscriber *sub)
{
	mUpnpTime currTime;
	mUpnpTime timeout;
	mUpnpTime expiredTime;
	
	timeout = mupnp_upnp_subscriber_gettimeout(sub);
	if(timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE) 
		return FALSE; 
			
	//Theo Beisch - use clinkc function 
	currTime = mupnp_getcurrentsystemtime(); //returns time in s 
	expiredTime = mupnp_upnp_subscriber_getsubscriptiontime(sub) + (timeout); //tb: removed( *1000);
	if (expiredTime < currTime)
		return TRUE;
			
	return FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return TRUE if succesful; otherwise FALSE
 */
static BOOL mupnp_upnp_subscriber_notifymain(mUpnpUpnpSubscriber *sub, mUpnpUpnpService *service, mUpnpUpnpStateVariable *statVar)
{
	char *host;
	int port;
	mUpnpUpnpNotifyRequest *notifyReq;
	mUpnpUpnpNotifyResponse *notifyRes;
	BOOL notifySuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	host = mupnp_upnp_subscriber_getdeliveryhost(sub);
	port = mupnp_upnp_subscriber_getdeliveryport(sub);

	notifyReq = mupnp_upnp_event_notify_request_new();
	mupnp_upnp_event_notify_request_setpropertysetnode(notifyReq, sub, service, statVar);
	notifyRes = mupnp_upnp_event_notify_request_post(notifyReq, host, port);
	notifySuccess = mupnp_upnp_event_notify_response_issuccessful(notifyRes);

	mupnp_http_request_print(notifyReq->httpReq);
	mupnp_http_response_print(notifyRes->httpRes);
	
	mupnp_upnp_event_notify_request_delete(notifyReq);

	if (notifySuccess == FALSE)
		return FALSE;
		
	mupnp_upnp_subscriber_incrementnotifycount(sub);
			
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param statVar The evented state variable
 * @return TRUE if succesful; otherwise FALSE
 */
BOOL mupnp_upnp_subscriber_notify(mUpnpUpnpSubscriber *sub, mUpnpUpnpStateVariable *statVar)
{
	return mupnp_upnp_subscriber_notifymain(sub, NULL, statVar);
}

/**
 * Post a notification to an event subscriber. This is called in a device.
 *
 * @param sub The event subscriber
 * @param service The evented service
 * @return TRUE if succesful; otherwise FALSE
 */
BOOL mupnp_upnp_subscriber_notifyall(mUpnpUpnpSubscriber *sub, /* mUpnpUpnpService */ void *service)
{
	return mupnp_upnp_subscriber_notifymain(sub, (mUpnpUpnpService *)service, NULL);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

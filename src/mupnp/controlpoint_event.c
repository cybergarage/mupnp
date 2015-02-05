/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: ccontrolpoint_event.c
*
*	Revision:
*
*	07/13/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change the following functions to set HOTS header according to UPnP Spec HOST.
*		  mupnp_upnp_controlpoint_subscribe()
*		  mupnp_upnp_controlpoint_resubscribe()
*		  mupnp_upnp_controlpoint_unsubscribe()
*		- Changed mupnp_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*	17-Jan-06 Aapo Makela
*		- Added expiration handler thread
*	04/24/07 Aapo Makela
*		- Do not set host in subscription request (it is handled automatically when sending request)
******************************************************************/

#include <mupnp/upnp_limit.h>
#include <mupnp/controlpoint.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT) && !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

mUpnpUpnpService *mupnp_upnp_device_getfirstexpiratedservice(mUpnpUpnpControlPoint *ctrlPoint,
						       mUpnpUpnpDevice *dev, 
						       mUpnpTime expirationTime)
{
	mUpnpUpnpDevice *childDev = NULL;
	mUpnpUpnpService *srv = NULL;
	mUpnpUpnpService *found_srv = NULL;
	mUpnpTime tmpTime;
	mUpnpTime currTime;
	long timeout;
	
	mupnp_log_debug_l4("Entering...\n");

	tmpTime = 0;
	currTime = mupnp_getcurrentsystemtime();
	
	for (srv = mupnp_upnp_device_getservices(dev); srv != NULL;
	     srv = mupnp_upnp_service_next(srv))
	{
		if (mupnp_upnp_service_issubscribed(srv) == FALSE) continue;
	
		tmpTime = mupnp_upnp_service_getsubscriptionexpiration(srv);
		
	mupnp_log_debug_s("Found subscribed service with expiration of  %ld\n",
			tmpTime);
		
		if (tmpTime > 0 && tmpTime <= currTime)
		{
		mupnp_log_debug_s("Trying to resubscribe!\n");
			/* Subscription has almost expired! resubscribe */
			timeout = mupnp_upnp_service_getsubscriptiontimeout(srv);
			if (mupnp_upnp_controlpoint_resubscribe(ctrlPoint, srv, timeout) == FALSE)
			{
			mupnp_log_debug_s("Resubscription unsuccesful!\n");
				tmpTime = 0;
			} else {
			mupnp_log_debug_s("Resubscription succesful!\n");
				tmpTime = mupnp_upnp_service_getsubscriptionexpiration(srv);
			}
			
		} 
		
		if (tmpTime > 0 && tmpTime < expirationTime)
		{
			expirationTime = tmpTime;
			found_srv = srv;
		}
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		srv = mupnp_upnp_device_getfirstexpiratedservice(ctrlPoint,
							      childDev,
							      expirationTime);
		
		if (srv != NULL) found_srv = srv;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return found_srv;
}


void mupnp_upnp_controlpoint_expirationhandler(mUpnpThread *thread)
{
	mUpnpUpnpControlPoint *ctrlPoint;
	mUpnpUpnpDevice *dev, *tmpDev;
	mUpnpUpnpService *srv;
	mUpnpUpnpSSDPPacket *ssdpPkt;
	mUpnpTime currTime;
	mUpnpTime expirationTime;
	mUpnpTime tmpTime;
	long timeout;
	
	mupnp_log_debug_l4("Entering...\n");

	ctrlPoint = (mUpnpUpnpControlPoint *)mupnp_thread_getuserdata(thread);
	mupnp_log_debug_s("CtrlPoint: %p\n", ctrlPoint);
	
	mupnp_mutex_lock(ctrlPoint->expMutex);
	
	while (mupnp_thread_isrunnable(thread) == TRUE)
	{
		currTime = mupnp_getcurrentsystemtime();
		mupnp_log_debug_s("Current time: %ld\n", currTime);
		expirationTime = 0;
		tmpTime = 0;
		
		/* Get expirations from all services and devices */
		mupnp_mutex_unlock(ctrlPoint->expMutex);
		mupnp_upnp_controlpoint_lock(ctrlPoint);
		
		for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint);
		     dev != NULL; dev = mupnp_upnp_device_next(dev))
		{
			/* Check device expiration */
			mupnp_log_debug_s("Checking device expiration.\n");
			ssdpPkt = mupnp_upnp_device_getssdppacket(dev);
			if (ssdpPkt != NULL)
			{
				tmpTime = mupnp_upnp_ssdp_packet_getexpiration(ssdpPkt);
				mupnp_log_debug_s("expiration for device %s: %ld\n", 
				mupnp_upnp_device_getfriendlyname(dev),
				mupnp_upnp_ssdp_packet_getexpiration(ssdpPkt));
				
				if (tmpTime > 0 && tmpTime <= currTime)
				{
					/* Device has already expired, remove it! */
					tmpDev = dev;
					dev = mupnp_upnp_device_next(tmpDev);
					
					mupnp_upnp_controlpoint_unlock(ctrlPoint);
					mupnp_upnp_controlpoint_removedevicebyssdppacket(
						ctrlPoint,
						mupnp_upnp_device_getssdppacket(tmpDev));
					mupnp_upnp_controlpoint_lock(ctrlPoint);
					
					tmpDev = NULL;
					
					/* Break out of loop */
					if (dev == NULL) break;
						
				} else if (tmpTime > 0 && 
					   (tmpTime < expirationTime || 
					    expirationTime == 0))
				{
					/* Mark expirationTime */
					expirationTime = tmpTime;
				}
			}
			
			/* Get service expiration */
			srv = mupnp_upnp_device_getfirstexpiratedservice(ctrlPoint, dev, expirationTime);
			
			if (srv != NULL) 
			{
				expirationTime = mupnp_upnp_service_getsubscriptionexpiration(srv);
			}
		}
		
		mupnp_upnp_controlpoint_unlock(ctrlPoint);
		mupnp_mutex_lock(ctrlPoint->expMutex);
		
		/* Now we know, how much we should sleep */
		if (expirationTime > currTime)
		{
			currTime = mupnp_getcurrentsystemtime();
			timeout = (long)(expirationTime - currTime);
			mupnp_log_debug_s("Sleeping for %ld sec\n", timeout);
		} else {
			timeout = 0;
			mupnp_log_debug_s("Sleeping until awaken\n");
		}
		
		if (mupnp_thread_isrunnable(thread) == FALSE) break;
		mupnp_cond_wait(ctrlPoint->expCond, ctrlPoint->expMutex, timeout);
		mupnp_log_debug_s("Sleep over!\n");
		
		/* Check the time after the wait */
		currTime = mupnp_getcurrentsystemtime();
		if (currTime < expirationTime)
		{
			mupnp_log_debug_s("New device or subscription! Or someone adjusted the clock!\n");
		}
	}
	mupnp_mutex_unlock(ctrlPoint->expMutex);
	
	mupnp_log_debug_l4("Leaving...\n");
}


/****************************************
* mupnp_upnp_controlpoint_resubscribe
****************************************/

BOOL mupnp_upnp_controlpoint_resubscribe(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpService *service, long timeout)
{
	mUpnpUpnpDevice *rootDev;
	mUpnpUpnpSubscriptionRequest *subReq;
	mUpnpUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_service_issubscribed(service) == FALSE)
		return FALSE;

	rootDev = mupnp_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	subReq = mupnp_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	mupnp_upnp_event_subscription_request_setrenewsubscription(subReq, service, mupnp_upnp_service_getsubscriptionsid(service), timeout);
	subRes = mupnp_upnp_event_subscription_request_post(subReq);
	isSuccess = mupnp_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		mupnp_upnp_service_setsubscriptionsid(service, mupnp_upnp_event_subscription_response_getsid(subRes));
		mupnp_upnp_service_setsubscriptiontimeout(service, mupnp_upnp_event_subscription_response_gettimeout(subRes));
		mupnp_upnp_service_setsubscriptiontimestamp(service, mupnp_getcurrentsystemtime());
	}
	else
		mupnp_upnp_service_clearsubscriptionsid(service);

	mupnp_upnp_event_subscription_request_delete(subReq);

	mupnp_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* mupnp_upnp_controlpoint_subscribe
****************************************/

BOOL mupnp_upnp_controlpoint_subscribe(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpService *service, long timeout)
{
	mUpnpUpnpDevice *rootDev;
	char *roodDevIfAddress;
	mUpnpUpnpSubscriptionRequest *subReq;
	mUpnpUpnpSubscriptionResponse *subRes;
	char eventSubURL[CG_UPNP_CONTROLPOINT_EVENTSUBURL_MAX];
	BOOL isSuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_service_issubscribed(service) == TRUE)
		return mupnp_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout);

	rootDev = mupnp_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
				mupnp_upnp_controlpoint_parsescservicescpd(service); 
#endif
	
	mupnp_upnp_service_lock(service);
	roodDevIfAddress = mupnp_upnp_device_getinterfaceaddressfromssdppacket(rootDev);

	mupnp_log_debug_s("roodevifaddress = %s\n", roodDevIfAddress);

	subReq = mupnp_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	mupnp_upnp_event_subscription_request_setnewsubscription(subReq, service, mupnp_upnp_controlpoint_geteventsubcallbackurl(ctrlPoint, roodDevIfAddress, eventSubURL, sizeof(eventSubURL)), timeout);
	/* Set the event key to zero before any events are received */
	mupnp_upnp_service_seteventkey(service, 0);
	subRes = mupnp_upnp_event_subscription_request_post(subReq);
	
	isSuccess = mupnp_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		mupnp_upnp_service_setsubscriptionsid(service, mupnp_upnp_event_subscription_response_getsid(subRes));
		mupnp_upnp_service_setsubscriptiontimeout(service, mupnp_upnp_event_subscription_response_gettimeout(subRes));
		mupnp_upnp_service_setsubscriptiontimestamp(service, mupnp_getcurrentsystemtime());
		
		/* New subscription, wake up expirationhandler thread */
		mupnp_mutex_lock(ctrlPoint->expMutex);
		mupnp_cond_signal(ctrlPoint->expCond);
		mupnp_mutex_unlock(ctrlPoint->expMutex);
	}
	else
		mupnp_upnp_service_clearsubscriptionsid(service);

	mupnp_upnp_event_subscription_request_delete(subReq);
	mupnp_upnp_service_unlock(service);
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return isSuccess;
}

/****************************************
* mupnp_upnp_event_subscription_request_setunsubscription
****************************************/

BOOL mupnp_upnp_controlpoint_unsubscribe(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpService *service)
{
	mUpnpUpnpDevice *rootDev;
	mUpnpUpnpSubscriptionRequest *subReq;
	mUpnpUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	mupnp_log_debug_l4("Entering...\n");

	rootDev = mupnp_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	subReq = mupnp_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	mupnp_upnp_event_subscription_request_setunsubscription(subReq, service);
	subRes = mupnp_upnp_event_subscription_request_post(subReq);
	isSuccess = mupnp_upnp_event_subscription_response_issuccessful(subRes);
	
	mupnp_upnp_service_clearsubscriptionsid(service);
	mupnp_upnp_service_setsubscriptiontimeout(service, 0);
	mupnp_upnp_service_setsubscriptiontimestamp(service, 0);
	
	mupnp_upnp_event_subscription_request_delete(subReq);

	mupnp_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* mupnp_upnp_controlpoint_subscribeall
****************************************/

BOOL mupnp_upnp_controlpoint_subscribeall(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpDevice *dev, long timeout)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	BOOL isSuccess;

	mupnp_log_debug_l4("Entering...\n");

	isSuccess = FALSE;

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_upnp_controlpoint_subscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(dev)) {
		if (mupnp_upnp_controlpoint_subscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* mupnp_upnp_controlpoint_resubscribeall
****************************************/

BOOL mupnp_upnp_controlpoint_resubscribeall(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpDevice *dev, long timeout)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	BOOL isSuccess;

	mupnp_log_debug_l4("Entering...\n");

	isSuccess = FALSE;

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(dev)) {
		if (mupnp_upnp_controlpoint_resubscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/****************************************
* mupnp_upnp_controlpoint_unsubscribeall
****************************************/

BOOL mupnp_upnp_controlpoint_unsubscribeall(mUpnpUpnpControlPoint *ctrlPoint, mUpnpUpnpDevice *dev)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	BOOL isSuccess;

	mupnp_log_debug_l4("Entering...\n");

	isSuccess = TRUE;

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_upnp_service_issubscribed(service) == TRUE)
		{
			if (mupnp_upnp_controlpoint_unsubscribe(ctrlPoint, service) == FALSE)
				isSuccess = FALSE;
		}
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		if (mupnp_upnp_controlpoint_unsubscribeall(ctrlPoint, childDev) == FALSE)
			isSuccess = FALSE;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return isSuccess;
}

/*

bool ControlPoint::isSubscribed(Service *service)
{
	mupnp_log_debug_l4("Entering...\n");

	if (service == NULL)
		return false;
	return service->isSubscribed();

	mupnp_log_debug_l4("Leaving...\n");
}

void ControlPoint::unsubscribe(Device *device)
{
	mupnp_log_debug_l4("Entering...\n");

	int n;

	ServiceList *serviceList = device->getServiceList();
	int serviceCnt = serviceList->size();
	for (n=0; n<serviceCnt; n++) {
		Service *service = serviceList->getService(n);
		if (service->hasSID() == true)
			unsubscribe(service);
	}

	DeviceList *childDevList = device->getDeviceList();
	int childDevCnt = childDevList->size();
	for (n=0; n<childDevCnt; n++) {
		Device *cdev = childDevList->getDevice(n);
		unsubscribe(cdev);
	}		

	mupnp_log_debug_l4("Leaving...\n");
}
*/

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

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
#include <mupnp/upnp_limit.h>
#include <mupnp/controlpoint.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_CONTROLPOINT) && !defined(MUPNP_NOUSE_SUBSCRIPTION)

bool mupnp_controlpoint_seteventlistener(mUpnpControlPoint* ctrlPoint, MUPNP_EVENT_LISTENER evlistener)
{

  mUpnpEventListenerList* nextEvListener;
  do {
    if (evlistener == NULL) {
      nextEvListener = (mUpnpEventListenerList*)ctrlPoint->eventListeners->next;
      mupnp_eventlistenerlist_remove(ctrlPoint->eventListeners, nextEvListener->listener);
    }
    else
      mupnp_eventlistenerlist_add(ctrlPoint->eventListeners, evlistener);
  } while (0);

  return true;
}

mUpnpService* mupnp_device_getfirstexpiratedservice(mUpnpControlPoint* ctrlPoint,
    mUpnpDevice* dev,
    mUpnpTime expirationTime)
{
  mUpnpDevice* childDev = NULL;
  mUpnpService* srv = NULL;
  mUpnpService* found_srv = NULL;
  mUpnpTime tmpTime;
  mUpnpTime currTime;
  long timeout;

  mupnp_log_debug_l4("Entering...\n");

  tmpTime = 0;
  currTime = mupnp_getcurrentsystemtime();

  for (srv = mupnp_device_getservices(dev); srv != NULL;
       srv = mupnp_service_next(srv)) {
    if (mupnp_service_issubscribed(srv) == false)
      continue;

    tmpTime = mupnp_service_getsubscriptionexpiration(srv);

    mupnp_log_debug_s("Found subscribed service with expiration of  %ld\n",
        tmpTime);

    if (tmpTime > 0 && tmpTime <= currTime) {
      mupnp_log_debug_s("Trying to resubscribe!\n");
      /* Subscription has almost expired! resubscribe */
      timeout = mupnp_service_getsubscriptiontimeout(srv);
      if (mupnp_controlpoint_resubscribe(ctrlPoint, srv, timeout) == false) {
        mupnp_log_debug_s("Resubscription unsuccesful!\n");
        tmpTime = 0;
      }
      else {
        mupnp_log_debug_s("Resubscription succesful!\n");
        tmpTime = mupnp_service_getsubscriptionexpiration(srv);
      }
    }

    if (tmpTime > 0 && tmpTime < expirationTime) {
      expirationTime = tmpTime;
      found_srv = srv;
    }
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    srv = mupnp_device_getfirstexpiratedservice(ctrlPoint,
        childDev,
        expirationTime);

    if (srv != NULL)
      found_srv = srv;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return found_srv;
}

void mupnp_controlpoint_expirationhandler(mUpnpThread* thread)
{
  mUpnpControlPoint* ctrlPoint;
  mUpnpDevice *dev, *tmpDev;
  mUpnpService* srv;
  mUpnpSSDPPacket* ssdpPkt;
  mUpnpTime currTime;
  mUpnpTime expirationTime;
  mUpnpTime tmpTime;
  long timeout;

  mupnp_log_debug_l4("Entering...\n");

  ctrlPoint = (mUpnpControlPoint*)mupnp_thread_getuserdata(thread);
  mupnp_log_debug_s("CtrlPoint: %p\n", ctrlPoint);

  mupnp_mutex_lock(ctrlPoint->expMutex);

  while (mupnp_thread_isrunnable(thread) == true) {
    currTime = mupnp_getcurrentsystemtime();
    mupnp_log_debug_s("Current time: %ld\n", currTime);
    expirationTime = 0;
    tmpTime = 0;

    /* Get expirations from all services and devices */
    mupnp_mutex_unlock(ctrlPoint->expMutex);
    mupnp_controlpoint_lock(ctrlPoint);

    for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
         dev != NULL;
         dev = mupnp_device_next(dev)) {
      /* Check device expiration */
      mupnp_log_debug_s("Checking device expiration.\n");
      ssdpPkt = mupnp_device_getssdppacket(dev);
      if (ssdpPkt != NULL) {
        tmpTime = mupnp_ssdp_packet_getexpiration(ssdpPkt);
        mupnp_log_debug_s("expiration for device %s: %ld\n",
            mupnp_device_getfriendlyname(dev),
            mupnp_ssdp_packet_getexpiration(ssdpPkt));

        if (tmpTime > 0 && tmpTime <= currTime) {
          /* Device has already expired, remove it! */
          tmpDev = dev;
          dev = mupnp_device_next(tmpDev);

          mupnp_controlpoint_unlock(ctrlPoint);
          mupnp_controlpoint_removedevicebyssdppacket(
              ctrlPoint,
              mupnp_device_getssdppacket(tmpDev));
          mupnp_controlpoint_lock(ctrlPoint);

          tmpDev = NULL;

          /* Break out of loop */
          if (dev == NULL)
            break;
        }
        else if (tmpTime > 0 && (tmpTime < expirationTime || expirationTime == 0)) {
          /* Mark expirationTime */
          expirationTime = tmpTime;
        }
      }

      /* Get service expiration */
      srv = mupnp_device_getfirstexpiratedservice(ctrlPoint, dev, expirationTime);

      if (srv != NULL) {
        expirationTime = mupnp_service_getsubscriptionexpiration(srv);
      }
    }

    mupnp_controlpoint_unlock(ctrlPoint);
    mupnp_mutex_lock(ctrlPoint->expMutex);

    /* Now we know, how much we should sleep */
    if (expirationTime > currTime) {
      currTime = mupnp_getcurrentsystemtime();
      timeout = (long)(expirationTime - currTime);
      mupnp_log_debug_s("Sleeping for %ld sec\n", timeout);
    }
    else {
      timeout = 0;
      mupnp_log_debug_s("Sleeping until awaken\n");
    }

    if (mupnp_thread_isrunnable(thread) == false)
      break;
    mupnp_cond_wait(ctrlPoint->expCond, ctrlPoint->expMutex, timeout);
    mupnp_log_debug_s("Sleep over!\n");

    /* Check the time after the wait */
    currTime = mupnp_getcurrentsystemtime();
    if (currTime < expirationTime) {
      mupnp_log_debug_s("New device or subscription! Or someone adjusted the clock!\n");
    }
  }
  mupnp_mutex_unlock(ctrlPoint->expMutex);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_controlpoint_resubscribe
****************************************/

bool mupnp_controlpoint_resubscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service, long timeout)
{
  mUpnpDevice* rootDev;
  mUpnpSubscriptionRequest* subReq;
  mUpnpSubscriptionResponse* subRes;
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_service_issubscribed(service) == false)
    return false;

  rootDev = mupnp_service_getrootdevice(service);
  if (rootDev == NULL)
    return false;

  subReq = mupnp_event_subscription_request_new();
  /**** Thanks for Theo Beisch (2005/08/25) ****/
  mupnp_event_subscription_request_setrenewsubscription(subReq, service, mupnp_service_getsubscriptionsid(service), timeout);
  subRes = mupnp_event_subscription_request_post(subReq);
  isSuccess = mupnp_event_subscription_response_issuccessful(subRes);
  if (isSuccess == true) {
    mupnp_service_setsubscriptionsid(service, mupnp_event_subscription_response_getsid(subRes));
    mupnp_service_setsubscriptiontimeout(service, mupnp_event_subscription_response_gettimeout(subRes));
    mupnp_service_setsubscriptiontimestamp(service, mupnp_getcurrentsystemtime());
  }
  else
    mupnp_service_clearsubscriptionsid(service);

  mupnp_event_subscription_request_delete(subReq);

  mupnp_log_debug_l4("Leaving...\n");

  return isSuccess;
}

/****************************************
* mupnp_controlpoint_subscribe
****************************************/

bool mupnp_controlpoint_subscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service, long timeout)
{
  mUpnpDevice* rootDev;
  char* roodDevIfAddress;
  mUpnpSubscriptionRequest* subReq;
  mUpnpSubscriptionResponse* subRes;
  char eventSubURL[MUPNP_CONTROLPOINT_EVENTSUBURL_MAX];
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_service_issubscribed(service) == true)
    return mupnp_controlpoint_resubscribe(ctrlPoint, service, timeout);

  rootDev = mupnp_service_getrootdevice(service);
  if (rootDev == NULL)
    return false;

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif

  mupnp_service_lock(service);
  roodDevIfAddress = mupnp_device_getinterfaceaddressfromssdppacket(rootDev);

  mupnp_log_debug_s("roodevifaddress = %s\n", roodDevIfAddress);

  subReq = mupnp_event_subscription_request_new();
  /**** Thanks for Theo Beisch (2005/08/25) ****/
  mupnp_event_subscription_request_setnewsubscription(subReq, service, mupnp_controlpoint_geteventsubcallbackurl(ctrlPoint, roodDevIfAddress, eventSubURL, sizeof(eventSubURL)), timeout);
  /* Set the event key to zero before any events are received */
  mupnp_service_seteventkey(service, 0);
  subRes = mupnp_event_subscription_request_post(subReq);

  isSuccess = mupnp_event_subscription_response_issuccessful(subRes);
  if (isSuccess == true) {
    mupnp_service_setsubscriptionsid(service, mupnp_event_subscription_response_getsid(subRes));
    mupnp_service_setsubscriptiontimeout(service, mupnp_event_subscription_response_gettimeout(subRes));
    mupnp_service_setsubscriptiontimestamp(service, mupnp_getcurrentsystemtime());

    /* New subscription, wake up expirationhandler thread */
    mupnp_mutex_lock(ctrlPoint->expMutex);
    mupnp_cond_signal(ctrlPoint->expCond);
    mupnp_mutex_unlock(ctrlPoint->expMutex);
  }
  else
    mupnp_service_clearsubscriptionsid(service);

  mupnp_event_subscription_request_delete(subReq);
  mupnp_service_unlock(service);

  mupnp_log_debug_l4("Leaving...\n");

  return isSuccess;
}

/****************************************
* mupnp_event_subscription_request_setunsubscription
****************************************/

bool mupnp_controlpoint_unsubscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service)
{
  mUpnpDevice* rootDev;
  mUpnpSubscriptionRequest* subReq;
  mUpnpSubscriptionResponse* subRes;
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  rootDev = mupnp_service_getrootdevice(service);
  if (rootDev == NULL)
    return false;

  subReq = mupnp_event_subscription_request_new();
  /**** Thanks for Theo Beisch (2005/08/25) ****/
  mupnp_event_subscription_request_setunsubscription(subReq, service);
  subRes = mupnp_event_subscription_request_post(subReq);
  isSuccess = mupnp_event_subscription_response_issuccessful(subRes);

  mupnp_service_clearsubscriptionsid(service);
  mupnp_service_setsubscriptiontimeout(service, 0);
  mupnp_service_setsubscriptiontimestamp(service, 0);

  mupnp_event_subscription_request_delete(subReq);

  mupnp_log_debug_l4("Leaving...\n");

  return isSuccess;
}

/****************************************
* mupnp_controlpoint_subscribeall
****************************************/

bool mupnp_controlpoint_subscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev, long timeout)
{
  mUpnpService* service;
  mUpnpDevice* childDev;
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  isSuccess = false;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_controlpoint_subscribe(ctrlPoint, service, timeout) == true)
      isSuccess = true;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(dev)) {
    if (mupnp_controlpoint_subscribeall(ctrlPoint, childDev, timeout) == true)
      isSuccess = true;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return isSuccess;
}

/****************************************
* mupnp_controlpoint_resubscribeall
****************************************/

bool mupnp_controlpoint_resubscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev, long timeout)
{
  mUpnpService* service;
  mUpnpDevice* childDev;
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  isSuccess = false;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_controlpoint_resubscribe(ctrlPoint, service, timeout) == true)
      isSuccess = true;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(dev)) {
    if (mupnp_controlpoint_resubscribeall(ctrlPoint, childDev, timeout) == true)
      isSuccess = true;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return isSuccess;
}

/****************************************
* mupnp_controlpoint_unsubscribeall
****************************************/

bool mupnp_controlpoint_unsubscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev)
{
  mUpnpService* service;
  mUpnpDevice* childDev;
  bool isSuccess;

  mupnp_log_debug_l4("Entering...\n");

  isSuccess = true;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_service_issubscribed(service) == true) {
      if (mupnp_controlpoint_unsubscribe(ctrlPoint, service) == false)
        isSuccess = false;
    }
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    if (mupnp_controlpoint_unsubscribeall(ctrlPoint, childDev) == false)
      isSuccess = false;
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
* MUPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

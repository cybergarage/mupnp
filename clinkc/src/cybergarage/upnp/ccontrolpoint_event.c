/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
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
*		  cg_upnp_controlpoint_subscribe()
*		  cg_upnp_controlpoint_resubscribe()
*		  cg_upnp_controlpoint_unsubscribe()
*		- Changed cg_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*
******************************************************************/

#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/ccontrolpoint.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT) && !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* static function defines
****************************************/

static char *cg_upnp_controlpoint_geteventsubcallbackurl(CgUpnpControlPoint *ctrlPoint, char *host, char *buf, int bufSize);

/****************************************
* cg_upnp_controlpoint_geteventsubcallbackurl
****************************************/

char *cg_upnp_controlpoint_geteventsubcallbackurl(CgUpnpControlPoint *ctrlPoint, char *host, char *buf, int bufSize)
{
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	return cg_net_getmodifierhosturl(host, cg_upnp_controlpoint_geteventport(ctrlPoint), cg_upnp_controlpoint_geteventsuburi(ctrlPoint), buf, bufSize, "<", ">");
}

/****************************************
* cg_upnp_controlpoint_resubscribe
****************************************/

BOOL cg_upnp_controlpoint_resubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout)
{
	CgUpnpDevice *rootDev;
	char *roodDevIfAddress;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	if (cg_upnp_service_issubscribed(service) == FALSE)
		return FALSE;

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	roodDevIfAddress = cg_upnp_device_getinterfaceaddressfromssdppacket(rootDev);

	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setrenewsubscription(subReq, service, cg_upnp_service_getsubscriptionsid(service), timeout, roodDevIfAddress);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		cg_upnp_service_setsubscriptionsid(service, cg_upnp_event_subscription_response_getsid(subRes));
		cg_upnp_service_setsubscriptiontimeout(service, cg_upnp_event_subscription_response_gettimeout(subRes));
	}
	else
		cg_upnp_service_clearsubscriptionsid(service);

	cg_upnp_event_subscription_request_delete(subReq);

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_subscribe
****************************************/

BOOL cg_upnp_controlpoint_subscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout)
{
	CgUpnpDevice *rootDev;
	char *roodDevIfAddress;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	char eventSubURL[CG_UPNP_CONTROLPOINT_EVENTSUBURL_MAX];
	BOOL isSuccess;

	if (cg_upnp_service_issubscribed(service) == TRUE)
		return cg_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout);

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	cg_upnp_service_lock(service);

	roodDevIfAddress = cg_upnp_device_getinterfaceaddressfromssdppacket(rootDev);

	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setnewsubscription(subReq, service, cg_upnp_controlpoint_geteventsubcallbackurl(ctrlPoint, roodDevIfAddress, eventSubURL, sizeof(eventSubURL)), timeout, roodDevIfAddress);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		cg_upnp_service_setsubscriptionsid(service, cg_upnp_event_subscription_response_getsid(subRes));
		cg_upnp_service_setsubscriptiontimeout(service, cg_upnp_event_subscription_response_gettimeout(subRes));
	}
	else
		cg_upnp_service_clearsubscriptionsid(service);

	cg_upnp_event_subscription_request_delete(subReq);

	cg_upnp_service_unlock(service);

	return isSuccess;
}

/****************************************
* cg_upnp_event_subscription_request_setunsubscription
****************************************/

BOOL cg_upnp_controlpoint_unsubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service)
{
	CgUpnpDevice *rootDev;
	char *roodDevIfAddress;
	CgUpnpSubscriptionRequest *subReq;
	CgUpnpSubscriptionResponse *subRes;
	BOOL isSuccess;

	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev == NULL)
		return FALSE;

	roodDevIfAddress = cg_upnp_device_getinterfaceaddressfromssdppacket(rootDev);
	subReq = cg_upnp_event_subscription_request_new();
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_setunsubscription(subReq, service, roodDevIfAddress);
	subRes = cg_upnp_event_subscription_request_post(subReq);
	isSuccess = cg_upnp_event_subscription_response_issuccessful(subRes);
	if (isSuccess == TRUE) {
		cg_upnp_service_setsubscriptionsid(service, cg_upnp_event_subscription_response_getsid(subRes));
		cg_upnp_service_setsubscriptiontimeout(service, cg_upnp_event_subscription_response_gettimeout(subRes));
	}
	else
		cg_upnp_service_clearsubscriptionsid(service);

	cg_upnp_event_subscription_request_delete(subReq);

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_subscribeall
****************************************/

BOOL cg_upnp_controlpoint_subscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	isSuccess = FALSE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_subscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev)) {
		if (cg_upnp_controlpoint_subscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_resubscribeall
****************************************/

BOOL cg_upnp_controlpoint_resubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	isSuccess = FALSE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_resubscribe(ctrlPoint, service, timeout) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev)) {
		if (cg_upnp_controlpoint_resubscribeall(ctrlPoint, childDev, timeout) == TRUE)
			isSuccess = TRUE;
	}

	return isSuccess;
}

/****************************************
* cg_upnp_controlpoint_unsubscribeall
****************************************/

BOOL cg_upnp_controlpoint_unsubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	BOOL isSuccess;

	isSuccess = FALSE;

	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_unsubscribe(ctrlPoint, service) == TRUE)
			isSuccess = TRUE;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev)) {
		if (cg_upnp_controlpoint_unsubscribeall(ctrlPoint, childDev) == TRUE)
			isSuccess = TRUE;
	}

	return isSuccess;
}

/*

bool ControlPoint::isSubscribed(Service *service)
{
	if (service == NULL)
		return false;
	return service->isSubscribed();
}

void ControlPoint::unsubscribe(Device *device)
{
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
}
*/

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif

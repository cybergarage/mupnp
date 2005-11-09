/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
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
*		  cg_upnp_event_subscription_request_setnewsubscription()
*		  cg_upnp_event_subscription_request_setrenewsubscription()
*		  cg_upnp_event_subscription_request_setunsubscription()
*		- Change cg_upnp_event_subscription_request_setservice() to add / to preceede relative path statement (required e.g. for intel devices).
*
*	10/31/05
*		- cg_upnp_event_subscription_request_setservice:
*		  changed eventSubURLStr[1] to [0]
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/upnp/cdevice.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_event_subscription_request_setsid
****************************************/

void cg_upnp_event_subscription_request_setsid(CgUpnpSubscriptionRequest *subReq, char *sid)
{
	CgString *headerSID;
	int uuidIdx;
	
	headerSID = cg_string_new();

	uuidIdx = cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (uuidIdx < 0)
		cg_string_addvalue(headerSID, CG_UPNP_ST_UUID_DEVICE ":");
	cg_string_addvalue(headerSID, sid);
	
	cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_SID, cg_string_getvalue(headerSID));

	cg_string_delete(headerSID);
}

/****************************************
* cg_upnp_event_subscription_request_settimeout
****************************************/

void cg_upnp_event_subscription_request_settimeout(CgUpnpSubscriptionRequest *subReq, long timeout)
{
	CgString *timeoutBuf;

	timeoutBuf = cg_string_new();
	cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_TIMEOUT, cg_upnp_event_subscription_totimeoutheaderstring(timeout, timeoutBuf));
	cg_string_delete(timeoutBuf);
}

/****************************************
* cg_upnp_event_subscription_request_setservice
****************************************/

static void cg_upnp_event_subscription_request_setservice(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service)
{
	CgUpnpDevice *dev;
	CgUpnpDevice *rootDev;
	char *eventSubURLStr;
	CgString *relativeEventSubURLStr;
	CgNetURL *eventSubURL;
	char *httpPostURL;

	eventSubURLStr = cg_upnp_service_geteventsuburl(service);

	httpPostURL = "";

	eventSubURL = cg_net_url_new();
	cg_net_url_set(eventSubURL, eventSubURLStr);
	if (cg_net_url_isabsolute(eventSubURL) == TRUE) {
		cg_http_request_seturi(subReq, cg_net_url_getrequest(eventSubURL));
		httpPostURL = eventSubURLStr;
	}
	else {
		/**** Thanks for Theo Beisch (2005/08/25) ****/
		if (0 < cg_strlen(eventSubURLStr) && eventSubURLStr[0] != '/') {
			relativeEventSubURLStr = cg_string_new();
			cg_string_addvalue(relativeEventSubURLStr, "/");
			cg_string_addvalue(relativeEventSubURLStr, eventSubURLStr);
			cg_http_request_seturi(subReq, cg_string_getvalue(relativeEventSubURLStr));
			cg_string_delete(relativeEventSubURLStr);
		}
		else
			cg_http_request_seturi(subReq, eventSubURLStr);
	}

	if (cg_strlen(httpPostURL) <=0) {
		dev = cg_upnp_service_getdevice(service);
		if (dev != NULL)
			httpPostURL = cg_upnp_device_geturlbase(dev);
	}

	if (cg_strlen(httpPostURL) <=0) {
		rootDev = cg_upnp_service_getrootdevice(service);
		if (rootDev != NULL)
			httpPostURL = cg_upnp_device_geturlbase(rootDev);
	}

	if (cg_strlen(httpPostURL) <=0) {
		rootDev = cg_upnp_service_getrootdevice(service);
		if (rootDev != NULL)
			httpPostURL = cg_upnp_device_getlocationfromssdppacket(rootDev);
	}

	cg_net_url_set(cg_http_request_getposturl(subReq), httpPostURL);

	cg_net_url_delete(eventSubURL);
}

/****************************************
* cg_upnp_event_subscription_request_setnewsubscription
****************************************/

void cg_upnp_event_subscription_request_setnewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *callback, long timeout, char *remoteAddress)
{
	cg_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setcallback(subReq, callback);
	cg_upnp_event_subscription_request_setnt(subReq, CG_UPNP_NT_EVENT);
	cg_upnp_event_subscription_request_settimeout(subReq, timeout);
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_sethost(subReq, remoteAddress);
}

/****************************************
* cg_upnp_event_subscription_request_setrenewsubscription
****************************************/

void cg_upnp_event_subscription_request_setrenewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *uuid, long timeout, char *remoteAddress)
{
	cg_http_request_setmethod(subReq, CG_HTTP_SUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setsid(subReq, uuid);
	cg_upnp_event_subscription_request_settimeout(subReq, timeout);
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_sethost(subReq, remoteAddress);
}

/****************************************
* cg_upnp_event_subscription_request_setunsubscription
****************************************/

void cg_upnp_event_subscription_request_setunsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *remoteAddress)
{
	cg_http_request_setmethod(subReq, CG_HTTP_UNSUBSCRIBE);
	cg_upnp_event_subscription_request_setservice(subReq, service);
	cg_upnp_event_subscription_request_setsid(subReq, cg_upnp_service_getsubscriptionsid(service));
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_upnp_event_subscription_request_sethost(subReq, remoteAddress);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

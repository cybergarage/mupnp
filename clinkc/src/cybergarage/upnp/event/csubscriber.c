/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csubscriber.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>
#include <cybergarage/upnp/event/cnotify.h>
#include <cybergarage/upnp/cupnp_limit.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_subscriber_new
****************************************/

CgUpnpSubscriber *cg_upnp_subscriber_new()
{
	CgUpnpSubscriber *sub = (CgUpnpSubscriber *)malloc(sizeof(CgUpnpSubscriber));
	cg_list_node_init((CgList *)sub);
	
	sub->sid = cg_string_new();
	sub->ifAddr = cg_string_new();
	sub->deliveryURL = cg_net_url_new();
	
	cg_upnp_subscriber_settimeout(sub, 0);
	cg_upnp_subscriber_renew(sub);
	
	return sub;
}

/****************************************
* cg_upnp_subscriber_delete
****************************************/

void cg_upnp_subscriber_delete(CgUpnpSubscriber *sub)
{
	cg_upnp_subscriber_clear(sub);
	cg_list_remove((CgList *)sub);

	cg_string_delete(sub->sid);
	cg_string_delete(sub->ifAddr);
	cg_net_url_delete(sub->deliveryURL);
	
	free(sub);
}

/****************************************
* cg_upnp_subscriber_clear
****************************************/

void cg_upnp_subscriber_clear(CgUpnpSubscriber *sub)
{
}

/****************************************
* cg_upnp_subscriber_renew
****************************************/

void cg_upnp_subscriber_renew(CgUpnpSubscriber *sub)
{
	cg_upnp_subscriber_setsubscriptiontime(sub, time(NULL));
	cg_upnp_subscriber_setnotifycount(sub, 0);
}

/****************************************
* cg_upnp_subscriber_incrementnotifycount
****************************************/

long cg_upnp_subscriber_incrementnotifycount(CgUpnpSubscriber *sub)
{
	if (CG_UPNP_NOTIFY_COUNT_MAX <= sub->notifyCount)
		sub->notifyCount = 0;
	sub->notifyCount++;
	return sub->notifyCount;
}

/****************************************
* cg_upnp_subscriber_isexpired
****************************************/

BOOL cg_upnp_subscriber_isexpired(CgUpnpSubscriber *sub)
{
	long currTime;
	long timeout;
	long expiredTime;
	
	timeout = cg_upnp_subscriber_gettimeout(sub);
	if(timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE) 
		return FALSE; 
			
	currTime = time(NULL);
	expiredTime = cg_upnp_subscriber_getsubscriptiontime(sub) + (timeout*1000);
	if (expiredTime < currTime)
		return TRUE;
			
	return FALSE;
}

/****************************************
* cg_upnp_subscriber_notify
****************************************/

BOOL cg_upnp_subscriber_notify(CgUpnpSubscriber *sub, CgUpnpStateVariable *statVar)
{
	char *varName;
	char *varValue;
	char *host;
	int port;
	CgUpnpNotifyRequest *notifyReq;
	CgUpnpNotifyResponse *notifyRes;
	BOOL notifySuccess;
	
	varName = cg_upnp_statevariable_getname(statVar);
	varValue = cg_upnp_statevariable_getvalue(statVar);

	host = cg_upnp_subscriber_getdeliveryhost(sub);
	port = cg_upnp_subscriber_getdeliveryport(sub);

	notifyReq = cg_upnp_event_notify_request_new();
	cg_upnp_event_notify_request_setpropertysetnode(notifyReq, sub, statVar);
	notifyRes = cg_upnp_event_notify_request_post(notifyReq, host, port);
	notifySuccess = cg_upnp_event_notify_response_issuccessful(notifyRes);
	cg_upnp_event_notify_request_delete(notifyReq);
	
	if (notifySuccess == FALSE)
		return FALSE;
		
	cg_upnp_subscriber_incrementnotifycount(sub);
			
	return TRUE;
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif

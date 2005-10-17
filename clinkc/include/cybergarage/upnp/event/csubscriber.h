/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csubscriber.h
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CSUBSCRIBER_H_
#define _CG_UPNP_CSUBSCRIBER_H_

#include <cybergarage/typedef.h>

#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/net/curl.h>

#include <cybergarage/upnp/cstatevariable.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpSubscriber {
	BOOL headFlag;
	struct _CgUpnpSubscriber *prev;
	struct _CgUpnpSubscriber *next;

	CgString *sid;
	CgString *ifAddr;
	CgNetURL *deliveryURL;
	
	long timeout;
	long notifyCount;
	long subscriptionTime;
} CgUpnpSubscriber, CgUpnpSubscriberList;

/****************************************
* Function (Subscriber)
****************************************/

CgUpnpSubscriber *cg_upnp_subscriber_new();
void cg_upnp_subscriber_delete(CgUpnpSubscriber *sub);
void cg_upnp_subscriber_clear(CgUpnpSubscriber *sub);

void cg_upnp_subscriber_renew(CgUpnpSubscriber *sub);

#define cg_upnp_subscriber_next(sub) (CgUpnpSubscriber *)cg_list_next((CgList *)sub)
#define cg_upnp_subscriber_remove(sub) cg_list_remove((CgList *)sub)

/**** sid ****/
#define cg_upnp_subscriber_setsid(sub, value) cg_string_setvalue(sub->sid, value)
#define cg_upnp_subscriber_getsid(sub) cg_string_getvalue(sub->sid)

/**** timeout ****/
#define cg_upnp_subscriber_settimeout(sub, value) (sub->timeout = value)
#define cg_upnp_subscriber_gettimeout(sub) (sub->timeout)

/**** subscriptionTime ****/
#define cg_upnp_subscriber_setsubscriptiontime(sub, value) (sub->subscriptionTime = value)
#define cg_upnp_subscriber_getsubscriptiontime(sub) (sub->subscriptionTime)

/**** notifycount ****/
#define cg_upnp_subscriber_setnotifycount(sub, value) (sub->notifyCount = value)
#define cg_upnp_subscriber_getnotifycount(sub) (sub->notifyCount)
long cg_upnp_subscriber_incrementnotifycount(CgUpnpSubscriber *sub);

/**** URL ****/
#define cg_upnp_subscriber_setdeliveryurl(sub, value) cg_net_url_set(sub->deliveryURL, value)
#define cg_upnp_subscriber_getdeliveryhost(sub) cg_net_url_gethost(sub->deliveryURL)
#define cg_upnp_subscriber_getdeliverypath(sub) cg_net_url_getpath(sub->deliveryURL)
#define cg_upnp_subscriber_getdeliveryport(sub) cg_net_url_getport(sub->deliveryURL)

/**** Expired ****/
BOOL cg_upnp_subscriber_isexpired(CgUpnpSubscriber *sub);

/**** Notify ****/
BOOL cg_upnp_subscriber_notify(CgUpnpSubscriber *sub, CgUpnpStateVariable *statVar);

/****************************************
* Function (SubscriberList)
****************************************/

CgUpnpSubscriberList *cg_upnp_subscriberlist_new();
void cg_upnp_subscriberlist_delete(CgUpnpSubscriberList *subscriberlist);

#define cg_upnp_subscriberlist_clear(subList) cg_list_clear((CgList *)subList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_subscriber_delete)
#define cg_upnp_subscriberlist_size(subList) cg_list_size((CgList *)subList)
#define cg_upnp_subscriberlist_gets(subList) (CgUpnpSubscriber *)cg_list_next((CgList *)subList)
#define cg_upnp_subscriberlist_add(subList, sub) cg_list_add((CgList *)subList, (CgList *)sub)

#ifdef  __cplusplus
}
#endif

#endif

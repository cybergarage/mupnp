/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cevent.h
*
*	Revision:
*
*	06/20/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CEVENT_H_
#define _CG_UPNP_CEVENT_H_

#include <cybergarage/typedef.h>

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/upnp/event/cproperty.h>
#include <cybergarage/upnp/cservice.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_SUBSCRIPTION_SID_SIZE (((4+1)*4) + 1)
#define CG_UPNP_SUBSCRIPTION_SID_HEADER_SIZE (5 + CG_UPNP_SUBSCRIPTION_SID_SIZE)

#define CG_UPNP_SUBSCRIPTION_XMLNS "urn:schemas-upnp-org:event-1-0"
#define CG_UPNP_SUBSCRIPTION_TIMEOUT_HEADER "Second-"
#define CG_UPNP_SUBSCRIPTION_INFINITE_STRING "infinite"
#define CG_UPNP_SUBSCRIPTION_INFINITE_VALUE (-1)
#define CG_UPNP_SUBSCRIPTION_UUID "uuid:"

#define CG_UPNP_SUBSCRIPTION_CALLBACK_START_WITH  "<"
#define CG_UPNP_SUBSCRIPTION_CALLBACK_END_WITH  ">"

/****************************************
* Data Type
****************************************/

typedef CgHttpRequest CgUpnpSubscriptionRequest;
typedef CgHttpResponse CgUpnpSubscriptionResponse;

typedef void (*CG_UPNP_EVENT_LISTENER)(CgUpnpProperty *);

typedef struct _CgUpnpEventListenerList {
	/** Used by cg_list_* functions to indicate start of list */
	BOOL headFlag;
	/** Used by cg_list_* functions to point to the previous item in list */
	struct _CgUpnpEventListenerList *prev;
	/** Used by cg_list_* functions to point to the next item in list */
	struct _CgUpnpEventListenerList *next;
 
	CG_UPNP_EVENT_LISTENER listener;
} CgUpnpEventListenerList;
 
/****************************************
* Function
****************************************/

char *cg_upnp_event_subscription_totimeoutheaderstring(long time, CgString *buf);
long cg_upnp_event_subscription_gettimeout(char *headerValue);
char * cg_upnp_event_subscription_createsid(char *buf, int bufSize);
char *cg_upnp_event_subscription_tosidheaderstring(char *sid, char *buf, int bufSize);
char *cg_upnp_event_subscription_getsid(char *headerValue);

/****************************************
* Function (Request)
****************************************/

#define cg_upnp_event_subscription_request_new() cg_http_request_new();
#define cg_upnp_event_subscription_request_delete(subReq) cg_http_request_delete(subReq);

#define cg_upnp_event_subscription_issubscriberequest(subReq) cg_streq(cg_http_request_getmethod(subReq), CG_HTTP_SUBSCRIBE)
#define cg_upnp_event_subscription_isunsubscriberequest(subReq) cg_streq(cg_http_request_getmethod(subReq), CG_HTTP_UNSUBSCRIBE)

/**** SID ****/
void cg_upnp_event_subscription_request_setsid(CgUpnpSubscriptionRequest *subReq, char *sid);
#define cg_upnp_event_subscription_request_getsid(subReq) cg_upnp_event_subscription_getsid(cg_http_packet_getheadervalue(((CgHttpPacket*)subReq), CG_HTTP_SID))
#define cg_upnp_event_subscription_request_hassid(subReq) cg_http_packet_hasheader(((CgHttpPacket*)subReq), CG_HTTP_SID)

/**** NT ****/
#define cg_upnp_event_subscription_request_setnt(subReq, value) cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_NT, value)
#define cg_upnp_event_subscription_request_getnt(subReq) cg_http_packet_getheadervalue(((CgHttpPacket*)subReq), CG_HTTP_NT)

/**** Callback ****/
#define cg_upnp_event_subscription_request_setcallback(subReq, value) cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_CALLBACK, value)
#define cg_upnp_event_subscription_request_getcallback(subReq) cg_http_packet_getheadervalue(((CgHttpPacket*)subReq), CG_HTTP_CALLBACK)
#define cg_upnp_event_subscription_request_hascallback(subReq) cg_http_packet_hasheader(((CgHttpPacket*)subReq), CG_HTTP_CALLBACK)

/**** Timeout ****/
void cg_upnp_event_subscription_request_settimeout(CgUpnpSubscriptionRequest *subReq, long timeout);
#define cg_upnp_event_subscription_request_gettimeout(subReq) cg_upnp_event_subscription_gettimeout(cg_http_packet_getheadervalue(((CgHttpPacket*)subReq), CG_HTTP_TIMEOUT))

/**** Subscriptoin Content ****/
void cg_upnp_event_subscription_request_setnewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *callback, long timeout, char *remoteAddress);
void cg_upnp_event_subscription_request_setrenewsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *uuid, long timeout, char *remoteAddress);
void cg_upnp_event_subscription_request_setunsubscription(CgUpnpSubscriptionRequest *subReq, CgUpnpService *service, char *remoteAddress);

/**** Host ****/
#define cg_upnp_event_subscription_request_sethost(subReq, value) cg_http_packet_setheadervalue(((CgHttpPacket*)subReq), CG_HTTP_HOST, value)
#define cg_upnp_event_subscription_request_gethost(subReq) cg_http_packet_getheadervalue(((CgHttpPacket*)subReq), CG_HTTP_HOST)

#define cg_upnp_event_subscription_request_post(subReq) cg_http_request_post(subReq, cg_net_url_gethost(cg_http_request_getposturl(subReq)), cg_net_url_getport(cg_http_request_getposturl(subReq)))

#define cg_upnp_event_subscription_request_postresponse(subReq, subRes) cg_http_request_postresponse(subReq, subRes)

/****************************************
* Function (Response)
****************************************/

#define cg_upnp_event_subscription_response_new() cg_http_response_new();
#define cg_upnp_event_subscription_response_delete(subRes) cg_http_response_delete(subRes);

#define cg_upnp_event_subscription_response_issuccessful(subRes) cg_http_response_issuccessful(subRes)

/**** Status Code ****/
#define cg_upnp_event_subscription_response_setstatuscode(subRes, code) cg_http_response_setstatuscode(subRes, code)
#define cg_upnp_event_subscription_response_getstatuscode(subRes, code) cg_http_response_getstatuscode(subRes)

/**** SID ****/
void cg_upnp_event_subscription_response_setsid(CgUpnpSubscriptionResponse *subRes, char *sid);
#define cg_upnp_event_subscription_response_getsid(subRes) cg_upnp_event_subscription_getsid(cg_http_packet_getheadervalue(((CgHttpPacket*)subRes), CG_HTTP_SID))

/**** Timeout ****/
void cg_upnp_event_subscription_response_settimeout(CgUpnpSubscriptionResponse *subRes, long value);
#define cg_upnp_event_subscription_response_gettimeout(subRes) cg_upnp_event_subscription_gettimeout(cg_http_packet_getheadervalue(((CgHttpPacket*)subRes), CG_HTTP_TIMEOUT))

/****************************************
* Function (Eventlistener list)
****************************************/
 
/**
* Create a new event listener list
*
*/
CgUpnpEventListenerList *cg_upnp_eventlistenerlist_new();
 
/**
* Delete a event listener list.
*
* \param eventListenerList The event listener list to delete
*
*/
void cg_upnp_eventlistenerlist_delete(CgUpnpEventListenerList *eventListenerList);
 
/**
* Clear the contents of a event listener list.
*
* \param eventListenerList The device list to clear
*
*/
#define cg_upnp_eventlistenerlist_clear(eventListenerList) cg_list_clear((CgList *)eventListenerList, (CG_LIST_DESTRUCTORFUNC)free)
 
/**
* Get the size of the device list
*
* \param eventListenerList The device list
*
*/
#define cg_upnp_eventlistenerlist_size(eventListenerList) cg_list_size((CgList *)eventListenerList)
 
/**
* \todo Correct description
*
* \param eventListenerList The event listenerlist list
*
*/
#define cg_upnp_eventlistenerlist_gets(eventListenerList) (CgUpnpEventListenerList*)cg_list_next((CgList *)eventListenerList)
#define cg_upnp_eventlistenerlist_next(eventListenerList) (CgUpnpEventListenerList*)cg_list_next((CgList *)eventListenerList)
 
/**
* Remove a listener from the event listener list
*
* \param eventListenerList The event listener list
* \param listener The listener to remove
*
*/
void cg_upnp_eventlistenerlist_remove(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener);
 
/**
* Add a listener to the event listener list
*
* \param eventListenerList The event listener list
* \param listener The listener to add
*
*/
void cg_upnp_eventlistenerlist_add(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener);
 
/**
* Call all event listeners in the list with the given data.
*
*/
void cg_upnp_eventlistenerlist_notify(CgUpnpEventListenerList* eventListenerList, CgUpnpProperty *property);


#ifdef  __cplusplus
}
#endif

#endif

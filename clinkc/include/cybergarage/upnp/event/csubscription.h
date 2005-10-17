/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csubscription.h
*
*	Revision:
*
*	06/20/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CSUBSCRIPTION_H_
#define _CG_UPNP_CSUBSCRIPTION_H_

#include <cybergarage/typedef.h>

#include <cybergarage/http/chttp.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/


/****************************************
* Data Type
****************************************/

typedef CgHttpRequest CgUpnpSubscriptionRequest;

/****************************************
* Function (Request)
****************************************/

CgUpnpSubscriptionRequest *cg_upnp_event_subscription_request_new();
#define cg_upnp_event_subscription_request_delete(subReq) cg_http_request_delete(subReq)

/**** NT ****/
#define cg_upnp_event_subscription_request_sethost(subReq, ipaddr, port) cg_http_request_sethost(subReq, ipaddr, port)
#define cg_upnp_event_subscription_request_gethost(subReq, value) cg_http_request_gethost(subReq)

/**** NT ****/
#define cg_upnp_event_subscription_request_setnt(subReq, value) cg_http_request_setheadervalue(subReq, CG_HTTP_NT, value)
#define cg_upnp_event_subscription_request_getnt(subReq, value) cg_http_request_getheadervalue(subReq, CG_HTTP_NT)

/**** CALLBACK ****/
#define cg_upnp_event_subscription_request_setcallback(subReq, value) cg_http_request_setheadervalue(subReq, CG_HTTP_CALLBACK, value)
#define cg_upnp_event_subscription_request_getcallback(subReq, value) cg_http_request_getheadervalue(subReq, CG_HTTP_CALLBACK)

/**** SID ****/
#define cg_upnp_event_subscription_request_setsid(subReq, value) cg_http_request_setheadervalue(subReq, CG_HTTP_SID, value)
#define cg_upnp_event_subscription_request_getsid(subReq, value) cg_http_request_getheadervalue(subReq, CG_HTTP_SID)

/**** Timeout ****/
void cg_upnp_event_subscription_request_settimeout(CgUpnpSubscriptionRequest *subReq, long timeout);
long cg_upnp_event_subscription_request_gettimeout(CgUpnpSubscriptionRequest *subReq);

#ifdef  __cplusplus
}
#endif

#endif

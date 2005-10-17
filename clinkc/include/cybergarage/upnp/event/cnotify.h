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

#ifndef _CG_UPNP_CNOTIFY_H_
#define _CG_UPNP_CNOTIFY_H_

#include <cybergarage/typedef.h>

#include <cybergarage/soap/csoap.h>
#include <cybergarage/util/cstring.h>

#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/upnp/event/cproperty.h>
#include <cybergarage/upnp/event/csubscriber.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_NOTIFY_XMLNS "e"
#define CG_UPNP_NOTIFY_PROPERTY "property"
#define CG_UPNP_NOTIFY_PROPERTYSET "propertyset"

/****************************************
* Data Type
****************************************/

typedef CgSoapRequest CgUpnpNotifyRequest;
typedef CgSoapResponse CgUpnpNotifyResponse;

/****************************************
* Function (Request)
****************************************/

CgUpnpNotifyRequest *cg_upnp_event_notify_request_new();
void cg_upnp_event_notify_request_delete(CgUpnpNotifyRequest *notifyReq);
void cg_upnp_event_notify_request_clear(CgUpnpNotifyRequest *notifyReq);

#define cg_upnp_event_notify_request_sethttprequest(notifyReq, httpReq) cg_soap_request_sethttprequest(notifyReq, httpReq)
#define cg_upnp_event_notify_request_post(notifyReq, host, port) cg_soap_request_post(notifyReq, host, port)
#define cg_upnp_event_notify_request_getpropertysetnode(notifyReq) cg_soap_request_getrootnoode(notifyReq)

#define cg_upnp_event_notify_request_setpropertylist(notifyReq, value) cg_soap_request_setuserdata(notifyReq, value)
CgUpnpPropertyList *cg_upnp_event_notify_request_getpropertylist(CgUpnpNotifyRequest *nofityReq);

/**** NT ****/
#define cg_upnp_event_notify_request_setnt(soapReq, value) cg_http_packet_setheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_NT, value)
#define cg_upnp_event_notify_request_getnt(soapReq) cg_http_packet_getheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_NT)

/**** NTS ****/
#define cg_upnp_event_notify_request_setnts(soapReq, value) cg_http_packet_setheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_NTS, value)
#define cg_upnp_event_notify_request_getnts(soapReq) cg_http_packet_getheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_NTS)

/**** SID ****/
void cg_upnp_event_notify_request_setsid(CgUpnpNotifyRequest *soapReq, char *sid);
#define cg_upnp_event_notify_request_getsid(soapReq) cg_upnp_event_subscription_getsid(cg_http_packet_getheadervalue((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SID))

/**** SEQ ****/
#define cg_upnp_event_notify_request_setseq(soapReq, value) cg_http_packet_setheaderlong((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SEQ, value)
#define cg_upnp_event_notify_request_getseq(soapReq) cg_http_packet_getheaderlong((CgHttpPacket*)(soapReq->httpReq), CG_HTTP_SEQ)

BOOL cg_upnp_event_notify_request_setpropertysetnode(CgUpnpNotifyRequest *notifyReq, CgUpnpSubscriber *sub, CgUpnpStateVariable *statVar);

/****************************************
* Function (Response)
****************************************/

#define cg_upnp_event_notify_response_issuccessful(notifyRes) cg_soap_response_issuccessful(notifyRes)

#ifdef  __cplusplus
}
#endif

#endif

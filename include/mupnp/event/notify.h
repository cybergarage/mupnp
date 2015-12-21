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

#ifndef _MUPNP_NOTIFY_H_
#define _MUPNP_NOTIFY_H_

#include <mupnp/typedef.h>

#include <mupnp/soap/soap.h>
#include <mupnp/util/string.h>

#include <mupnp/statevariable.h>
#include <mupnp/event/property.h>
#include <mupnp/event/subscriber.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/** Define for XML namespace
 */
#define MUPNP_NOTIFY_XMLNS "e"

/**
 * Define for notify property
 */
#define MUPNP_NOTIFY_PROPERTY "property"

/**
 * Define for notify property set
 */
#define MUPNP_NOTIFY_PROPERTYSET "propertyset"

/****************************************
* Data Type
****************************************/

/**
 * Data type for UPnP Notification request
 */
typedef mUpnpSoapRequest mUpnpNotifyRequest;

/**
 * Data type for UPnP notify response
 */
typedef mUpnpSoapResponse mUpnpNotifyResponse;

/****************************************
* Function (Request)
****************************************/

/**
 * Create new Notification request
 */
mUpnpNotifyRequest* mupnp_event_notify_request_new();

/**
 * Delete Notification request
 *
 * @param notifyReq Notification request
 */
void mupnp_event_notify_request_delete(mUpnpNotifyRequest* notifyReq);

/**
 * Clear Notification request
 *
 * @param notifyReq Notification request
 */
void mupnp_event_notify_request_clear(mUpnpNotifyRequest* notifyReq);

/**
 * Set HTTP request for event Notification request
 *
 * @param notifyReq Notification request
 * @param httpReq HTTP request
 */
#define mupnp_event_notify_request_sethttprequest(notifyReq, httpReq) mupnp_soap_request_sethttprequest(notifyReq, httpReq)

/**
 * Sends event Notification request to control point
 *
 * @param notifyReq Notification request
 * @param host Host address
 * @param port Host port
 *
 * @return Notify response
 */
#define mupnp_event_notify_request_post(notifyReq, host, port) mupnp_soap_request_post(notifyReq, host, port)

/**
 * Get root node associated with current Notification request
 *
 * @param notifyReq Notification request
 *
 * @return XML node
 */
#define mupnp_event_notify_request_getpropertysetnode(notifyReq) mupnp_soap_request_getrootnoode(notifyReq)

/**
 * Set property list for Notification request
 *
 * @param notifyReq Notification request
 * @param value Property list value
 */
#define mupnp_event_notify_request_setpropertylist(notifyReq, value) mupnp_soap_request_setuserdata(notifyReq, value)

/**
 * Get property list from Notification request
 *
 * @param nofityReq Notification request
 */
mUpnpPropertyList* mupnp_event_notify_request_getpropertylist(mUpnpNotifyRequest* nofityReq);

/**** NT ****/

/**
 * Set NT (Notificatio type) for Notification request
 *
 * @param soapReq Notification request
 * @param value Notification type
 */
#define mupnp_event_notify_request_setnt(soapReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_NT, value)

/**
 * Get NT from notification request
 *
 * @param soapReq Notification request
 */
#define mupnp_event_notify_request_getnt(soapReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_NT)
//Theo Beisch
#define mupnp_event_notify_request_hasnt(soapReq) mupnp_http_packet_hasheader((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_NT)

/**** NTS ****/

/**
 * Set NTS (Notification sub type) for current Notification request
 *
 * @param soapReq Notification request
 * @param value NTS
 */
#define mupnp_event_notify_request_setnts(soapReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_NTS, value)

/**
 * Get NTS from notification request
 *
 * @param soapReq Notification request
 */
#define mupnp_event_notify_request_getnts(soapReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_NTS)
//Theo Beisch
#define mupnp_event_notify_request_hasnts(soapReq) mupnp_http_packet_hasheader(((mUpnpHttpPacket*)soapReq->httpReq), MUPNP_HTTP_NTS)

/**** SID ****/

/**
 * Set SID (Subscription ID) for notification request
 *
 * @param soapReq Notification request
 * @param sid SID
 */
void mupnp_event_notify_request_setsid(mUpnpNotifyRequest* soapReq, char* sid);

/**
 * Get SID from notification request
 *
 * @param soapReq Notification request
 */
#define mupnp_event_notify_request_getsid(soapReq) mupnp_event_subscription_getsid(mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SID))
//Theo Beisch
#define mupnp_event_notify_request_hassid(soapReq) mupnp_http_packet_hasheader(((mUpnpHttpPacket*)(soapReq->httpReq)), MUPNP_HTTP_SID)

/**** SEQ ****/

/**
 * Set sequence number for notification request
 *
 * @param soapReq Notification request
 * @param value Sequence number
 */
#define mupnp_event_notify_request_setseq(soapReq, value) mupnp_http_packet_setheaderlong((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SEQ, value)

/**
 * Get sequence number from notification request
 *
 * @param soapReq Notification request
 */
#define mupnp_event_notify_request_getseq(soapReq) mupnp_http_packet_getheadersizet((mUpnpHttpPacket*)(soapReq->httpReq), MUPNP_HTTP_SEQ)

/** 
 * \todo Check what this does exactly...
 */
bool mupnp_event_notify_request_setpropertysetnode(mUpnpNotifyRequest* notifyReq, mUpnpSubscriber* sub, /* mUpnpService */ void* service, mUpnpStateVariable* statVar);

/****************************************
* Function (Response)
****************************************/

/**
 * Check if notification response was successfull
 *
 * @param notifyRes Notification response
 */
#define mupnp_event_notify_response_issuccessful(notifyRes) mupnp_soap_response_issuccessful(notifyRes)

/**
 * Get the status code of the specified response
 *
 * @param notifyRes Notification response
 */
#define mupnp_event_notify_response_getstatuscode(notifyRes) mupnp_soap_response_getstatuscode(notifyRes)

#ifdef __cplusplus
}
#endif

#endif

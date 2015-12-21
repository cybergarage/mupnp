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

#ifndef _MUPNP_EVENT_H_
#define _MUPNP_EVENT_H_

#include <mupnp/typedef.h>

#include <mupnp/http/http.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>
#include <mupnp/event/property.h>
#include <mupnp/service.h>
#include <mupnp/upnp_function.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/**
 * Definition for SID size
 */
#define MUPNP_SUBSCRIPTION_SID_SIZE (MUPNP_UUID_MAX_LEN + 8)

/**
 * Definition for SID header size
 */
#define MUPNP_SUBSCRIPTION_SID_HEADER_SIZE (5 + MUPNP_SUBSCRIPTION_SID_SIZE)

/**
 * Definition for subscription xml namespace
 */
#define MUPNP_SUBSCRIPTION_XMLNS "urn:schemas-upnp-org:event-1-0"

/**
 * Definition for subscription timeout header prefix
 */
#define MUPNP_SUBSCRIPTION_TIMEOUT_HEADER "Second-"

/**
 * Definition for infinite string in subscription
 */
#define MUPNP_SUBSCRIPTION_INFINITE_STRING "infinite"

/**
 * Definition for infinite value in subscription
 */
#define MUPNP_SUBSCRIPTION_INFINITE_VALUE (-1)

/**
 * Definition for subscription uuid prefix
 */
#define MUPNP_SUBSCRIPTION_UUID "uuid:"

/**
 * Definition for subscription callback start tag
 */
#define MUPNP_SUBSCRIPTION_CALLBACK_START_WITH "<"

/**
 * Definition for subscription callback end tag
 */
#define MUPNP_SUBSCRIPTION_CALLBACK_END_WITH ">"

/**
 * Definition for subscription delay
 */
#define MUPNP_SUBSCRIPTION_DELAY 30

/**
 * Definition for maximum event sequence number
 */
#define MUPNP_EVENT_MAX_SEQ 2147483647

/**
 * Definition for maximum upnp subscription timeout
 */
#define MUPNP_SUBSCRIPTION_MAX_TIMEOUT 300

/****************************************
* Data Type
****************************************/

/**
 * Type definition for subsription request
 */
typedef mUpnpHttpRequest mUpnpSubscriptionRequest;

/**
 * Type definition for subsrciption response
 */
typedef mUpnpHttpResponse mUpnpSubscriptionResponse;

/**
 * Type definition for event listener callback
 */
typedef void (*MUPNP_EVENT_LISTENER)(mUpnpProperty*);

/**
 * Type definition for event listener list
 */
typedef struct _mUpnpEventListenerList {
  MUPNP_LIST_STRUCT_MEMBERS

  MUPNP_EVENT_LISTENER listener;
} mUpnpEventListenerList;

/****************************************
* Function
****************************************/

/**
 * Create timeout header string
 *
 * @param time Timeout
 * @param buf Buffer used when creating header string
 *
 * @return Timeout header C string
 */
const char* mupnp_event_subscription_totimeoutheaderstring(mUpnpTime time, mUpnpString* buf);

/**
 * Get event subscription timeout
 *
 * @param headerValue Header value
 *
 * @return Timeout
 */
mUpnpTime mupnp_event_subscription_gettimeout(const char* headerValue);

/**
 * Create SID for subscription
 *
 * @param buf Character buffer where SID is created
 * @param bufSize Buffer size.
 *
 * @return Pointer to buffer containing SID
 */
const char* mupnp_event_subscription_createsid(char* buf, size_t bufSize);

/**
 * Creates SID header string
 *
 * @param sid Character array containing the SID
 * @param buf A buffer where header string is created
 * @param bufSize Size of the used buffer
 *
 * @return Pointer to buffer containing SID header string
 */
const char* mupnp_event_subscription_tosidheaderstring(const char* sid, char* buf, size_t bufSize);

/**
 * Get SID
 *
 * @param headerValue Raw header C string
 * 
 * @return C string containing SID
 */
const char* mupnp_event_subscription_getsid(const char* headerValue);

/****************************************
* Function (Request)
****************************************/

/**
 * Subscription request constructor
 *
 * @return Pointer to newly created subsription request
 */
#define mupnp_event_subscription_request_new() mupnp_http_request_new();

/**
 * Subscription request destructor
 *
 * @param subReq Reference to the subscription request to be deleted
 */
#define mupnp_event_subscription_request_delete(subReq) mupnp_http_request_delete(subReq);

/**
 * Checks if request type is subscription request
 *
 * @param subReq The request to be checked
 *
 * @return True if request is subscription request, false otherwise
 */
#define mupnp_event_subscription_issubscriberequest(subReq) mupnp_streq(mupnp_http_request_getmethod(subReq), MUPNP_HTTP_SUBSCRIBE)

/**
 * Checks if request type is unsubscription request
 *
 * @param subReq The request to be checked
 *
 * @return True if request is unsubscription request
 */
#define mupnp_event_subscription_isunsubscriberequest(subReq) mupnp_streq(mupnp_http_request_getmethod(subReq), MUPNP_HTTP_UNSUBSCRIBE)

/**** SID ****/

/**
 * Set sid to subscription request
 *
 * @param subReq Subscription request
 * @param sid SID (Subscription id)
 */
void mupnp_event_subscription_request_setsid(mUpnpSubscriptionRequest* subReq, const char* sid);

/**
 * Get sid from subscription request
 *
 * @param subReq Subscription request
 *
 * @return Character array containing the requested SID
 */
#define mupnp_event_subscription_request_getsid(subReq) mupnp_event_subscription_getsid(mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_SID))

/**
 * Checks if subscription object has SID
 *
 * @param subReq Subscription request
 *
 * @return True if subscription has SID, false otherwise
 */
#define mupnp_event_subscription_request_hassid(subReq) mupnp_http_packet_hasheader(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_SID)

/**** NT ****/

/**
 * Set NT (Notification target) for subscription request  
 *
 * @param subReq Subscription request
 * @param value Notification type
 */
#define mupnp_event_subscription_request_setnt(subReq, value) mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_NT, value)

/**
 * Get NT from subscription request
 *
 * @param subReq Subscription request
 *
 * @return Character array containing the NT header value
 */
#define mupnp_event_subscription_request_getnt(subReq) mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_NT)

/**
 * Checks if subscription request has NT
 *
 * @param subReq Subscription request
 *
 * @return True if subscription request contains NT, false otherwise
 */
#define mupnp_event_subscription_request_hasnt(subReq) mupnp_http_packet_hasheader(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_NT)

/**** Callback ****/

/**
 * Set subscription request callback URL
 *
 * @param subReq Subscription request
 * @param value Callback URL string
 */
#define mupnp_event_subscription_request_setcallback(subReq, value) mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_CALLBACK, value)

/**
 * Get subscription callback URL
 *
 * @param subReq Subscription request
 *
 * @return Character array containing the callback URL
 */
#define mupnp_event_subscription_request_getcallback(subReq) mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_CALLBACK)

/**
 * Checks if subscription request has callback URL
 *
 * @param subReq Subscription request
 *
 * @return True if subscription request has callback URL, false otherwise
 */
#define mupnp_event_subscription_request_hascallback(subReq) mupnp_http_packet_hasheader(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_CALLBACK)

/**** Timeout ****/

/**
 * Set timeout for subscription request
 *
 * @param subReq Subscription request
 * @param timeout Timeout value for subscription request
 */
void mupnp_event_subscription_request_settimeout(mUpnpSubscriptionRequest* subReq, mUpnpTime timeout);

/**
 * Get timeout value from subscription request
 *
 * @param subReq Subscription request
 */
#define mupnp_event_subscription_request_gettimeout(subReq) mupnp_event_subscription_gettimeout(mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_TIMEOUT))

/**** Subscription Content ****/

/**
 * (Re)initializes subscription request
 *
 * @param subReq Subscription request
 * @param service Service to be subscribed
 * @param callback URL to be called when service state changes
 * @param timeout Subscription timeout
 */
void mupnp_event_subscription_request_setnewsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service, const char* callback, mUpnpTime timeout);

/**
 * (Re)initializes subscription request to be renewal request
 *
 * @param subReq Subscription request
 * @param service Service to be subscribed
 * @param uuid Devices unique identification string
 * @param timeout Subscription timeout
 */
void mupnp_event_subscription_request_setrenewsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service, const char* uuid, mUpnpTime timeout);

/**
 * (Re)initializes subscription request to be unsubscription request
 *
 * @param subReq Subscription request
 * @param service Service where to be unsubscribed
 */
void mupnp_event_subscription_request_setunsubscription(mUpnpSubscriptionRequest* subReq, mUpnpService* service);

/**** Host ****/

/**
 * Set HTTP host header value
 *
 * @param subReq Subscription request
 * @param value HTTP Host: header content
 */
#define mupnp_event_subscription_request_sethost(subReq, value) mupnp_http_packet_setheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_HOST, value)

/**
 * Get host header value from subscription request
 *
 * @param subReq Subscription request
 */
#define mupnp_event_subscription_request_gethost(subReq) mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subReq), MUPNP_HTTP_HOST)

/**
 * Send subscription request to device
 *
 * @param subReq Subscription request
 */
#define mupnp_event_subscription_request_post(subReq) mupnp_http_request_post(subReq, mupnp_net_url_gethost(mupnp_http_request_getposturl(subReq)), mupnp_net_url_getport(mupnp_http_request_getposturl(subReq)))

/**
 * Send response to subscription request
 *
 * @param subReq Subscription request
 * @param subRes Subscription response
 */
#define mupnp_event_subscription_request_postresponse(subReq, subRes) mupnp_http_request_postresponse(subReq, subRes)

/****************************************
* Function (Response)
****************************************/

/**
 * Constructor for subscription response
 */
#define mupnp_event_subscription_response_new() mupnp_http_response_new();

/**
 * Destructor for subscription response
 *
 * @param subRes Subscription response
 */
#define mupnp_event_subscription_response_delete(subRes) mupnp_http_response_delete(subRes);

/**
 * Checks if subscription was successfull by checking status code from subscription response
 *
 * @param subRes Subscription response
 */
#define mupnp_event_subscription_response_issuccessful(subRes) mupnp_http_response_issuccessful(subRes)

/**** Status Code ****/

/**
 * Set status code for subscription response
 *
 * @param subRes Subscription response
 * @param code Status code
 */
#define mupnp_event_subscription_response_setstatuscode(subRes, code) mupnp_http_response_setstatuscode(subRes, code)

/**
 * Get status code from subscription response object
 *
 * @param subRes Subscription response
 * @param code Status code
 *
 * @return Status code
 *
 * \bug Status code is not needed here! (It is not even passed to the wrapped function)
 */
#define mupnp_event_subscription_response_getstatuscode(subRes, code) mupnp_http_response_getstatuscode(subRes)

/**** SID ****/

/**
 * Set SID (Subscription ID) for response
 *
 * @param subRes Subscription response
 * @param sid Subscription ID
 */
void mupnp_event_subscription_response_setsid(mUpnpSubscriptionResponse* subRes, const char* sid);

/**
 * Get SID
 *
 * @param subRes Subscription request
 *
 * @return C string containing SID
 */
#define mupnp_event_subscription_response_getsid(subRes) mupnp_event_subscription_getsid(mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subRes), MUPNP_HTTP_SID))

/**** Timeout ****/

/**
 * Set HTTP timeout for subscription response
 *
 * @param subRes Subscription response
 * @param value Timeout
 */
void mupnp_event_subscription_response_settimeout(mUpnpSubscriptionResponse* subRes, long value);

/**
 * Get timeout value from subscription response
 *
 * @param subRes Subscription response
 *
 * @return C string containing the timeout
 */
#define mupnp_event_subscription_response_gettimeout(subRes) mupnp_event_subscription_gettimeout(mupnp_http_packet_getheadervalue(((mUpnpHttpPacket*)subRes), MUPNP_HTTP_TIMEOUT))

/**
 * Set response code for subscription response
 *
 * @param subRes Subscription response
 * @param code Response code
 */
void mupnp_event_subscription_subscriberesponse_setresponse(mUpnpSubscriptionResponse* subRes, int code);
/* TODO getter? */

/****************************************
* Function (Eventlistener list)
****************************************/

/**
* Create a new event listener list
*
*/
mUpnpEventListenerList* mupnp_eventlistenerlist_new();

/**
* Delete a event listener list.
*
* \param eventListenerList The event listener list to delete
*
*/
void mupnp_eventlistenerlist_delete(mUpnpEventListenerList* eventListenerList);

/**
* Clear the contents of a event listener list.
*
* \param eventListenerList The device list to clear
*
*/
#define mupnp_eventlistenerlist_clear(eventListenerList) mupnp_list_clear((mUpnpList*)eventListenerList, (MUPNP_LIST_DESTRUCTORFUNC)free)

/**
* Get the size of the device list
*
* \param eventListenerList The device list
*
*/
#define mupnp_eventlistenerlist_size(eventListenerList) mupnp_list_size((mUpnpList*)eventListenerList)

/**
* Fetches next list element from event listener list
*
* @param eventListenerList Event listener list
*/
#define mupnp_eventlistenerlist_gets(eventListenerList) (mUpnpEventListenerList*) mupnp_list_next((mUpnpList*)eventListenerList)

/**
* Fetches next list element from event listener list
*
* @param eventListenerList Event listener list
*/
#define mupnp_eventlistenerlist_next(eventListenerList) (mUpnpEventListenerList*) mupnp_list_next((mUpnpList*)eventListenerList)

/**
* Remove a listener from the event listener list
*
* \param eventListenerList The event listener list
* \param listener The listener to remove
*
*/
void mupnp_eventlistenerlist_remove(mUpnpEventListenerList* eventListenerList, MUPNP_EVENT_LISTENER listener);

/**
* Add a listener to the event listener list
*
* \param eventListenerList The event listener list
* \param listener The listener to add
*
*/
void mupnp_eventlistenerlist_add(mUpnpEventListenerList* eventListenerList, MUPNP_EVENT_LISTENER listener);

/**
 * Call all event listeners in the list with the given data.
 *
 * @param eventListenerList The list to iterate thru
 * @param property The property that has been evented
 */
void mupnp_eventlistenerlist_notify(mUpnpEventListenerList* eventListenerList, mUpnpProperty* property);

#ifdef __cplusplus
}
#endif

#endif

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

#ifndef _MUPNP_SERVICE_H_
#define _MUPNP_SERVICE_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/time.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>
#include <mupnp/util/mutex.h>
#include <mupnp/net/url.h>

#include <mupnp/action.h>
#include <mupnp/statevariable.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/event/subscriber.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************************************************
* Define
*****************************************************************************/

#define MUPNP_SERVICE_ELEM_NAME "service"
#define MUPNP_SERVICELIST_ELEM_NAME "serviceList"

#define MUPNP_SERVICE_TYPE "serviceType"
#define MUPNP_SERVICE_ID "serviceId"
#define MUPNP_SERVICE_SCPDURL "SCPDURL"
#define MUPNP_SERVICE_CONTROL_URL "controlURL"
#define MUPNP_SERVICE_EVENT_SUB_URL "eventSubURL"
#define MUPNP_SERVICE_EVENT_KEY_INVALID 0
#define MUPNP_SERVICE_NOTIFY_WAITTIME 1000

/****************************************************************************
* Data Type
*****************************************************************************/

typedef struct _mUpnpUpnpService {
	BOOL headFlag;
	struct _mUpnpUpnpService *prev;
	struct _mUpnpUpnpService *next;
	mUpnpXmlNodeList *scpdNodeList;
	mUpnpXmlNode *serviceNode;
	mUpnpUpnpActionList *actionList;
	mUpnpUpnpServiceStateTable *serviceStateTable;
	void *parentDevice;
	mUpnpMutex *mutex;
#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
	mUpnpUpnpSubscriberList *subscriberList;
#endif
	BOOL parsed;
	/**** Execution Data ****/
	mUpnpString *subscriptionSid;
	mUpnpTime subscriptionTimeout;

	void *userData;
	size_t eventKey;
	mUpnpTime subscriptionTimestamp;
} mUpnpUpnpService, mUpnpUpnpServiceList;

/****************************************
* Function (Service)
****************************************/

/**
 * Create a new UPnP service
 */
mUpnpUpnpService *mupnp_upnp_service_new();

/**
 * Destroy a UPnP service
 *
 * @param service The service to destroy
 */
void mupnp_upnp_service_delete(mUpnpUpnpService *service);

/**
 * Clear the contents of a UPnP service
 *
 * @param service The service to clear
 */
void mupnp_upnp_service_clear(mUpnpUpnpService *service);

/**
 * Get the next service in a service list. Use as an iterator.
 *
 * @param service Current service
 */
#define mupnp_upnp_service_next(service) (mUpnpUpnpService *)mupnp_list_next((mUpnpList *)service)

/**
 * Check, whether the given XML node is a service root node
 *
 * @param node mUpnpXmlNode*
 * @return TRUE if the node is a service root node; otherwise FALSE
 */
#define mupnp_upnp_service_isservicenode(node) mupnp_xml_node_isname(node, MUPNP_SERVICE_ELEM_NAME)

/**
 * Get the service's SCPD node
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_getscpdnode(service) mupnp_xml_nodelist_gets(service->scpdNodeList)

/**
 * Set the root XML node for the service
 *
 * @param service The service in question
 * @param node The node to set
 */
#define mupnp_upnp_service_setservicenode(service,node) (service->serviceNode = node)

/**
 * Get the root XML node for the service
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_getservicenode(service) (service->serviceNode)

/**
 * Get the parent device's XML node for the service
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_getdevicenode(service) mupnp_xml_node_getparentnode(mupnp_xml_node_getparentnode(service->serviceNode))

/**
 * Create the service's contents from the given XML document
 *
 * @param service The service to create
 * @param description The XML document to parse
 * @param descriptionLen The length of the XML document
 */
BOOL mupnp_upnp_service_parsedescription(mUpnpUpnpService *service, const char *description, size_t descriptionLen);

/**
 * Create the service's contents from the given URL. Gets the XML document
 * from the URL and passes it again to @ref mupnp_upnp_service_parsedescription
 * 
 * @param service The service to create
 * @param url The URL of the document to parse
 */
BOOL mupnp_upnp_service_parsedescriptionurl(mUpnpUpnpService *service, mUpnpNetURL *url);

/**
 * Get the service's description XML nodes as a string
 *
 * @param service The service in question
 * @param descStr The description string
 */
char *mupnp_upnp_service_getdescription(mUpnpUpnpService *service, mUpnpString *descStr);

/**
 * Get the service's parsing status (i.e. whether the service has been
 * completely constructed). Useful mainly in optimized control point mode.
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_isparsed(service) (service->parsed)

/****************************************************************************
 * serviceType
 ****************************************************************************/

/**
 * Set the service's complete type string
 * (ex. "urn:schemas-upnp-org:service:FooService:1")
 *
 * @param service The service in question
 * @param value The type string
 */
#define mupnp_upnp_service_setservicetype(service, value) mupnp_xml_node_setchildnode(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_TYPE, value)

/**
 * Get the service's complete type string
 * (ex. "urn:schemas-upnp-org:service:FooService:1")
 *
 * @param service The service in question
 * @return char*
 */
#define mupnp_upnp_service_getservicetype(service) mupnp_xml_node_getchildnodevalue(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_TYPE)

/** 
 * Get the identifier-part of a service type string (usually "urn") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getidentifier(const char* serviceType);

/** 
 * Get the URN part of a service type string (usually "schemas-upnp-org") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_geturn(const char* serviceType);

/** 
 * Get the service part of a service type string (usually just "service")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getservice(const char* serviceType);

/** 
 * Get the type part of a service type string (ex. "ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_gettype(const char* serviceType);

/** 
 * Get the schema type part of a service type string (without last colon)
 * (ex. "urn:schemas-upnp-org:service:ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_servicetype_getschematype(const char* serviceType);

/** 
 * Get the version part of a service type string (ex. "1")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getversion(const char* serviceType);

/****************************************************************************
 * serviceID
 ****************************************************************************/

/**
 * Set the service's service ID
 *
 * @param service The service in question
 * @param value Service ID string
 */
#define mupnp_upnp_service_setserviceid(service, value) mupnp_xml_node_setchildnode(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_ID, value)

/**
 * Get the service's service ID
 * 
 * @param service The service in question
 * @return char*
 */
#define mupnp_upnp_service_getserviceid(service) mupnp_xml_node_getchildnodevalue(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_ID)

/****************************************************************************
 * SCPDURL
 ****************************************************************************/

/**
 * Set the service's SCPD (service description) URL
 *
 * @param service The service in question
 * @param value The SCPD URL string
 */
#define mupnp_upnp_service_setscpdurl(service, value) mupnp_xml_node_setchildnode(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_SCPDURL, value)

/**
 * Get the service's SCPD (service description) URL
 *
 * @param service The service in question
 * @return mUpnpNetURL Pointer to URL/URI structure
 */

mUpnpNetURL *mupnp_upnp_service_getscpdurl(mUpnpUpnpService *service);

/**
 * Compare the service's SCPD URL and the given location
 * 
 * @param service The service in question
 * @param url The URL (location) to compare
 * @return TRUE if location is found from URL; otherwise FALSE
 */
BOOL mupnp_upnp_service_isscpdurl(mUpnpUpnpService *service, const char *url);

/****************************************************************************
 * controlURL
 ****************************************************************************/

/**
 * Set the service's control URL
 *
 * @param service The service in question
 * @param value The control URL string
 */
#define mupnp_upnp_service_setcontrolurl(service, value) mupnp_xml_node_setchildnode(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_CONTROL_URL, value)

/**
 * Get the service's control URL
 *
 * @param service The service in question
 * @return mUpnpNetURL Pointer to URL/URI structure
 */
mUpnpNetURL *mupnp_upnp_service_getcontrolurl(mUpnpUpnpService *service);

/****************************************************************************
 * eventSubURL
 ****************************************************************************/

/**
 * Get the service's event subscription URL
 * 
 * @param service The service in question
 * @param value The event subscription URL string
 */
#define mupnp_upnp_service_seteventsuburl(service, value) mupnp_xml_node_setchildnode(mupnp_upnp_service_getservicenode(service), MUPNP_SERVICE_EVENT_SUB_URL, value)

/**
 * Get the service's event subscription URL
 *
 * @param service The service in question
 * @return mUpnpNetURL Pointer to URL/URI structure
 */
mUpnpNetURL *mupnp_upnp_service_geteventsuburl(mUpnpUpnpService *service);

/****************************************************************************
 * Action
 ****************************************************************************/

/**
 * Get the service's list of actions (head of list)
 *
 * @param service The service in question
 */
mUpnpUpnpActionList *mupnp_upnp_service_getactionlist(mUpnpUpnpService *service);

/**
 * Get the first action in the service's list of actions. Use as the
 * starting point in iteration loops.
 *
 * @param service The service in question
 */
mUpnpUpnpAction *mupnp_upnp_service_getactions(mUpnpUpnpService *service);

/**
 * Get an action from the service by the action's name
 *
 * @param service The service in question
 * @param name The name of the action to look for
 * @return mUpnpUpnpAction* or NULL
 */
mUpnpUpnpAction *mupnp_upnp_service_getactionbyname(mUpnpUpnpService *service, const char *name);

/**
 * Check, whether a service has an action by the given name
 *
 * @param service The service in question
 * @param name The name of the action
 */
#define mupnp_upnp_service_hasactionbyname(service, name) ((mupnp_upnp_service_getactionbyname(service,name) != NULL) ? TRUE : FALSE)

/****************************************************************************
 * ServiceStateTable
 ****************************************************************************/

/**
 * Get the service's state table
 *
 * @param service The service in question
 */
mUpnpUpnpServiceStateTable *mupnp_upnp_service_getservicestatetable(mUpnpUpnpService *service);

/**
 * Get the first state variable in the service's state table. Use as the
 * starting point in iteration loops.
 *
 * @param service The service in question
 */
mUpnpUpnpStateVariable *mupnp_upnp_service_getstatevariables(mUpnpUpnpService *service);

/**
 * Get a state variable from the service's state table by the variable's name
 *
 * @param service The service in question
 * @param name the name of the variable
 * @return mUpnpUpnpStateVariable* or NULL
 */
mUpnpUpnpStateVariable *mupnp_upnp_service_getstatevariablebyname(mUpnpUpnpService *service, const char *name);

/**
 * Check, whether the service has the given state variable
 *
 * @param service The service in question
 * @param name The name of the state variable
 * @return TRUE if found; otherwise FALSE
 */
#define mupnp_upnp_service_hasstatevariablebyname(service, name) ((mupnp_upnp_service_getstatevariablebyname(service,name) != NULL) ? TRUE : FALSE)

/****************************************************************************
 * Advertisement
 ****************************************************************************/

/**
 * Send a service announcement (advertisement) from the given address
 *
 * @param service The service to advertise
 * @param bindAddr The address to attach to the announcement
 */
BOOL mupnp_upnp_service_announcefrom(mUpnpUpnpService *service, const char *bindAddr);

/**
 * Send a byebye announcement (i.e. a cancelling advertisement) from the given
 * address
 * 
 * @param service The service to announce from
 * @param bindAddr The address to attach to the announcement
 */
BOOL mupnp_upnp_service_byebyefrom(mUpnpUpnpService *service, const char *bindAddr);

/****************************************************************************
 * Listener 
 ****************************************************************************/

/**
 * Set an action listener to the service
 *
 * @param service The service in question
 * @param actionListener A callback function of type MUPNP_ACTION_LISTNER
 */
void mupnp_upnp_service_setactionlistener(mUpnpUpnpService *service, MUPNP_ACTION_LISTNER actionListener);

/**
 * Set a query listener to the service
 *
 * @param service The service in question
 * @param queryListener A callback function of type MUPNP_STATEVARIABLE_LISTNER
 */
void mupnp_upnp_service_setquerylistener(mUpnpUpnpService *service, MUPNP_STATEVARIABLE_LISTNER queryListener);

/****************************************************************************
 * Parent Device
 ****************************************************************************/

/**
 * Set the service's parent device
 *
 * @param service The service in question
 * @param dev The parent device
 */
#define mupnp_upnp_service_setdevice(service, dev) (service->parentDevice = dev)

/**
 * Get the service's parent device
 *
 * @param service The service in question
 * @return mUpnpUpnpDevice*
 */
#define mupnp_upnp_service_getdevice(service) ((mUpnpUpnpDevice *)service->parentDevice)

/****************************************************************************
 * Root Device
 ****************************************************************************/

/**
 * Get the service's top-most root device
 *
 * @param service The service in question
 * @return mUpnpUpnpDevice*
 */
#define mupnp_upnp_service_getrootdevice(service) mupnp_upnp_device_getrootdevice((mUpnpUpnpDevice *)service->parentDevice)

/****************************************************************************
 * Subscriber
 ****************************************************************************/

/**
 * Get the first node in the service's list of event subscribers. Use as the
 * starting point in iteration loops.
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_getsubscribers(service) mupnp_upnp_subscriberlist_gets(service->subscriberList)

/**
 * Get the number of subscribers for the service
 * 
 * @param service The service in question
 * @return int
 */
#define mupnp_upnp_service_getnsubscribers(service) mupnp_upnp_subscriberlist_size(service->subscriberList)

/**
 * Add a subscriber to the service
 *
 * @param service The service in question
 * @param sub The subscriber
 */
BOOL mupnp_upnp_service_addsubscriber(mUpnpUpnpService *service, mUpnpUpnpSubscriber *sub);

/**
 * Remove a subscriber from the service
 *
 * @param service The service in question
 * @param sub The subscriber to remove
 */
BOOL mupnp_upnp_service_removesubscriber(mUpnpUpnpService *service, mUpnpUpnpSubscriber *sub);

/**
 * Find a subscriber from the service's list of subscribers by its SID
 * (Subscription ID)
 *
 * @param service The service in question
 * @param sid The SID
 */
mUpnpUpnpSubscriber *mupnp_upnp_service_getsubscriberbysid(mUpnpUpnpService *service, const char *sid);

/****************************************************************************
 * Announce
 ****************************************************************************/

char *mupnp_upnp_service_getnotifyservicetypent(mUpnpUpnpService *service, char *buf, int bufSize);
char *mupnp_upnp_service_getnotifyservicetypeusn(mUpnpUpnpService *service, char *buf, int bufSize);

/****************************************************************************
 * Notify
 ****************************************************************************/

/**
 * Send a notification of a service's changed state variable
 *
 * @param service The service in question
 * @param statVar The changed state variable
 */
BOOL mupnp_upnp_service_notify(mUpnpUpnpService *service, mUpnpUpnpStateVariable *statVar);

/**
 * Send a notification message to all of the service's subscribers
 *
 * @param service The service in question
 */
BOOL mupnp_upnp_service_notifyall(mUpnpUpnpService *service, BOOL doBracket );

/**
* Create a new thread to send a notification message to all of the service's subscribers
*
 * @param service The service in question
*  @param waitTime The wait time to send
*/
void mupnp_upnp_service_createnotifyallthread(mUpnpUpnpService *service, mUpnpTime waitTime);

/**
 * Send a notification message to all of the service's subscribers
 *
 * @param service The service in question
 * @deprecated Use mupnp_upnp_service_notifyall().
 */
#define mupnp_upnp_service_notifyallstatevariables(service) mupnp_upnp_service_notifyall(service, FALSE);

/****************************************************************************
 * Mutex
 ****************************************************************************/

/**
 * Lock the service's mutex
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_lock(service) mupnp_mutex_lock(service->mutex)

/**
 * Unlock the service's mutex
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_unlock(service) mupnp_mutex_unlock(service->mutex)

/****************************************************************************
 * SID
 ****************************************************************************/

/**
 * Set the service's subscription's SID (subscription ID)
 *
 * @param service The service in question
 * @param value The SID
 */
#define mupnp_upnp_service_setsubscriptionsid(service, value) mupnp_string_setvalue(service->subscriptionSid, value)

/**
 * Get the service's SID (subscription ID)
 *
 * @param service The service in question
 * @return char*
 */
#define mupnp_upnp_service_getsubscriptionsid(service) mupnp_string_getvalue(service->subscriptionSid)

/**
 * Check, whether the service has any subscribers
 *
 * @param service The service in question
 * @return TRUE if at least one subscriber is found; otherwise FALSE
 */
#define mupnp_upnp_service_issubscribed(service) ((0 < mupnp_string_length(service->subscriptionSid)) ? TRUE : FALSE)

/**
 * Clear (i.e. empty) the service's SID (subscription ID)
 *
 * @param service The service in question
 */
#define mupnp_upnp_service_clearsubscriptionsid(service) mupnp_string_setvalue(service->subscriptionSid, "")

/****************************************************************************
 * Timeout
 ****************************************************************************/

/**
 * Set the service's subscription timeout (expiration) value
 *
 * @param service The service in question
 * @param value The timeout value
 */
#define mupnp_upnp_service_setsubscriptiontimeout(service, value) (service->subscriptionTimeout = value)

/**
 * Get the service's subscription timeout (expiration) value
 *
 * @param service The service in question
 * @return long
 */
#define mupnp_upnp_service_getsubscriptiontimeout(service) (service->subscriptionTimeout)

/****************************************************************************
 * User Data
 ****************************************************************************/

/**
 * Set the service's arbitrary user data pointer
 *
 * @param service The service in question
 * @param value The user data pointer
 */
#define mupnp_upnp_service_setuserdata(service, value) (service->userData = value)

/**
 * Get the service's arbitrary user data pointer
 *
 * @param service The service in question
 * @return void*
 */
#define mupnp_upnp_service_getuserdata(service) (service->userData)

/****************************************************************************
 * Event Key
 ****************************************************************************/

/**
 * Set the service's event key (i.e. the sequential number used in events)
 *
 * @param service The service in question
 * @param value The event key value (uint)
 */
#define mupnp_upnp_service_seteventkey(service, value) (service->eventKey = value)

/**
 * Get the service's event key
 *
 * @param service The service in question
 * @return uint
 */
#define mupnp_upnp_service_geteventkey(service) (service->eventKey)

/****************************************************************************
 * Timestamp
 ****************************************************************************/

/**
 * Set the service's subscription time stamp
 *
 * @param service The service in question
 * @param value The time stamp
 */
#define mupnp_upnp_service_setsubscriptiontimestamp(service, value) (service->subscriptionTimestamp = value)

/**
 * Get the service's subscription time stamp
 *
 * @param service The service in question
 * @return mUpnpTime
 */
#define mupnp_upnp_service_getsubscriptiontimestamp(service) (service->subscriptionTimestamp)

/**
 * Get the service's subscription expiration time
 *
 * @param service The service in question
 * @return mUpnpTime
 */
#define mupnp_upnp_service_getsubscriptionexpiration(service) (service->subscriptionTimestamp + service->subscriptionTimeout - MUPNP_SUBSCRIPTION_DELAY)


/****************************************************************************
 * Function (ServiceList)
 ****************************************************************************/

/**
 * Create a new list of services
 */
mUpnpUpnpServiceList *mupnp_upnp_servicelist_new();

/**
 * Destroy a list of services
 *
 * @param serviceList The list of services to destroy
 */
void mupnp_upnp_servicelist_delete(mUpnpUpnpServiceList *serviceList);


/**
 * Clear the contents of a list of services
 *
 * @param serviceList The list of services to clear
 */
#define mupnp_upnp_servicelist_clear(serviceList) mupnp_list_clear((mUpnpList *)serviceList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_service_delete)

/**
 * Get the number of services in a list of services
 *
 * @param serviceList The list of services
 * @return int
 */
#define mupnp_upnp_servicelist_size(serviceList) mupnp_list_size((mUpnpList *)serviceList)

/**
 * Get the first service in a list of services. Use as the starting point in
 * iteration loops.
 *
 * @param serviceList The service list
 */
#define mupnp_upnp_servicelist_gets(serviceList) (mUpnpUpnpService *)mupnp_list_next((mUpnpList *)serviceList)

/**
 * Add a service to a list of services
 * 
 * @param serviceList The list to add the service to
 * @param service The service to add to the list
 */
#define mupnp_upnp_servicelist_add(serviceList, service) mupnp_list_add((mUpnpList *)serviceList, (mUpnpList *)service)

/****************************************************************************
 * Function (SSDP)
 ****************************************************************************/

/**
 * The service's handler function that deals with received SSDP messages.
 *
 * @param service The service in question
 * @param ssdpPkt The received SSDP packet
 */
void mupnp_upnp_service_ssdpmessagereceived(mUpnpUpnpService *service, mUpnpUpnpSSDPPacket *ssdpPkt);

#ifdef  __cplusplus
}
#endif

#endif

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

#ifndef _MUPNP_CONTROLPOINT_H_
#define _MUPNP_CONTROLPOINT_H_

#include <mupnp/util/string.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/cond.h>
#include <mupnp/xml/xml.h>
#include <mupnp/net/interface.h>

#include <mupnp/device.h>
#include <mupnp/argument.h>
#include <mupnp/upnp_status.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/event/event.h>
#include <mupnp/event/notify.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/** Definition for SSDP default search MX */
#define MUPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX 3

/** Definition for control point SSDP minimum delay */
#define MUPNP_CONTROLPOINT_SSDP_MIN_DELAY 20

/** Definition for control point default response port */
#define MUPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT 39400

/** Definition for numeric constant to specify the max tries to open a SSDP Response Port (rosfran.borges) */
#define MUPNP_CONTROLPOINT_SSDP_RESPONSE_PORT_MAX_TRIES_INDEX 80

/** Definition for HTTP event default port */
#define MUPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT 39500

/** Definition for default controlpoint event callback "location" */
#define MUPNP_CONTROLPOINT_HTTP_EVENTSUB_URI "/eventSub"

/****************************************
* Data Type
****************************************/

/**
 * Device listener status parameters.
 * 
 * @ref mUpnpDeviceStatusAdded means a device has been added to local cache.
 * @ref mUpnpDeviceStatusUpdated means a device description has been updated
 * because its IP address or description has changed. The user should renew
 * any active subscriptions if this has been received.
 *
 * @ref mUpnpDeviceStatusInvalid is basically the same as @ref mUpnpDeviceStatusRemoved,
 * but @ref mUpnpDeviceStatusRemoved is sent only when a device leaves the
 * network in a proper way (i.e. with UPnP ByeBye messages). The user should
 * immediately cease using the device, because it will be removed in both cases.
 */
typedef enum _mUpnpDeviceStatus {
  mUpnpDeviceStatusAdded = 0,
  mUpnpDeviceStatusUpdated,
  mUpnpDeviceStatusInvalid,
  mUpnpDeviceStatusRemoved
} mUpnpDeviceStatus;

/**
 * Prototype for control point's device listener callback.
 * 
 * @param udn The UDN of the device, that the status update concerns
 * @param status The new status
 */

typedef struct _mUpnpControlPoint {
  mUpnpMutex* mutex;
  mUpnpXmlNodeList* deviceRootNodeList;
  mUpnpDeviceList* deviceList;
  mUpnpSSDPServerList* ssdpServerList;
  mUpnpSSDPResponseServerList* ssdpResServerList;
  mUpnpHttpServerList* httpServerList;
  void (*deviceListener)(struct _mUpnpControlPoint*, const char*, mUpnpDeviceStatus); /* MUPNP_DEVICE_LISTENER */
  MUPNP_HTTP_LISTENER httpListener;
  MUPNP_SSDP_LISTNER ssdpListener;
  MUPNP_SSDP_RESPONSE_LISTNER ssdpResListener;
  mUpnpEventListenerList* eventListeners;
  int ssdpResPort;
  mUpnpString* httpEventURI;
  int httpEventPort;
  int ssdpSearchMx;
  void* userData;

  /* Expiration handling */
  mUpnpThread* expThread;
  mUpnpMutex* expMutex;
  mUpnpCond* expCond;

  /** List of cached interfaces */
  mUpnpNetworkInterfaceList* ifCache;
} mUpnpControlPoint;

typedef void (*MUPNP_DEVICE_LISTENER)(mUpnpControlPoint* ctrlPoint, const char* udn, mUpnpDeviceStatus status);

/****************************************************************************
 * Control Point top-level control
 ****************************************************************************/

/**
 * Create a new control point. Does not start any threads.
 *
 * @return A newly-created mUpnpControlPoint
 */
mUpnpControlPoint* mupnp_controlpoint_new();

/**
 * Destroy the given control point
 *
 * @param ctrlPoint The control point struct to destroy
 */
void mupnp_controlpoint_delete(mUpnpControlPoint* ctrlPoint);

/**
 * Activate the control point. Starts listening for SSDP messages etc.
 * You must call this function before you can actually use a control point.
 *
 * @param ctrlPoint The control point to start
 *
 * @return true if successful; otherwise false
 *
 */
bool mupnp_controlpoint_start(mUpnpControlPoint* ctrlPoint);

/**
 * Stop the control point. Stops sending/receiveing/responding to any messages.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return true if successful; otherwise false
 *
 */
bool mupnp_controlpoint_stop(mUpnpControlPoint* ctrlPoint);

/**
* Check if  the control point is activated.
*
* @param ctrlPoint The control point to stop
*
* @return true if running; otherwise false
*
*/
bool mupnp_controlpoint_isrunning(mUpnpControlPoint* ctrlPoint);

/****************************************************************************
 * Control Point locking
 ****************************************************************************/

/**
 * Lock the control point's mutex. 
 * The control point should be ALWAYS locked, when a mUpnpDevice*,
 * mUpnpService*, mUpnpAction* or other pointer has been taken into use from
 * the stack. This effectively prevents devices/services from being updated/
 * removed or added while the control point is locked. You should release the
 * lock as soon as possible with @ref mupnp_controlpoint_unlock
 *
 * @note Do NOT save any mUpnp* pointers to user-space variables. Use them
 * only as local variables (inside one function) after gaining a mutex lock.
 * Release the lock as soon as you are done accessing the pointer, and then
 * discard the pointer immediately.
 *
 * @param ctrlPoint The control point in question
 */

#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#define mupnp_controlpoint_lock(ctrlPoint) mupnp_mutex_lock_trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, ctrlPoint->mutex)
#else
bool mupnp_controlpoint_lock(mUpnpControlPoint* ctrlPoint);
#endif
/**
 * Release a previously locked control point mutex.
 * See @ref mupnp_controlpoint_lock for a more detailed description on
 * the control point locking mechanism.
 *
 * @param ctrlPoint The control point in question
 */
#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#define mupnp_controlpoint_unlock(ctrlPoint) mupnp_mutex_unlock_trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, ctrlPoint->mutex)
#else
bool mupnp_controlpoint_unlock(mUpnpControlPoint* ctrlPoint);
#endif

/****************************************************************************
 * Device searching
 ****************************************************************************/

/**
 * Find a device from the control point by the exact type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example: 
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to disregard
 * the version, use @ref mupnp_controlpoint_getdevicebytype
 *
 * @param ctrlPoint Controlpoint in question
 * @param exacttype Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebyexacttype(mUpnpControlPoint* ctrlPoint, const char* exacttype);

/**
 * Find a device from the controlpoint by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use @ref mupnp_devicetype_getversion
 *
 * @param ctrlPoint Controlpoint in question
 * @param type Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebytype(mUpnpControlPoint* ctrlPoint, const char* type);

/**
 * Find a device from the controlpoint by the UDN of the device.
 *
 * @param ctrlPoint Controlpoint in question
 * @param type Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebyudn(mUpnpControlPoint* ctrlPoint, const char* udn);

/****************************************************************************
 * SSDP response
 ****************************************************************************/

/**
 * Get the list of SSDP servers associated to the control point
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_getssdpserverlist(ctrlPoint) (ctrlPoint->ssdpServerList)

/**
 * Get the list of SSDP response servers associated to the control point
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_getssdpresponseserverlist(ctrlPoint) (ctrlPoint->ssdpResServerList)

/**
 * Get the list of HTTP servers associated to the control point
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_gethttpserverlist(ctrlPoint) (ctrlPoint->httpServerList)

/****************************************************************************
 * SSDP listener
 ****************************************************************************/

/**
 * Set an SSDP message listener for the control point. Use this function to
 * get SSDP messages to user-space applications.
 *
 * @param ctrlPoint The control point in question
 * @param func A callback function that is of type @ref MUPNP_SSDP_LISTNER
 */
#define mupnp_controlpoint_setssdplistener(ctrlPoint, func) (ctrlPoint->ssdpListener = func)

/**
 * Get the SSDP message listener for the control point. 
 *
 * @param ctrlPoint The control point in question
 * @return A callback function that is of type @ref MUPNP_SSDP_LISTNER or NULL
 */
#define mupnp_controlpoint_getssdplistener(ctrlPoint) (ctrlPoint->ssdpListener)

/**
 * Set an SSDP response listener for the control point. Use this function to
 * get SSDP responses to user-space applications.
 *
 * @param ctrlPoint The control point in question
 * @param func A callback function that is of type @ref MUPNP_SSDP_RESPONSE_LISTNER
 */
#define mupnp_controlpoint_setssdpresponselistener(ctrlPoint, func) (ctrlPoint->ssdpResListener = func)

/**
 * Get the SSDP response listener for the control point.
 *
 * @param ctrlPoint The control point in question
 * @return A callback function that is of type @ref MUPNP_SSDP_RESPONSE_LISTNER or NULL
 */
#define mupnp_controlpoint_getssdpresponselistener(ctrlPoint) (ctrlPoint->ssdpResListener)

/**
 * Set device listener for the control point.
 *
 * @param ctrlPoint The control point
 * @param func A callback function that is type @ref MUPNP_DEVICE_LISTENER or NULL
 */
#define mupnp_controlpoint_setdevicelistener(ctrlPoint, func) (ctrlPoint->deviceListener = func)

/**
 * Get the device listener for the control point.
 *
 * @param ctrlPoint The control point in question
 * @return A callback function that is of type @ref MUPNP_DEVICE_LISTENER or NULL
 */
#define mupnp_controlpoint_getdevicelistener(ctrlPoint) (ctrlPoint->deviceListener)

/****************************************************************************
 * Event listening
 ****************************************************************************/

/**
 * Set an event listener for the control point. Use this function to
 * get event notifications to user-space applications.
 *
 * @param ctrlPoint The control point in question
 * @param listener A callback function that is of type @ref MUPNP_EVENT_LISTENER
 */
#define mupnp_controlpoint_addeventlistener(ctrlPoint, listener) (mupnp_eventlistenerlist_add(ctrlPoint->eventListeners, listener))

/**
 * Remove an event listener from the control point.
 *
 * @param ctrlPoint The control point in question
 * @param listener The callback function to remove, that is of type @ref MUPNP_EVENT_LISTENER
 */
#define mupnp_controlpoint_removeeventlistener(ctrlPoint, listener) (mupnp_eventlistenerlist_remove(ctrlPoint->eventListeners, listener))

/**
 * Set a single event listener for the control point. Use this function to
 * get event notifications to user-space applications if you need only one listener.
 *
 * @param ctrlPoint The control point in question
 * @param evlistener The callback function to set, that is of type @ref MUPNP_EVENT_LISTENER, or NULL
 */
bool mupnp_controlpoint_seteventlistener(mUpnpControlPoint* ctrlPoint, MUPNP_EVENT_LISTENER evlistener);

/**
 * Get the single event listener for the control point.
 *
 * @param ctrlPoint The control point in question
 * @return The callback function, that is of type @ref MUPNP_EVENT_LISTENER,
 *         or NULL if there is no listener
 */
#define mupnp_controlpoint_geteventlistener(ctrlPoint) (ctrlPoint->eventListeners->next->listener)

/**
 * Get the list of event listeners for the control point.
 *
 * @param ctrlPoint The control point in question
 * @return List of @ref MUPNP_EVENT_LISTENER functions
 */
#define mupnp_controlpoint_geteventlisteners(ctrlPoint) (ctrlPoint->eventListeners)

/****************************************************************************
 * SSDP Response port
 ****************************************************************************/

/**
 * Set the IP port number used for the control point's SSDP responses
 *
 * @param ctrlPoint The control point in question
 * @param port The IP port number
 */
#define mupnp_controlpoint_setssdpresponseport(ctrlPoint, port) (ctrlPoint->ssdpResPort = port)

/**
 * Get the IP port number used for the control point's SSDP responses
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_getssdpresponseport(ctrlPoint) (ctrlPoint->ssdpResPort)

/****************************************************************************
 * Eventing
 ****************************************************************************/

/**
 * Set the IP port number for the control point's event reception
 *
 * @param ctrlPoint The control point in question
 * @param port The IP port number
 */
#define mupnp_controlpoint_seteventport(ctrlPoint, port) (ctrlPoint->httpEventPort = port)

/**
 * Get the IP port number for the control point's event reception
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_geteventport(ctrlPoint) (ctrlPoint->httpEventPort)

/**
 * Set the URI used for the control point's event subscription notifications
 *
 * @param ctrlPoint The control point in question
 * @param uri The URI to set
 */
#define mupnp_controlpoint_seteventsuburi(ctrlPoint, uri) mupnp_string_setvalue(ctrlPoint->httpEventURI, uri)

/**
 * Get the URI used for the control point's event subscription notifications
 *
 * @param ctrlPoint The control point in question
 */
#define mupnp_controlpoint_geteventsuburi(ctrlPoint) mupnp_string_getvalue(ctrlPoint->httpEventURI)

/****************************************************************************
 * M-SEARCH
 ****************************************************************************/

/**
 * Do an M-SEARCH to look for devices in the network.
 *
 * @param ctrlPoint The control point in question
 * @param target The Search Target parameter (ex. "ssdp:all")
 */
bool mupnp_controlpoint_search(mUpnpControlPoint* ctrlPoint, const char* target);

/**
 * Set the MX-parameter used for SSDP searches i.e. Set the time to wait 
 * (in seconds) for device responses to an M-SEARCH
 *
 * @param ctrlPoint The control point in question
 * @param value MX; Time to wait
 */
#define mupnp_controlpoint_setssdpsearchmx(ctrlPoint, value) (ctrlPoint->ssdpSearchMx = value)

/**
 * Get the MX-parameter used for SSDP searches
 *
 * @param ctrlPoint The control point in question
 * @return value MX; Time to wait
 */
#define mupnp_controlpoint_getssdpsearchmx(ctrlPoint) (ctrlPoint->ssdpSearchMx)

/****************************************************************************
 * HTTP listener
 ****************************************************************************/

/**
 * Set an HTTP listener function to the control point. Use this function to
 * get HTTP requests to user-space applications.
 *
 * @param ctrlPoint The control point in question
 * @param func The listener function, that is of type @ref MUPNP_HTTP_LISTENER
 */
#define mupnp_controlpoint_sethttplistener(ctrlPoint, func) (ctrlPoint->httpListener = func)

/**
 * Get the HTTP listener function for the control point.
 *
 * @param ctrlPoint The control point in question
 * @return The listener function, that is of type @ref MUPNP_HTTP_LISTENER
 */
#define mupnp_controlpoint_gethttplistener(ctrlPoint) (ctrlPoint->httpListener)

/**
 * The function that calls all HTTP listener callback functions. Do not call
 * this from applications.
 *
 * @param httpReq The received HTTP request
 */
void mupnp_controlpoint_httprequestreceived(mUpnpHttpRequest* httpReq);

/**
 * When an event is received, update also the associated service's
 * state table to include the evented state variable.
 *
 * @param service The service, whose state table to update
 * @param prop The evented property from which to update
 */
void mupnp_controlpoint_updatestatetablefromproperty(mUpnpService* service,
    mUpnpProperty* prop);

/****************************************************************************
 * User Data
 ****************************************************************************/

/**
 * Set the user data pointer (arbitrary user data) to the control point.
 * 
 * @param ctrlPoint The control point in question
 * @param value Arbitrary user data
 */
#define mupnp_controlpoint_setuserdata(ctrlPoint, value) (ctrlPoint->userData = value)

/**
 * Get the user data pointer (arbitrary user data) from the control point.
 * 
 * @param dev The control point in question
 * @return Pointer to user data or NULL
 */
#define mupnp_controlpoint_getuserdata(ctrlPoint) (ctrlPoint->userData)

/****************************************************************************
 * Service SCPD
 ****************************************************************************/

/**
 * Parse the service description from the service's SCPD URL. Do not call
 * this from user applications.
 * 
 * @param service The service in question
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_parsescservicescpd(mUpnpService* service);

/**
 * Parse the device's services using the received SSDP packet. Do not call this
 * from user applications.
 *
 * @param dev The device in question
 * @param ssdpPkt An SSDP packet
 */
bool mupnp_controlpoint_parseservicesfordevice(mUpnpDevice* dev, mUpnpSSDPPacket* ssdpPkt);

/****************************************************************************
 * Device adding/removal by SSDP packets
 ****************************************************************************/

/**
 * Add a device on the basis of an SSDP packet. Do not call this from user
 * applications.
 *
 * @param ctrlPoint The control point, whose device list to handle
 * @param ssdpPkt The received SSDP packet
 */
void mupnp_controlpoint_adddevicebyssdppacket(mUpnpControlPoint* ctrlPoint,
    mUpnpSSDPPacket* ssdpPkt);

/**
 * Remove a device on the basis of an SSDP packet. Do not call this from user
 * applications.
 *
 * @param ctrlPoint The control point, whose device list to handle
 * @param ssdpPkt The received SSDP packet
 */
void mupnp_controlpoint_removedevicebyssdppacket(mUpnpControlPoint* ctrlPoint,
    mUpnpSSDPPacket* ssdpPkt);

/****************************************
 * Subscription
 ****************************************/

/**
 * Subscribe to a service's events
 * 
 * @param ctrlPoint The control point in use
 * @param service The service to subscribe to
 * @param timeout Timeout for subscription expiration/renewal
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_subscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service, long timeout);

/**
 * Re-subscribe to a service's events (i.e. renew subscription)
 * 
 * @param ctrlPoint The control point in use
 * @param service The service to subscribe to
 * @param timeout Timeout for subscription expiration/renewal
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_resubscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service, long timeout);

/**
 * Unsubscribe to a service's events (i.e. cancel subscription)
 * 
 * @param ctrlPoint The control point in use
 * @param service The service to unsubscribe to
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_unsubscribe(mUpnpControlPoint* ctrlPoint, mUpnpService* service);

/**
 * Subscribe to all of the device's services' events
 * 
 * @param ctrlPoint The control point in use
 * @param dev The device to subscribe to
 * @param timeout Timeout for subscription expiration/renewal
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_subscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev, long timeout);

/**
 * Re-subscribe to all of the device's services' events (i.e. renew subscription)
 * 
 * @param ctrlPoint The control point in use
 * @param dev The device to subscribe to
 * @param timeout Timeout for subscription expiration/renewal
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_resubscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev, long timeout);

/**
 * Unsubscribe to all of the device's services' events (i.e. cancel subscription)
 * 
 * @param ctrlPoint The control point in use
 * @param dev The device to unsubscribe to
 * @return true if successful; otherwise false
 */
bool mupnp_controlpoint_unsubscribeall(mUpnpControlPoint* ctrlPoint, mUpnpDevice* dev);

/****************************************
 * Function (DeviceList)
 ****************************************/

/**
 * Get the head of the control point's list of devices (use for iteration)
 * 
 * @param ctrlPoint The control point in use
 * @return The head of the device list
 */
#define mupnp_controlpoint_getdevices(ctrlPoint) mupnp_devicelist_gets(ctrlPoint->deviceList)

/**
 * Get the number of devices known by the control point
 * 
 * @param ctrlPoint The control point in use
 * @return The number of devices in the control point's device list
 */
#define mupnp_controlpoint_getndevices(ctrlPoint) mupnp_devicelist_size(ctrlPoint->deviceList)

/**
 * Get the head of the control point's list of devices (use for iteration)
 * 
 * @param ctrlPoint The control point in use
 * @param index The index of the device to get
 * @return The specified device
 */
#define mupnp_controlpoint_getdevice(ctrlPoint, index) mupnp_devicelist_get(ctrlPoint->deviceList, index)

/**
 * Add a device to the control point's list of devices. Do not call this from
 * user applications.
 * 
 * @param ctrlPoint The control point in use
 * @param dev The device to add
 */
#define mupnp_controlpoint_adddevice(ctrlPoint, dev) mupnp_devicelist_add(ctrlPoint->deviceList, dev)

/****************************************
 * Environment handling
 ****************************************/

/**
 * Worker thread for handling expirations.
 *
 * \param thread the thread in question
 */
void mupnp_controlpoint_expirationhandler(mUpnpThread* thread);

/**
 * Notify the control point that any IP of the host has been changed.
 *
 * \param ctrlpoint The control point in use
 * \return success of changing used interfaces
 */
bool mupnp_controlpoint_ipchanged(mUpnpControlPoint* ctrlpoint);

/**
 * Get the event subscription callback URI
 *
 * @param ctrlPoint The control point in question
 */
const char* mupnp_controlpoint_geteventsubcallbackurl(mUpnpControlPoint* ctrlPoint, char* ifaddr, char* buf, size_t bufLen);

#ifdef __cplusplus
}
#endif

#endif /* _MUPNP_CONTROLPOINT_H_ */

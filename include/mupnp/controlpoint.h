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

#include <mupnp/net/interface.h>
#include <mupnp/util/cond.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/string.h>
#include <mupnp/xml/xml.h>

#include <mupnp/argument.h>
#include <mupnp/device.h>
#include <mupnp/event/event.h>
#include <mupnp/event/notify.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/upnp_status.h>

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
 * @brief Create a new control point instance
 *
 * @details
 * Allocates and initializes a new UPnP control point object. The control point
 * is initially inactive and does not start any network threads or listeners.
 * You must call mupnp_controlpoint_start() to activate it and begin discovering
 * devices on the network.
 *
 * The function initializes:
 * - Device list (empty)
 * - SSDP servers for multicast listening
 * - HTTP servers for event reception
 * - Internal mutexes for thread safety
 * - Cached network interface list
 *
 * @return A newly-created mUpnpControlPoint on success, or NULL if memory
 *         allocation fails
 *
 * @note The returned control point must be freed with mupnp_controlpoint_delete()
 *       when no longer needed to avoid memory leaks.
 * @note Thread-safe: This function can be called concurrently from multiple threads.
 *
 * @see mupnp_controlpoint_delete()
 * @see mupnp_controlpoint_start()
 *
 * @code
 * // Example: Create and start a control point
 * mUpnpControlPoint* ctrlPoint = mupnp_controlpoint_new();
 * if (ctrlPoint == NULL) {
 *     fprintf(stderr, "Failed to create control point\n");
 *     return -1;
 * }
 * 
 * // Set device listener before starting
 * mupnp_controlpoint_setdevicelistener(ctrlPoint, device_listener_callback);
 * 
 * if (!mupnp_controlpoint_start(ctrlPoint)) {
 *     fprintf(stderr, "Failed to start control point\n");
 *     mupnp_controlpoint_delete(ctrlPoint);
 *     return -1;
 * }
 * 
 * // Perform M-SEARCH to discover devices
 * mupnp_controlpoint_search(ctrlPoint, "ssdp:all");
 * @endcode
 */
mUpnpControlPoint* mupnp_controlpoint_new(void);

/**
 * @brief Destroy a control point and free all associated resources
 *
 * @details
 * Releases all resources associated with the control point, including:
 * - All discovered devices and their descriptions
 * - Network listeners (SSDP, HTTP)
 * - Internal mutexes and threads
 * - Cached network interface information
 *
 * If the control point is still running when this function is called,
 * it will be stopped automatically before resources are freed.
 *
 * @param ctrlPoint The control point to destroy. May be NULL (no-op if NULL).
 *
 * @note After calling this function, the ctrlPoint pointer is invalid and
 *       must not be used.
 * @note This function will block until all internal threads have terminated.
 * @note Thread-safe: Must not be called concurrently with other operations
 *       on the same control point. The caller is responsible for ensuring
 *       exclusive access.
 *
 * @warning Do not call this function while holding locks or from within
 *          event callbacks, as this may cause deadlocks.
 *
 * @see mupnp_controlpoint_new()
 * @see mupnp_controlpoint_stop()
 */
void mupnp_controlpoint_delete(mUpnpControlPoint* ctrlPoint);

/**
 * @brief Activate the control point and start network listeners
 *
 * @details
 * Starts all network services required for UPnP operation:
 * - SSDP multicast listeners on all network interfaces
 * - SSDP unicast response servers (default port: 39400)
 * - HTTP event notification servers (default port: 39500)
 * - Device expiration monitoring thread
 *
 * After successful activation, the control point will:
 * - Receive and process SSDP advertisements (ssdp:alive, ssdp:byebye)
 * - Respond to M-SEARCH requests if configured
 * - Accept event subscriptions and notifications
 * - Automatically remove expired devices
 *
 * The control point must be started before performing any network operations
 * such as device discovery or action invocations.
 *
 * @param ctrlPoint The control point to activate. Must not be NULL.
 *
 * @retval true  Successfully started all services
 * @retval false Failed to start (e.g., port already in use, network error,
 *               insufficient permissions, or ctrlPoint is NULL)
 *
 * @note This function may fail if the required ports are already in use.
 *       Use mupnp_controlpoint_setssdpresponseport() and
 *       mupnp_controlpoint_seteventport() to configure alternative ports
 *       before calling start if needed.
 * @note Thread-safe: Can be called from any thread, but must not be called
 *       concurrently on the same control point.
 * @note Calling this function on an already-running control point has no effect
 *       and returns true.
 *
 * @warning On some platforms, binding to multicast addresses may require
 *          elevated privileges (e.g., root/administrator).
 *
 * @see mupnp_controlpoint_stop()
 * @see mupnp_controlpoint_isrunning()
 * @see mupnp_controlpoint_search()
 */
bool mupnp_controlpoint_start(mUpnpControlPoint* ctrlPoint);

/**
 * @brief Stop the control point and shut down network listeners
 *
 * @details
 * Gracefully shuts down all network services:
 * - Closes all SSDP multicast listeners
 * - Closes SSDP unicast response servers
 * - Closes HTTP event notification servers
 * - Terminates the device expiration monitoring thread
 *
 * After stopping, the control point will no longer:
 * - Receive SSDP advertisements or M-SEARCH requests
 * - Accept event notifications
 * - Monitor device expirations
 *
 * However, the device list is retained and can be accessed after stopping.
 * The control point can be restarted with mupnp_controlpoint_start().
 *
 * @param ctrlPoint The control point to stop. Must not be NULL.
 *
 * @retval true  Successfully stopped all services
 * @retval false Failed to stop (unlikely), or ctrlPoint is NULL
 *
 * @note This function blocks until all listener threads have terminated.
 *       This may take up to a few seconds.
 * @note Calling this function on an already-stopped control point has no effect
 *       and returns true.
 * @note Thread-safe: Can be called from any thread, but must not be called
 *       concurrently on the same control point.
 * @note Active event subscriptions are not automatically cancelled. The
 *       application should explicitly unsubscribe before stopping if cleanup
 *       is required.
 *
 * @see mupnp_controlpoint_start()
 * @see mupnp_controlpoint_isrunning()
 * @see mupnp_controlpoint_delete()
 */
bool mupnp_controlpoint_stop(mUpnpControlPoint* ctrlPoint);

/**
 * @brief Check if the control point is currently active
 *
 * @details
 * Queries whether the control point's network services are active and
 * ready to send/receive UPnP messages.
 *
 * @param ctrlPoint The control point to query. Must not be NULL.
 *
 * @retval true  The control point is active (started)
 * @retval false The control point is inactive, or ctrlPoint is NULL
 *
 * @note Thread-safe: This function can be called from any thread at any time.
 *
 * @see mupnp_controlpoint_start()
 * @see mupnp_controlpoint_stop()
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
 * @brief Find a device by exact type string including version
 *
 * @details
 * Searches the control point's device list for a device whose complete type
 * string matches the given type, including the version number. This performs
 * an exact string comparison.
 *
 * The search includes embedded devices (child devices within root devices).
 *
 * @param ctrlPoint The control point to search. Must not be NULL.
 * @param exacttype The exact device type to match, including version.
 *                  Must not be NULL. Format:
 *                  "urn:schemas-upnp-org:device:<deviceType>:<ver>"
 *                  Example: "urn:schemas-upnp-org:device:MediaServer:2"
 *
 * @return Pointer to the first matching device, or NULL if not found or
 *         if parameters are NULL.
 *
 * @note Thread-safe: The control point should be locked with
 *       mupnp_controlpoint_lock() before calling this function and accessing
 *       the returned device pointer. Release with mupnp_controlpoint_unlock()
 *       as soon as possible.
 * @note Do not cache the returned pointer. Devices may be removed from the
 *       list at any time when the control point is unlocked.
 * @note If multiple devices match, only the first one found is returned.
 *
 * @warning The returned pointer is only valid while the control point is locked.
 *
 * @see mupnp_controlpoint_getdevicebytype()
 * @see mupnp_controlpoint_getdevicebyudn()
 * @see mupnp_controlpoint_lock()
 *
 * @code
 * // Example: Find and use a device safely
 * mupnp_controlpoint_lock(ctrlPoint);
 * 
 * mUpnpDevice* dev = mupnp_controlpoint_getdevicebyexacttype(
 *     ctrlPoint, "urn:schemas-upnp-org:device:MediaServer:2");
 * 
 * if (dev != NULL) {
 *     const char* friendlyName = mupnp_device_getfriendlyname(dev);
 *     printf("Found device: %s\n", friendlyName);
 * }
 * 
 * mupnp_controlpoint_unlock(ctrlPoint);
 * // dev pointer is now invalid
 * @endcode
 */
mUpnpDevice* mupnp_controlpoint_getdevicebyexacttype(mUpnpControlPoint* ctrlPoint, const char* exacttype);

/**
 * @brief Find a device by type, ignoring version number
 *
 * @details
 * Searches the control point's device list for a device whose type matches
 * the given string, excluding the version suffix. This allows matching devices
 * regardless of their version number.
 *
 * The search includes embedded devices (child devices within root devices).
 *
 * @param ctrlPoint The control point to search. Must not be NULL.
 * @param type The device type to match, without version.
 *             Must not be NULL. Format:
 *             "urn:schemas-upnp-org:device:<deviceType>"
 *             Example: "urn:schemas-upnp-org:device:MediaServer"
 *             This will match "...MediaServer:1", "...MediaServer:2", etc.
 *
 * @return Pointer to the first matching device, or NULL if not found or
 *         if parameters are NULL.
 *
 * @note Thread-safe: The control point should be locked with
 *       mupnp_controlpoint_lock() before calling this function.
 * @note To get the actual version of the matched device, use
 *       mupnp_devicetype_getversion() on the device's type string.
 * @note If multiple devices of different versions match, the first one
 *       found is returned (no specific ordering guaranteed).
 *
 * @see mupnp_controlpoint_getdevicebyexacttype()
 * @see mupnp_devicetype_getversion()
 * @see mupnp_controlpoint_lock()
 */
mUpnpDevice* mupnp_controlpoint_getdevicebytype(mUpnpControlPoint* ctrlPoint, const char* type);

/**
 * @brief Find a device by its Unique Device Name (UDN)
 *
 * @details
 * Searches the control point's device list for a device with the specified
 * UDN. The UDN uniquely identifies a device instance, even if multiple devices
 * of the same type exist on the network.
 *
 * The search includes embedded devices (child devices within root devices).
 *
 * @param ctrlPoint The control point to search. Must not be NULL.
 * @param udn The Unique Device Name to search for. Must not be NULL.
 *            Format: "uuid:<device-UUID>"
 *            Example: "uuid:12345678-1234-1234-1234-123456789abc"
 *
 * @return Pointer to the matching device, or NULL if not found or if
 *         parameters are NULL.
 *
 * @note Thread-safe: The control point should be locked with
 *       mupnp_controlpoint_lock() before calling this function.
 * @note UDNs are unique per device instance, so at most one device will match.
 * @note The UDN persists across device restarts if properly implemented by
 *       the device manufacturer.
 *
 * @see mupnp_controlpoint_getdevicebytype()
 * @see mupnp_device_getudn()
 * @see mupnp_controlpoint_lock()
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
 * @brief Send an M-SEARCH multicast message to discover UPnP devices
 *
 * @details
 * Broadcasts an SSDP M-SEARCH request on all network interfaces to discover
 * UPnP devices matching the specified search target. Devices that match will
 * respond with unicast M-SEARCH responses, which are processed asynchronously.
 *
 * The control point must be started (mupnp_controlpoint_start()) before calling
 * this function. Discovered devices trigger the device listener callback if set.
 *
 * Common search targets:
 * - "ssdp:all" - All devices and services
 * - "upnp:rootdevice" - Root devices only
 * - "urn:schemas-upnp-org:device:<deviceType>:<version>" - Specific device type
 * - "urn:schemas-upnp-org:service:<serviceType>:<version>" - Specific service type
 * - "uuid:<device-UUID>" - Specific device by UUID
 *
 * The MX (Maximum Wait) header determines how long devices should wait before
 * responding (default: 3 seconds). Set with mupnp_controlpoint_setssdpsearchmx().
 *
 * @param ctrlPoint The control point to use. Must not be NULL and must be running.
 * @param target The Search Target (ST) string. Must not be NULL or empty.
 *               See UPnP Device Architecture specification for valid values.
 *
 * @retval true  M-SEARCH successfully sent on at least one interface
 * @retval false Failed to send (ctrlPoint is NULL, not running, or network error)
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Devices may take up to MX seconds to respond. Responses are handled
 *       asynchronously via the device listener callback.
 * @note Multiple M-SEARCH requests can be sent without waiting for responses.
 * @note Not all devices respond to M-SEARCH; some only advertise via
 *       periodic ssdp:alive messages.
 *
 * @see mupnp_controlpoint_setdevicelistener()
 * @see mupnp_controlpoint_setssdpsearchmx()
 * @see mupnp_controlpoint_start()
 *
 * @code
 * // Example: Discover all devices
 * mUpnpControlPoint* cp = mupnp_controlpoint_new();
 * mupnp_controlpoint_setdevicelistener(cp, my_device_listener);
 * mupnp_controlpoint_start(cp);
 * 
 * // Search for all devices
 * if (mupnp_controlpoint_search(cp, "ssdp:all")) {
 *     printf("M-SEARCH sent, waiting for responses...\n");
 *     sleep(5); // Wait for devices to respond
 * }
 * 
 * // Search for specific device type
 * mupnp_controlpoint_search(cp, "urn:schemas-upnp-org:device:MediaRenderer:1");
 * @endcode
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
 * @param ctrlPoint The control point in question
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
 * @brief Subscribe to a UPnP service's event notifications
 *
 * @details
 * Sends a SUBSCRIBE request to the service's event subscription URL to receive
 * notifications when the service's state variables change. The service will
 * send an initial event message with current values, followed by updates
 * whenever evented state variables change.
 *
 * The subscription is identified by a Subscription ID (SID) returned by the
 * service and stored in the service object. Event notifications are received
 * on the control point's HTTP event server and dispatched to registered event
 * listeners.
 *
 * Subscriptions must be renewed before they expire using
 * mupnp_controlpoint_resubscribe() or they will be automatically cancelled
 * by the service.
 *
 * @param ctrlPoint The control point to use. Must not be NULL and must be running.
 * @param service The service to subscribe to. Must not be NULL and must have
 *                a valid event subscription URL.
 * @param timeout The requested subscription duration in seconds.
 *                Use 0 or negative value for infinite duration (if supported
 *                by the service). Typical values: 300-1800 seconds.
 *                The service may grant a different duration than requested.
 *
 * @retval true  Subscription successful. Check service's SID for subscription ID.
 * @retval false Subscription failed due to:
 *               - NULL parameters
 *               - Control point not running
 *               - Network error
 *               - Service doesn't support eventing
 *               - Service rejected subscription (e.g., too many subscribers)
 *
 * @note Thread-safe: Can be called from any thread, but the control point
 *       should be locked if accessing device/service pointers.
 * @note Side effect: Updates the service's subscriptionSid and subscriptionTimeout
 *       fields on success.
 * @note Event notifications are delivered asynchronously via event listener
 *       callbacks. Set listeners with mupnp_controlpoint_addeventlistener().
 * @note The initial event notification (containing all evented state variables)
 *       should arrive shortly after subscription.
 *
 * @warning Always unsubscribe before destroying the control point to avoid
 *          orphaned subscriptions on the service side.
 *
 * @see mupnp_controlpoint_resubscribe()
 * @see mupnp_controlpoint_unsubscribe()
 * @see mupnp_controlpoint_addeventlistener()
 * @see mupnp_service_getsubscriptionsid()
 *
 * @code
 * // Example: Subscribe to a service's events
 * mupnp_controlpoint_lock(ctrlPoint);
 * mUpnpDevice* dev = mupnp_controlpoint_getdevicebytype(ctrlPoint, "...:MediaServer");
 * if (dev) {
 *     mUpnpService* service = mupnp_device_getservicebytype(dev, "ContentDirectory");
 *     if (service) {
 *         // Subscribe for 30 minutes
 *         if (mupnp_controlpoint_subscribe(ctrlPoint, service, 1800)) {
 *             printf("Subscribed with SID: %s\n", 
 *                    mupnp_service_getsubscriptionsid(service));
 *         }
 *     }
 * }
 * mupnp_controlpoint_unlock(ctrlPoint);
 * @endcode
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

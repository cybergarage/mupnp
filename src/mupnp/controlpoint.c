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

#include <mupnp/controlpoint.h>
#include <mupnp/upnp_limit.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>
#include <mupnp/net/uri.h>

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_CONTROLPOINT)

/****************************************
 * static function defines
 ****************************************/

#if defined(MUPNP_USE_STDDCP)
char* mupnp_service_getstddcp(mUpnpService* service);
bool mupnp_service_hasstddcp(mUpnpService* service);
#endif

/****************************************
* static function defines
****************************************/

static void mupnp_controlpoint_ssdplistner(mUpnpSSDPPacket* ssdpPkt);
static void mupnp_controlpoint_ssdpresponselistner(mUpnpSSDPPacket* ssdpPkt);

/**
 * Get the event subscription callback URI
 *
 * @param ctrlPoint The control point in question
 * @param ifaddr Interface address
 * @param buf Buffer used to store callback url
 * @param bufLen Buffer length
 *
 * @return Event callback url (pointer to buf)
 */

const char* mupnp_controlpoint_geteventsubcallbackurl(mUpnpControlPoint* ctrlPoint, char* ifaddr, char* buf, size_t bufLen)
{
  mupnp_log_debug_l4("Entering...\n");

  /**** Thanks for Theo Beisch (2005/08/25) ****/
  return mupnp_net_getmodifierhosturl(ifaddr, mupnp_controlpoint_geteventport(ctrlPoint), mupnp_controlpoint_geteventsuburi(ctrlPoint), "<", ">", buf, bufLen);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************************************************
 * Control Point top-level control
 ****************************************************************************/

/**
 * Create a new control point. Does not start any threads.
 *
 * @return A newly-created mUpnpControlPoint
 */
mUpnpControlPoint* mupnp_controlpoint_new()
{
  mUpnpControlPoint* ctrlPoint;

  mupnp_log_debug_l4("Entering...\n");

  ctrlPoint = (mUpnpControlPoint*)malloc(sizeof(mUpnpControlPoint));

  if (NULL != ctrlPoint) {
#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
    mupnp_http_persistentconnection_init();
#endif
    ctrlPoint->mutex = mupnp_mutex_new();
    ctrlPoint->deviceRootNodeList = mupnp_xml_nodelist_new();
    ctrlPoint->deviceList = mupnp_devicelist_new();
    ctrlPoint->ssdpServerList = mupnp_ssdp_serverlist_new();
    ctrlPoint->ssdpResServerList = mupnp_ssdpresponse_serverlist_new();
    ctrlPoint->httpServerList = mupnp_http_serverlist_new();
    ctrlPoint->httpEventURI = mupnp_string_new();
    ctrlPoint->eventListeners = mupnp_eventlistenerlist_new();

    /* Expiration handling */
    ctrlPoint->expThread = mupnp_thread_new();
    mupnp_thread_setaction(ctrlPoint->expThread, mupnp_controlpoint_expirationhandler);
    mupnp_thread_setuserdata(ctrlPoint->expThread, ctrlPoint);
    ctrlPoint->expMutex = mupnp_mutex_new();
    ctrlPoint->expCond = mupnp_cond_new();

    ctrlPoint->ifCache = mupnp_net_interfacelist_new();

    mupnp_controlpoint_setssdplistener(ctrlPoint, NULL);
    mupnp_controlpoint_setssdpresponselistener(ctrlPoint, NULL);
    mupnp_controlpoint_sethttplistener(ctrlPoint, NULL);
    mupnp_controlpoint_setdevicelistener(ctrlPoint, NULL);

    mupnp_controlpoint_setssdpresponseport(ctrlPoint, MUPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT);
    mupnp_controlpoint_setssdpsearchmx(ctrlPoint, MUPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX);
    mupnp_controlpoint_seteventport(ctrlPoint, MUPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT);
    mupnp_controlpoint_seteventsuburi(ctrlPoint, MUPNP_CONTROLPOINT_HTTP_EVENTSUB_URI);

    mupnp_controlpoint_setuserdata(ctrlPoint, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return ctrlPoint;
}

/**
 * Destroy the given control point
 *
 * @param ctrlPoint The control point struct to destroy
 */
void mupnp_controlpoint_delete(mUpnpControlPoint* ctrlPoint)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_controlpoint_stop(ctrlPoint);

  /* Delete cached interfaces */
  mupnp_net_interfacelist_delete(ctrlPoint->ifCache);

  /* Delete expiration handlers */
  mupnp_thread_delete(ctrlPoint->expThread);
  mupnp_mutex_delete(ctrlPoint->expMutex);
  mupnp_cond_delete(ctrlPoint->expCond);

  /* Delete others */
  mupnp_mutex_delete(ctrlPoint->mutex);
  mupnp_xml_nodelist_delete(ctrlPoint->deviceRootNodeList);
  mupnp_devicelist_delete(ctrlPoint->deviceList);
  mupnp_ssdp_serverlist_delete(ctrlPoint->ssdpServerList);
  mupnp_ssdpresponse_serverlist_delete(ctrlPoint->ssdpResServerList);
  mupnp_http_serverlist_delete(ctrlPoint->httpServerList);
  mupnp_string_delete(ctrlPoint->httpEventURI);
  mupnp_eventlistenerlist_delete(ctrlPoint->eventListeners);

#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
  mupnp_http_persistentconnection_clear();
#endif
  free(ctrlPoint);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Activate the control point. Starts listening for SSDP messages etc.
 * You must call this function before you can actually use a control point.
 *
 * @param ctrlPoint The control point to start
 *
 * @return true if successful; otherwise false
 *
 */
bool mupnp_controlpoint_start(mUpnpControlPoint* ctrlPoint)
{
  mUpnpHttpServerList* httpServerList;
  MUPNP_HTTP_LISTENER httpListener;
  int httpEventPort;
  mUpnpSSDPServerList* ssdpServerList;
  mUpnpSSDPResponseServerList* ssdpResServerList;
  int ssdpResPort;
  int ssdpMaxResPort;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_controlpoint_stop(ctrlPoint);

  /* Expiration handling */
  mupnp_thread_start(ctrlPoint->expThread);

  /**** Cache current interfaces ****/
  mupnp_net_gethostinterfaces(ctrlPoint->ifCache);

  /**** HTTP Server ****/
  httpEventPort = mupnp_controlpoint_geteventport(ctrlPoint);
  httpServerList = mupnp_controlpoint_gethttpserverlist(ctrlPoint);
  /* Opening HTTP server may fail, so try many ports */
  while (mupnp_http_serverlist_open(httpServerList, httpEventPort) == false) {
    mupnp_controlpoint_seteventport(ctrlPoint, httpEventPort + 1);
    httpEventPort = mupnp_controlpoint_geteventport(ctrlPoint);
  }
  mupnp_http_serverlist_setuserdata(httpServerList, ctrlPoint);
  httpListener = mupnp_controlpoint_gethttplistener(ctrlPoint);
  if (httpListener == NULL)
    httpListener = mupnp_controlpoint_httprequestreceived;
  mupnp_http_serverlist_setlistener(httpServerList, httpListener);
  mupnp_http_serverlist_start(httpServerList);

  /**** SSDP Server ****/
  ssdpServerList = mupnp_controlpoint_getssdpserverlist(ctrlPoint);
  if (mupnp_ssdp_serverlist_open(ssdpServerList) == false)
    return false;
  mupnp_ssdp_serverlist_setlistener(ssdpServerList, mupnp_controlpoint_ssdplistner);
  mupnp_ssdp_serverlist_setuserdata(ssdpServerList, ctrlPoint);
  if (mupnp_ssdp_serverlist_start(ssdpServerList) == false)
    return false;

  /**** SSDP Response Server ****/
  ssdpResPort = mupnp_controlpoint_getssdpresponseport(ctrlPoint);
  /* below is the max SSDP Response port number to assign (rosfran.borges) */
  ssdpMaxResPort = ssdpResPort + MUPNP_CONTROLPOINT_SSDP_RESPONSE_PORT_MAX_TRIES_INDEX;
  ssdpResServerList = mupnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
  /* Opening SSDP response server may fail, so try many ports */
  while (mupnp_ssdpresponse_serverlist_open(ssdpResServerList, ssdpResPort) == false && (ssdpResPort < ssdpMaxResPort)) {
    mupnp_controlpoint_setssdpresponseport(ctrlPoint, ssdpResPort + 1);
    ssdpResPort = mupnp_controlpoint_getssdpresponseport(ctrlPoint);
  }
  mupnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, mupnp_controlpoint_ssdpresponselistner);
  mupnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, ctrlPoint);
  if (mupnp_ssdpresponse_serverlist_start(ssdpResServerList) == false)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/**
 * Stop the control point. Stops sending/receiveing/responding to any messages.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return true if successful; otherwise false
 *
 */
bool mupnp_controlpoint_stop(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev = NULL;
  mUpnpSSDPServerList* ssdpServerList;
  mUpnpSSDPResponseServerList* ssdpResServerList;
  mUpnpHttpServerList* httpServerList;
  const char* udn = NULL;
  MUPNP_DEVICE_LISTENER listener = mupnp_controlpoint_getdevicelistener(ctrlPoint);

  mupnp_log_debug_l4("Entering...\n");

  /* Stop expiration handling */
  mupnp_thread_stop_with_cond(ctrlPoint->expThread, ctrlPoint->expCond);
  mupnp_log_debug_s("Expiration thread stopped.\n");

  /**** SSDP Server ****/
  ssdpServerList = mupnp_controlpoint_getssdpserverlist(ctrlPoint);
  mupnp_log_debug_s("Stopping ssdp servers.\n");
  mupnp_ssdp_serverlist_setlistener(ssdpServerList, NULL);
  mupnp_ssdp_serverlist_setuserdata(ssdpServerList, NULL);
  mupnp_ssdp_serverlist_stop(ssdpServerList);
  mupnp_log_debug_s("Done\n");
  mupnp_ssdp_serverlist_close(ssdpServerList);
  mupnp_ssdp_serverlist_clear(ssdpServerList);

  /**** SSDP Response Server ****/
  ssdpResServerList = mupnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
  mupnp_log_debug_s("Stopping ssdp response servers.\n");
  mupnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, NULL);
  mupnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, NULL);
  mupnp_ssdpresponse_serverlist_stop(ssdpResServerList);
  mupnp_log_debug_s("Done\n");
  mupnp_ssdpresponse_serverlist_close(ssdpResServerList);
  mupnp_ssdpresponse_serverlist_clear(ssdpResServerList);

  /**** HTTP Server ****/
  httpServerList = mupnp_controlpoint_gethttpserverlist(ctrlPoint);
  mupnp_log_debug_s("Stopping http servers.\n");
  mupnp_http_serverlist_setlistener(httpServerList, NULL);
  mupnp_http_serverlist_stop(httpServerList);
  mupnp_log_debug_s("Done\n");
  mupnp_http_serverlist_close(httpServerList);
  mupnp_http_serverlist_clear(httpServerList);

  mupnp_controlpoint_lock(ctrlPoint);

  mupnp_log_debug_s("Got controlpoint lock.\n");

  /* Unsubscribe from all services */

  for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
       dev != NULL;
       dev = mupnp_device_next(dev)) {
    udn = mupnp_device_getudn(dev);

    /* Call device listener for each device */
    if (udn != NULL && listener != NULL) {
      mupnp_controlpoint_unlock(ctrlPoint);
      listener(ctrlPoint, udn, mUpnpDeviceStatusRemoved);
      mupnp_controlpoint_lock(ctrlPoint);
    }
  }
  /* Empty device cache */
  mupnp_devicelist_clear(ctrlPoint->deviceList);
  mupnp_log_debug_s("Device list cleared.\n");

  mupnp_controlpoint_unlock(ctrlPoint);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return true if running; otherwise false
 *
 */
bool mupnp_controlpoint_isrunning(mUpnpControlPoint* ctrlPoint);
/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return true if running; otherwise false
 *
 */
bool mupnp_controlpoint_isrunning(mUpnpControlPoint* ctrlPoint)
{
  mUpnpHttpServerList* httpServerList;

  httpServerList = mupnp_controlpoint_gethttpserverlist(ctrlPoint);
  if (mupnp_http_serverlist_size(httpServerList) == 0)
    return false;

  return true;
}

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
#ifndef WITH_THREAD_LOCK_TRACE
bool mupnp_controlpoint_lock(mUpnpControlPoint* ctrlPoint)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_mutex_lock(ctrlPoint->mutex);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Release a previously locked control point mutex.
 * See @ref mupnp_controlpoint_lock for a more detailed description on
 * the control point locking mechanism.
 *
 * @param ctrlPoint The control point in question
 */
bool mupnp_controlpoint_unlock(mUpnpControlPoint* ctrlPoint)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_mutex_unlock(ctrlPoint->mutex);

  mupnp_log_debug_l4("Leaving...\n");
}
#endif
/****************************************************************************
 * Device searching
 ****************************************************************************/

/**
 * Find a device from the control point by the exact type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example: 
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to disregard
 * the version, use \ref mupnp_controlpoint_getdevicebytype
 *
 * \param ctrlPoint Controlpoint in question
 * \param exacttype Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebyexacttype(mUpnpControlPoint* ctrlPoint, const char* exacttype)
{
  mUpnpDevice* dev = NULL;
  mUpnpDevice* childDev = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(exacttype) <= 0 || ctrlPoint == NULL) {
    return NULL;
  }

  for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
       dev != NULL;
       dev = mupnp_device_next(dev)) {
    if (mupnp_strcmp(mupnp_device_getdevicetype(dev),
            exacttype)
        == 0) {
      return dev;
    }

    childDev = mupnp_device_getdevicebyexacttype(dev, exacttype);
    if (childDev != NULL) {
      return childDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a device from the controlpoint by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use \ref mupnp_devicetype_getversion
 *
 * \param ctrlPoint Controlpoint in question
 * \param type Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebytype(mUpnpControlPoint* ctrlPoint, const char* type)
{
  mUpnpDevice* dev = NULL;
  mUpnpDevice* childDev = NULL;
  const char* typeString = NULL;
  char* part = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(type) <= 0 || ctrlPoint == NULL) {
    return NULL;
  }

  for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
       dev != NULL;
       dev = mupnp_device_next(dev)) {
    typeString = mupnp_device_getdevicetype(dev);
    if (typeString != NULL) {
      part = mupnp_devicetype_getschematype(typeString);
      if (mupnp_strcmp(part, type) == 0) {
        free(part);

        return dev;
      }
      else {
        free(part);
      }
    }

    childDev = mupnp_device_getdevicebytype(dev, type);
    if (childDev != NULL) {
      return childDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a device from the controlpoint by the UDN of the device.
 *
 * \param ctrlPoint Controlpoint in question
 * \param udn Type of the device
 *
 */
mUpnpDevice* mupnp_controlpoint_getdevicebyudn(mUpnpControlPoint* ctrlPoint, const char* udn)
{
  mUpnpDevice* dev = NULL;
  mUpnpDevice* childDev = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(udn) <= 0 || ctrlPoint == NULL) {
    return NULL;
  }

  for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
       dev != NULL;
       dev = mupnp_device_next(dev)) {
    if (mupnp_strcmp(mupnp_device_getudn(dev), udn) == 0) {
      return dev;
    }
    childDev = mupnp_device_getdevicebyudn(dev, udn);
    if (childDev != NULL) {
      return childDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

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
bool mupnp_controlpoint_parsescservicescpd(mUpnpService* service)
{
  mUpnpNetURL* scpdURL;
  bool scpdParseSuccess;

  mupnp_log_debug_l4("Entering...\n");

  scpdURL = mupnp_service_getscpdurl(service);

  if (NULL == scpdURL)
    return false;

  mupnp_log_debug_s("SCPD URL: %s\n", mupnp_net_url_getrequest(scpdURL));
  scpdParseSuccess = mupnp_service_parsedescriptionurl(service, scpdURL);

  mupnp_net_url_delete(scpdURL);
  if (scpdParseSuccess == true)
    return true;

#if defined(MUPNP_USE_STDDCP)
  if (mupnp_service_hasstddcp(service)) {
    char* stdDCP = mupnp_service_getstddcp(service);
    scpdParseSuccess = mupnp_service_parsedescription(service, stdDCP, mupnp_strlen(stdDCP));
  }
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return scpdParseSuccess;
}

/****************************************
* mupnp_controlpoint_parseservicesfordevice
****************************************/

bool mupnp_controlpoint_parseservicesfordevice(mUpnpDevice* dev, mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_controlpoint_parsescservicescpd(service) == false) {
      return false;
    }
  }

  /* Now only root SCPDs for root services are parsed, but also child 
	   devices' services have to be parsed, so parse them */
  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    if (mupnp_controlpoint_parseservicesfordevice(childDev, ssdpPkt) == false) {
      return false;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

static mUpnpDevice* mupnp_controlpoint_createdevicefromssdkpacket(mUpnpSSDPPacket* ssdpPkt)
{
  const char* location;
  mUpnpNetURL* url;
  mUpnpDevice* dev;
  bool parseSuccess;

  mupnp_log_debug_l4("Entering...\n");

  location = mupnp_ssdp_packet_getlocation(ssdpPkt);
  if (mupnp_strlen(location) <= 0)
    return NULL;

  dev = mupnp_device_new();

  url = mupnp_net_url_new();
  mupnp_net_url_set(url, location);
  parseSuccess = mupnp_device_parsedescriptionurl(dev, url);
  if (parseSuccess) {
    mupnp_device_sethttpport(dev, mupnp_net_url_getport(url));
  }
  mupnp_net_url_delete(url);

  if (parseSuccess == false) {
    mupnp_device_delete(dev);
    return NULL;
  }

  mupnp_device_setssdppacket(dev, ssdpPkt);

#ifndef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == false) {
    mupnp_device_delete(dev);
    return NULL;
  }
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return dev;
}

/****************************************
* mupnp_controlpoint_adddevicebyssdppacket
****************************************/

void mupnp_controlpoint_adddevicebyssdppacket(mUpnpControlPoint* ctrlPoint, mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpDevice* dev = NULL;
  const char* usn = NULL;
  char udn[MUPNP_UDN_LEN_MAX];
  MUPNP_DEVICE_LISTENER listener = NULL;
  mUpnpDeviceStatus status = 0;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_controlpoint_getdevicelistener(ctrlPoint);

  usn = mupnp_ssdp_packet_getusn(ssdpPkt);
  mupnp_usn_getudn(usn, udn, sizeof(udn));

  mupnp_controlpoint_lock(ctrlPoint);

  dev = mupnp_controlpoint_getdevicebyudn(ctrlPoint, udn);

  if (dev != NULL) {
    /* Device was found from local cache */
    if (mupnp_device_updatefromssdppacket(dev, ssdpPkt) == true) {
      mupnp_mutex_lock(ctrlPoint->expMutex);
      mupnp_cond_signal(ctrlPoint->expCond);
      mupnp_mutex_unlock(ctrlPoint->expMutex);

      /* Device was successfully updated */
      status = mUpnpDeviceStatusUpdated;
    }
    else { /* Problems occurred in device update */
      status = mUpnpDeviceStatusInvalid;
    }
  }
  else {
    /* This is a new device */
    dev = mupnp_controlpoint_createdevicefromssdkpacket(ssdpPkt);
    if (dev == NULL) {
      /* Problems occurred in device creation */
      status = mUpnpDeviceStatusInvalid;
    }
    else {
      mupnp_controlpoint_adddevice(ctrlPoint, dev);

      /* Device added, wake up expirationhandler thread */
      mupnp_mutex_lock(ctrlPoint->expMutex);
      mupnp_cond_signal(ctrlPoint->expCond);
      mupnp_mutex_unlock(ctrlPoint->expMutex);

      status = mUpnpDeviceStatusAdded;
    }
  }

  mupnp_controlpoint_unlock(ctrlPoint);

  if (listener != NULL) {
    listener(ctrlPoint, udn, status);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_controlpoint_removedevicebyssdppacket
****************************************/

void mupnp_controlpoint_removedevicebyssdppacket(mUpnpControlPoint* ctrlPoint, mUpnpSSDPPacket* ssdpPkt)
{
  const char* usn;
  char udn[MUPNP_UDN_LEN_MAX];
  mUpnpDevice* dev;
  MUPNP_DEVICE_LISTENER listener = mupnp_controlpoint_getdevicelistener(ctrlPoint);

  mupnp_log_debug_l4("Entering...\n");

  usn = mupnp_ssdp_packet_getusn(ssdpPkt);
  mupnp_usn_getudn(usn, udn, sizeof(udn));

  mupnp_controlpoint_lock(ctrlPoint);

  dev = mupnp_controlpoint_getdevicebyudn(ctrlPoint, udn);

  if (dev == NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    return;
  }

  if (listener != NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    listener(ctrlPoint, udn, mUpnpDeviceStatusRemoved);
    mupnp_controlpoint_lock(ctrlPoint);
  }

  mupnp_device_delete(dev);

  mupnp_controlpoint_unlock(ctrlPoint);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************************************************
 * M-SEARCH
 ****************************************************************************/

/**
 * Do an M-SEARCH to look for devices in the network.
 *
 * @param ctrlPoint The control point in question
 * @param target The Search Target parameter (ex. "ssdp:all")
 */
bool mupnp_controlpoint_search(mUpnpControlPoint* ctrlPoint, const char* target)
{
  mUpnpSSDPRequest* ssdpReq;
  mUpnpSSDPResponseServerList* ssdpResServerList;
  int i = 0;
  bool retval = false;

  mupnp_log_debug_l4("Entering...\n");

  ssdpReq = mupnp_ssdprequest_new();
  mupnp_ssdprequest_setmethod(ssdpReq, MUPNP_HTTP_MSEARCH);
  mupnp_ssdprequest_setst(ssdpReq, target);
  mupnp_ssdprequest_setmx(ssdpReq, mupnp_controlpoint_getssdpsearchmx(ctrlPoint));
  mupnp_ssdprequest_setman(ssdpReq, MUPNP_MAN_DISCOVER);

  mupnp_log_debug("Announcing %d times.\n", mupnp_ssdp_getannouncecount());
  mupnp_ssdprequest_print(ssdpReq);

  for (i = 0; i < mupnp_ssdp_getannouncecount(); i++) {
    ssdpResServerList = mupnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
    retval = (mupnp_ssdpresponse_serverlist_post(ssdpResServerList, ssdpReq) || retval);
    mupnp_wait(MUPNP_CONTROLPOINT_SSDP_MIN_DELAY);
  }

  mupnp_ssdprequest_delete(ssdpReq);

  mupnp_log_debug_l4("Leaving...\n");

  return retval;
}

/****************************************
* mupnp_controlpoint_ipchanged
****************************************/

bool mupnp_controlpoint_ipchanged(mUpnpControlPoint* ctrlPoint)
{
  mUpnpNetworkInterfaceList *current, *added, *removed;
  mUpnpNetworkInterface* netIf;
  mUpnpDevice *dev, *tmp;
  mUpnpSSDPPacket* ssdpPkt;
  char* address;

  mupnp_log_debug_l4("Entering...\n");

  current = mupnp_net_interfacelist_new();
  added = mupnp_net_interfacelist_new();
  removed = mupnp_net_interfacelist_new();

  if (current == NULL || added == NULL || removed == NULL) {
    if (current != NULL)
      mupnp_net_interfacelist_delete(current);
    if (added != NULL)
      mupnp_net_interfacelist_delete(added);
    if (removed != NULL)
      mupnp_net_interfacelist_delete(removed);
    return false;
  }

  /* Get Interface changes */
  mupnp_net_gethostinterfaces(current);
  mupnp_net_interfacelist_getchanges(ctrlPoint->ifCache, current, added, removed);

  /* Remove all devices registered through old interface */
  for (netIf = mupnp_net_interfacelist_gets(removed);
       netIf != NULL;
       netIf = mupnp_net_interface_next(netIf)) {
    mupnp_controlpoint_lock(ctrlPoint);
    tmp = mupnp_controlpoint_getdevices(ctrlPoint);
    while (tmp != NULL) {
      dev = tmp;
      tmp = mupnp_device_next(dev);
      ssdpPkt = mupnp_device_getssdppacket(dev);
      address = mupnp_ssdp_packet_getlocaladdress(ssdpPkt);

      if (address != NULL && mupnp_strcmp(address, mupnp_net_interface_getaddress(netIf)) == 0) {
        /* This device has been received from the 
				   removed interface, so it does not exist */
        mupnp_controlpoint_unlock(ctrlPoint);
        mupnp_controlpoint_removedevicebyssdppacket(ctrlPoint,
            ssdpPkt);
        mupnp_controlpoint_lock(ctrlPoint);
        address = NULL;
        dev = NULL;
        ssdpPkt = NULL;
      }
    }
    mupnp_controlpoint_unlock(ctrlPoint);
  }

  /* Launch new M-SEARCH */
  mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE);

  /**** Cache current interfaces ****/
  mupnp_net_gethostinterfaces(ctrlPoint->ifCache);

  mupnp_net_interfacelist_delete(current);
  mupnp_net_interfacelist_delete(added);
  mupnp_net_interfacelist_delete(removed);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_controlpoint_ssdplistner
****************************************/

static void mupnp_controlpoint_ssdplistner(mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpControlPoint* ctrlPoint;
  MUPNP_SSDP_LISTNER listener;

  mupnp_log_debug_l4("Entering...\n");

  ctrlPoint = (mUpnpControlPoint*)mupnp_ssdp_packet_getuserdata(ssdpPkt);
  if (ctrlPoint == NULL)
    return;

  /* We filter out all but rootdevice, since it must be advertized by all
	 * devices. This way we avoid lots of device updates during advertizement
	 * cycle. */
  if (mupnp_ssdp_packet_isrootdevice(ssdpPkt) == true) {
    if (mupnp_ssdp_packet_isalive(ssdpPkt) == true)
      mupnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
    else if (mupnp_ssdp_packet_isbyebye(ssdpPkt) == true)
      mupnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
  }

  listener = mupnp_controlpoint_getssdplistener(ctrlPoint);
  if (listener != NULL)
    listener(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_controlpoint_ssdpresponselistner
****************************************/

static void mupnp_controlpoint_ssdpresponselistner(mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpControlPoint* ctrlPoint;
  MUPNP_SSDP_RESPONSE_LISTNER listener;

  mupnp_log_debug_l4("Entering...\n");

  ctrlPoint = (mUpnpControlPoint*)mupnp_ssdp_packet_getuserdata(ssdpPkt);
  if (ctrlPoint == NULL)
    return;

  mupnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);

  listener = mupnp_controlpoint_getssdpresponselistener(ctrlPoint);
  if (listener != NULL)
    listener(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

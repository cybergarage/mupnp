/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: ccontrolpoint.c
*
*	Revision:
*
*	05/26/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed mupnp_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*	09/06/05
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Added mupnp_upnp_controlpoint_setuserdata() and mupnp_upnp_controlpoint_getuserdatga().
*	10/31/05
*		- Fixed severe bug in getting child devices
*	12/14/05
*		- Fixed to be more robust on opening servers
*	12/14/05
*		- Fixed mupnp_upnp_controlpoint_removedevicebyssdppacket() to use
*		  mupnp_upnp_device_delete() instead of mupnp_upnp_device_remove().
*
*	11-Jan-06 Heikki Junnila
*		- Removed mupnp_upnp_device_isname and _getbyname because
*		  according to UPnP specs UDN, type or friendlyname is
*		  not the same as the device's name.
*   
*	16-Jan-06 Aapo Makela
*		- Fixed to send many M-SEARCHes (and not too often)
*	25-Jan-05 Aapo Makela
*		- Modified to handle local IP change
*		- Modified to remove devices when CP is stopped.
******************************************************************/

#include <mupnp/controlpoint.h>
#include <mupnp/upnp_limit.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>
#include <mupnp/net/uri.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
 * static function defines
 ****************************************/

#if defined(CG_UPNP_USE_STDDCP)
char *mupnp_upnp_service_getstddcp(CgUpnpService *service);
BOOL mupnp_upnp_service_hasstddcp(CgUpnpService *service);
#endif

/****************************************
* static function defines
****************************************/

static void mupnp_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt);
static void mupnp_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt);

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

const char *mupnp_upnp_controlpoint_geteventsubcallbackurl(CgUpnpControlPoint *ctrlPoint, char *ifaddr, char *buf, size_t bufLen)
{
	mupnp_log_debug_l4("Entering...\n");

	/**** Thanks for Theo Beisch (2005/08/25) ****/
	return mupnp_net_getmodifierhosturl(ifaddr, mupnp_upnp_controlpoint_geteventport(ctrlPoint), mupnp_upnp_controlpoint_geteventsuburi(ctrlPoint),  "<", ">", buf, bufLen);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************************************************
 * Control Point top-level control
 ****************************************************************************/

/**
 * Create a new control point. Does not start any threads.
 *
 * @return A newly-created CgUpnpControlPoint
 */
CgUpnpControlPoint *mupnp_upnp_controlpoint_new()
{
	CgUpnpControlPoint *ctrlPoint;

	mupnp_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)malloc(sizeof(CgUpnpControlPoint));

	if ( NULL != ctrlPoint )
	{
#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS	
		mupnp_http_persistentconnection_init();
#endif
		ctrlPoint->mutex = mupnp_mutex_new();
		ctrlPoint->deviceRootNodeList = mupnp_xml_nodelist_new();
		ctrlPoint->deviceList = mupnp_upnp_devicelist_new();
		ctrlPoint->ssdpServerList = mupnp_upnp_ssdp_serverlist_new();
		ctrlPoint->ssdpResServerList = mupnp_upnp_ssdpresponse_serverlist_new();
		ctrlPoint->httpServerList = mupnp_http_serverlist_new();
		ctrlPoint->httpEventURI = mupnp_string_new();
		ctrlPoint->eventListeners = mupnp_upnp_eventlistenerlist_new();

		/* Expiration handling */
		ctrlPoint->expThread = mupnp_thread_new();
		mupnp_thread_setaction(ctrlPoint->expThread, mupnp_upnp_controlpoint_expirationhandler);
		mupnp_thread_setuserdata(ctrlPoint->expThread, ctrlPoint);
		ctrlPoint->expMutex = mupnp_mutex_new();
		ctrlPoint->expCond = mupnp_cond_new();
		
		ctrlPoint->ifCache = mupnp_net_interfacelist_new();
		
		mupnp_upnp_controlpoint_setssdplistener(ctrlPoint, NULL);
		mupnp_upnp_controlpoint_setssdpresponselistener(ctrlPoint, NULL);
		mupnp_upnp_controlpoint_sethttplistener(ctrlPoint, NULL);
		mupnp_upnp_controlpoint_setdevicelistener(ctrlPoint, NULL);
		
		mupnp_upnp_controlpoint_setssdpresponseport(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT);
		mupnp_upnp_controlpoint_setssdpsearchmx(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX);
		mupnp_upnp_controlpoint_seteventport(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT);
		mupnp_upnp_controlpoint_seteventsuburi(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENTSUB_URI);

		mupnp_upnp_controlpoint_setuserdata(ctrlPoint, NULL);
	}

	mupnp_log_debug_l4("Leaving...\n");

	return ctrlPoint;
}

/**
 * Destroy the given control point
 *
 * @param ctrlPoint The control point struct to destroy
 */
void mupnp_upnp_controlpoint_delete(CgUpnpControlPoint *ctrlPoint)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_controlpoint_stop(ctrlPoint);
	
	/* Delete cached interfaces */
	mupnp_net_interfacelist_delete(ctrlPoint->ifCache);
	
	/* Delete expiration handlers */
	mupnp_thread_delete(ctrlPoint->expThread);
	mupnp_mutex_delete(ctrlPoint->expMutex);
	mupnp_cond_delete(ctrlPoint->expCond);
	
	/* Delete others */
	mupnp_mutex_delete(ctrlPoint->mutex);
	mupnp_xml_nodelist_delete(ctrlPoint->deviceRootNodeList);
	mupnp_upnp_devicelist_delete(ctrlPoint->deviceList);
	mupnp_upnp_ssdp_serverlist_delete(ctrlPoint->ssdpServerList);
	mupnp_upnp_ssdpresponse_serverlist_delete(ctrlPoint->ssdpResServerList);
	mupnp_http_serverlist_delete(ctrlPoint->httpServerList);
	mupnp_string_delete(ctrlPoint->httpEventURI);
	mupnp_upnp_eventlistenerlist_delete(ctrlPoint->eventListeners);	

#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS	
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
 * @return TRUE if successful; otherwise FALSE
 *
 */
BOOL mupnp_upnp_controlpoint_start(CgUpnpControlPoint *ctrlPoint)
{
	CgHttpServerList *httpServerList;
	CG_HTTP_LISTENER httpListener;
	int httpEventPort;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	int ssdpResPort;
	int ssdpMaxResPort;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_controlpoint_stop(ctrlPoint);

	/* Expiration handling */
	mupnp_thread_start(ctrlPoint->expThread);
	
	/**** Cache current interfaces ****/
	mupnp_net_gethostinterfaces(ctrlPoint->ifCache);
	
	/**** HTTP Server ****/
	httpEventPort = mupnp_upnp_controlpoint_geteventport(ctrlPoint);
	httpServerList = mupnp_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	/* Opening HTTP server may fail, so try many ports */
	while(mupnp_http_serverlist_open(httpServerList, httpEventPort) == FALSE) {
		mupnp_upnp_controlpoint_seteventport(ctrlPoint, httpEventPort + 1);
		httpEventPort = mupnp_upnp_controlpoint_geteventport(ctrlPoint);
	}
	mupnp_http_serverlist_setuserdata(httpServerList, ctrlPoint);
	httpListener = mupnp_upnp_controlpoint_gethttplistener(ctrlPoint);
	if (httpListener == NULL)
		httpListener = mupnp_upnp_controlpoint_httprequestreceived;
	mupnp_http_serverlist_setlistener(httpServerList, httpListener);
	mupnp_http_serverlist_start(httpServerList);

	/**** SSDP Server ****/
	ssdpServerList = mupnp_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	if (mupnp_upnp_ssdp_serverlist_open(ssdpServerList) == FALSE)
		return FALSE;
	mupnp_upnp_ssdp_serverlist_setlistener(ssdpServerList, mupnp_upnp_controlpoint_ssdplistner);
	mupnp_upnp_ssdp_serverlist_setuserdata(ssdpServerList, ctrlPoint);
	if (mupnp_upnp_ssdp_serverlist_start(ssdpServerList) == FALSE)
		return FALSE;

	/**** SSDP Response Server ****/
	ssdpResPort = mupnp_upnp_controlpoint_getssdpresponseport(ctrlPoint);
	/* below is the max SSDP Response port number to assign (rosfran.borges) */
	ssdpMaxResPort = ssdpResPort + CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_PORT_MAX_TRIES_INDEX;
	ssdpResServerList = mupnp_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	/* Opening SSDP response server may fail, so try many ports */
	while(mupnp_upnp_ssdpresponse_serverlist_open(ssdpResServerList, ssdpResPort) == FALSE &&
		(ssdpResPort < ssdpMaxResPort) ) {
		mupnp_upnp_controlpoint_setssdpresponseport(ctrlPoint, ssdpResPort + 1);
		ssdpResPort = mupnp_upnp_controlpoint_getssdpresponseport(ctrlPoint);
	}
	mupnp_upnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, mupnp_upnp_controlpoint_ssdpresponselistner);
	mupnp_upnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, ctrlPoint);
	if (mupnp_upnp_ssdpresponse_serverlist_start(ssdpResServerList) == FALSE)
		return FALSE;

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Stop the control point. Stops sending/receiveing/responding to any messages.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
BOOL mupnp_upnp_controlpoint_stop(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	CgHttpServerList *httpServerList;
	const char *udn = NULL;
	CG_UPNP_DEVICE_LISTENER listener = mupnp_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	mupnp_log_debug_l4("Entering...\n");

	/* Stop expiration handling */
	mupnp_thread_stop_with_cond(ctrlPoint->expThread, ctrlPoint->expCond);
	mupnp_log_debug_s("Expiration thread stopped.\n");
	
	/**** SSDP Server ****/
	ssdpServerList = mupnp_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	mupnp_log_debug_s("Stopping ssdp servers.\n");
  mupnp_upnp_ssdp_serverlist_setlistener(ssdpServerList, NULL);
  mupnp_upnp_ssdp_serverlist_setuserdata(ssdpServerList, NULL);
	mupnp_upnp_ssdp_serverlist_stop(ssdpServerList);
	mupnp_log_debug_s("Done\n");
	mupnp_upnp_ssdp_serverlist_close(ssdpServerList);
	mupnp_upnp_ssdp_serverlist_clear(ssdpServerList);
	
	/**** SSDP Response Server ****/
	ssdpResServerList = mupnp_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	mupnp_log_debug_s("Stopping ssdp response servers.\n");
  mupnp_upnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, NULL);
  mupnp_upnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, NULL);
	mupnp_upnp_ssdpresponse_serverlist_stop(ssdpResServerList);
	mupnp_log_debug_s("Done\n");
	mupnp_upnp_ssdpresponse_serverlist_close(ssdpResServerList);
	mupnp_upnp_ssdpresponse_serverlist_clear(ssdpResServerList);
	
	/**** HTTP Server ****/
	httpServerList = mupnp_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	mupnp_log_debug_s("Stopping http servers.\n");
  mupnp_http_serverlist_setlistener(httpServerList, NULL);
	mupnp_http_serverlist_stop(httpServerList);
	mupnp_log_debug_s("Done\n");
	mupnp_http_serverlist_close(httpServerList);
	mupnp_http_serverlist_clear(httpServerList);

	mupnp_upnp_controlpoint_lock(ctrlPoint);

	mupnp_log_debug_s("Got controlpoint lock.\n");
	
	/* Unsubscribe from all services */

	for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = mupnp_upnp_device_next(dev))
	{
		udn = mupnp_upnp_device_getudn(dev);
		
		/* Call device listener for each device */
		if (udn != NULL && listener != NULL)
		{
			mupnp_upnp_controlpoint_unlock(ctrlPoint);
			listener(ctrlPoint, udn, CgUpnpDeviceStatusRemoved);
			mupnp_upnp_controlpoint_lock(ctrlPoint);
		}
	}
	/* Empty device cache */
	mupnp_upnp_devicelist_clear(ctrlPoint->deviceList);
	mupnp_log_debug_s("Device list cleared.\n");

	mupnp_upnp_controlpoint_unlock(ctrlPoint);
		
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if running; otherwise FALSE
 *
 */
BOOL mupnp_upnp_controlpoint_isrunning(CgUpnpControlPoint *ctrlPoint);
/**
 * Check if  the control point is activated.
 *
 * @param ctrlPoint The control point to stop
 *
 * @return TRUE if running; otherwise FALSE
 *
 */
BOOL mupnp_upnp_controlpoint_isrunning(CgUpnpControlPoint *ctrlPoint)
{
	CgHttpServerList *httpServerList;
	
	httpServerList = mupnp_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	if (mupnp_http_serverlist_size(httpServerList) == 0)
		return FALSE;
	
	return TRUE;
}

/****************************************************************************
 * Control Point locking
 ****************************************************************************/
/**
 * Lock the control point's mutex. 
 * The control point should be ALWAYS locked, when a CgUpnpDevice*,
 * CgUpnpService*, CgUpnpAction* or other pointer has been taken into use from
 * the stack. This effectively prevents devices/services from being updated/
 * removed or added while the control point is locked. You should release the
 * lock as soon as possible with @ref mupnp_upnp_controlpoint_unlock
 *
 * @note Do NOT save any CgUpnp* pointers to user-space variables. Use them
 * only as local variables (inside one function) after gaining a mutex lock.
 * Release the lock as soon as you are done accessing the pointer, and then
 * discard the pointer immediately.
 *
 * @param ctrlPoint The control point in question
 */
#ifndef WITH_THREAD_LOCK_TRACE
BOOL mupnp_upnp_controlpoint_lock(CgUpnpControlPoint *ctrlPoint)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_mutex_lock(ctrlPoint->mutex);

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Release a previously locked control point mutex.
 * See @ref mupnp_upnp_controlpoint_lock for a more detailed description on
 * the control point locking mechanism.
 *
 * @param ctrlPoint The control point in question
 */
BOOL mupnp_upnp_controlpoint_unlock(CgUpnpControlPoint *ctrlPoint)
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
 * the version, use \ref mupnp_upnp_controlpoint_getdevicebytype
 *
 * \param ctrlPoint Controlpoint in question
 * \param exacttype Type of the device
 *
 */
CgUpnpDevice *mupnp_upnp_controlpoint_getdevicebyexacttype(CgUpnpControlPoint *ctrlPoint,
						   char *exacttype)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(exacttype) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}
	
	for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = mupnp_upnp_device_next(dev))
	{
		if (mupnp_strcmp(mupnp_upnp_device_getdevicetype(dev),
			      exacttype) == 0)
		{
			return dev;
		}

		childDev = mupnp_upnp_device_getdevicebyexacttype(dev, exacttype);
		if (childDev != NULL)
		{
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
 * to know the version of a device, use \ref mupnp_upnp_devicetype_getversion
 *
 * \param ctrlPoint Controlpoint in question
 * \param type Type of the device
 *
 */
CgUpnpDevice *mupnp_upnp_controlpoint_getdevicebytype(CgUpnpControlPoint *ctrlPoint,
						   char *type)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	const char* typeString = NULL;
	char* part = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(type) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}

	for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint);
	     dev != NULL;
	     dev = mupnp_upnp_device_next(dev))
	{
		typeString = mupnp_upnp_device_getdevicetype(dev);
		if (typeString != NULL)
		{
			part = mupnp_upnp_devicetype_getschematype(typeString);
			if (mupnp_strcmp(part, type) == 0)
			{
				free(part);
				
				return dev;
			}
			else
			{
				free(part);
			}
		}
				
		childDev = mupnp_upnp_device_getdevicebytype(dev, type);
		if (childDev != NULL)
		{
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
CgUpnpDevice *mupnp_upnp_controlpoint_getdevicebyudn(CgUpnpControlPoint *ctrlPoint,
						  char *udn)
{
	CgUpnpDevice *dev = NULL;
	CgUpnpDevice *childDev = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(udn) <= 0 || ctrlPoint == NULL)
	{
		return NULL;
	}
	
	for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint); 
	     dev != NULL;
	     dev = mupnp_upnp_device_next(dev))
	{
		if (mupnp_strcmp(mupnp_upnp_device_getudn(dev), udn) == 0)
		{
			return dev;
		}
		childDev = mupnp_upnp_device_getdevicebyudn(dev, udn);
		if (childDev != NULL)
		{
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
 * @return TRUE if successful; otherwise FALSE
 */
BOOL mupnp_upnp_controlpoint_parsescservicescpd(CgUpnpService *service)
{
	CgNetURL *scpdURL;
	BOOL scpdParseSuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	scpdURL = mupnp_upnp_service_getscpdurl(service); 

	if ( NULL == scpdURL )		
		return FALSE;
	
	mupnp_log_debug_s("SCPD URL: %s\n", mupnp_net_url_getrequest(scpdURL));
	scpdParseSuccess = mupnp_upnp_service_parsedescriptionurl(service, scpdURL);
	
	mupnp_net_url_delete(scpdURL);
	if (scpdParseSuccess == TRUE)
		return TRUE;

#if defined(CG_UPNP_USE_STDDCP)
	if (mupnp_upnp_service_hasstddcp(service)) {
		char *stdDCP = mupnp_upnp_service_getstddcp(service);
		scpdParseSuccess = mupnp_upnp_service_parsedescription(service, stdDCP, mupnp_strlen(stdDCP));
	}
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return scpdParseSuccess;
}

/****************************************
* mupnp_upnp_controlpoint_parseservicesfordevice
****************************************/

BOOL mupnp_upnp_controlpoint_parseservicesfordevice(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_upnp_controlpoint_parsescservicescpd(service) == FALSE) {
			return FALSE;
		}
	}
	
	/* Now only root SCPDs for root services are parsed, but also child 
	   devices' services have to be parsed, so parse them */
	for (childDev=mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		if (mupnp_upnp_controlpoint_parseservicesfordevice(childDev, ssdpPkt) == FALSE)
		{
			return FALSE;
		}
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

static CgUpnpDevice *mupnp_upnp_controlpoint_createdevicefromssdkpacket(CgUpnpSSDPPacket *ssdpPkt)
{
	const char *location;
	CgNetURL *url;
	CgUpnpDevice *dev;
	BOOL parseSuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	location = mupnp_upnp_ssdp_packet_getlocation(ssdpPkt);
	if (mupnp_strlen(location) <= 0)
		return NULL;
			
	dev = mupnp_upnp_device_new();
	
	url = mupnp_net_url_new();
	mupnp_net_url_set(url, location);
	parseSuccess =  mupnp_upnp_device_parsedescriptionurl(dev, url);
	mupnp_net_url_delete(url);
	
	if (parseSuccess == FALSE) {
		mupnp_upnp_device_delete(dev);
		return NULL;
	}

	mupnp_upnp_device_setssdppacket(dev, ssdpPkt);

#ifndef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == FALSE)
	{
		mupnp_upnp_device_delete(dev);
		return NULL;
	}
#endif
	
	mupnp_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* mupnp_upnp_controlpoint_adddevicebyssdppacket
****************************************/

void mupnp_upnp_controlpoint_adddevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev = NULL;
	const char *usn = NULL;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CG_UPNP_DEVICE_LISTENER listener = NULL;
	CgUpnpDeviceStatus status = 0;

	mupnp_log_debug_l4("Entering...\n");

	listener = mupnp_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	usn = mupnp_upnp_ssdp_packet_getusn(ssdpPkt);
	mupnp_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	mupnp_upnp_controlpoint_lock(ctrlPoint);
	
	dev = mupnp_upnp_controlpoint_getdevicebyudn(ctrlPoint, udn);
	
	if (dev != NULL)
	{
		/* Device was found from local cache */
		if (mupnp_upnp_device_updatefromssdppacket(dev, ssdpPkt) == TRUE)
		{
			mupnp_mutex_lock(ctrlPoint->expMutex);
			mupnp_cond_signal(ctrlPoint->expCond);
			mupnp_mutex_unlock(ctrlPoint->expMutex);
			
			/* Device was successfully updated */
			status = CgUpnpDeviceStatusUpdated;
		}
		else
		{	/* Problems occurred in device update */
			status = CgUpnpDeviceStatusInvalid;
		}
	}
	else
	{
		/* This is a new device */
		dev = mupnp_upnp_controlpoint_createdevicefromssdkpacket(ssdpPkt);
		if (dev == NULL)
		{
			/* Problems occurred in device creation */
			status = CgUpnpDeviceStatusInvalid;
		}
		else
		{
			mupnp_upnp_controlpoint_adddevice(ctrlPoint, dev);

			/* Device added, wake up expirationhandler thread */
			mupnp_mutex_lock(ctrlPoint->expMutex);
			mupnp_cond_signal(ctrlPoint->expCond);
			mupnp_mutex_unlock(ctrlPoint->expMutex);
			
			status = CgUpnpDeviceStatusAdded;
		}
	}
	
	mupnp_upnp_controlpoint_unlock(ctrlPoint);
	
	if (listener != NULL)
	{
		listener(ctrlPoint, udn, status);
	}

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_controlpoint_removedevicebyssdppacket
****************************************/

void mupnp_upnp_controlpoint_removedevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	const char *usn;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CgUpnpDevice *dev;
	CG_UPNP_DEVICE_LISTENER listener = mupnp_upnp_controlpoint_getdevicelistener(ctrlPoint);
	
	mupnp_log_debug_l4("Entering...\n");

	usn = mupnp_upnp_ssdp_packet_getusn(ssdpPkt);
	mupnp_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	mupnp_upnp_controlpoint_lock(ctrlPoint);
	
	dev = mupnp_upnp_controlpoint_getdevicebyudn(ctrlPoint, udn);

	if (dev == NULL) {
		mupnp_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
	
	if (listener != NULL)
	{
		mupnp_upnp_controlpoint_unlock(ctrlPoint);
		listener(ctrlPoint, udn, CgUpnpDeviceStatusRemoved);
		mupnp_upnp_controlpoint_lock(ctrlPoint);
	}
	
	mupnp_upnp_device_delete(dev);
	
	mupnp_upnp_controlpoint_unlock(ctrlPoint);

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
BOOL mupnp_upnp_controlpoint_search(CgUpnpControlPoint *ctrlPoint, const char *target)
{
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	int i = 0;
	BOOL retval = FALSE;
	
	mupnp_log_debug_l4("Entering...\n");

	ssdpReq = mupnp_upnp_ssdprequest_new();
	mupnp_upnp_ssdprequest_setmethod(ssdpReq, CG_HTTP_MSEARCH);
	mupnp_upnp_ssdprequest_setst(ssdpReq, target);
	mupnp_upnp_ssdprequest_setmx(ssdpReq, mupnp_upnp_controlpoint_getssdpsearchmx(ctrlPoint));
	mupnp_upnp_ssdprequest_setman(ssdpReq, CG_UPNP_MAN_DISCOVER);
	
	mupnp_log_debug("Announcing %d times.\n", mupnp_upnp_ssdp_getannouncecount());
	mupnp_upnp_ssdprequest_print(ssdpReq);
	
	for (i = 0; i < mupnp_upnp_ssdp_getannouncecount(); i++)
	{
		ssdpResServerList = mupnp_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
		retval = ( mupnp_upnp_ssdpresponse_serverlist_post(ssdpResServerList, ssdpReq ) || retval );
		mupnp_wait(CG_UPNP_CONTROLPOINT_SSDP_MIN_DELAY);
	}
	
	mupnp_upnp_ssdprequest_delete(ssdpReq);

	mupnp_log_debug_l4("Leaving...\n");

	return retval;
}

/****************************************
* mupnp_upnp_controlpoint_ipchanged
****************************************/

BOOL mupnp_upnp_controlpoint_ipchanged(CgUpnpControlPoint *ctrlPoint)
{
	CgNetworkInterfaceList *current, *added, *removed;
	CgNetworkInterface *netIf;
	CgUpnpDevice *dev, *tmp;
	CgUpnpSSDPPacket *ssdpPkt;
	char *address;
	
	mupnp_log_debug_l4("Entering...\n");

	current = mupnp_net_interfacelist_new();
	added = mupnp_net_interfacelist_new();
	removed = mupnp_net_interfacelist_new();

	if (current == NULL || added == NULL || removed == NULL)
	{
		if (current != NULL) mupnp_net_interfacelist_delete(current);
		if (added != NULL) mupnp_net_interfacelist_delete(added);
		if (removed != NULL) mupnp_net_interfacelist_delete(removed);
		return FALSE;
	}
	
	/* Get Interface changes */
	mupnp_net_gethostinterfaces(current);
	mupnp_net_interfacelist_getchanges(ctrlPoint->ifCache, current, 
					added, removed);
	
	/* Remove all devices registered through old interface */
	for (netIf = mupnp_net_interfacelist_gets(removed);
	     netIf != NULL; netIf = mupnp_net_interface_next(netIf))
	{
		mupnp_upnp_controlpoint_lock(ctrlPoint);
		tmp = mupnp_upnp_controlpoint_getdevices(ctrlPoint);
		while (tmp != NULL)
		{
			dev = tmp; tmp = mupnp_upnp_device_next(dev);
			ssdpPkt = mupnp_upnp_device_getssdppacket(dev);
			address = mupnp_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
			
			if (address != NULL && 
			    mupnp_strcmp(address, mupnp_net_interface_getaddress(netIf)) == 0)
			{
				/* This device has been received from the 
				   removed interface, so it does not exist */
				mupnp_upnp_controlpoint_unlock(ctrlPoint);
				mupnp_upnp_controlpoint_removedevicebyssdppacket(ctrlPoint, 
									      ssdpPkt);
				mupnp_upnp_controlpoint_lock(ctrlPoint);
				address = NULL; dev = NULL; ssdpPkt = NULL;
			}
		}
		mupnp_upnp_controlpoint_unlock(ctrlPoint);
	}

	/* Launch new M-SEARCH */
	mupnp_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);
	
	/**** Cache current interfaces ****/
	mupnp_net_gethostinterfaces(ctrlPoint->ifCache);
	
	mupnp_net_interfacelist_delete(current);
	mupnp_net_interfacelist_delete(added);
	mupnp_net_interfacelist_delete(removed);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_controlpoint_ssdplistner
****************************************/

static void mupnp_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_LISTNER listener;

	mupnp_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)mupnp_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	/* We filter out all but rootdevice, since it must be advertized by all
	 * devices. This way we avoid lots of device updates during advertizement
	 * cycle. */
	if (mupnp_upnp_ssdp_packet_isrootdevice(ssdpPkt) == TRUE) {
		if (mupnp_upnp_ssdp_packet_isalive(ssdpPkt) == TRUE)
			mupnp_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
		else if (mupnp_upnp_ssdp_packet_isbyebye(ssdpPkt) == TRUE)
			mupnp_upnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
	}
				
	listener = mupnp_upnp_controlpoint_getssdplistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_controlpoint_ssdpresponselistner
****************************************/

static void mupnp_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;

	mupnp_log_debug_l4("Entering...\n");

	ctrlPoint = (CgUpnpControlPoint *)mupnp_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	mupnp_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
				
	listener = mupnp_upnp_controlpoint_getssdpresponselistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);	

	mupnp_log_debug_l4("Leaving...\n");
}

#endif

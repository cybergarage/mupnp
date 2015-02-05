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
*	File: cdevice.h
*
*	Revision:
*
*	02/14/05
*		- first revision
*
*	10/31/05
*		- Added comments to all functions and structs
*
*	01/09/06 Heikki Junnila
*		- Added mupnp_upnp_device_getservicebytype() to enable
*		  searching for services without version information.
*
*	10-Jan-06 Heikki Junnila
*		- Renamed mupnp_upnp_device_getservicebyname to
*		  mupnp_upnp_device_getservicebyexacttype and created a compat
*		  macro for _getservicebyname
*
*	11-Jan-06 Heikki Junnila
*		- Removed mupnp_upnp_device_isname and _getbyname because
*		  according to UPnP specs UDN, type or friendlyname is
*		  not the same as the device's name.
*		- Added mupnp_upnp_device_getdevicebyudn()
*	04/03/06 Theo Beisch
*		- Added mupnp_upnp_device_getservicebysid
*		- changed leasetime and timer to type CgTime
*		- added mupnp_upnp_device_waitforlock (nonblocking) 
*	22-Apr-2008 
*		- Added mupnp_upnp_device_getnservices() and mupnp_upnp_device_getservice() to get a device by the index.
*
******************************************************************/

#ifndef _CG_UPNP_CDEVICE_H_
#define _CG_UPNP_CDEVICE_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/thread.h>
#include <mupnp/http/http.h>
#include <mupnp/net/url.h>
#include <mupnp/net/interface.h>

#include <mupnp/service.h>
#include <mupnp/icon.h>
#include <mupnp/ssdp/ssdp_server.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
 * Value macros
 ****************************************/

/** Definition for device XML element name */
#define CG_UPNP_DEVICE_ELEM_NAME "device"

/** Definition for device XML element list name */
#define CG_UPNP_DEVICELIST_ELEM_NAME "deviceList"

/** Definition for UPnP rootdevice ID */
#define CG_UPNP_DEVICE_UPNP_ROOTDEVICE "upnp:rootdevice"

/** Definition for default device start up wait time */
#define CG_UPNP_DEVICE_DEFAULT_STARTUP_WAIT_TIME 1000

/** Definition for default device discovery wait time */
#define CG_UPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME 500

/** Definition for default device lease time */
#define CG_UPNP_DEVICE_DEFAULT_LEASE_TIME (30 * 60)

/** Definition for default device HTTP port */
#define CG_UPNP_DEVICE_HTTP_DEFAULT_PORT 38400

/** Definition for default description file "location" */
#define CG_UPNP_DEVICE_DEFAULT_DESCRIPTION_URI "/description.xml"

/** Definition for maximum URL base length */
#define CG_UPNP_DEVICE_URLBASE_MAXLEN 64

/** Definition for M-SEARCH filter interval */
#define CG_UPNP_DEVICE_M_SEARCH_FILTER_INTERVAL 15

/** Definition for urlbase XML element name */
#define CG_UPNP_DEVICE_URLBASE_NAME "URLBase"

/** Definition for device type XML element name */
#define CG_UPNP_DEVICE_DEVICE_TYPE "deviceType"

/** Definition for device friendly name XML element name */
#define CG_UPNP_DEVICE_FRIENDLY_NAME "friendlyName"

/** Definition for device manufacturer XML element name */
#define CG_UPNP_DEVICE_MANUFACTURER "manufacturer"

/** Definition for manufacturer URL XML element name */
#define CG_UPNP_DEVICE_MANUFACTURER_URL "manufacturerURL"

/** Definition for device model description XML element name */
#define CG_UPNP_DEVICE_MODEL_DESCRIPTION "modelDescription"

/** Definition fo device model name XML element name */
#define CG_UPNP_DEVICE_MODEL_NAME "modelName"

/** Definition for device model number XML element name */
#define CG_UPNP_DEVICE_MODEL_NUMBER "modelNumber"

/** Definition for device model URL XML element name */
#define CG_UPNP_DEVICE_MODEL_URL "modelURL"

/** Definition for device serial number XML element name */
#define CG_UPNP_DEVICE_SERIAL_NUMBER "serialNumber"

/** Definition for device UDN XML element name */
#define CG_UPNP_DEVICE_UDN "UDN"

/** Definition for device UPC XML element name */
#define CG_UPNP_DEVICE_UPC "UPC"

/** Definition for device presentation URL XML element name */
#define CG_UPNP_DEVICE_PRESENTATION_URL "presentationURL"

/** Definition for default presentation URL" */
#define CG_UPNP_DEVICE_DEFAULT_PRESENTATION_URI "/presentation"

/** Definition for presentation listener */
typedef void (*CG_UPNP_PRESENTATION_LISTNER)(CgHttpRequest *);
  
/****************************************
 * Data Type
 ****************************************/

/**
 * \brief The generic UPnP device structure
 *
 * \note This struct can also be cast to a CgList* and used as a node in a 
 * linked list with mupnp_list_* functions.
 */
typedef struct _CgUpnpDevice
{
  /** Used by mupnp_list_* functions to indicate start of list */
  BOOL headFlag;
  /** Used by mupnp_list_* functions to point to the previous item in list */
  struct _CgUpnpDevice *prev;
  /** Used by mupnp_list_* functions to point to the next item in list */
  struct _CgUpnpDevice *next;
  
  /** List of XML root nodes */
  CgXmlNodeList *rootNodeList;
  /** This device's description XML node */
  CgXmlNode *deviceNode;
  
  /** This device's parent device */
  struct _CgUpnpDevice *parentDevice;
  /** List of devices */
  struct _CgUpnpDevice *deviceList;
  /** List of services */
  struct _CgUpnpService *serviceList;
  /** List of icons */
  struct _CgUpnpIcon *iconList;
  
  /** Mutex used in multithreading */
  CgMutex *mutex;
  /** List of HTTP servers */
  CgHttpServerList *httpServerList;
  /** List of SSDP servers */
  CgUpnpSSDPServer *ssdpServerList;
  /** Device advertiser thread */
  CgThread *advertiser;
  
  /** HTTP request listener */
  CG_HTTP_LISTENER httpListener;
  
  /** Presentation listener */
  CG_UPNP_PRESENTATION_LISTNER presentationListener;
  
  /** URI for this device's description */
  CgString *descriptionURI;
  /** Advertisement lease time */
  CgTime leaseTime;
  /** HTTP Port to listen to */
  int httpPort;
  /** This device's SSDP packet */
  CgUpnpSSDPPacket *ssdpPkt;
  /** User data used to pass miscellaneous data */
  void *userData;

  /** BOOTID.UPNP.ORG (UPnP DA v1.1) */
  int bootId;

  /* List of cached interfaces */
  CgNetworkInterfaceList *ifCache;
} CgUpnpDevice, CgUpnpDeviceList;

/****************************************
 * Function (Device)
 ****************************************/

/**
 * Create a new UPnP device
 */
CgUpnpDevice *mupnp_upnp_device_new();
  
/**
 * Delete a UPnP device
 *
 * \param dev Device in question
 */
void mupnp_upnp_device_delete(CgUpnpDevice *dev);

/**
 * Clear a UPnP device structure. This is an internal function and should not be called from outside.
 *
 * \param dev Device in question
 */
void mupnp_upnp_device_clear(CgUpnpDevice *dev);

/**
 * Get the next device in the device list. Use as an iterator.
 *
 * \param dev Current device
 */
#define mupnp_upnp_device_next(dev) (CgUpnpDevice *)mupnp_list_next((CgList *)dev)

/**
 * Remove the device from the device list.
 *
 * \param dev Device in question
 */
#define mupnp_upnp_device_remove(dev) mupnp_list_remove((CgList *)dev)

/**
 * Check whether the given node is of type CG_UPNP_DEVICE_ELEM_NAME
 *
 * \param node CgXmlAttribute
 */
#define mupnp_upnp_device_isdevicenode(node) mupnp_xml_node_isname(node, CG_UPNP_DEVICE_ELEM_NAME)

/**
 * Return the device's root node
 * \todo Correct explanation...
 *
 * \param dev Device in question
 */
#define mupnp_upnp_device_getrootnode(dev) mupnp_xml_nodelist_gets(dev->rootNodeList)

/**
 * Set the device description node and initialise its child nodes
 *
 * \param dev Device in question
 * \param node The XML structure to be set as the device's description
 */
void mupnp_upnp_device_setdevicenode(CgUpnpDevice *dev, CgXmlNode *node);

/**
 * Return the device XML description node
 *
 * \param dev Device in question
 */
#define mupnp_upnp_device_getdevicenode(dev) (dev->deviceNode)

/**
 * Check whether the given device contains a list of root nodes
 * i.e. is it the root device
 *
 * \param dev Device in question
 */
#define mupnp_upnp_device_isrootdevice(dev) ((dev->rootNodeList != NULL) ? TRUE : FALSE)

/**
 * Set a parent device for the given child device
 *
 * \param dev Child device
 * \param pDev Parent device
 */
#define mupnp_upnp_device_setparentdevice(dev, pDev) (dev->parentDevice = pDev)

/**
 * Return the device's parent device
 *
 * \param dev Device in question
 */
#define mupnp_upnp_device_getparentdevice(dev) (dev->parentDevice)

/**
 * Return the root (i.e. the topmost) device in the device structure
 *
 * \param dev Traverses the device tree upwards starting from this device
 */
CgUpnpDevice *mupnp_upnp_device_getrootdevice(CgUpnpDevice *dev);

/**
 * Parse the device description XML document.
 *
 * \param dev Device in question
 * \param description Buffer containing the device description in XML format
 * \param descriptionLen Buffer length
 *
 * \return TRUE, if the buffer was successfully parsed; otherwise FALSE
 */
BOOL mupnp_upnp_device_parsedescription(CgUpnpDevice *dev, const char *desciption, size_t descriptionLen);

/**
 * Fetch and parse the device description XML file, located in the given URL.
 *
 * \param dev Device in question
 * \param url An internet resource
 *
 * \return TRUE, if the buffer was successfully parsed; otherwise FALSE
 */
BOOL mupnp_upnp_device_parsedescriptionurl(CgUpnpDevice *dev, CgNetURL *url);


#if defined(CG_USE_CFILE)
/**
 * Load and parse the device description from an XML file.
 * Not available for T-Engine, ITron and BTron platforms.
 *
 * \param dev Device in question
 * \param fileName The file name containing the device description XML document
 *
 * \return TRUE, if the buffer was successfully parsed; otherwise FALSE
 */
BOOL mupnp_upnp_device_loaddescriptionfile(CgUpnpDevice *dev, char *fileName);
#endif

/****************************************************************************
 * Description updates
 ****************************************************************************/
 
/**
 * Update the device's contents from an SSDP packet if necessary.
 *
 * @param dev The device to potentially update
 * @param ssdpPkt The SSDP packet to make decisions on
 * @return TRUE if the device was updated; otherwise FALSE
 */
BOOL mupnp_upnp_device_updatefromssdppacket(CgUpnpDevice* dev,
					 CgUpnpSSDPPacket* ssdpPkt);

/*****************************************************************************
 * Device Type
 *****************************************************************************/

/**
 * Modify the device type.
 *
 * \param dev Device in question
 * \param value Type to set
 */
#define mupnp_upnp_device_setdevicetype(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_DEVICE_TYPE, value)

/**
 * Return the device type
 *
 * \param dev Device in question
 *
 * \return Device type
 */
#define mupnp_upnp_device_getdevicetype(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_DEVICE_TYPE)

/**
 * Check, whether the device is of the exact specified type (including version)
 *
 * \param dev Device in question
 * @param value Device type
 *
 * \return BOOL: TRUE if device matches given type; otherwise FALSE
 */
#define mupnp_upnp_device_isdevicetype(dev, value) mupnp_streq(mupnp_upnp_device_getdevicetype(dev), value)

/** 
 * Get the identifier-part of a device type string (usually "urn") 
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getidentifier(const char* deviceType);

/** 
 * Get the URN part of a device type string (usually "schemas-upnp-org") 
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_geturn(const char* deviceType);

/** 
 * Get the device part of a device type string (usually just "device")
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getdevice(const char* deviceType);

/** 
 * Get the type part of a device type string (ex. "ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_gettype(const char* deviceType);

/** 
 * Get the schema type part of a device type string (without last colon)
 * (ex. "urn:schemas-upnp-org:device:ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getschematype(const char* deviceType);

/** 
 * Get the version part of a device type string (ex. "1")
 *
 * @param deviceType A device type string (usually the result from
 *	  @ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getversion(const char* deviceType);

/*****************************************************************************
 * Friendly Name
 *****************************************************************************/

/**
 * Modify the device's friendly name.
 *
 * \param dev Device in question
 * \param value The friendly name
 *
 */
#define mupnp_upnp_device_setfriendlyname(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_FRIENDLY_NAME, value)

/**
 * Return the device's friendly name.
 *
 * \param dev Device in question
 *
 * \return The device's friendly name
 */
#define mupnp_upnp_device_getfriendlyname(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_FRIENDLY_NAME)

/*****************************************************************************
 * Manufacturer
 *****************************************************************************/

/**
 * Modify the device's manufacturer.
 *
 * \param dev Device in question
 * \param value The manufacturer's name
 *
 */
#define mupnp_upnp_device_setmanufacturer(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURER, value)

/**
 * Get the device's manufacturer.
 *
 * \param dev Device in question
 *
 * \return value The manufacturer's name
 */
#define mupnp_upnp_device_getmanufacturer(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURER)

/*****************************************************************************
 * Manufacturer URL
 *****************************************************************************/

/**
 * Modify the device's manufacturer URL
 *
 * \param dev Device in question
 * \param value The manufacturer URL
 *
 */
#define mupnp_upnp_device_setmanufacturerurl(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURER_URL, value)

/**
 * Get the device's manufacturer URL
 *
 * \param dev Device in question
 *
 * \return The manufacturer URL
 */
#define mupnp_upnp_device_getmanufacturerurl(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURER_URL)

/*****************************************************************************
 * Model Description
 *****************************************************************************/

/**
 * Modify the device model description
 *
 * \param dev Device in question
 * \param value The device model description
 *
 */
#define mupnp_upnp_device_setmodeldescription(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_DESCRIPTION, value)

/**
 * Get the device model description
 *
 * \param dev Device in question
 *
 * \return The device model description
 */
#define mupnp_upnp_device_getmodeldescription(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_DESCRIPTION)
	
/*****************************************************************************
 * Model Name
 *****************************************************************************/

/**
 * Modify the device's model name
 *
 * \param dev Device in question
 * \param value The model name
 *
 */
#define mupnp_upnp_device_setmodelname(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NAME, value)

/**
 * Get the device's model name
 *
 * \param dev Device in question
 *
 * \return The model name
 */
#define mupnp_upnp_device_getmodelname(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NAME)
	
/*****************************************************************************
 * Model Number
 *****************************************************************************/

/**
 * Modify the device's model number
 *
 * \param dev Device in question
 * \param value The model number
 *
 */
#define mupnp_upnp_device_setmodelnumber(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NUMBER, value)

/**
 * Get the device's model number
 *
 * \param dev Device in question
 *
 * \return The model number
 */
#define mupnp_upnp_device_getmodelnumber(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NUMBER)

/*****************************************************************************
 * Model URL
 *****************************************************************************/

/**
 * Modify the device's model URL
 *
 * \param dev Device in question
 * \param value The model URL
 *
 */
#define mupnp_upnp_device_setmodelurl(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_URL, value)

/**
 * Get the device's model URL
 *
 * \param dev Device in question
 *
 * \return The model URL
 */
#define mupnp_upnp_device_getmodelurl(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_URL)
	
/*****************************************************************************
 * serialNumber
 *****************************************************************************/

/**
 * Modify the device's serial number
 *
 * \param dev Device in question
 * \param value The serial number
 *
 */
#define mupnp_upnp_device_setserialnumber(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_SERIAL_NUMBER, value)

/**
 * Get the device's model number
 *
 * \param dev Device in question
 * 
 * \return The serial number
 */
#define mupnp_upnp_device_getserialnumber(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_SERIAL_NUMBER)
	
/*****************************************************************************
 * Unique Device Name (UDN)
 *****************************************************************************/

/**
 * Modify the device's Unique Device Name (UDN)
 *
 * \param dev Device in question
 * \param value The Unique Device Name
 *
 */
#define mupnp_upnp_device_setudn(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UDN, value)

/**
 * Get the device's Unique Device Name (UDN)
 *
 * \param dev Device in question
 *
 * \return The Unique Device Name
 */
#define mupnp_upnp_device_getudn(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UDN)

/**
 * Check whether the device has a Unique Device Name (UDN)
 *
 * \param dev Device in question
 *
 * \return TRUE if the device has a UDN; otherwise FALSE
 */
#define mupnp_upnp_device_hasudn(dev) ((0 < mupnp_strlen(mupnp_upnp_device_getudn(dev))) ? TRUE : FALSE)

/**
 * Update new Unique Device Name (UDN)
 *
 * \param dev Device in question
 *
 */
void mupnp_upnp_device_updateudn(CgUpnpDevice *dev);

/*****************************************************************************
 * Universal Product Code (UPC)
 *****************************************************************************/

/**
 * Modify the device's Universal Product Code (UPC)
 *
 * \param dev Device in question
 * \param value The Universal Product Code
 *
 */
#define mupnp_upnp_device_setupc(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UPC, value)

/**
 * Get the device's Universal Product Code (UPC)
 *
 * \param dev Device in question
 *
 * \return The Universal Product Code
 */
#define mupnp_upnp_device_getupc(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UPC)
	
/*****************************************************************************
 * Presentation URL
 *****************************************************************************/

/**
 * Modify the device's presentation URL
 *
 * \param dev Device in question
 * \param value The presentation URL
 *
 */
#define mupnp_upnp_device_setpresentationurl(dev, value) mupnp_xml_node_setchildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_PRESENTATION_URL, value)

/**
 * Get the device's presentation URL
 *
 * \param dev Device in question
 *
 * \return The presentation URL
 */
#define mupnp_upnp_device_getpresentationurl(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_PRESENTATION_URL)

/**
 * Remove the device's presentation URL
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_removepresentationurl(dev) mupnp_xml_node_removechildnode(mupnp_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_PRESENTATION_URL)
  
/*
 * Set an presentation listener function to the device
 *
 * \param dev The device in question
 * \param func The listener function
 *
 */
void mupnp_upnp_device_setpresentationlistener(CgUpnpDevice *dev, CG_UPNP_PRESENTATION_LISTNER func);
  
/**
 * Get the device's presentation listener function
 *
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getpresentationlistener(dev) (dev->presentationListener)
  
/*****************************************************************************
 * URLBase
 *****************************************************************************/

/**
 * Modify the device's URL base
 * The base URL is the root for all relative URLs.
 *
 * \param dev Device in question
 * \param value The URL base
 *
 */
void mupnp_upnp_device_seturlbase(CgUpnpDevice *dev, char *value);

/**
 * Get the device's base URL.
 * The base URL is the root for all relative URLs.
 *
 * \param dev Device in question
 * \return The URL base
 *
 */
#define mupnp_upnp_device_geturlbase(dev) mupnp_xml_node_getchildnodevalue(mupnp_upnp_device_getrootnode(mupnp_upnp_device_getrootdevice(dev)), CG_UPNP_DEVICE_URLBASE_NAME)

/*****************************************************************************
 * BOOTID.UPNP.ORG
 *****************************************************************************/

/**
 * Modify the device's boot id
 *
 * \param dev Device in question
 * \param value The boot id
 *
 */
#define mupnp_upnp_device_setbootid(dev, value) (dev->bootId = value)

/**
 * Get the device's boot id
 *
 * \param dev Device in question
 *
 * \return The boot id
 */
#define mupnp_upnp_device_getbootid(dev) (dev->bootId)

/*****************************************************************************
 * Start/Stop
 *****************************************************************************/

/**
 * Start the device. This essentially starts:
 * \li The HTTP server
 * \li The SSDP server
 * \li Advertising the device
 * \li Alive notification
 *
 * \param dev Device in question
 *
 */
BOOL mupnp_upnp_device_start(CgUpnpDevice *dev);

/**
 * Stop the device. This concerns:
 * \li The HTTP server
 * \li The SSDP server
 * \li Advertising the device
 * \li Byebye notification
 *
 * \param dev Device in question
 *
 */
BOOL mupnp_upnp_device_stop(CgUpnpDevice *dev);

/**
 * Check if the device] has been started
 *
 * \param dev Device that is being advertised
 */
BOOL mupnp_upnp_device_isrunning(CgUpnpDevice *dev);
	
/*****************************************************************************
 * URL
 *****************************************************************************/

/**
 * Get device's location URL. Transforms the given <host> address to URL form:
 * "http://<host>:<port>/description.xml". Port is usually 80 or 38400.
 * If <host> is an IPv6 address, the address is "http://[<host>]:<port>/description.xml".
 *
 * \param dev Device in question
 * \param host The device host
 * \param buf Buffer to store the resulting URL
 * \param bufSize Buffer length
 *
 */
const char *mupnp_upnp_device_getlocationurl(CgUpnpDevice *dev, const char *host, char *buf, int bufSize);

/*****************************************************************************
 * Notify 
 *****************************************************************************/

/**
 * Broadcast an SSDP:alive to notify control points that the device is available.
 *
 * \param dev Device in question
 *
 */
void mupnp_upnp_device_announce(CgUpnpDevice *dev);

/**
 * Broadcast an SSDP:byebye message to notify control points that the device is no longer available.
 *
 * \param dev Device in question
 *
 */
void mupnp_upnp_device_byebye(CgUpnpDevice *dev);

/*****************************************************************************
 * Mutex 
 *****************************************************************************/

/**
 * Lock the device's mutex (grant exclusive access to the device) until \ref mupnp_upnp_device_unlock is called.
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_lock(dev) mupnp_mutex_lock(dev->mutex)

/**
 * Lock the device's mutex with timeout, return TRUE if exclusive access is gained 
 * Release lock with  \ref mupnp_upnp_device_unlock is called.
 *
 * \param dev Device in question
 * \param t timeout in ms
 *
 */
#define mupnp_upnp_device_waitforlock(dev,t) mupnp_mutex_waitforlock(dev->mutex,t)

/**
 * Unlock the device's mutex (release exclusive access to the device) locked previously with \ref mupnp_upnp_device_lock.
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_unlock(dev) mupnp_mutex_unlock(dev->mutex)

/*****************************************************************************
 * Listener
 *****************************************************************************/

/**
 * Set an action listener to the device. Actions are basically commands, that the
 * device (service) responds to.
 *
 * \param dev Device in question
 * \param actionListener The action listener
 *
 */
void mupnp_upnp_device_setactionlistener(CgUpnpDevice *dev, CG_UPNP_ACTION_LISTNER actionListner);

/**
 * Set a query listener to the device. Queries are meant for state variables.
 *
 * \param dev Device in question
 * \param actionListener The query listener
 *
 */
void mupnp_upnp_device_setquerylistener(CgUpnpDevice *dev, CG_UPNP_STATEVARIABLE_LISTNER queryListner);

/*****************************************************************************
 * User Data
 *****************************************************************************/

/**
 * Set device's user data pointer
 *
 * \param dev Device in question
 * \param value User data pointer
 *
 */
#define mupnp_upnp_device_setuserdata(dev, value) (dev->userData = value)

/**
 * Get device's user data pointer
 *
 * \param dev Device in question
 * \return pointer to user data
 *
 */
#define mupnp_upnp_device_getuserdata(dev) (dev->userData)
 
/*****************************************************************************
* Function
******************************************************************************/

/*****************************************************************************
 * DescriptionURL 
 *****************************************************************************/

/**
 * Set the given URL as the device's description URI
 *
 * \param dev Device in question
 * \param url Description URL
 *
 */
#define mupnp_upnp_device_setdescriptionuri(dev,url) mupnp_string_setvalue(dev->descriptionURI, url)

/**
 * Get the device's description URI
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_getdescriptionuri(dev) mupnp_string_getvalue(dev->descriptionURI)

/**
 * Check, whether the given URL matches the device's description URI
 *
 * \param dev Device in question
 * \param url Description URL to check
 *
 */
#define mupnp_upnp_device_isdescriptionuri(dev, url) mupnp_streq(mupnp_string_getvalue(dev->descriptionURI), url)

/*****************************************************************************
 * Lease Time
 *****************************************************************************/

/**
 * Set the device's lease time. The lease time is basically the maximum amount
 * of time that can elapse before an advertisement is broadcast again. It does not
 * set the exact advertisement interval, but rather a time window for each advertisement.
 * Advertisement actually occurs at (more or less) random intervals inside this time window.
 *
 * \param dev Device in question
 * \param value Lease time in seconds
 *
 */
#define mupnp_upnp_device_setleasetime(dev,value) (dev->leaseTime = value)

/**
 * Get the device's lease time. See \ref mupnp_upnp_device_setleasetime.
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_getleasetime(dev) (dev->leaseTime)

/*****************************************************************************
* Functions (Embedded)
******************************************************************************/

/*****************************************************************************
 * Embedded DeviceList
 *****************************************************************************/

/**
 * Get the device's internal device list
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_getdevicelist(dev) (dev->deviceList)

/**
 * Find a certain device from the given device list
 *
 * \param dev Device in question
 * \param idx Device index number in list
 * \return NULL if not found; otherwise the device pointer
 *
 */
//Theo Beisch added missing (CgList*) cast
#define mupnp_upnp_device_getdevice(dev,idx) ((CgUpnpDevice *)mupnp_list_get((CgList*)dev->deviceList,idx))

/**
 * \todo Correct explanation...
 *
 * \param dev Device in question
 *
 */
//Theo Beisch : added missing (CgList*) cast
#define mupnp_upnp_device_getdevices(dev) ((CgUpnpDevice *)mupnp_list_gets((CgList *)dev->deviceList))

/**
 * Find a device from the device's children by the exact type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example: 
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to disregard
 * the version, use \ref mupnp_upnp_device_getdevicebytype
 *
 * \param ctrlPoint Controlpoint in question
 * \param exacttype Type of the device
 *
 */
CgUpnpDevice *mupnp_upnp_device_getdevicebyexacttype(CgUpnpDevice* dev,
						   char *exacttype);

/**
 * Find a device from the device's children by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use \ref mupnp_upnp_devicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the device
 *
 */
CgUpnpDevice *mupnp_upnp_device_getdevicebytype(CgUpnpDevice *dev, char *type);

/**
 * Find a device from the device's children by the UDN of the device.
 *
 * \param dev Device in question
 * \param type Type of the device
 *
 */
CgUpnpDevice *mupnp_upnp_device_getdevicebyudn(CgUpnpDevice *dev, char *udn);

/**
 * Find a child device by its description URI.
 *
 * \param dev Device in question
 * \param url Description URI
 *
 */
CgUpnpDevice *mupnp_upnp_device_getdevicebydescriptionuri(CgUpnpDevice *dev, char *url);

/*****************************************************************************
 * Embedded ServiceList
 *****************************************************************************/

/**
 * Get the device's list of services
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_getservicelist(dev) (dev->serviceList)

/**
 * Get the number of services known by the device
 * 
 * \param dev Device in question
 * @return The number of devices in the control point's device list
 */
#define mupnp_upnp_device_getnservices(dev) mupnp_upnp_servicelist_size(dev->serviceList)

/**
 * Get a service from <idx> from the device
 *
 * \param dev Device in question
 * \param idx Device index number
 *
 */
//Theo Beisch : added missing (CgList*) cast
#define mupnp_upnp_device_getservice(dev,idx) ((CgUpnpService *)mupnp_list_get((CgList *)dev->serviceList,idx))

/**
 * \todo Correct explanation...
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_getservices(dev) ((CgUpnpService *)mupnp_list_gets((CgList *)dev->serviceList))

/**
 * Find service from the device by a given serviceId.
 * This function searches recursively device and its subdevices
 * for a service with a given serviceId
 *
 * \param dev Device in question
 * \param serviceId serviceId of the service
 * \return service matching the given serviceId
 */
CgUpnpService *mupnp_upnp_device_getservicebyserviceid(CgUpnpDevice *dev, const char *serviceId);

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *complete type string*
 * matches the given string, including version number. For example: 
 * "urn:schemas-upnp-org:service:ContentDirectory:1". If you need to
 * know the version of a service, use \ref mupnp_upnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 * 
 */
CgUpnpService *mupnp_upnp_device_getservicebyexacttype(CgUpnpDevice *dev, const char *type);

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *type* part of the service type
 * string matches the given string. For example: "ContentDirectory".
 * If you need to search for an exact service type, use
 * \ref mupnp_upnp_device_getservicebyname
 *
 * \param dev Device in question
 * \param type Type of the service
 *
 */
CgUpnpService *mupnp_upnp_device_getservicebytype(CgUpnpDevice *dev, const char *type);

/**
 * Find a service from the device by the subscribtion ID
 *
 * \param dev Device in question
 * \param sid Subscription ID of the service
 */
CgUpnpService *mupnp_upnp_device_getservicebysid(CgUpnpDevice *dev, const char *sid);

/**
 * Find a service from the device by the SCPD (Service Control Protocol Definition) URL of the service.
 *
 * \param dev Device in question
 * \param url SCPD URL
 *
 */
CgUpnpService *mupnp_upnp_device_getservicebyscpdurl(CgUpnpDevice *dev, const char *url);

/**
 * Find a service from a device by its control URL
 *
 * \param dev Device in question
 * \param url Control URL
 *
 */
CgUpnpService *mupnp_upnp_device_getservicebycontrolurl(CgUpnpDevice *dev, const char *url);

/**
 * Find a service from a device by its event subscription URL
 *
 * \param dev Device in question
 * \param url Event Subscription URL
 *
 */
CgUpnpService *mupnp_upnp_device_getservicebyeventsuburl(CgUpnpDevice *dev, const char *url);

/**
 * Find a service from its Subscription ID
 *
 * \param dev Device in question
 * \param char* Subscription-ID (SID)
 *
 */
CgUpnpService *mupnp_upnp_device_getservicebysid(CgUpnpDevice *dev, const char *sid);


/*****************************************************************************
 * Embedded IconList
 *****************************************************************************/

/**
 * Get the device's list of icons
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_geticonlist(dev) (dev->iconList)

/**
 * Get an icon by its index from the device's list of icons
 *
 * \param dev Device in question
 * \param idx Icon index number
 *
 */
#define mupnp_upnp_device_geticon(dev,idx) ((CgUpnpIcon *)mupnp_list_get((CgList *)dev->iconList,idx))

/**
 * \todo Correct explanation...
 *
 * \param dev Device in question
 *
 */
#define mupnp_upnp_device_geticons(dev) ((CgUpnpIcon *)mupnp_list_gets((CgList *)dev->iconList))

/**
* Get a smallest icon
*
* \param dev Device in question
*
* \return A smallest icon.
*/
CgUpnpIcon *mupnp_upnp_device_getsmallesticon(CgUpnpDevice *dev);

/**
 * Get a smallest icon of a specified mime type
 *
 * \param dev Device in question
 * \param mimeType Mime type to get
 *
 * \return A smallest icon.
*/
CgUpnpIcon *mupnp_upnp_device_getsmallesticonbymimetype(CgUpnpDevice *dev, const char *mimeType);

/**
 * Get an icon by its index from the device's list of icons
 *
 * \param dev Device in question
 * \param icon Icon in question
 * \param buf Buffer to return the full icon's url
 *
 * \return TRUE if the absolute url is created, otherwise FALSE.
 */
BOOL mupnp_upnp_device_getabsoluteiconurl(CgUpnpDevice *dev, CgUpnpIcon *icon, CgString *buf);

/**
* Get a smallest icon
 *
 * \param dev Device in question
 * \param icon Icon to add
 *
 */
BOOL mupnp_upnp_device_addicon(CgUpnpDevice *dev, CgUpnpIcon *icon);
	
/*****************************************************************************
 * Embedded Action
 *****************************************************************************/

/**
 * Find an action from a device by its name
 * 
 * \param dev Device in question
 * \param name Name of the action
 *
 */
CgUpnpAction *mupnp_upnp_device_getactionbyname(CgUpnpDevice *dev, const char *name);

/*****************************************************************************
 * Embedded StateVariable 
 *****************************************************************************/

/**
 * Find a state variable from a device by its name
 * 
 * \param dev Device in question
 * \param name Name of the state variable
 *
 */
CgUpnpStateVariable *mupnp_upnp_device_getstatevariablebyname(CgUpnpDevice *dev, const char *name);

/*****************************************************************************
 * Function (DeviceList)
 *****************************************************************************/

/**
 * Create a new device list
 * 
 */
CgUpnpDeviceList *mupnp_upnp_devicelist_new();

/**
 * Delete a device list completely, freeing all devices and their contents.
 * 
 * \param devList The device list to delete
 *
 */
void mupnp_upnp_devicelist_delete(CgUpnpDeviceList *devList);

/**
 * Clear the contents of a device list.
 * Use \ref mupnp_upnp_devicelist_delete instead of this function.
 * 
 * \param devList The device list to clear
 *
 */
#define mupnp_upnp_devicelist_clear(devList) mupnp_list_clear((CgList *)devList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_device_delete)

/**
 * Get the size of the device list
 * 
 * \param devList The device list
 *
 */
#define mupnp_upnp_devicelist_size(devList) mupnp_list_size((CgList *)devList)

/**
 * \todo Correct description
 * 
 * \param devList The device list
 *
 */
#define mupnp_upnp_devicelist_gets(devList) (CgUpnpDevice *)mupnp_list_next((CgList *)devList)

/**
 * Get an device from the device list by the index
 *
 * \param devList The device list
 * \param index The index of the device to get
 *
 */
#define mupnp_upnp_devicelist_get(devList, index) (CgUpnpDevice *)mupnp_list_get((CgList *)devList, index)

/**
 * Add a device to the device list
 * 
 * \param devList The device list
 * \param dev The device to add
 *
 */
#define mupnp_upnp_devicelist_add(devList, dev) mupnp_list_add((CgList *)devList, (CgList *)dev)

/****************************************
* Function (Service)
****************************************/

/**
 * Get the device associated to the given service
 * 
 * \param service The service in question
 *
 */
//Theo Beisch: namespace conflict - removed duplicate - already defined in cservice.h
//#define mupnp_upnp_service_getdevice(service) ((CgUpnpDevice *)service->parentDevice)

/**
 * Get the root device of the given service
 * 
 * \param service The service in question
 *
 */
//Theo Beisch: namespace conflict - removed duplicate - already defined in cservice.h
//#define mupnp_upnp_service_getrootdevice(service) mupnp_upnp_device_getrootdevice((CgUpnpDevice *)service->parentDevice)

/*****************************************************************************
* Function (HTTP)
******************************************************************************/

/*****************************************************************************
 * HTTP Port 
 *****************************************************************************/

/**
 * Modify the port that the device's HTTP server is listening
 * 
 * \param dev The device in question
 * \param value Port number
 *
 */
#define mupnp_upnp_device_sethttpport(dev,value) (dev->httpPort = value)

/**
 * Get the device's HTTP port
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_gethttpport(dev) (dev->httpPort)

/**
 * Set an HTTP listener function to the device
 * 
 * \param dev The device in question
 * \param func The listener function
 *
 */
#define mupnp_upnp_device_sethttplistener(dev, func) (dev->httpListener = func)

/**
 * Get the device's HTTP listener function
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_gethttplistener(dev) (dev->httpListener)

/**
 * Get the HTTP server list 
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_gethttpserverlist(dev) (dev->httpServerList)
	
/**
 * Handler function for a received HTTP request. Delegates GET and POST requests
 * to their respective handlers, or treats as a BAD REQUEST, when appropriate.
 * 
 * \param httpReq Received HTTP request
 *
 */
void mupnp_upnp_device_httprequestrecieved(CgHttpRequest *httpReq);

/*****************************************************************************
 * Function (SSDP)
 *****************************************************************************/

/**
 * Get the device's list of SSDP servers
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getssdpserverlist(dev) (dev->ssdpServerList)

/**
 * The SSDP message handler function.
 * 
 * \param ssdpPkt SSDP Packet
 *
 */
void mupnp_upnp_device_ssdplistener(CgUpnpSSDPPacket *ssdpPkt);

/**
 * Get the SSDP server list 
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getssdpserverlist(dev) (dev->ssdpServerList)
	
/**
 * Get the device's Notification Type (NT). For the root device this is "upnp:rootdevice",
 * otherwise the device's UDN (Unique Device Name).
 * 
 * \param dev The device in question
 * \param buf Buffer for the Notification Type
 * \param bufSize Buffer length
 *
 */
char *mupnp_upnp_device_getnotifydevicent(CgUpnpDevice *dev, char *buf, int bufSize);

//Theo Beisch: added reference
/**
 * Get the device's USN (Unique Service Name). 
 * For the root device this is "<UDN>::upnp:rootdevice",
 * otherwise the UDN (Unique Device Name).
 * 
 * \param dev The device in question
 *
 */
char *mupnp_upnp_device_getnotifydeviceusn(CgUpnpDevice *dev, char *buf, int bufSize);

/**
 * Get the device type
 * 
 * \param dev The device in question
 * \param buf Buffer for the device type
 * \param bufSize Buffer length
 *
 */
char *mupnp_upnp_device_getnotifydevicetypent(CgUpnpDevice *dev, char *buf, int bufSize);

/**
 * Get the device's USN (Unique Service Name). For the root device this is "<UDN>::upnp:rootdevice",
 * otherwise the UDN (Unique Device Name).
 * 
 * \param dev The device in question
 * @param buf Buffer where USN is stored after call
 * @param bufSize Buffer size
 *
 * @return Pointer to buf
 */
char *mupnp_upnp_device_getnotifydevicetypeusn(CgUpnpDevice *dev, char *buf, int bufSize);

/**
 * SSDP message handler
 * 
 * \param dev The device in question
 * \param ssdpPkt Received SSDP packet
 *
 */
void mupnp_upnp_device_ssdpmessagereceived(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, int filter);

/**
 * Post a response to an SSDP search message
 * 
 * \param dev The device in question
 * \param ssdpPkt Received SSDP packet
 * \param st Service Type (ST)
 * \param usn Unique Service Name (USN)
 *
 */
BOOL mupnp_upnp_device_postsearchresponse(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, const char *st, const char *usn);

/**
 * Notify device that IP address of the host has been changed.
 *
 * \param dev dev The device in question
 * \return success of changing used interfaces
 */
BOOL mupnp_upnp_device_ipchanged(CgUpnpDevice *dev);

/*****************************************************************************
 * Function (SSDPPacket)
 *****************************************************************************/

/**
 * Get the device's SSDP packet
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getssdppacket(dev) (dev->ssdpPkt)

/**
 * Modify the device's SSDP packet (creates a copy of the original)
 * 
 * \param dev The device in question
 * \param srcSsdpPkt The SSDP packet
 *
 */
#define mupnp_upnp_device_setssdppacket(dev, srcSsdpPkt) mupnp_upnp_ssdp_packet_copy(dev->ssdpPkt, srcSsdpPkt)

/**
 * Extract location information from the device's SSDP packet
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getlocationfromssdppacket(dev) mupnp_upnp_ssdp_packet_getlocation(dev->ssdpPkt)

/**
 * Extract the interface address from the device's SSDP packet
 * 
 * \param dev The device in question
 *
 */
#define mupnp_upnp_device_getinterfaceaddressfromssdppacket(dev) mupnp_upnp_ssdp_packet_getlocaladdress(dev->ssdpPkt)

/****************************************
* Function (Advertiser)
****************************************/

/**
 * Start the advertiser thread for the given device
 *
 * \param dev The device in question
 */
BOOL mupnp_upnp_device_advertiser_start(CgUpnpDevice *dev);

/**
 * Stop the advertiser thread for the given device
 *
 * \param dev The device in question
 */
BOOL mupnp_upnp_device_advertiser_stop(CgUpnpDevice *dev);

/**
 * Check if the advertiser has been started
 *
 * \param dev Device that is being advertised
 */
BOOL mupnp_upnp_device_advertiser_isrunning(CgUpnpDevice *dev);
	
/**
 * Get the advertiser thread from the device
 *
 * \param dev The device in question
 */
#define mupnp_upnp_device_getadvertiserthead(dev) (dev->advertiser)

#ifdef  __cplusplus

} /* extern C */

#endif

#endif

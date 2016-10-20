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

#include <mupnp/device.h>
#include <mupnp/controlpoint.h>
#include <mupnp/upnp_limit.h>
#include <mupnp/util/time.h>
#include <mupnp/net/interface.h>
#include <mupnp/net/url.h>
#include <mupnp/upnp_function.h>
#include <mupnp/ssdp/ssdp.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/control/control.h>
#include <mupnp/io/file.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>

/****************************************
* prototype define for static functions
****************************************/

static void mupnp_device_initchildnodes(mUpnpDevice* dev);
static void mupnp_device_initdevicelist(mUpnpDevice* dev);
static void mupnp_device_initservicelist(mUpnpDevice* dev);
static void mupnp_device_initiconlist(mUpnpDevice* dev);

static void mupnp_device_notifywait(mUpnpDevice* dev);

/****************************************
* mupnp_device_new
****************************************/

mUpnpDevice* mupnp_device_new()
{
  mUpnpDevice* dev;

  mupnp_log_debug_l4("Entering...\n");

  dev = (mUpnpDevice*)malloc(sizeof(mUpnpDevice));

  if (NULL != dev) {
    mupnp_list_node_init((mUpnpList*)dev);

#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
    mupnp_http_persistentconnection_init();
#endif

    dev->rootNodeList = NULL;
    dev->deviceNode = NULL;

    dev->deviceList = mupnp_devicelist_new();
    dev->serviceList = mupnp_servicelist_new();
    dev->iconList = mupnp_iconlist_new();

    dev->mutex = mupnp_mutex_new();

    dev->httpServerList = mupnp_http_serverlist_new();
    dev->ssdpServerList = mupnp_ssdp_serverlist_new();

    dev->advertiser = mupnp_thread_new();

    dev->descriptionURI = mupnp_string_new();

    dev->ssdpPkt = mupnp_ssdp_packet_new();

    dev->ifCache = NULL;

    mupnp_device_setparentdevice(dev, NULL);
    mupnp_device_setdescriptionuri(dev, MUPNP_DEVICE_DEFAULT_DESCRIPTION_URI);
    mupnp_device_setleasetime(dev, MUPNP_DEVICE_DEFAULT_LEASE_TIME);
    mupnp_device_sethttpport(dev, MUPNP_DEVICE_HTTP_DEFAULT_PORT);
    mupnp_device_sethttplistener(dev, NULL);
    mupnp_device_setuserdata(dev, NULL);
    mupnp_device_setbootid(dev, mupnp_createbootid());
    mupnp_device_setpresentationlistener(dev, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return dev;
}

/****************************************
* mupnp_device_delete
****************************************/

void mupnp_device_delete(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)dev);

  /* Delete cached interfaces */
  if (dev->ifCache != NULL)
    mupnp_net_interfacelist_delete(dev->ifCache);

  mupnp_device_clear(dev);

  mupnp_devicelist_delete(dev->deviceList);
  mupnp_servicelist_delete(dev->serviceList);
  mupnp_iconlist_delete(dev->iconList);

  mupnp_mutex_delete(dev->mutex);

  mupnp_http_serverlist_delete(dev->httpServerList);
  mupnp_ssdp_serverlist_delete(dev->ssdpServerList);
  mupnp_thread_delete(dev->advertiser);

  mupnp_string_delete(dev->descriptionURI);

  mupnp_ssdp_packet_delete(dev->ssdpPkt);

  free(dev);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_clear
****************************************/

void mupnp_device_clear(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  if (dev->rootNodeList != NULL) {
    mupnp_xml_nodelist_delete(dev->rootNodeList);
    dev->rootNodeList = NULL;
  }
  dev->deviceNode = NULL;

  mupnp_devicelist_clear(dev->deviceList);
  mupnp_servicelist_clear(dev->serviceList);
  mupnp_iconlist_clear(dev->iconList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_setdevicenode
****************************************/

void mupnp_device_setdevicenode(mUpnpDevice* dev, mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  dev->deviceNode = node;
  mupnp_device_initchildnodes(dev);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_getrootdevice
****************************************/

mUpnpDevice* mupnp_device_getrootdevice(mUpnpDevice* dev)
{
  mUpnpDevice* rootDev;

  mupnp_log_debug_l4("Entering...\n");

  rootDev = dev;
  while (mupnp_device_getparentdevice(rootDev) != NULL)
    rootDev = mupnp_device_getparentdevice(rootDev);

  mupnp_log_debug_l4("Leaving...\n");

  return rootDev;
}

/****************************************
* mupnp_device_parsedescription
****************************************/

bool mupnp_device_parsedescription(mUpnpDevice* dev, const char* desciption, size_t descriptionLen)
{
  mUpnpXmlParser* xmlParser;
  bool xmlParseSuccess;
  mUpnpXmlNode* rootNode;
  char uuidBuf[MUPNP_UUID_MAX_LEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_clear(dev);
  dev->rootNodeList = mupnp_xml_nodelist_new();

  xmlParser = mupnp_xml_parser_new();
  xmlParseSuccess = mupnp_xml_parse(xmlParser, dev->rootNodeList, desciption, descriptionLen);
  mupnp_xml_parser_delete(xmlParser);
  if (xmlParseSuccess == false)
    return false;

  if (mupnp_xml_nodelist_size(dev->rootNodeList) <= 0) {
    mupnp_device_clear(dev);
    return false;
  }

  rootNode = mupnp_device_getrootnode(dev);
  if (rootNode == NULL) {
    mupnp_device_clear(dev);
    return false;
  }

  dev->deviceNode = mupnp_xml_node_getchildnode(rootNode, MUPNP_DEVICE_ELEM_NAME);
  if (dev->deviceNode == NULL) {
    mupnp_device_clear(dev);
    return false;
  }

  if (mupnp_device_hasudn(dev) == false) {
    mupnp_device_setudn(dev, mupnp_createuuid(uuidBuf, sizeof(uuidBuf)));
  }
  
  mupnp_device_initchildnodes(dev);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_device_parsedescriptionurl
****************************************/

bool mupnp_device_parsedescriptionurl(mUpnpDevice* dev, mUpnpNetURL* url)
{
  char* host;
  int port;
  char* request;
  mUpnpHttpRequest* httpReq;
  mUpnpHttpResponse* httpRes;
  /*int statusCode;*/
  char* content;
  ssize_t contentLen;
  bool parseSuccess;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_net_url_ishttpprotocol(url) == false)
    return false;

  host = mupnp_net_url_gethost(url);
  port = mupnp_net_url_getport(url);
  if (port <= 0)
    port = MUPNP_HTTP_DEFAULT_PORT;
  request = mupnp_net_url_getrequest(url);

  httpReq = mupnp_http_request_new();
  mupnp_http_request_setmethod(httpReq, MUPNP_HTTP_GET);
  mupnp_http_request_seturi(httpReq, request);
  mupnp_http_request_setcontentlength(httpReq, 0);
  httpRes = mupnp_http_request_post(httpReq, host, port);

  /*statusCode = mupnp_http_response_getstatuscode(httpRes);
          if (statusCode != MUPNP_HTTP_STATUS_OK) {*/
  if (!mupnp_http_response_issuccessful(httpRes)) {
    mupnp_http_request_delete(httpReq);
    return false;
  }

  content = mupnp_http_response_getcontent(httpRes);
  contentLen = mupnp_http_response_getcontentlength(httpRes);

  parseSuccess = mupnp_device_parsedescription(dev, content, contentLen);

  mupnp_http_request_delete(httpReq);

  mupnp_log_debug_l4("Leaving...\n");

  return parseSuccess;
}

/****************************************
* mupnp_device_loaddescriptionfile
****************************************/

#if defined(MUPNP_USE_CFILE)

bool mupnp_device_loaddescriptionfile(mUpnpDevice* dev, char* fileName)
{
  mUpnpFile* file;
  char* description;
  int descriptionLen;
  bool parseSuccess;

  mupnp_log_debug_l4("Entering...\n");

  file = mupnp_file_new();
  mupnp_file_setname(file, fileName);
  if (mupnp_file_load(file) == false) {
    mupnp_file_delete(file);
    return false;
  }

  description = mupnp_file_getcontent(file);
  descriptionLen = mupnp_strlen(description);

  parseSuccess = mupnp_device_parsedescription(dev, description, descriptionLen);

  mupnp_file_delete(file);

  mupnp_log_debug_l4("Leaving...\n");

  return parseSuccess;
}

#endif

/**
 * Update the device's contents from an SSDP packet if necessary.
 *
 * @param dev The device to potentially update
 * @param ssdpPkt The SSDP packet to make decisions on
 * @return true if the device was updated; otherwise false
 */
bool mupnp_device_updatefromssdppacket(mUpnpDevice* dev,
    mUpnpSSDPPacket* ssdpPkt)
{
  const char* usn = NULL;
  char udn[MUPNP_UDN_LEN_MAX];
  mUpnpSSDPPacket* tmpSsdpPkt = NULL;
  const char* oldLocation = NULL;
  const char* newLocation = NULL;
  mUpnpNetURL* url = NULL;

  mupnp_log_debug_l4("Entering...\n");

  usn = mupnp_ssdp_packet_getusn(ssdpPkt);
  mupnp_usn_getudn(usn, udn, sizeof(udn));

  tmpSsdpPkt = mupnp_device_getssdppacket(dev);
  if (tmpSsdpPkt == NULL) {
    return false;
  }

  /* Here we check, if the location of the device has changed */
  oldLocation = mupnp_ssdp_packet_getlocation(tmpSsdpPkt);
  newLocation = mupnp_ssdp_packet_getlocation(ssdpPkt);

  if (mupnp_streq(oldLocation, newLocation) == true) {
    /* The device's location has not changed, just update
		   the SSDP packet */
    mupnp_device_setssdppacket(dev, ssdpPkt);

    return true;
  }
  else {
    /* The device's location HAS changed. We must get a new
		   description. */
    mupnp_device_setssdppacket(dev, ssdpPkt);

    url = mupnp_net_url_new();
    if (url == NULL) {
      return false;
    }

    /* Use the new location as the description URL */
    mupnp_net_url_set(url, newLocation);

    /* Get a new description for the device */
    mupnp_device_parsedescriptionurl(dev, url);

    mupnp_net_url_delete(url);

/* ADD Fabrice Fontaine Orange 16/04/2007 */
/* Bug correction : Solving compilation issue when using DMUPNP_NOUSE_CONTROLPOINT flag */
#ifndef MUPNP_NOUSE_CONTROLPOINT
/* ADD END Fabrice Fontaine Orange 16/04/2007 */
#ifndef MUPNP_OPTIMIZED_CP_MODE
    if (mupnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == false) {
      mupnp_device_delete(dev);
      return false;
    }
#endif
/* ADD Fabrice Fontaine Orange 16/04/2007 */
#endif
    /* ADD END Fabrice Fontaine Orange 16/04/2007 */
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/*****************************************************************************
 * Device Type
 *****************************************************************************/

/** 
 * Get the identifier-part of a device type string (usually "urn") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_getidentifier(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t len = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Find the next delimiter ':' character */
  for (tail = 0; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (tail > 0 && tail < len) {
    part = (char*)malloc(tail + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType, tail + 1);
      part[tail + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the URN part of a device type string (usually "schemas-upnp-org") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_geturn(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t head = 0;
  size_t len = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Find the first delimiter ':' character */
  for (head = 0; head < len; head++) {
    if (deviceType[head] == ':') {
      head++; /* Don't include the colon */
      break;
    }
  }

  /* Find the next delimiter ':' character */
  for (tail = head; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType + head, tail - head + 1);
      part[tail - head + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the device part of a device type string (usually just "device")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_getdevice(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t head = 0;
  size_t len = 0;
  size_t count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Skip the first two delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (deviceType[head] == ':') {
      if (count == 1) {
        head++; /* Don't include the colon */
        break;
      }
      else {
        count++;
      }
    }
  }

  /* Find the next delimiter ':' character */
  for (tail = head; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType + head, tail - head + 1);
      part[tail - head + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the type part of a device type string (ex. "ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_gettype(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t head = 0;
  size_t len = 0;
  size_t count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Skip the first three delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (deviceType[head] == ':') {
      if (count == 2) {
        head++; /* Don't include the colon */
        break;
      }
      else {
        count++;
      }
    }
  }

  /* Find the next delimiter ':' character */
  for (tail = head; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType + head, tail - head + 1);
      part[tail - head + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the schema type part of a device type string (without last colon)
 * (ex. "urn:schemas-upnp-org:device:ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_getschematype(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t len = 0;
  size_t count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Skip the first four delimiter ':' characters */
  for (tail = 0; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      if (count == 3) {
        tail--; /* Don't include the colon */
        break;
      }
      else {
        count++;
      }
    }
  }

  /* Copy the part to a new buffer */
  if (tail > 0 && tail < len) {
    part = (char*)malloc(tail + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType, tail + 1);
      part[tail + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the version part of a device type string (ex. "1")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_devicetype_getversion(const char* deviceType)
{
  char* part = NULL;
  size_t tail = 0;
  size_t head = 0;
  size_t len = 0;
  size_t count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (deviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(deviceType);

  /* Skip the first four delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (deviceType[head] == ':') {
      if (count == 3) {
        head++; /* Don't include the colon */
        break;
      }
      else {
        count++;
      }
    }
  }

  /* Find the next delimiter ':' character (or stop at the end) */
  for (tail = head; tail < len; tail++) {
    if (deviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail <= len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, deviceType + head, tail - head + 1);
      part[tail - head + 1] = '\0';
    }
    else
      mupnp_log_debug_s("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* mupnp_device_initchildnodes
****************************************/

static void mupnp_device_initchildnodes(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_initdevicelist(dev);
  mupnp_device_initservicelist(dev);
  mupnp_device_initiconlist(dev);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Device
*
****************************************/

/****************************************
* mupnp_device_initdevicelist
****************************************/

static void mupnp_device_initdevicelist(mUpnpDevice* dev)
{
  mUpnpXmlNode* devNode;
  mUpnpXmlNode* devListNode;
  mUpnpXmlNode* childNode;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_devicelist_clear(dev->deviceList);

  devNode = mupnp_device_getdevicenode(dev);
  if (devNode == NULL)
    return;

  devListNode = mupnp_xml_node_getchildnode(devNode, MUPNP_DEVICELIST_ELEM_NAME);
  if (devListNode == NULL)
    return;

  for (childNode = mupnp_xml_node_getchildnodes(devListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
    if (mupnp_device_isdevicenode(childNode) == false)
      continue;
    childDev = mupnp_device_new();
    /* Propagate "lease-time" from parent device to the child-devices; call "setleasetime"
		   (line below added by: rborges) */
    mupnp_device_setleasetime(childDev, mupnp_device_getleasetime(dev));
    /* Propagate important data (description URI, HTTP port) from the parent
	         * device to the child-devices; call "setdescriptionuri" and 
		 * "sethttpport" - (rosfran.borges) */
    mupnp_device_setdescriptionuri(childDev, mupnp_device_getdescriptionuri(dev));
    mupnp_device_sethttpport(childDev, mupnp_device_gethttpport(dev));
    mupnp_device_setdevicenode(childDev, childNode);
    mupnp_devicelist_add(dev->deviceList, childDev);
    mupnp_device_setparentdevice(childDev, dev);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Find a device from the device's children by the type of the device.
 * This function searches for devices, whose *type part* (i.e. not including
 * the version) of the device type string matches the given string.
 * For example: "urn:schemas-upnp-org:device:FooDevice". If you need
 * to know the version of a device, use \ref mupnp_devicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the device
 *
 */
mUpnpDevice* mupnp_device_getdevicebytype(mUpnpDevice* dev, const char* type)
{
  mUpnpDevice* childDev = NULL;
  mUpnpDevice* moreChildDev = NULL;
  const char* typeString = NULL;
  char* part = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(type) <= 0 || dev == NULL) {
    return NULL;
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    typeString = mupnp_device_getdevicetype(childDev);
    if (typeString != NULL) {
      part = mupnp_devicetype_getschematype(typeString);
      if (mupnp_strcmp(part, type) == 0) {
        free(part);
        return childDev;
      }
      else {
        free(part);
      }
    }

    moreChildDev = mupnp_device_getdevicebytype(childDev, type);
    if (moreChildDev != NULL) {
      return moreChildDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a device from the device's children by the type of the device.
 * This function searches for devices, whose *complete type string*
 * matches the given string, including version number. For example:
 * "urn:schemas-upnp-org:device:FooDevice:1". If you need to
 * disregard the version number, use \ref mupnp_device_getdevicebytype
 *
 * \param dev Device in question
 * \param exacttype Type of the device
 * 
 */
mUpnpDevice* mupnp_device_getdevicebyexacttype(mUpnpDevice* dev, const char* exacttype)
{
  mUpnpDevice* childDev = NULL;
  mUpnpDevice* moreChildDev = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(exacttype) <= 0 || dev == NULL) {
    return NULL;
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    if (mupnp_strcmp(mupnp_device_getdevicetype(childDev),
            exacttype)
        == 0) {
      return childDev;
    }

    moreChildDev = mupnp_device_getdevicebyexacttype(childDev,
        exacttype);
    if (moreChildDev != NULL) {
      return moreChildDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a device from the device's children by the UDN of the device.
 *
 * \param dev Device in question
 * \param udn Type of the device
 *
 */
mUpnpDevice* mupnp_device_getdevicebyudn(mUpnpDevice* dev, const char* udn)
{
  mUpnpDevice* childDev = NULL;
  mUpnpDevice* moreChildDev = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(udn) <= 0 || dev == NULL) {
    return NULL;
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    if (mupnp_strcmp(mupnp_device_getudn(childDev), udn) == 0) {
      return childDev;
    }

    moreChildDev = mupnp_device_getdevicebyudn(childDev, udn);
    if (moreChildDev != NULL) {
      return moreChildDev;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_device_getdevicebydescriptionuri
****************************************/

mUpnpDevice* mupnp_device_getdevicebydescriptionuri(mUpnpDevice* dev, const char* url)
{
  mUpnpDevice* childDev;
  mUpnpDevice* moreChildDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(url) <= 0)
    return NULL;

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    if (mupnp_device_isdescriptionuri(dev, url) == true)
      return dev;
    moreChildDev = mupnp_device_getdevicebydescriptionuri(childDev, url);
    if (moreChildDev != NULL)
      return moreChildDev;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
*
* Notify
*
****************************************/

/****************************************
* mupnp_device_notifywait
****************************************/

static void mupnp_device_notifywait(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_waitrandom(MUPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_getlocationurl
****************************************/

const char* mupnp_device_getlocationurl(mUpnpDevice* dev, const char* host, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_net_gethosturl(
      host,
      mupnp_device_gethttpport(dev),
      mupnp_device_getdescriptionuri(dev),
      buf,
      bufSize);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_getnotifydevicent
****************************************/

char* mupnp_device_getnotifydevicent(mUpnpDevice* dev, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_device_isrootdevice(dev) == false) {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "%s", mupnp_device_getudn(dev));
#else
    mupnp_strcpy(buf, mupnp_device_getudn(dev));
#endif
  }
  else {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "%s", MUPNP_DEVICE_UPNP_ROOTDEVICE);
#else
    mupnp_strcpy(buf, MUPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
  }
  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_device_getnotifydeviceusn
****************************************/

char* mupnp_device_getnotifydeviceusn(mUpnpDevice* dev, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_device_isrootdevice(dev) == true) {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "%s::%s", mupnp_device_getudn(dev), MUPNP_DEVICE_UPNP_ROOTDEVICE);
#else
    sprintf(buf, "%s::%s", mupnp_device_getudn(dev), MUPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
  }
  else {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "%s", mupnp_device_getudn(dev));
#else
    sprintf(buf, "%s", mupnp_device_getudn(dev));
#endif
  }

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_device_getnotifydevicetypent
****************************************/

char* mupnp_device_getnotifydevicetypent(mUpnpDevice* dev, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%s", mupnp_device_getdevicetype(dev));
#else
  sprintf(buf, "%s", mupnp_device_getdevicetype(dev));
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_device_getnotifydevicetypeusn
****************************************/

char* mupnp_device_getnotifydevicetypeusn(mUpnpDevice* dev, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%s::%s", mupnp_device_getudn(dev), mupnp_device_getdevicetype(dev));
#else
  sprintf(buf, "%s::%s", mupnp_device_getudn(dev), mupnp_device_getdevicetype(dev));
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_device_announcefrom
****************************************/

bool mupnp_device_announcefrom(mUpnpDevice* dev, char* bindAddr)
{
  char ssdpLineBuf[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  mUpnpServiceList* serviceList;
  mUpnpService* service;
  mUpnpDeviceList* devList;
  mUpnpDevice* childDev;
  mUpnpSSDPRequest* ssdpReq;
  mUpnpSSDPSocket* ssdpSock;
  bool sentResult;

  mupnp_log_debug_l4("Entering...\n");
  mupnp_log_debug_s("Announcing from %s\n", bindAddr);

  ssdpSock = mupnp_ssdp_socket_new();
  ssdpReq = mupnp_ssdprequest_new();

  mupnp_ssdprequest_setserver(ssdpReq, mupnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setleasetime(ssdpReq, mupnp_device_getleasetime(dev));
  mupnp_ssdprequest_setlocation(ssdpReq, mupnp_device_getlocationurl(dev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setnts(ssdpReq, MUPNP_SSDP_NTS_ALIVE);
  mupnp_ssdprequest_setbootid(ssdpReq, mupnp_device_getbootid(dev));

  /**** uuid:device-UUID(::upnp:rootdevice) ****/
  if (mupnp_device_isrootdevice(dev) == true) {
    mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
    mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
    mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
    mupnp_wait(20);
  }

  /**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
  mupnp_wait(20);

  /**** root or embedded device UUID ****/
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getudn(dev));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getudn(dev));
  sentResult = mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
  mupnp_wait(20);

  mupnp_ssdprequest_delete(ssdpReq);
  mupnp_ssdp_socket_close(ssdpSock);
  mupnp_ssdp_socket_delete(ssdpSock);

  /**** child services ****/
  serviceList = mupnp_device_getservicelist(dev);
  for (service = mupnp_servicelist_gets(serviceList); service != NULL; service = mupnp_service_next(service))
    mupnp_service_announcefrom(service, bindAddr);

  /**** child deveices ****/
  devList = mupnp_device_getdevicelist(dev);
  for (childDev = mupnp_devicelist_gets(devList); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    mupnp_device_announcefrom(childDev, bindAddr);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return sentResult;
}

/****************************************
* mupnp_device_announce
****************************************/

void mupnp_device_announce(mUpnpDevice* dev)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  char* bindAddr;
  int ssdpCount, i;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_notifywait(dev);

  ssdpCount = mupnp_ssdp_getannouncecount();

  netIfList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(netIfList);
  for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    bindAddr = mupnp_net_interface_getaddress(netIf);
    if (mupnp_strlen(bindAddr) <= 0)
      continue;
    for (i = 0; i < ssdpCount; i++) {
      mupnp_device_announcefrom(dev, bindAddr);
    }
  }

  mupnp_net_interfacelist_delete(netIfList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_byebyefrom
****************************************/

bool mupnp_device_byebyefrom(mUpnpDevice* dev, char* bindAddr)
{
  char ssdpLineBuf[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  mUpnpServiceList* serviceList;
  mUpnpService* service;
  mUpnpDeviceList* devList;
  mUpnpDevice* childDev;
  mUpnpSSDPRequest* ssdpReq;
  mUpnpSSDPSocket* ssdpSock;
  bool sentResult;

  mupnp_log_debug_l4("Entering...\n");

  ssdpSock = mupnp_ssdp_socket_new();
  ssdpReq = mupnp_ssdprequest_new();

  mupnp_ssdprequest_setnts(ssdpReq, MUPNP_SSDP_NTS_BYEBYE);

  /**** uuid:device-UUID(::upnp:rootdevice) ****/
  if (mupnp_device_isrootdevice(dev) == true) {
    mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
    mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
    mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
  }

  /**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

  /**** root or embedded device UUID ****/
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_device_getudn(dev));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_device_getudn(dev));
  sentResult = mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

  mupnp_ssdprequest_delete(ssdpReq);
  mupnp_ssdp_socket_close(ssdpSock);
  mupnp_ssdp_socket_delete(ssdpSock);

  serviceList = mupnp_device_getservicelist(dev);
  for (service = mupnp_servicelist_gets(serviceList); service != NULL; service = mupnp_service_next(service))
    mupnp_service_byebyefrom(service, bindAddr);

  devList = mupnp_device_getdevicelist(dev);
  for (childDev = mupnp_devicelist_gets(devList); childDev != NULL; childDev = mupnp_device_next(childDev))
    mupnp_device_byebyefrom(childDev, bindAddr);

  mupnp_log_debug_l4("Leaving...\n");

  return sentResult;
}

/****************************************
* mupnp_device_byebye
****************************************/

void mupnp_device_byebye(mUpnpDevice* dev)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  char* bindAddr;
  int ssdpCount, i;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_notifywait(dev);

  ssdpCount = mupnp_ssdp_getannouncecount();

  netIfList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(netIfList);
  for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    bindAddr = mupnp_net_interface_getaddress(netIf);
    if (mupnp_strlen(bindAddr) <= 0)
      continue;
    for (i = 0; i < ssdpCount; i++)
      mupnp_device_byebyefrom(dev, bindAddr);
  }

  mupnp_net_interfacelist_delete(netIfList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_postsearchresponse
****************************************/

bool mupnp_device_postsearchresponse(mUpnpDevice* dev, mUpnpSSDPPacket* ssdpPkt, const char* st, const char* usn)
{
  mUpnpDevice* rootDev;
  mUpnpSSDPResponse* ssdpRes;
  char httpDateStr[MUPNP_HTTP_DATE_MAXLEN];
  char* localAddr;
  char* remoteAddr;
  int remotePort;
  char rootDevLocation[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  char serverBuf[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  int ssdpCount;
  mUpnpSSDPSocket* ssdpSock;
  int n;

  mupnp_log_debug_l4("Entering...\n");

  localAddr = mupnp_ssdp_packet_getlocaladdress(ssdpPkt);
  mupnp_log_debug_s("Local address: <%s>\n", localAddr);
  rootDev = mupnp_device_getrootdevice(dev);

  mupnp_device_getlocationurl(rootDev, localAddr, rootDevLocation, sizeof(rootDevLocation));

  ssdpRes = mupnp_ssdpresponse_new();
  mupnp_ssdpresponse_setleasetime(ssdpRes, mupnp_device_getleasetime(dev));
  mupnp_ssdpresponse_setdate(ssdpRes, mupnp_http_getdate(mupnp_getcurrentsystemtime(), httpDateStr, sizeof(httpDateStr)));
  mupnp_ssdpresponse_setst(ssdpRes, st);
  mupnp_ssdpresponse_setbootid(ssdpRes, mupnp_device_getbootid(dev));

  mupnp_ssdpresponse_setext(ssdpRes);
  mupnp_getservername(serverBuf, MUPNP_SSDP_HEADER_LINE_MAXSIZE);
  mupnp_ssdpresponse_setserver(ssdpRes, serverBuf);

  mupnp_ssdpresponse_setusn(ssdpRes, usn);
  mupnp_ssdpresponse_setlocation(ssdpRes, rootDevLocation);

  remoteAddr = mupnp_ssdp_packet_getremoteaddress(ssdpPkt);
  remotePort = mupnp_ssdp_packet_getremoteport(ssdpPkt);
  mupnp_log_debug_s("Remote address: <%s>\n", remoteAddr);
  ssdpCount = mupnp_ssdp_getannouncecount();

  ssdpSock = mupnp_ssdp_socket_new();

  for (n = 0; n < ssdpCount; n++) {
    mupnp_waitrandom(20);
    mupnp_ssdp_socket_postresponse(ssdpSock, ssdpRes, remoteAddr, remotePort);
  }

  mupnp_ssdp_socket_delete(ssdpSock);

  mupnp_ssdpresponse_delete(ssdpRes);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
*
* Start/Stop
*
****************************************/

/****************************************
* mupnp_device_start
****************************************/

bool mupnp_device_start(mUpnpDevice* dev)
{
  MUPNP_HTTP_LISTENER httpListener;
  int httpPort;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_stop(dev);

  /* Create interface cache, if it does not exist and cache current */
  if (dev->ifCache == NULL)
    dev->ifCache = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(dev->ifCache);

  /**** HTTP Server ****/
  httpPort = mupnp_device_gethttpport(dev);
  /* Opening HTTP server may fail, so try many ports */
  while (mupnp_http_serverlist_open(dev->httpServerList, httpPort) == false) {
    mupnp_device_sethttpport(dev, httpPort + 1);
    httpPort = mupnp_device_gethttpport(dev);
  }
  mupnp_http_serverlist_setuserdata(dev->httpServerList, dev);
  httpListener = mupnp_device_gethttplistener(dev);
  if (httpListener == NULL)
    httpListener = mupnp_device_httprequestrecieved;
  mupnp_http_serverlist_setlistener(dev->httpServerList, httpListener);
  mupnp_http_serverlist_start(dev->httpServerList);

  /**** SSDP Server ****/
  if (mupnp_ssdp_serverlist_open(dev->ssdpServerList) == false)
    return false;
  mupnp_ssdp_serverlist_setlistener(dev->ssdpServerList, mupnp_device_ssdplistener);
  mupnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, dev);
  mupnp_ssdp_serverlist_start(dev->ssdpServerList);

  /**** Update BootId ****/
  mupnp_device_setbootid(dev, mupnp_createbootid());

  /**** Announce ****/
  mupnp_device_announce(dev);

  /**** Advertiser ****/
  mupnp_device_advertiser_start(dev);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_device_ipchanged
****************************************/

bool mupnp_device_ipchanged(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return false;
}

/****************************************
* mupnp_device_stop
****************************************/

bool mupnp_device_stop(mUpnpDevice* dev)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_byebye(dev);

  /**** Advertiser ****/
  if (mupnp_device_advertiser_isrunning(dev))
    mupnp_device_advertiser_stop(dev);

  /**** HTTP Server ****/
  if (0 < mupnp_http_headerlist_size(dev->httpServerList)) {
    mupnp_http_serverlist_setlistener(dev->httpServerList, NULL);
    mupnp_http_serverlist_stop(dev->httpServerList);
    mupnp_http_serverlist_close(dev->httpServerList);
    mupnp_http_serverlist_clear(dev->httpServerList);
  }

  /**** SSDP Server ****/
  if (0 < mupnp_ssdp_serverlist_size(dev->ssdpServerList)) {
    mupnp_ssdp_serverlist_setlistener(dev->ssdpServerList, NULL);
    mupnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, NULL);
    mupnp_ssdp_serverlist_stop(dev->ssdpServerList);
    mupnp_ssdp_serverlist_close(dev->ssdpServerList);
    mupnp_ssdp_serverlist_clear(dev->ssdpServerList);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_device_isrunning
 ****************************************/

bool mupnp_device_isrunning(mUpnpDevice* dev)
{
  return mupnp_device_advertiser_isrunning(dev);
}

/****************************************
*
* Embedded Service
*
****************************************/

/****************************************
* mupnp_device_initservicelist
****************************************/

static void mupnp_device_initservicelist(mUpnpDevice* dev)
{
  mUpnpXmlNode* devNode;
  mUpnpXmlNode* serviceListNode;
  mUpnpXmlNode* childNode;
  mUpnpService* childService;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_servicelist_clear(dev->serviceList);

  devNode = mupnp_device_getdevicenode(dev);
  if (devNode == NULL)
    return;

  serviceListNode = mupnp_xml_node_getchildnode(devNode, MUPNP_SERVICELIST_ELEM_NAME);
  if (serviceListNode == NULL)
    return;

  for (childNode = mupnp_xml_node_getchildnodes(serviceListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
    if (mupnp_service_isservicenode(childNode) == false)
      continue;
    childService = mupnp_service_new();
    mupnp_service_setservicenode(childService, childNode);
    mupnp_servicelist_add(dev->serviceList, childService);
    mupnp_service_setdevice(childService, dev);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

mUpnpService* mupnp_device_getservicebyserviceid(mUpnpDevice* dev, const char* serviceId)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(serviceId) <= 0 || dev == NULL) {
    return NULL;
  }

  for (service = mupnp_device_getservices(dev);
       service != NULL;
       service = mupnp_service_next(service)) {
    if (mupnp_strcmp(mupnp_service_getserviceid(service),
            serviceId)
        == 0) {
#ifdef MUPNP_OPTIMIZED_CP_MODE
      if (mupnp_service_isparsed(service) == false)
        mupnp_controlpoint_parsescservicescpd(service);
#endif
      return service;
    }
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebyserviceid(childDev, serviceId);
    if (service != NULL) {
      return service;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *complete type string*
 * matches the given string, including version number. For example:
 * "urn:schemas-upnp-org:service:ContentDirectory:1". If you need to
 * know the version of a service, use \ref mupnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 * 
 */
mUpnpService* mupnp_device_getservicebyexacttype(mUpnpDevice* dev, const char* type)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(type) <= 0 || dev == NULL) {
    return NULL;
  }

  for (service = mupnp_device_getservices(dev);
       service != NULL;
       service = mupnp_service_next(service)) {
    if (mupnp_strcmp(mupnp_service_getservicetype(service),
            type)
        == 0) {
#ifdef MUPNP_OPTIMIZED_CP_MODE
      if (mupnp_service_isparsed(service) == false)
        mupnp_controlpoint_parsescservicescpd(service);
#endif
      return service;
    }
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebyexacttype(childDev, type);
    if (service != NULL) {
      return service;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/**
 * Find a service from the device by the type of the service.
 * This function searches for services, whose *type part* (i.e. not including
 * the version) of the service type string matches the given string.
 * For example: "urn:schemas-upnp-org:service:ContentDirectory". If you need
 * to know the version of a service, use \ref mupnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 *
 */
mUpnpService* mupnp_device_getservicebytype(mUpnpDevice* dev, const char* type)
{
  mUpnpService* service = NULL;
  mUpnpDevice* childDev = NULL;
  const char* typeString = NULL;
  char* part = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(type) <= 0) {
    return NULL;
  }

  for (service = mupnp_device_getservices(dev);
       service != NULL;
       service = mupnp_service_next(service)) {
    typeString = mupnp_service_getservicetype(service);
    if (typeString != NULL) {
      if (mupnp_strcmp(typeString, type) == 0)
        return service;
      part = mupnp_servicetype_getschematype(typeString);
      if (mupnp_strcmp(part, type) == 0) {
        free(part);
#ifdef MUPNP_OPTIMIZED_CP_MODE
        if (mupnp_service_isparsed(service) == false)
          mupnp_controlpoint_parsescservicescpd(service);
#endif
        return service;
      }
      else {
        free(part);
      }
    }
  }

  for (childDev = mupnp_device_getdevices(dev);
       childDev != NULL;
       childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebytype(childDev, type);
    if (service != NULL) {
      return service;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_device_getservicebyscpdurl
****************************************/

mUpnpService* mupnp_device_getservicebyscpdurl(mUpnpDevice* dev, const char* url)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(url) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    mupnp_log_debug_s("Child node v: %s\n", mupnp_xml_node_getchildnodevalue(mupnp_service_getservicenode(service), MUPNP_SERVICE_SCPDURL));
    if (mupnp_service_isscpdurl(service, url) == true)
      return service;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebyscpdurl(childDev, url);
    if (service != NULL)
      return service;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_device_getservicebycontrolurl
****************************************/

mUpnpService* mupnp_device_getservicebycontrolurl(mUpnpDevice* dev, const char* url)
{
  mUpnpService* service;
  mUpnpDevice* childDev;
  mUpnpNetURL* service_url;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(url) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    /* mupnp_log_debug_s("<%s> == <%s> ?\n", url, mupnp_net_url_getrequest(mupnp_service_getcontrolurl(service))); */
    /* MODIFICATION Fabrice Fontaine Orange 23/04/07
		if (mupnp_strstr(mupnp_net_url_getrequest(mupnp_service_getcontrolurl(service)), url) != -1)*/
    /* Memory leak correction : mupnp_service_getcontrolurl return a malloc */
    /* structure, this structure must be freed after use */
    service_url = mupnp_service_getcontrolurl(service);
    if (service_url) {
      if (mupnp_strstr(mupnp_net_url_getrequest(service_url), url) != -1) {
        mupnp_net_url_delete(service_url);
        /* MODIFICATION END Fabrice Fontaine Orange 23/04/07 */
        return service;
        /* ADD Fabrice Fontaine Orange 23/04/07 */
      }
      mupnp_net_url_delete(service_url);
    }
    /* ADD END Fabrice Fontaine Orange 23/04/07 */
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebycontrolurl(childDev, url);
    if (service != NULL)
      return service;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_device_getservicebysid
****************************************/

mUpnpService* mupnp_device_getservicebysid(mUpnpDevice* dev, const char* sid)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(sid) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_service_getsubscriberbysid(service, sid) != NULL) {
      return service;
    }
    else if (mupnp_service_issubscribed(service) == true && mupnp_strcmp(mupnp_service_getsubscriptionsid(service), sid) == 0) {
      return service;
    }
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebysid(childDev, sid);
    if (service != NULL)
      return service;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_device_setactionlistener
****************************************/

void mupnp_device_setactionlistener(mUpnpDevice* dev, MUPNP_ACTION_LISTNER actionListner)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service))
    mupnp_service_setactionlistener(service, actionListner);

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev))
    mupnp_device_setactionlistener(childDev, actionListner);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_setquerylistener
****************************************/

void mupnp_device_setquerylistener(mUpnpDevice* dev, MUPNP_STATEVARIABLE_LISTNER queryListner)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service))
    mupnp_service_setquerylistener(service, queryListner);

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev))
    mupnp_device_setquerylistener(childDev, queryListner);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_getservicebyeventsuburl
****************************************/

mUpnpService* mupnp_device_getservicebyeventsuburl(mUpnpDevice* dev, const char* url)
{
  mUpnpService* service;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(url) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    if (mupnp_streq(mupnp_net_url_getpath(mupnp_service_geteventsuburl(service)), url) == true)
      return service;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    service = mupnp_device_getservicebyeventsuburl(childDev, url);
    if (service != NULL)
      return service;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
 * mupnp_device_getsmallesticonbymimetype
 ****************************************/

mUpnpIcon* mupnp_device_getsmallesticonbymimetype(mUpnpDevice* dev, const char* mimeType)
{
  mUpnpIcon* icon;
  mUpnpIcon* smallestIcon;

  smallestIcon = NULL;
  for (icon = mupnp_device_geticons(dev); icon; icon = mupnp_icon_next(icon)) {
    if (0 < mupnp_strlen(mimeType)) {
      if (!mupnp_streq(mupnp_icon_getmimetype(icon), mimeType))
        continue;
    }
    if (!smallestIcon) {
      smallestIcon = icon;
      continue;
    }
    if (mupnp_icon_getwidth(icon) < mupnp_icon_getwidth(smallestIcon))
      smallestIcon = icon;
  }

  return smallestIcon;
}

/****************************************
* mupnp_device_getsmallesticon
****************************************/

mUpnpIcon* mupnp_device_getsmallesticon(mUpnpDevice* dev)
{
  return mupnp_device_getsmallesticonbymimetype(dev, "");
}

/****************************************
 * mupnp_device_getabsoluteiconurl
 ****************************************/

bool mupnp_device_getabsoluteiconurl(mUpnpDevice* dev, mUpnpIcon* icon, mUpnpString* buf)
{
  mUpnpNetURI* uri;
  mUpnpNetURI* ssdpUri;
  mUpnpDevice* rootDev;
  const char* ssdplocation;

  uri = mupnp_net_uri_new();

  mupnp_net_uri_set(uri, mupnp_icon_geturl(icon));
  if (mupnp_net_uri_isabsolute(uri)) {
    mupnp_string_setvalue(buf, mupnp_net_uri_geturi(uri));
    mupnp_net_uri_delete(uri);
    return true;
  }

  rootDev = mupnp_device_getrootdevice(dev);
  if (rootDev) {
    ssdplocation = mupnp_device_getlocationfromssdppacket(rootDev);
    ssdpUri = mupnp_net_uri_new();
    if (0 < mupnp_strlen(ssdplocation)) {
      mupnp_net_uri_set(uri, ssdplocation);
      mupnp_net_uri_setpath(uri, mupnp_icon_geturl(icon));
      mupnp_string_setvalue(buf, mupnp_net_uri_getvalue(uri));
      mupnp_net_uri_delete(uri);
      return true;
    }
    mupnp_net_uri_delete(ssdpUri);
  }

  mupnp_net_uri_delete(uri);

  return false;
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_device_getactionbyname
****************************************/

mUpnpAction* mupnp_device_getactionbyname(mUpnpDevice* dev, const char* name)
{
  mUpnpService* service;
  mUpnpAction* action;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(name) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    action = mupnp_service_getactionbyname(service, name);
    if (action != NULL)
      return action;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    action = mupnp_device_getactionbyname(childDev, name);
    if (action != NULL)
      return action;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
*
* Embedded StateVariable
*
****************************************/

/****************************************
* mupnp_device_getstatevariablebyname
****************************************/

mUpnpStateVariable* mupnp_device_getstatevariablebyname(mUpnpDevice* dev, const char* name)
{
  mUpnpService* service;
  mUpnpStateVariable* statVar;
  mUpnpDevice* childDev;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(name) <= 0)
    return NULL;

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    statVar = mupnp_service_getstatevariablebyname(service, name);
    if (statVar != NULL)
      return statVar;
  }

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev)) {
    statVar = mupnp_device_getstatevariablebyname(childDev, name);
    if (statVar != NULL)
      return statVar;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
*
* Embedded Icon
*
****************************************/

/****************************************
* mupnp_device_initiconlist
****************************************/

static void mupnp_device_initiconlist(mUpnpDevice* dev)
{
  mUpnpXmlNode* devNode;
  mUpnpXmlNode* iconListNode;
  mUpnpXmlNode* childNode;
  mUpnpIcon* childIcon;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_iconlist_clear(dev->iconList);

  devNode = mupnp_device_getdevicenode(dev);
  if (devNode == NULL)
    return;

  iconListNode = mupnp_xml_node_getchildnode(devNode, MUPNP_ICONLIST_ELEM_NAME);
  if (iconListNode == NULL)
    return;

  for (childNode = mupnp_xml_node_getchildnodes(iconListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
    if (mupnp_icon_isiconnode(childNode) == false)
      continue;
    childIcon = mupnp_icon_new();
    mupnp_icon_seticonnode(childIcon, childNode);
    mupnp_iconlist_add(dev->iconList, childIcon);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_device_addicon
 ****************************************/

bool mupnp_device_addicon(mUpnpDevice* dev, mUpnpIcon* icon)
{
  mUpnpXmlNode* devNode;
  mUpnpXmlNode* iconListNode;
  mUpnpXmlNode* iconNode;
  mUpnpXmlNode* copyIconNode;
  mUpnpIcon* copyIcon;

  iconNode = mupnp_icon_geticonnode(icon);
  if (iconNode == NULL)
    return false;

  devNode = mupnp_device_getdevicenode(dev);
  if (devNode == NULL)
    return false;

  iconListNode = mupnp_xml_node_getchildnode(devNode, MUPNP_ICONLIST_ELEM_NAME);
  if (iconListNode == NULL) {
    iconListNode = mupnp_xml_node_new();
    mupnp_xml_node_setname(iconListNode, MUPNP_ICONLIST_ELEM_NAME);
    mupnp_xml_node_addchildnode(devNode, iconListNode);
  }

  copyIconNode = mupnp_xml_node_new();
  mupnp_xml_node_copy(copyIconNode, iconNode);
  mupnp_xml_node_addchildnode(iconListNode, copyIconNode);

  copyIcon = mupnp_icon_new();
  mupnp_icon_seticonnode(copyIcon, copyIconNode);
  mupnp_iconlist_add(dev->iconList, copyIcon);

  return true;
}

/****************************************
 * mupnp_device_updateudn
 ****************************************/

void mupnp_device_updateudn(mUpnpDevice* dev)
{
  char uuid[MUPNP_UUID_MAX_LEN];
  mupnp_createuuid(uuid, sizeof(uuid));
  mupnp_device_setudn(dev, uuid);
}

/****************************************
 * mupnp_device_setpresentationlistener
 ****************************************/

void mupnp_device_setpresentationlistener(mUpnpDevice* dev, MUPNP_PRESENTATION_LISTNER func)
{
  mupnp_device_removepresentationurl(dev);
  if (func)
    mupnp_device_setpresentationurl(dev, MUPNP_DEVICE_DEFAULT_PRESENTATION_URI);

  dev->presentationListener = func;
}

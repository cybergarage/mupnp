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
#include <mupnp/service.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/upnp.h>
#include <mupnp/upnp_function.h>
#include <mupnp/device.h>
#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>
#include <mupnp/net/url.h>
#include <mupnp/net/uri.h>

#ifdef MUPNP_OPTIMIZED_CP_MODE
#include <mupnp/controlpoint.h>
#endif

/****************************************
* prototype define for static functions
****************************************/

static void mupnp_service_initchildnodes(mUpnpService* service);
static void mupnp_service_initactionlist(mUpnpService* service);
static void mupnp_service_initservicestatetable(mUpnpService* service);
static mUpnpNetURL* mupnp_service_mangleurl(mUpnpService* service, char* type);

/****************************************
* mupnp_service_new
****************************************/

mUpnpService* mupnp_service_new()
{
  mUpnpService* service;

  mupnp_log_debug_l4("Entering...\n");

  service = (mUpnpService*)malloc(sizeof(mUpnpService));

  if (NULL != service) {
    mupnp_list_node_init((mUpnpList*)service);

    service->scpdNodeList = NULL;
    service->serviceNode = NULL;

    service->actionList = mupnp_actionlist_new();
    service->serviceStateTable = mupnp_servicestatetable_new();

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
    service->subscriberList = mupnp_subscriberlist_new();
#endif

#ifdef MUPNP_OPTIMIZED_CP_MODE
    service->parsed = false;
#endif

    service->mutex = mupnp_mutex_new();
    service->subscriptionSid = mupnp_string_new();

    mupnp_service_setdevice(service, NULL);
    mupnp_service_setsubscriptiontimeout(service, 0);
    mupnp_service_setsubscriptiontimestamp(service, 0);

    mupnp_service_setuserdata(service, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return service;
}

/****************************************
* mupnp_service_delete
****************************************/

void mupnp_service_delete(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)service);

  mupnp_service_clear(service);

  mupnp_actionlist_delete(service->actionList);
  mupnp_servicestatetable_delete(service->serviceStateTable);

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  mupnp_subscriberlist_delete(service->subscriberList);
#endif

  mupnp_mutex_delete(service->mutex);
  mupnp_string_delete(service->subscriptionSid);

  free(service);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_service_clear
****************************************/

void mupnp_service_clear(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

  /* Don't clear the service node */
  /* service->serviceNode = NULL; */

  if (service->scpdNodeList != NULL) {
    mupnp_xml_nodelist_delete(service->scpdNodeList);
    service->scpdNodeList = NULL;
  }

  mupnp_actionlist_clear(service->actionList);
  mupnp_servicestatetable_clear(service->serviceStateTable);
#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  mupnp_subscriberlist_clear(service->subscriberList);
#endif

  mupnp_service_setsubscriptionsid(service, NULL);
  mupnp_service_setsubscriptiontimeout(service, 0);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Compare the service's SCPD URL and the given location
 * 
 * @param service The service in question
 * @param url The URL (location) to compare
 * @return true if location is found from URL; otherwise false
 */
bool mupnp_service_isscpdurl(mUpnpService* service, const char* url)
{
  mUpnpXmlNode* s = mupnp_service_getservicenode(service);
  const char* v = mupnp_xml_node_getchildnodevalue(s, MUPNP_SERVICE_SCPDURL);
  return mupnp_net_uri_isequivalent(v, url);
}

/**
 * Get the service's event subscription URL
 *
 * @param service The service in question
 * @return char*
 */
mUpnpNetURL* mupnp_service_geteventsuburl(mUpnpService* service)
{
  return mupnp_service_mangleurl(service, MUPNP_SERVICE_EVENT_SUB_URL);
}

/**
 * Get the service's control URL
 * @param service The service in question
 * @return char*
 */
mUpnpNetURL* mupnp_service_getcontrolurl(mUpnpService* service)
{
  return mupnp_service_mangleurl(service, MUPNP_SERVICE_CONTROL_URL);
}

/**
 * Get the service's SCPD (service description) URL
 *
 * @param service The service in question                                                                 
 * @return mUpnpNetURL Pointer to URL/URI structure
 */
mUpnpNetURL* mupnp_service_getscpdurl(mUpnpService* service)
{
  return mupnp_service_mangleurl(service, MUPNP_SERVICE_SCPDURL);
}

/****************************************
* mupnp_service_parsedescription
****************************************/

bool mupnp_service_parsedescription(mUpnpService* service, const char* desciption, size_t descriptionLen)
{
  mUpnpXmlParser* xmlParser;
  bool xmlParseSuccess;
  mUpnpXmlNode* scpdNode;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_service_lock(service);
  mupnp_service_clear(service);

#ifdef MUPNP_OPTIMIZED_CP_MODE
  service->parsed = false;
#endif
  service->scpdNodeList = mupnp_xml_nodelist_new();

  xmlParser = mupnp_xml_parser_new();
  xmlParseSuccess = mupnp_xml_parse(xmlParser, service->scpdNodeList, desciption, descriptionLen);
  mupnp_xml_parser_delete(xmlParser);
  if (xmlParseSuccess == false) {
    mupnp_service_unlock(service);
    return false;
  }

  if (mupnp_xml_nodelist_size(service->scpdNodeList) <= 0) {
    mupnp_service_clear(service);
    mupnp_service_unlock(service);
    return false;
  }

  scpdNode = mupnp_service_getscpdnode(service);
  if (scpdNode == NULL) {
    mupnp_service_clear(service);
    mupnp_service_unlock(service);
    return false;
  }

  mupnp_service_initchildnodes(service);

#ifdef MUPNP_OPTIMIZED_CP_MODE
  service->parsed = true;
#endif
  mupnp_service_unlock(service);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_service_parsedescriptionurl
****************************************/

bool mupnp_service_parsedescriptionurl(mUpnpService* service, mUpnpNetURL* url)
{
  char* host;
  int port;
  char* request;
  mUpnpHttpRequest* httpReq;
  mUpnpHttpResponse* httpRes;
  char* content;
  size_t contentLen;
  bool parseSuccess;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_net_url_ishttpprotocol(url) == false)
    return false;

  host = mupnp_net_url_gethost(url);
  port = mupnp_net_url_getport(url);
  if (port <= 0)
    port = MUPNP_HTTP_DEFAULT_PORT;
  /**** Thanks for Theo Beisch (08/16/05) ****/
  request = mupnp_net_url_getrequest(url);
  if (mupnp_strlen(request) <= 0)
    request = "/";

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

  parseSuccess = mupnp_service_parsedescription(service, content, contentLen);

  mupnp_http_request_delete(httpReq);

  mupnp_log_debug_l4("Leaving...\n");

  return parseSuccess;
}

/****************************************
* mupnp_service_getdescription
****************************************/

char* mupnp_service_getdescription(mUpnpService* service, mUpnpString* descStr)
{
  mUpnpXmlNode* scpdNode;

  mupnp_log_debug_l4("Entering...\n");

  scpdNode = mupnp_service_getscpdnode(service);
  if (scpdNode != NULL) {
    mupnp_string_addvalue(descStr, MUPNP_XML_DECLARATION);
    mupnp_string_addvalue(descStr, "\n");
    mupnp_xml_node_tostring(scpdNode, true, descStr);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(descStr);
}

/****************************************
 * Service Type
 ****************************************/

/** 
 * Get the identifier-part of a service type string (usually "urn") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_servicetype_getidentifier(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  size_t len = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (serviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(serviceType);

  /* Find the next delimiter ':' character */
  for (tail = 0; tail < len; tail++) {
    if (serviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (tail > 0 && tail < len) {
    part = (char*)malloc(tail + 2);

    if (NULL != part) {
      mupnp_strncpy(part, serviceType, tail + 1);
      part[tail + 1] = '\0';
    }
    else
      mupnp_log_debug("Memory allocation problem!\n");
  }
  else {
    part = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return part;
}

/** 
 * Get the URN part of a service type string (usually "schemas-upnp-org") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_servicetype_geturn(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  int head = 0;
  size_t len = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (serviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(serviceType);

  /* Find the first delimiter ':' character */
  for (head = 0; head < len; head++) {
    if (serviceType[head] == ':') {
      head++; /* Don't include the colon */
      break;
    }
  }

  /* Find the next delimiter ':' character */
  for (tail = head; tail < len; tail++) {
    if (serviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the service part of a service type string (usually just "service")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_servicetype_getservice(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  int head = 0;
  size_t len = 0;
  int count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (serviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(serviceType);

  /* Skip the first two delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (serviceType[head] == ':') {
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
    if (serviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the type part of a service type string (ex. "ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_servicetype_gettype(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  int head = 0;
  size_t len = 0;
  int count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (serviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(serviceType);

  /* Skip the first three delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (serviceType[head] == ':') {
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
    if (serviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail < len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, serviceType + head, tail - head + 1);
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
 * Get the schema type part of a service type string (without last colon)
 * (ex. "urn:schemas-upnp-org:service:ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_servicetype_getschematype(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  size_t len = 0;
  int count = 0;

  if (serviceType == NULL) {
    return NULL;
  }

  mupnp_log_debug_l4("Entering...\n");

  len = mupnp_strlen(serviceType);

  /* Skip the first four delimiter ':' characters */
  for (tail = 0; tail < len; tail++) {
    if (serviceType[tail] == ':') {
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
      mupnp_strncpy(part, serviceType, tail + 1);
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
 * Get the version part of a service type string (ex. "1")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_servicetype_getversion(const char* serviceType)
{
  char* part = NULL;
  int tail = 0;
  int head = 0;
  size_t len = 0;
  int count = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (serviceType == NULL) {
    return NULL;
  }

  len = mupnp_strlen(serviceType);

  /* Skip the first four delimiter ':' characters */
  for (head = 0; head < len; head++) {
    if (serviceType[head] == ':') {
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
    if (serviceType[tail] == ':') {
      tail--; /* Don't take the colon */
      break;
    }
  }

  /* Copy the part to a new buffer */
  if (head > 0 && head < len && tail > 0 && tail <= len) {
    part = (char*)malloc(tail - head + 2);

    if (NULL != part) {
      mupnp_strncpy(part, serviceType + head, tail - head + 1);
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
* Announce
*
****************************************/

char* mupnp_service_getnotifyservicetypent(mUpnpService* service, char* buf, int bufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_strncpy(buf, mupnp_service_getservicetype(service), bufSize);
  buf[bufSize - 1] = '\0';

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

char* mupnp_service_getnotifyservicetypeusn(mUpnpService* service, char* buf, int bufSize)
{
  mUpnpDevice* dev;

  mupnp_log_debug_l4("Entering...\n");

  dev = mupnp_service_getdevice(service);
#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%s::%s", mupnp_device_getudn(dev), mupnp_service_getservicetype(service));
#else
  sprintf(buf, "%s::%s", mupnp_device_getudn(dev), mupnp_service_getservicetype(service));
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

bool mupnp_service_announcefrom(mUpnpService* service, const char* bindAddr)
{
  /**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
  char ssdpLineBuf[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  mUpnpDevice* rootDev;
  mUpnpDevice* dev;
  mUpnpSSDPRequest* ssdpReq;
  mUpnpSSDPSocket* ssdpSock;
  bool sentResult = true;

  mupnp_log_debug_l4("Entering...\n");

  rootDev = mupnp_service_getrootdevice(service);
  dev = mupnp_service_getdevice(service);

  ssdpReq = mupnp_ssdprequest_new();
  mupnp_ssdprequest_setserver(ssdpReq, mupnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setleasetime(ssdpReq, mupnp_device_getleasetime(dev));
  mupnp_ssdprequest_setlocation(ssdpReq, mupnp_device_getlocationurl(rootDev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setnts(ssdpReq, MUPNP_SSDP_NTS_ALIVE);
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));

  ssdpSock = mupnp_ssdp_socket_new();
  sentResult = mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
  mupnp_wait(20);
  mupnp_ssdp_socket_delete(ssdpSock);

  mupnp_ssdprequest_delete(ssdpReq);

  mupnp_log_debug_l4("Leaving...\n");

  return sentResult;
}

bool mupnp_service_byebyefrom(mUpnpService* service, const char* bindAddr)
{
  /**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
  char ssdpLineBuf[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  mUpnpSSDPRequest* ssdpReq;
  mUpnpSSDPSocket* ssdpSock;
  bool sentResult;

  mupnp_log_debug_l4("Entering...\n");

  ssdpReq = mupnp_ssdprequest_new();

  mupnp_ssdprequest_setnts(ssdpReq, MUPNP_SSDP_NTS_BYEBYE);
  mupnp_ssdprequest_setnt(ssdpReq, mupnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
  mupnp_ssdprequest_setusn(ssdpReq, mupnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));

  ssdpSock = mupnp_ssdp_socket_new();
  sentResult = mupnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
  mupnp_ssdp_socket_delete(ssdpSock);

  mupnp_ssdprequest_delete(ssdpReq);

  mupnp_log_debug_l4("Leaving...\n");

  return sentResult;
}

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* mupnp_service_initchildnodes
****************************************/

static void mupnp_service_initchildnodes(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_service_initactionlist(service);
  mupnp_service_initservicestatetable(service);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_service_initactionlist
****************************************/

static void mupnp_service_initactionlist(mUpnpService* service)
{
  mUpnpXmlNode* scdpNode;
  mUpnpXmlNode* actionListNode;
  //	mUpnpXmlNode *serviceNode;
  mUpnpXmlNode* childNode;
  mUpnpAction* action;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_actionlist_clear(service->actionList);

  scdpNode = mupnp_service_getscpdnode(service);
  if (scdpNode == NULL)
    return;

  actionListNode = mupnp_xml_node_getchildnode(scdpNode, MUPNP_ACTIONLIST_ELEM_NAME);
  if (actionListNode == NULL)
    return;

  //serviceNode = mupnp_service_getservicenode(service);
  for (childNode = mupnp_xml_node_getchildnodes(actionListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
    if (mupnp_action_isactionnode(childNode) == false)
      continue;
    action = mupnp_action_new();
    mupnp_action_setservice(action, service);
    mupnp_action_setactionnode(action, childNode);
    mupnp_actionlist_add(service->actionList, action);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_service_getactionbyname
****************************************/

mUpnpAction* mupnp_service_getactionbyname(mUpnpService* service, const char* name)
{
  mUpnpActionList* actionList;
  mUpnpAction* action;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(name) <= 0)
    return NULL;

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  actionList = mupnp_service_getactionlist(service);
  for (action = mupnp_actionlist_gets(actionList); action != NULL; action = mupnp_action_next(action)) {
    if (mupnp_action_isname(action, name) == true)
      return action;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
*
* Embedded ServiceStateTable
*
****************************************/

/****************************************
* mupnp_service_initservicestatetable
****************************************/

static void mupnp_service_initservicestatetable(mUpnpService* service)
{
  mUpnpXmlNode* scdpNode;
  mUpnpXmlNode* stateTableNode;
  //	mUpnpXmlNode *serviceNode;
  mUpnpXmlNode* childNode;
  mUpnpStateVariable* statVar;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_servicestatetable_clear(service->serviceStateTable);

  scdpNode = mupnp_service_getscpdnode(service);
  if (scdpNode == NULL)
    return;

  stateTableNode = mupnp_xml_node_getchildnode(scdpNode, MUPNP_SERVICESTATETALBE_ELEM_NAME);
  if (stateTableNode == NULL)
    return;

  //	serviceNode = mupnp_service_getservicenode(service);
  for (childNode = mupnp_xml_node_getchildnodes(stateTableNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
    if (mupnp_statevariable_isstatevariablenode(childNode) == false)
      continue;
    statVar = mupnp_statevariable_new();
    mupnp_statevariable_setservice(statVar, service);
    mupnp_statevariable_setstatevariablenode(statVar, childNode);
    mupnp_servicestatetable_add(service->serviceStateTable, statVar);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_service_getstatevariablebyname
****************************************/

mUpnpStateVariable* mupnp_service_getstatevariablebyname(mUpnpService* service, const char* name)
{
  mUpnpServiceStateTable* stateTable;
  mUpnpStateVariable* stateVar;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(name) <= 0)
    return NULL;

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  stateTable = mupnp_service_getservicestatetable(service);
  for (stateVar = mupnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = mupnp_statevariable_next(stateVar)) {
    if (mupnp_statevariable_isname(stateVar, name) == true)
      return stateVar;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_service_setactionlistener
****************************************/

void mupnp_service_setactionlistener(mUpnpService* service, MUPNP_ACTION_LISTNER actionListener)
{
  mUpnpActionList* actionList;
  mUpnpAction* action;

  mupnp_log_debug_l4("Entering...\n");

  actionList = mupnp_service_getactionlist(service);
  for (action = mupnp_actionlist_gets(actionList); action != NULL; action = mupnp_action_next(action))
    mupnp_action_setlistner(action, actionListener);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_service_setquerylistener
****************************************/

void mupnp_service_setquerylistener(mUpnpService* service, MUPNP_STATEVARIABLE_LISTNER queryListener)
{
  mUpnpServiceStateTable* stateTable;
  mUpnpStateVariable* stateVar;

  mupnp_log_debug_l4("Entering...\n");

  stateTable = mupnp_service_getservicestatetable(service);
  for (stateVar = mupnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = mupnp_statevariable_next(stateVar))
    mupnp_statevariable_setlistener(stateVar, queryListener);

  mupnp_log_debug_l4("Leaving...\n");
}

mUpnpActionList* mupnp_service_getactionlist(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  return service->actionList;

  mupnp_log_debug_l4("Leaving...\n");
}

mUpnpAction* mupnp_service_getactions(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  return mupnp_actionlist_gets(service->actionList);

  mupnp_log_debug_l4("Leaving...\n");
}

mUpnpServiceStateTable* mupnp_service_getservicestatetable(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  return service->serviceStateTable;

  mupnp_log_debug_l4("Leaving...\n");
}

mUpnpStateVariable* mupnp_service_getstatevariables(mUpnpService* service)
{
  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_OPTIMIZED_CP_MODE
  if (mupnp_service_isparsed(service) == false)
    mupnp_controlpoint_parsescservicescpd(service);
#endif
  return mupnp_servicestatetable_gets(service->serviceStateTable);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
*	Subscription
*
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_service_addsubscriber
****************************************/

bool mupnp_service_addsubscriber(mUpnpService* service, mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_subscriberlist_add(service->subscriberList, sub);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_service_removesubscriber
****************************************/

bool mupnp_service_removesubscriber(mUpnpService* service, mUpnpSubscriber* sub)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_subscriber_remove(sub);
  mupnp_subscriber_delete(sub);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_service_getsubscriberbysid
****************************************/

mUpnpSubscriber* mupnp_service_getsubscriberbysid(mUpnpService* service, const char* sid)
{
  mUpnpSubscriber* sub;
  char* subSid;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_strlen(sid) <= 0)
    return NULL;

  if (0 <= mupnp_strstr(sid, MUPNP_ST_UUID_DEVICE))
    sid += mupnp_strlen(MUPNP_ST_UUID_DEVICE) + 1;

  for (sub = mupnp_service_getsubscribers(service); sub != NULL; sub = mupnp_subscriber_next(sub)) {
    subSid = mupnp_subscriber_getsid(sub);
    if (0 <= mupnp_strstr(subSid, MUPNP_ST_UUID_DEVICE))
      subSid += mupnp_strlen(MUPNP_ST_UUID_DEVICE) + 1;
    if (mupnp_streq(sid, subSid) == true)
      return sub;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/* Private helper functions */

mUpnpNetURL* mupnp_service_mangleabsoluteurl(const char* serviceURLStr, const char* baseURLStr, const char* locationURLStr)
{
  mUpnpNetURL* absServiceURL;
  mUpnpNetURL* serviceURL;
  char* basePath;

  if (mupnp_strlen(serviceURLStr) <= 0)
    return NULL;

  absServiceURL = mupnp_net_url_new();

  mupnp_net_url_set(absServiceURL, serviceURLStr);

  /* Absolute URL case */
  if (mupnp_net_url_isabsolute(absServiceURL) == true) {
    mupnp_log_debug_s("Mangled URL: %s\n", mupnp_net_url_getrequest(absServiceURL));
    return absServiceURL;
  }

  /* URL base + absolute/relative path */
  if (0 < mupnp_strlen(baseURLStr)) {
    serviceURL = mupnp_net_url_new();
    mupnp_net_url_set(serviceURL, serviceURLStr);

    mupnp_net_url_set(absServiceURL, baseURLStr);

    if (mupnp_net_url_isabsolutepath(serviceURLStr)) {
      mupnp_net_url_setpath(absServiceURL, mupnp_net_url_getpath(serviceURL));
      mupnp_net_url_setquery(absServiceURL, mupnp_net_url_getquery(serviceURL));
    }
    else {
      mupnp_net_url_addpath(absServiceURL, mupnp_net_url_getpath(serviceURL));
      mupnp_net_url_setquery(absServiceURL, mupnp_net_url_getquery(serviceURL));
      mupnp_net_url_rebuild(absServiceURL);
    }

    mupnp_net_url_delete(serviceURL);

    mupnp_log_debug_s("Mangled URL: %s\n", mupnp_net_url_getrequest(absServiceURL));
    return absServiceURL;
  }

  /* URL base from location + absolute/relative path */
  mupnp_log_debug_s("Location from SSDP packet: %s\n", locationURLStr);

  if (mupnp_strlen(locationURLStr) <= 0) {
    mupnp_log_debug_s("Could not get location string from SSDP packet!\n");
    return absServiceURL;
  }

  mupnp_net_url_set(absServiceURL, locationURLStr);

  mupnp_log_debug_s("URL from location: %s - %d -%s",
      mupnp_net_url_gethost(absServiceURL),
      mupnp_net_url_getport(absServiceURL),
      mupnp_net_url_getpath(absServiceURL));
  /* UPnP spec says that path in location URL with last part removed should be 
     * considered as base path when getting service descriptions, if relative
     * paths are used in description XML.
     *
     * So we convert location http://192.168.1.1/base/path/description
     * into http://192.168.1.1/base/path/
     */

  basePath = mupnp_net_url_getupnpbasepath(absServiceURL);
  if (basePath) {
    mupnp_net_url_setpath(absServiceURL, basePath);
    free(basePath);
    locationURLStr = NULL;
  }

  serviceURL = mupnp_net_url_new();
  mupnp_net_url_set(serviceURL, serviceURLStr);

  if (mupnp_net_url_isabsolutepath(serviceURLStr)) {
    mupnp_net_url_setpath(absServiceURL, mupnp_net_url_getpath(serviceURL));
    mupnp_net_url_setquery(absServiceURL, mupnp_net_url_getquery(serviceURL));
  }
  else {
    mupnp_net_url_addpath(absServiceURL, mupnp_net_url_getpath(serviceURL));
    mupnp_net_url_setquery(absServiceURL, mupnp_net_url_getquery(serviceURL));
  }

  mupnp_net_url_delete(serviceURL);

  mupnp_log_debug_s("Mangled URL: %s\n", mupnp_net_url_getrequest(absServiceURL));

  return absServiceURL;
}

static mUpnpNetURL* mupnp_service_mangleurl(mUpnpService* service, char* type)
{
  const char* serviceURLStr;
  const char* baseURLStr;
  const char* locationURLStr;
  mUpnpDevice* rootDev;

  serviceURLStr = mupnp_xml_node_getchildnodevalue(mupnp_service_getservicenode(service), type);

  rootDev = mupnp_service_getrootdevice(service);
  if (rootDev != NULL) {
    baseURLStr = mupnp_device_geturlbase(rootDev);
    locationURLStr = mupnp_device_getlocationfromssdppacket(rootDev);
  }
  else {
    baseURLStr = NULL;
    locationURLStr = NULL;
  }

  return mupnp_service_mangleabsoluteurl(serviceURLStr, baseURLStr, locationURLStr);
}

#endif

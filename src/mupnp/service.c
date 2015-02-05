/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cservice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	08/16/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed mupnp_upnp_service_parsedescriptionurl() to set "/" as the default path when the url has no path.
*
*	09-Jan-06 Heikki Junnila
*		- Added service type parsing functions:
*		  mupnp_upnp_servicetype_getidentifier
*		  mupnp_upnp_servicetype_geturn
*		  mupnp_upnp_servicetype_getservice
*		  mupnp_upnp_servicetype_gettype
*		  mupnp_upnp_servicetype_getversion
*
*	10-Jan-06 Heikki Junnila
*		- Added mupnp_upnp_servicetype_getschematype() and fixed
*		  existing servicetype functions to accept 1-character strings
*
*	11-Jan-06 Heikki Junnila
*		- Removed mupnp_upnp_service_isname() and _getname() because
*		  according to UPnP specs, services have only types, not names
*
*	22-Oct-07 Aapo Makela
*		- Fixed memory leak (free location_str in mupnp_upnp_service_mangleurl())
*
 *	05-Jan-08  Satoshi Konno <skonno@cybergarage.org>
 *		- Fixed mupnp_upnp_service_mangleurl() to return correct url using mupnp_net_uri_rebuild() when a device has URLBase and the service's URL is relative.
 ******************************************************************/

#include <mupnp/service.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <mupnp/upnp.h>
#include <mupnp/upnp_function.h>
#include <mupnp/device.h>
#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>
#include <mupnp/net/url.h>
#include <mupnp/net/uri.h>

#ifdef CG_OPTIMIZED_CP_MODE
#include <mupnp/controlpoint.h>
#endif

/****************************************
* prototype define for static functions
****************************************/

static void mupnp_upnp_service_initchildnodes(CgUpnpService *service);
static void mupnp_upnp_service_initactionlist(CgUpnpService *service);
static void mupnp_upnp_service_initservicestatetable(CgUpnpService *service);
static CgNetURL *mupnp_upnp_service_mangleurl(CgUpnpService *service, char *type);

/****************************************
* mupnp_upnp_service_new
****************************************/

CgUpnpService *mupnp_upnp_service_new()
{
	CgUpnpService *service;

	mupnp_log_debug_l4("Entering...\n");

	service = (CgUpnpService *)malloc(sizeof(CgUpnpService));

	if ( NULL != service )
	{
		mupnp_list_node_init((CgList *)service);

		service->scpdNodeList = NULL;
		service->serviceNode = NULL;

		service->actionList = mupnp_upnp_actionlist_new();
		service->serviceStateTable = mupnp_upnp_servicestatetable_new();

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
		service->subscriberList = mupnp_upnp_subscriberlist_new();
#endif
	
#ifdef CG_OPTIMIZED_CP_MODE
		service->parsed = FALSE;
#endif

		service->mutex = mupnp_mutex_new();
		service->subscriptionSid = mupnp_string_new();
		
		mupnp_upnp_service_setdevice(service, NULL);
		mupnp_upnp_service_setsubscriptiontimeout(service, 0);
		mupnp_upnp_service_setsubscriptiontimestamp(service, 0);
		
		mupnp_upnp_service_setuserdata(service, NULL);
	}	
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return service;
}

/****************************************
* mupnp_upnp_service_delete
****************************************/

void mupnp_upnp_service_delete(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)service);
	
	mupnp_upnp_service_clear(service);

	mupnp_upnp_actionlist_delete(service->actionList);
	mupnp_upnp_servicestatetable_delete(service->serviceStateTable);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	mupnp_upnp_subscriberlist_delete(service->subscriberList);
#endif
	
	mupnp_mutex_delete(service->mutex);
	mupnp_string_delete(service->subscriptionSid);
	
	free(service);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_service_clear
****************************************/

void mupnp_upnp_service_clear(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

	/* Don't clear the service node */
	/* service->serviceNode = NULL; */
	
	if (service->scpdNodeList != NULL) {
		mupnp_xml_nodelist_delete(service->scpdNodeList);
		service->scpdNodeList = NULL;
	}

	mupnp_upnp_actionlist_clear(service->actionList);
	mupnp_upnp_servicestatetable_clear(service->serviceStateTable);
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	mupnp_upnp_subscriberlist_clear(service->subscriberList);
#endif
	
	mupnp_upnp_service_setsubscriptionsid(service, NULL);
	mupnp_upnp_service_setsubscriptiontimeout(service, 0);

	mupnp_log_debug_l4("Leaving...\n");
}	

/**
 * Compare the service's SCPD URL and the given location
 * 
 * @param service The service in question
 * @param url The URL (location) to compare
 * @return TRUE if location is found from URL; otherwise FALSE
 */
BOOL mupnp_upnp_service_isscpdurl(CgUpnpService *service, const char *url)
{
	CgXmlNode *s = mupnp_upnp_service_getservicenode(service);
	const char *v = mupnp_xml_node_getchildnodevalue(s, CG_UPNP_SERVICE_SCPDURL);
	return mupnp_net_uri_isequivalent(v, url);
}

/**
 * Get the service's event subscription URL
 *
 * @param service The service in question
 * @return char*
 */
CgNetURL *mupnp_upnp_service_geteventsuburl(CgUpnpService *service) 
{
	return mupnp_upnp_service_mangleurl(service, CG_UPNP_SERVICE_EVENT_SUB_URL);
}

/**
 * Get the service's control URL
 * @param service The service in question
 * @return char*
 */
CgNetURL *mupnp_upnp_service_getcontrolurl(CgUpnpService *service)
{ 
	return mupnp_upnp_service_mangleurl(service, CG_UPNP_SERVICE_CONTROL_URL);
}

/**
 * Get the service's SCPD (service description) URL
 *
 * @param service The service in question                                                                 
 * @return CgNetURL Pointer to URL/URI structure
 */
CgNetURL *mupnp_upnp_service_getscpdurl(CgUpnpService *service)
{
	return mupnp_upnp_service_mangleurl(service, CG_UPNP_SERVICE_SCPDURL);
}



/****************************************
* mupnp_upnp_service_parsedescription
****************************************/

BOOL mupnp_upnp_service_parsedescription(CgUpnpService *service, const char *desciption, size_t descriptionLen)
{
	CgXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	CgXmlNode *scpdNode;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_service_lock(service);
	mupnp_upnp_service_clear(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = FALSE;
#endif	
	service->scpdNodeList = mupnp_xml_nodelist_new();

	xmlParser = mupnp_xml_parser_new();
	xmlParseSuccess = mupnp_xml_parse(xmlParser, service->scpdNodeList, desciption, descriptionLen);
	mupnp_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
	{
		mupnp_upnp_service_unlock(service);
		return FALSE;
	}
	
	if (mupnp_xml_nodelist_size(service->scpdNodeList) <= 0) {
		mupnp_upnp_service_clear(service);
		mupnp_upnp_service_unlock(service);
		return FALSE;
	}

	scpdNode = mupnp_upnp_service_getscpdnode(service);
	if (scpdNode == NULL) {
		mupnp_upnp_service_clear(service);
		mupnp_upnp_service_unlock(service);
		return FALSE;
	}
		
	mupnp_upnp_service_initchildnodes(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = TRUE;
#endif
	mupnp_upnp_service_unlock(service);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_service_parsedescriptionurl
****************************************/

BOOL mupnp_upnp_service_parsedescriptionurl(CgUpnpService *service, CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	char *content;
	size_t contentLen;
	BOOL parseSuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = mupnp_net_url_gethost(url);
	port = mupnp_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	/**** Thanks for Theo Beisch (08/16/05) ****/
	request = mupnp_net_url_getrequest(url);
	if (mupnp_strlen(request) <= 0)
		request = "/";

	httpReq = mupnp_http_request_new();
	mupnp_http_request_setmethod(httpReq, CG_HTTP_GET);
	mupnp_http_request_seturi(httpReq, request);
	mupnp_http_request_setcontentlength(httpReq, 0);
	httpRes = mupnp_http_request_post(httpReq, host, port);
	
	/*statusCode = mupnp_http_response_getstatuscode(httpRes);
          if (statusCode != CG_HTTP_STATUS_OK) {*/
        if ( !mupnp_http_response_issuccessful(httpRes) ) {
		mupnp_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = mupnp_http_response_getcontent(httpRes);
	contentLen = mupnp_http_response_getcontentlength(httpRes);

	parseSuccess = mupnp_upnp_service_parsedescription(service, content, contentLen);

	mupnp_http_request_delete(httpReq);
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return parseSuccess;
}

/****************************************
* mupnp_upnp_service_getdescription
****************************************/

char *mupnp_upnp_service_getdescription(CgUpnpService *service, CgString *descStr)
{
	CgXmlNode *scpdNode;
	
	mupnp_log_debug_l4("Entering...\n");

	scpdNode = mupnp_upnp_service_getscpdnode(service);
	if (scpdNode != NULL) {
		mupnp_string_addvalue(descStr, CG_UPNP_XML_DECLARATION);
		mupnp_string_addvalue(descStr, "\n");
		mupnp_xml_node_tostring(scpdNode, TRUE, descStr);
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
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getidentifier(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	size_t len = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(serviceType);
	
	/* Find the next delimiter ':' character */
        for (tail = 0; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }

        /* Copy the part to a new buffer */
        if (tail > 0 && tail < len)
        {
		part = (char*) malloc(tail + 2);

		if ( NULL != part )
		{
			mupnp_strncpy(part, serviceType, tail + 1);
			part[tail + 1] = '\0';
		}
		else
			mupnp_log_debug("Memory allocation problem!\n");
        }
        else
        {
		part = NULL;
        }

	mupnp_log_debug_l4("Leaving...\n");
	
	return part;
}

/** 
 * Get the URN part of a service type string (usually "schemas-upnp-org") 
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_geturn(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	size_t len = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}

	len = mupnp_strlen(serviceType);
	
	/* Find the first delimiter ':' character */
	for (head = 0; head < len; head++)
	{
		if (serviceType[head] == ':')
		{
			head++; /* Don't include the colon */
			break;
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	mupnp_strncpy(part, serviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			mupnp_log_debug_s("Memory allocation problem!\n");
        }
        else
        {
		part = NULL;
        }

	mupnp_log_debug_l4("Leaving...\n");
        
	return part;
}

/** 
 * Get the service part of a service type string (usually just "service")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getservice(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	size_t len = 0;
	int count = 0;

	mupnp_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}

	len = mupnp_strlen(serviceType);
	
	/* Skip the first two delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (serviceType[head] == ':')
		{
                        if (count == 1)
                        {
				head++; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	mupnp_strncpy(part, serviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			mupnp_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
		part = NULL;
        }

	mupnp_log_debug_l4("Leaving...\n");
        
	return part;
}

/** 
 * Get the type part of a service type string (ex. "ContentDirectory")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_gettype(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	size_t len = 0;
	int count = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(serviceType);
	
	/* Skip the first three delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (serviceType[head] == ':')
		{
                        if (count == 2)
                        {
				head++; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }


        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail < len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	mupnp_strncpy(part, serviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			mupnp_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
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
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_servicetype_getschematype(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	size_t len = 0;
	int count = 0;
	
	if (serviceType == NULL)
	{
		return NULL;
	}
	
	mupnp_log_debug_l4("Entering...\n");

	len = mupnp_strlen(serviceType);
	
	/* Skip the first four delimiter ':' characters */
        for (tail = 0; tail < len; tail++)
        {
		if (serviceType[tail] == ':')
		{
                        if (count == 3)
                        {
				tail--; /* Don't include the colon */
				break;
                        }
                        else
                        {
				count++;
                        }
                }
        }	
	
        /* Copy the part to a new buffer */
        if (tail > 0 && tail < len)
        {
                part = (char*) malloc(tail + 2);

		if ( NULL != part )
		{
                	mupnp_strncpy(part, serviceType, tail + 1);
	                part[tail + 1] = '\0';
		}
		else
			mupnp_log_debug_s("Memory allocation problem!\n");
        }
	else
        {
		part = NULL;
        }

	mupnp_log_debug_l4("Leaving...\n");
	
        return part;
}

/** 
 * Get the version part of a service type string (ex. "1")
 *
 * @param serviceType A service type string (usually the result from
 *	  \ref mupnp_upnp_service_getservicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
const char* mupnp_upnp_servicetype_getversion(const char* serviceType)
{
	char* part = NULL;
	int tail = 0;
	int head = 0;
	size_t len = 0;
	int count = 0;

	mupnp_log_debug_l4("Entering...\n");

	if (serviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(serviceType);
	
        /* Skip the first four delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
                if (serviceType[head] == ':')
                {
                        if (count == 3)
                        {
				head++; /* Don't include the colon */
                                break;
                        }
                        else
                        {
                                count++;
                        }
                }
        }

	/* Find the next delimiter ':' character (or stop at the end) */
        for (tail = head; tail < len; tail++)
        {
                if (serviceType[tail] == ':')
                {
                        tail--; /* Don't take the colon */
                        break;
                }
        }
	
        /* Copy the part to a new buffer */
        if (head > 0 && head < len &&
            tail > 0 && tail <= len)
        {
                part = (char*) malloc(tail - head + 2);

		if ( NULL != part )
		{
                	mupnp_strncpy(part, serviceType + head, tail - head + 1);
	                part[tail - head + 1] = '\0';
		}
		else
			mupnp_log_debug_s("Memory allocation problem!\n");
        }
        else
        {
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

char *mupnp_upnp_service_getnotifyservicetypent(CgUpnpService *service, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_strncpy(buf, mupnp_upnp_service_getservicetype(service), bufSize);
	buf[bufSize-1]='\0';
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return buf;
}

char *mupnp_upnp_service_getnotifyservicetypeusn(CgUpnpService *service, char *buf, int bufSize)
{
	CgUpnpDevice *dev;

	mupnp_log_debug_l4("Entering...\n");

	dev = mupnp_upnp_service_getdevice(service);
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", mupnp_upnp_device_getudn(dev), mupnp_upnp_service_getservicetype(service));
#else
	sprintf(buf, "%s::%s", mupnp_upnp_device_getudn(dev), mupnp_upnp_service_getservicetype(service));
#endif
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return buf;
}

BOOL mupnp_upnp_service_announcefrom(CgUpnpService *service, const char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpDevice *rootDev;
	CgUpnpDevice *dev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult = TRUE;
	
	mupnp_log_debug_l4("Entering...\n");

	rootDev = mupnp_upnp_service_getrootdevice(service);
	dev = mupnp_upnp_service_getdevice(service);
	
	ssdpReq = mupnp_upnp_ssdprequest_new();
	mupnp_upnp_ssdprequest_setserver(ssdpReq, mupnp_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setleasetime(ssdpReq, mupnp_upnp_device_getleasetime(dev));
	mupnp_upnp_ssdprequest_setlocation(ssdpReq, mupnp_upnp_device_getlocationurl(rootDev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	
	ssdpSock = mupnp_upnp_ssdp_socket_new();
	sentResult = mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	mupnp_wait(20);
	mupnp_upnp_ssdp_socket_delete(ssdpSock);

	mupnp_upnp_ssdprequest_delete(ssdpReq);
	
	mupnp_log_debug_l4("Leaving...\n");

	return sentResult;
}

BOOL mupnp_upnp_service_byebyefrom(CgUpnpService *service, const char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
		
	mupnp_log_debug_l4("Entering...\n");

	ssdpReq = mupnp_upnp_ssdprequest_new();
	
	mupnp_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));

	ssdpSock = mupnp_upnp_ssdp_socket_new();
	sentResult = mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	mupnp_upnp_ssdp_socket_delete(ssdpSock);
	
	mupnp_upnp_ssdprequest_delete(ssdpReq);

	mupnp_log_debug_l4("Leaving...\n");

	return sentResult;
}


/****************************************
*
* Child Node
*
****************************************/

/****************************************
* mupnp_upnp_service_initchildnodes
****************************************/

static void mupnp_upnp_service_initchildnodes(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_service_initactionlist(service);
	mupnp_upnp_service_initservicestatetable(service);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_upnp_service_initactionlist
****************************************/

static void mupnp_upnp_service_initactionlist(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *actionListNode;
//	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpAction *action;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_actionlist_clear(service->actionList);
	
	scdpNode = mupnp_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	actionListNode = mupnp_xml_node_getchildnode(scdpNode, CG_UPNP_ACTIONLIST_ELEM_NAME);
	if (actionListNode == NULL)
		return;
		
	//serviceNode = mupnp_upnp_service_getservicenode(service);
	for (childNode = mupnp_xml_node_getchildnodes(actionListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
		if (mupnp_upnp_action_isactionnode(childNode) == FALSE)
			continue;
		action = mupnp_upnp_action_new();
		mupnp_upnp_action_setservice(action, service);
		mupnp_upnp_action_setactionnode(action, childNode);
		mupnp_upnp_actionlist_add(service->actionList, action);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_service_getactionbyname
****************************************/

CgUpnpAction *mupnp_upnp_service_getactionbyname(CgUpnpService *service, const char *name)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	actionList = mupnp_upnp_service_getactionlist(service);
	for (action=mupnp_upnp_actionlist_gets(actionList); action != NULL; action = mupnp_upnp_action_next(action)) {
		if (mupnp_upnp_action_isname(action, name) == TRUE)
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
* mupnp_upnp_service_initservicestatetable
****************************************/

static void mupnp_upnp_service_initservicestatetable(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *stateTableNode;
//	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpStateVariable *statVar;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_servicestatetable_clear(service->serviceStateTable);
	
	scdpNode = mupnp_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	stateTableNode = mupnp_xml_node_getchildnode(scdpNode, CG_UPNP_SERVICESTATETALBE_ELEM_NAME);
	if (stateTableNode == NULL)
		return;
		
//	serviceNode = mupnp_upnp_service_getservicenode(service);
	for (childNode = mupnp_xml_node_getchildnodes(stateTableNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
		if (mupnp_upnp_statevariable_isstatevariablenode(childNode) == FALSE)
			continue;
		statVar = mupnp_upnp_statevariable_new();
		mupnp_upnp_statevariable_setservice(statVar, service);
		mupnp_upnp_statevariable_setstatevariablenode(statVar, childNode);
		mupnp_upnp_servicestatetable_add(service->serviceStateTable, statVar);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_service_getstatevariablebyname
****************************************/

CgUpnpStateVariable *mupnp_upnp_service_getstatevariablebyname(CgUpnpService *service, const char *name)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	stateTable = mupnp_upnp_service_getservicestatetable(service);
	for (stateVar=mupnp_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = mupnp_upnp_statevariable_next(stateVar)) {
		if (mupnp_upnp_statevariable_isname(stateVar, name) == TRUE)
			return stateVar;
	}
	
	mupnp_log_debug_l4("Leaving...\n");
	
	return NULL;
}

/****************************************
* mupnp_upnp_service_setactionlistener
****************************************/

void mupnp_upnp_service_setactionlistener(CgUpnpService *service, CG_UPNP_ACTION_LISTNER actionListener)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	mupnp_log_debug_l4("Entering...\n");

	actionList = mupnp_upnp_service_getactionlist(service);
	for (action=mupnp_upnp_actionlist_gets(actionList); action != NULL; action = mupnp_upnp_action_next(action))
		mupnp_upnp_action_setlistner(action, actionListener);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_service_setquerylistener
****************************************/

void mupnp_upnp_service_setquerylistener(CgUpnpService *service, CG_UPNP_STATEVARIABLE_LISTNER queryListener)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	mupnp_log_debug_l4("Entering...\n");

	stateTable = mupnp_upnp_service_getservicestatetable(service);
	for (stateVar=mupnp_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = mupnp_upnp_statevariable_next(stateVar)) 
		mupnp_upnp_statevariable_setlistener(stateVar, queryListener);

	mupnp_log_debug_l4("Leaving...\n");
}

CgUpnpActionList *mupnp_upnp_service_getactionlist(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	return service->actionList;

	mupnp_log_debug_l4("Leaving...\n");
}

CgUpnpAction *mupnp_upnp_service_getactions(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	return mupnp_upnp_actionlist_gets(service->actionList);

	mupnp_log_debug_l4("Leaving...\n");
}

CgUpnpServiceStateTable *mupnp_upnp_service_getservicestatetable(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	return service->serviceStateTable;

	mupnp_log_debug_l4("Leaving...\n");
}

CgUpnpStateVariable *mupnp_upnp_service_getstatevariables(CgUpnpService *service)
{
	mupnp_log_debug_l4("Entering...\n");

#ifdef CG_OPTIMIZED_CP_MODE
	if (mupnp_upnp_service_isparsed(service) == FALSE)
		mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
	return mupnp_upnp_servicestatetable_gets(service->serviceStateTable);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
*	Subscription
*
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_service_addsubscriber
****************************************/

BOOL mupnp_upnp_service_addsubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_subscriberlist_add(service->subscriberList, sub);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_service_removesubscriber
****************************************/

BOOL mupnp_upnp_service_removesubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_subscriber_remove(sub);
	mupnp_upnp_subscriber_delete(sub);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_service_getsubscriberbysid
****************************************/

CgUpnpSubscriber *mupnp_upnp_service_getsubscriberbysid(CgUpnpService *service, const char *sid)
{
	CgUpnpSubscriber *sub;
	char *subSid;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(sid) <= 0)
		return NULL;
	
	if (0 <= mupnp_strstr(sid, CG_UPNP_ST_UUID_DEVICE))
		sid += mupnp_strlen(CG_UPNP_ST_UUID_DEVICE) + 1;

	for (sub = mupnp_upnp_service_getsubscribers(service); sub != NULL; sub = mupnp_upnp_subscriber_next(sub)) {
		subSid = mupnp_upnp_subscriber_getsid(sub);
		if (0 <= mupnp_strstr(subSid, CG_UPNP_ST_UUID_DEVICE))
			subSid += mupnp_strlen(CG_UPNP_ST_UUID_DEVICE) + 1;
		if (mupnp_streq(sid, subSid) == TRUE)
			return sub;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/* Private helper functions */

CgNetURL *mupnp_upnp_service_mangleabsoluteurl(const char *serviceURLStr, const char *baseURLStr, const char *locationURLStr)
{
	CgNetURL *absServiceURL;
	CgNetURL *serviceURL;
  char *basePath;
  
	if (mupnp_strlen(serviceURLStr) <= 0)
		return NULL;
    
	absServiceURL = mupnp_net_url_new();
    
	mupnp_net_url_set(absServiceURL, serviceURLStr);
    
	/* Absolute URL case */
	if (mupnp_net_url_isabsolute(absServiceURL) == TRUE) {
		mupnp_log_debug_s("Mangled URL: %s\n", mupnp_net_url_getrequest(absServiceURL));
		return absServiceURL;
	}
    
	/* URL base + absolute/relative path */
    if (0 < mupnp_strlen(baseURLStr)) {
        serviceURL = mupnp_net_url_new();
        mupnp_net_url_set(serviceURL, serviceURLStr);
            
        mupnp_net_url_set(absServiceURL, baseURLStr);
            
        if (mupnp_net_url_isabsolutepath(serviceURLStr) ) {
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
		
    if ( mupnp_net_url_isabsolutepath(serviceURLStr) ) {
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

static CgNetURL *mupnp_upnp_service_mangleurl(CgUpnpService *service, char *type)
{
  const char *serviceURLStr;
  const char *baseURLStr;
  const char *locationURLStr;
	CgUpnpDevice *rootDev;

	serviceURLStr = mupnp_xml_node_getchildnodevalue(mupnp_upnp_service_getservicenode(service), type);

	rootDev = mupnp_upnp_service_getrootdevice(service);
	if (rootDev != NULL) {
        baseURLStr = mupnp_upnp_device_geturlbase(rootDev);
        locationURLStr = mupnp_upnp_device_getlocationfromssdppacket(rootDev);
  }
  else {
      baseURLStr = NULL;
      locationURLStr = NULL;
  }
    
  return mupnp_upnp_service_mangleabsoluteurl(serviceURLStr, baseURLStr, locationURLStr);
}

#endif

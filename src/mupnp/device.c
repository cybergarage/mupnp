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
*	File: cdevice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo (aapo.makela@nokia.com)
*		- Added mupnp_upnp_device_getservicebysid().
*	10/31/05
*		- mupnp_upnp_device_getdevicebyname: Changed dev to childDev in for
*		- mupnp_upnp_device_byebyefrom & _announcefrom: Added missing 
*		  advertisement and byebye messages
*		- Added EXT handling to postsearchresponse
*	11/17/05
*		- Fixed many wrong variable names in for loops (dev => childDev)
*	01/09/06 Heikki Junnila
*		- Added mupnp_upnp_device_getservicebytype() to enable
*		  searching for services without version information.
*	10-Jan-06 Heikki Junnila
*		- Renamed mupnp_upnp_device_getservicebyname to
*		  mupnp_upnp_device_getservicebyexacttype and created a compat
*		  macro for _getservicebyname
*	11-Jan-06 Heikki Junnila
*		- Removed mupnp_upnp_device_isname and _getbyname because
*		  according to UPnP specs UDN, type or friendlyname is
*		  not the same as the device's name.
*		- Added mupnp_upnp_device_getdevicebyudn()
*	23-Jan-06 Aapo Makela
*		- Modified to try the next HTTP-port, if opening a port fails
*	05-May-06 Rosfran Borges
*		- Propagate some important data (description URI, lease-time), 
*		  from the root device, to the embededd sub-devices, because
*		  all embedded devices advertisements goes with wrong Location
*		  path.
*	06/13/07 Fabrice Fontaine Orange
*		- Fixed compilation issue when using DCG_UPNP_NOUSE_CONTROLPOINT flag in mupnp_upnp_device_updatefromssdppacket().
*		- Fixed a memory leak in mupnp_upnp_device_getservicebycontrolurl().
*	12/08/08
 *		- Added mupnp_upnp_device_getabsoluteiconurl().
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

static void mupnp_upnp_device_initchildnodes(mUpnpUpnpDevice *dev);
static void mupnp_upnp_device_initdevicelist(mUpnpUpnpDevice *dev);
static void mupnp_upnp_device_initservicelist(mUpnpUpnpDevice *dev);
static void mupnp_upnp_device_initiconlist(mUpnpUpnpDevice *dev);

static void mupnp_upnp_device_notifywait(mUpnpUpnpDevice *dev);

/****************************************
* mupnp_upnp_device_new
****************************************/

mUpnpUpnpDevice *mupnp_upnp_device_new()
{
	mUpnpUpnpDevice *dev;

	mupnp_log_debug_l4("Entering...\n");

	dev = (mUpnpUpnpDevice *)malloc(sizeof(mUpnpUpnpDevice));

	if  ( NULL != dev )
	{
		mupnp_list_node_init((mUpnpList *)dev);

#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS
		mupnp_http_persistentconnection_init();
#endif

		dev->rootNodeList = NULL;
		dev->deviceNode = NULL;

		dev->deviceList = mupnp_upnp_devicelist_new();
		dev->serviceList = mupnp_upnp_servicelist_new();
		dev->iconList = mupnp_upnp_iconlist_new();
		
		dev->mutex = mupnp_mutex_new();
		
		dev->httpServerList = mupnp_http_serverlist_new();
		dev->ssdpServerList = mupnp_upnp_ssdp_serverlist_new();
		
		dev->advertiser = mupnp_thread_new();
		
		dev->descriptionURI = mupnp_string_new();

		dev->ssdpPkt = mupnp_upnp_ssdp_packet_new();
		
		dev->ifCache = NULL;
		
		mupnp_upnp_device_setparentdevice(dev, NULL);
		mupnp_upnp_device_setdescriptionuri(dev, CG_UPNP_DEVICE_DEFAULT_DESCRIPTION_URI);
		mupnp_upnp_device_setleasetime(dev, CG_UPNP_DEVICE_DEFAULT_LEASE_TIME);
		mupnp_upnp_device_sethttpport(dev, CG_UPNP_DEVICE_HTTP_DEFAULT_PORT);
		mupnp_upnp_device_sethttplistener(dev, NULL);
		mupnp_upnp_device_setuserdata(dev, NULL);
    mupnp_upnp_device_setbootid(dev, mupnp_upnp_createbootid());
    mupnp_upnp_device_setpresentationlistener(dev, NULL);
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return dev;
}

/****************************************
* mupnp_upnp_device_delete
****************************************/

void mupnp_upnp_device_delete(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((mUpnpList *)dev);
	
	/* Delete cached interfaces */
	if (dev->ifCache != NULL)
		mupnp_net_interfacelist_delete(dev->ifCache);
	
	mupnp_upnp_device_clear(dev);
	
	mupnp_upnp_devicelist_delete(dev->deviceList);
	mupnp_upnp_servicelist_delete(dev->serviceList);
	mupnp_upnp_iconlist_delete(dev->iconList);

	mupnp_mutex_delete(dev->mutex);
	
	mupnp_http_serverlist_delete(dev->httpServerList);
	mupnp_upnp_ssdp_serverlist_delete(dev->ssdpServerList);
	mupnp_thread_delete(dev->advertiser);
	
	mupnp_string_delete(dev->descriptionURI);

	mupnp_upnp_ssdp_packet_delete(dev->ssdpPkt);
	
	free(dev);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_clear
****************************************/

void mupnp_upnp_device_clear(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	if (dev->rootNodeList != NULL) {
		mupnp_xml_nodelist_delete(dev->rootNodeList);
		dev->rootNodeList = NULL;
	}
	dev->deviceNode = NULL;
	
	mupnp_upnp_devicelist_clear(dev->deviceList);
	mupnp_upnp_servicelist_clear(dev->serviceList);
	mupnp_upnp_iconlist_clear(dev->iconList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_setdevicenode
****************************************/

void mupnp_upnp_device_setdevicenode(mUpnpUpnpDevice *dev, mUpnpXmlNode *node)
{
	mupnp_log_debug_l4("Entering...\n");

	dev->deviceNode = node;
	mupnp_upnp_device_initchildnodes(dev);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_getrootdevice
****************************************/

mUpnpUpnpDevice *mupnp_upnp_device_getrootdevice(mUpnpUpnpDevice *dev)
{
	mUpnpUpnpDevice *rootDev;
	
	mupnp_log_debug_l4("Entering...\n");

	rootDev = dev;
	while (mupnp_upnp_device_getparentdevice(rootDev) != NULL)
		rootDev = mupnp_upnp_device_getparentdevice(rootDev);
	
	mupnp_log_debug_l4("Leaving...\n");

	return rootDev;
}

/****************************************
* mupnp_upnp_device_parsedescription
****************************************/

BOOL mupnp_upnp_device_parsedescription(mUpnpUpnpDevice *dev, const char *desciption, size_t descriptionLen)
{
	mUpnpXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	mUpnpXmlNode *rootNode;
	char uuidBuf[CG_UPNP_UUID_MAX_LEN];
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_clear(dev);
	dev->rootNodeList = mupnp_xml_nodelist_new();

	xmlParser = mupnp_xml_parser_new();
	xmlParseSuccess = mupnp_xml_parse(xmlParser, dev->rootNodeList, desciption, descriptionLen);
	mupnp_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
		return FALSE;

	if (mupnp_xml_nodelist_size(dev->rootNodeList) <= 0) {
		mupnp_upnp_device_clear(dev);
		return FALSE;
	}

	rootNode = mupnp_upnp_device_getrootnode(dev);
	if (rootNode == NULL) {
		mupnp_upnp_device_clear(dev);
		return FALSE;
	}
		
	dev->deviceNode = mupnp_xml_node_getchildnode(rootNode, CG_UPNP_DEVICE_ELEM_NAME);
	if (dev->deviceNode == NULL) {
		mupnp_upnp_device_clear(dev);
		return FALSE;
	}

	if (mupnp_upnp_device_hasudn(dev) == FALSE)
		mupnp_upnp_device_setudn(dev, mupnp_upnp_createuuid(uuidBuf, sizeof(uuidBuf)));
	
	mupnp_upnp_device_initchildnodes(dev);
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_device_parsedescriptionurl
****************************************/

BOOL mupnp_upnp_device_parsedescriptionurl(mUpnpUpnpDevice *dev, mUpnpNetURL *url)
{
	char *host;
	int port;
	char *request;
	mUpnpHttpRequest *httpReq;
	mUpnpHttpResponse *httpRes;
	/*int statusCode;*/
	char *content;
	ssize_t contentLen;
	BOOL parseSuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = mupnp_net_url_gethost(url);
	port = mupnp_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	request = mupnp_net_url_getrequest(url);
	
	httpReq = mupnp_http_request_new();
	mupnp_http_request_setmethod(httpReq, CG_HTTP_GET);
	mupnp_http_request_seturi(httpReq, request);
	mupnp_http_request_setcontentlength(httpReq, 0);
	httpRes = mupnp_http_request_post(httpReq, host, port);

	/*statusCode = mupnp_http_response_getstatuscode(httpRes);
          if (statusCode != CG_HTTP_STATUS_OK) {*/
	if ( !mupnp_http_response_issuccessful(httpRes)) {
		mupnp_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = mupnp_http_response_getcontent(httpRes);
	contentLen = mupnp_http_response_getcontentlength(httpRes);

	parseSuccess = mupnp_upnp_device_parsedescription(dev, content, contentLen);

	mupnp_http_request_delete(httpReq);
	
	mupnp_log_debug_l4("Leaving...\n");

	return parseSuccess;
}

/****************************************
* mupnp_upnp_device_loaddescriptionfile
****************************************/

#if defined(CG_USE_CFILE)

BOOL mupnp_upnp_device_loaddescriptionfile(mUpnpUpnpDevice *dev, char *fileName)
{
	mUpnpFile *file;
	char *description;
	int descriptionLen;
	BOOL parseSuccess;

	mupnp_log_debug_l4("Entering...\n");

	file = mupnp_file_new();
	mupnp_file_setname(file, fileName);
	if (mupnp_file_load(file) == FALSE) {
		mupnp_file_delete(file);
		return FALSE;
	}

	description = mupnp_file_getcontent(file);
	descriptionLen = mupnp_strlen(description);

	parseSuccess = mupnp_upnp_device_parsedescription(dev, description, descriptionLen);

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
 * @return TRUE if the device was updated; otherwise FALSE
 */
BOOL mupnp_upnp_device_updatefromssdppacket(mUpnpUpnpDevice* dev,
					 mUpnpUpnpSSDPPacket* ssdpPkt)
{
	const char *usn = NULL;
	char udn[CG_UPNP_UDN_LEN_MAX];
	mUpnpUpnpSSDPPacket *tmpSsdpPkt = NULL;
	const char* oldLocation = NULL;
	const char* newLocation = NULL;
	mUpnpNetURL *url = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	usn = mupnp_upnp_ssdp_packet_getusn(ssdpPkt);
	mupnp_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	tmpSsdpPkt = mupnp_upnp_device_getssdppacket(dev);
	if (tmpSsdpPkt == NULL)
	{
		return FALSE;
	}
		
	/* Here we check, if the location of the device has changed */
	oldLocation = mupnp_upnp_ssdp_packet_getlocation(tmpSsdpPkt);
	newLocation = mupnp_upnp_ssdp_packet_getlocation(ssdpPkt);

	if (mupnp_streq(oldLocation, newLocation) == TRUE)
	{
		/* The device's location has not changed, just update
		   the SSDP packet */
		mupnp_upnp_device_setssdppacket(dev, ssdpPkt);
		
		return TRUE;
	}
	else
	{
		/* The device's location HAS changed. We must get a new
		   description. */
		mupnp_upnp_device_setssdppacket(dev, ssdpPkt);
		
		url = mupnp_net_url_new();
		if (url == NULL)
		{
			return FALSE;
		}
		
		/* Use the new location as the description URL */
		mupnp_net_url_set(url, newLocation);
		
		/* Get a new description for the device */
		mupnp_upnp_device_parsedescriptionurl(dev, url);
		
		mupnp_net_url_delete(url);
		
/* ADD Fabrice Fontaine Orange 16/04/2007 */
/* Bug correction : Solving compilation issue when using DCG_UPNP_NOUSE_CONTROLPOINT flag */
#ifndef CG_UPNP_NOUSE_CONTROLPOINT
/* ADD END Fabrice Fontaine Orange 16/04/2007 */
#ifndef CG_OPTIMIZED_CP_MODE
		if (mupnp_upnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == FALSE)
		{
			mupnp_upnp_device_delete(dev);
			return FALSE;
		}
#endif
/* ADD Fabrice Fontaine Orange 16/04/2007 */
#endif
/* ADD END Fabrice Fontaine Orange 16/04/2007 */
	}

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/*****************************************************************************
 * Device Type
 *****************************************************************************/

/** 
 * Get the identifier-part of a device type string (usually "urn") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getidentifier(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t len = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(deviceType);
	
	/* Find the next delimiter ':' character */
        for (tail = 0; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
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
			mupnp_strncpy(part, deviceType, tail + 1);
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
 * Get the URN part of a device type string (usually "schemas-upnp-org") 
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_geturn(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t head = 0;
	size_t len = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}

	len = mupnp_strlen(deviceType);
	
	/* Find the first delimiter ':' character */
	for (head = 0; head < len; head++)
	{
		if (deviceType[head] == ':')
		{
			head++; /* Don't include the colon */
			break;
                }
        }

        /* Find the next delimiter ':' character */
        for (tail = head; tail < len; tail++)
        {
                if (deviceType[tail] == ':')
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
                	mupnp_strncpy(part, deviceType + head, tail - head + 1);
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
 * Get the device part of a device type string (usually just "device")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getdevice(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t head = 0;
	size_t len = 0;
	size_t count = 0;

	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}

	len = mupnp_strlen(deviceType);
	
	/* Skip the first two delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (deviceType[head] == ':')
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
                if (deviceType[tail] == ':')
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
                	mupnp_strncpy(part, deviceType + head, tail - head + 1);
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
 * Get the type part of a device type string (ex. "ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_gettype(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t head = 0;
	size_t len = 0;
	size_t count = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(deviceType);
	
	/* Skip the first three delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
		if (deviceType[head] == ':')
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
                if (deviceType[tail] == ':')
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
                	mupnp_strncpy(part, deviceType + head, tail - head + 1);
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
 * Get the schema type part of a device type string (without last colon)
 * (ex. "urn:schemas-upnp-org:device:ContentDirectory")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getschematype(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t len = 0;
	size_t count = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(deviceType);
	
	/* Skip the first four delimiter ':' characters */
        for (tail = 0; tail < len; tail++)
        {
		if (deviceType[tail] == ':')
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
                	mupnp_strncpy(part, deviceType, tail + 1);
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
 * Get the version part of a device type string (ex. "1")
 *
 * @param deviceType A device type string (usually the result from
 *	  \ref mupnp_upnp_device_getdevicetype)
 *
 * @return A newly-created char* if successful; otherwise NULL
 */
char* mupnp_upnp_devicetype_getversion(const char* deviceType)
{
	char* part = NULL;
	size_t tail = 0;
	size_t head = 0;
	size_t len = 0;
	size_t count = 0;

	mupnp_log_debug_l4("Entering...\n");

	if (deviceType == NULL)
	{
		return NULL;
	}
	
	len = mupnp_strlen(deviceType);
	
        /* Skip the first four delimiter ':' characters */
        for (head = 0; head < len; head++)
        {
                if (deviceType[head] == ':')
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
                if (deviceType[tail] == ':')
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
                	mupnp_strncpy(part, deviceType + head, tail - head + 1);
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
* Child Node
*
****************************************/

/****************************************
* mupnp_upnp_device_initchildnodes
****************************************/

static void mupnp_upnp_device_initchildnodes(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_initdevicelist(dev);
	mupnp_upnp_device_initservicelist(dev);
	mupnp_upnp_device_initiconlist(dev);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
*
* Embedded Device
*
****************************************/

/****************************************
* mupnp_upnp_device_initdevicelist
****************************************/

static void mupnp_upnp_device_initdevicelist(mUpnpUpnpDevice *dev)
{
	mUpnpXmlNode *devNode;
	mUpnpXmlNode *devListNode;
	mUpnpXmlNode *childNode;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_devicelist_clear(dev->deviceList);
	
	devNode = mupnp_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	devListNode = mupnp_xml_node_getchildnode(devNode, CG_UPNP_DEVICELIST_ELEM_NAME);
	if (devListNode == NULL)
		return;
	
	for (childNode = mupnp_xml_node_getchildnodes(devListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
		if (mupnp_upnp_device_isdevicenode(childNode) == FALSE)
			continue;
		childDev = mupnp_upnp_device_new();
		/* Propagate "lease-time" from parent device to the child-devices; call "setleasetime"
		   (line below added by: rborges) */
		mupnp_upnp_device_setleasetime(childDev, mupnp_upnp_device_getleasetime(dev));
                /* Propagate important data (description URI, HTTP port) from the parent
	         * device to the child-devices; call "setdescriptionuri" and 
		 * "sethttpport" - (rosfran.borges) */
		mupnp_upnp_device_setdescriptionuri(childDev, mupnp_upnp_device_getdescriptionuri(dev));
		mupnp_upnp_device_sethttpport(childDev, mupnp_upnp_device_gethttpport(dev));		
		mupnp_upnp_device_setdevicenode(childDev, childNode);
		mupnp_upnp_devicelist_add(dev->deviceList, childDev);
		mupnp_upnp_device_setparentdevice(childDev, dev);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}

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
mUpnpUpnpDevice *mupnp_upnp_device_getdevicebytype(mUpnpUpnpDevice *dev, char *type)
{
	mUpnpUpnpDevice *childDev = NULL;
	mUpnpUpnpDevice *moreChildDev = NULL;
	const char* typeString = NULL;
	char* part = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(type) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = mupnp_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		typeString = mupnp_upnp_device_getdevicetype(childDev);
		if (typeString != NULL)
		{
			part = mupnp_upnp_devicetype_getschematype(typeString);
			if (mupnp_strcmp(part, type) == 0)
			{
				free(part);
				return childDev;
			}
			else
			{
				free(part);
			}
		}
				
		moreChildDev = mupnp_upnp_device_getdevicebytype(childDev, type);
		if (moreChildDev != NULL)
		{
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
 * disregard the version number, use \ref mupnp_upnp_device_getdevicebytype
 *
 * \param dev Device in question
 * \param exacttype Type of the device
 * 
 */
mUpnpUpnpDevice* mupnp_upnp_device_getdevicebyexacttype(mUpnpUpnpDevice* dev,
						  char* exacttype)
{
	mUpnpUpnpDevice *childDev = NULL;
	mUpnpUpnpDevice *moreChildDev = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(exacttype) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = mupnp_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		if (mupnp_strcmp(mupnp_upnp_device_getdevicetype(childDev),
			      exacttype) == 0)
		{
			return childDev;
		}

		moreChildDev = mupnp_upnp_device_getdevicebyexacttype(childDev, 
								   exacttype);
		if (moreChildDev != NULL)
		{
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
mUpnpUpnpDevice *mupnp_upnp_device_getdevicebyudn(mUpnpUpnpDevice *dev, char *udn)
{
	mUpnpUpnpDevice *childDev = NULL;
	mUpnpUpnpDevice *moreChildDev = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(udn) <= 0 || dev == NULL)
	{
		return NULL;
	}
	
	for (childDev = mupnp_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		if (mupnp_strcmp(mupnp_upnp_device_getudn(childDev), udn) == 0)
		{
			return childDev;
		}

		moreChildDev = mupnp_upnp_device_getdevicebyudn(childDev, udn);
		if (moreChildDev != NULL)
		{
			return moreChildDev;
		}
	}

	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* mupnp_upnp_device_getdevicebydescriptionuri
****************************************/

mUpnpUpnpDevice *mupnp_upnp_device_getdevicebydescriptionuri(mUpnpUpnpDevice *dev, char *url)
{
	mUpnpUpnpDevice *childDev;
	mUpnpUpnpDevice *moreChildDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(url) <= 0)
		return NULL;
			
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		if (mupnp_upnp_device_isdescriptionuri(dev, url) == TRUE)
			return dev;
		moreChildDev = mupnp_upnp_device_getdevicebydescriptionuri(childDev, url);
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
* mupnp_upnp_device_notifywait
****************************************/

static void mupnp_upnp_device_notifywait(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_waitrandom(CG_UPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_getlocationurl
****************************************/

const char *mupnp_upnp_device_getlocationurl(mUpnpUpnpDevice *dev, const char *host, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_net_gethosturl(
					host,
					mupnp_upnp_device_gethttpport(dev),
					mupnp_upnp_device_getdescriptionuri(dev),
					buf,
					bufSize);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_getnotifydevicent
****************************************/

char *mupnp_upnp_device_getnotifydevicent(mUpnpUpnpDevice *dev, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_device_isrootdevice(dev) == FALSE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", mupnp_upnp_device_getudn(dev));
#else
		mupnp_strcpy(buf, mupnp_upnp_device_getudn(dev));
#endif
	} else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		mupnp_strcpy(buf, CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	mupnp_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* mupnp_upnp_device_getnotifydeviceusn
****************************************/

char *mupnp_upnp_device_getnotifydeviceusn(mUpnpUpnpDevice *dev, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_device_isrootdevice(dev) == TRUE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s::%s", mupnp_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		sprintf(buf, "%s::%s", mupnp_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", mupnp_upnp_device_getudn(dev));
#else
		sprintf(buf, "%s", mupnp_upnp_device_getudn(dev));
#endif
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* mupnp_upnp_device_getnotifydevicetypent
****************************************/

char *mupnp_upnp_device_getnotifydevicetypent(mUpnpUpnpDevice *dev, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s", mupnp_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s", mupnp_upnp_device_getdevicetype(dev));
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return buf;
}

/****************************************
* mupnp_upnp_device_getnotifydevicetypeusn
****************************************/

char *mupnp_upnp_device_getnotifydevicetypeusn(mUpnpUpnpDevice *dev, char *buf, int bufSize)
{
	mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", mupnp_upnp_device_getudn(dev), mupnp_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s::%s", mupnp_upnp_device_getudn(dev), mupnp_upnp_device_getdevicetype(dev));
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return buf;
}	

/****************************************
* mupnp_upnp_device_announcefrom
****************************************/

BOOL mupnp_upnp_device_announcefrom(mUpnpUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	mUpnpUpnpServiceList *serviceList;
	mUpnpUpnpService *service;
	mUpnpUpnpDeviceList *devList;
	mUpnpUpnpDevice *childDev;
	mUpnpUpnpSSDPRequest *ssdpReq;
	mUpnpUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
	
	mupnp_log_debug_l4("Entering...\n");
	mupnp_log_debug_s("Announcing from %s\n", bindAddr);
	
	ssdpSock = mupnp_upnp_ssdp_socket_new();
	ssdpReq = mupnp_upnp_ssdprequest_new();
	
	mupnp_upnp_ssdprequest_setserver(ssdpReq, mupnp_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setleasetime(ssdpReq, mupnp_upnp_device_getleasetime(dev));
	mupnp_upnp_ssdprequest_setlocation(ssdpReq, mupnp_upnp_device_getlocationurl(dev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	mupnp_upnp_ssdprequest_setbootid(ssdpReq, mupnp_upnp_device_getbootid(dev));

	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (mupnp_upnp_device_isrootdevice(dev) == TRUE) {
		mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
		mupnp_wait(20);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	mupnp_wait(20);

	/**** root or embedded device UUID ****/
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getudn(dev));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getudn(dev));
	sentResult = mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	mupnp_wait(20);
	
	mupnp_upnp_ssdprequest_delete(ssdpReq);
	mupnp_upnp_ssdp_socket_close(ssdpSock);
	mupnp_upnp_ssdp_socket_delete(ssdpSock);

	/**** child services ****/
	serviceList = mupnp_upnp_device_getservicelist(dev);
	for (service=mupnp_upnp_servicelist_gets(serviceList); service != NULL; service = mupnp_upnp_service_next(service))
		mupnp_upnp_service_announcefrom(service, bindAddr);
	
	/**** child deveices ****/
	devList = mupnp_upnp_device_getdevicelist(dev);
	for (childDev = mupnp_upnp_devicelist_gets(devList); childDev != NULL; childDev = mupnp_upnp_device_next(childDev))
	{
		mupnp_upnp_device_announcefrom(childDev, bindAddr);
	}
		
	mupnp_log_debug_l4("Leaving...\n");

	return sentResult;
}

/****************************************
* mupnp_upnp_device_announce
****************************************/

void mupnp_upnp_device_announce(mUpnpUpnpDevice *dev)
{
	mUpnpNetworkInterfaceList *netIfList;
	mUpnpNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_notifywait(dev);
	
	ssdpCount = mupnp_upnp_ssdp_getannouncecount();
	
	netIfList = mupnp_net_interfacelist_new();
	mupnp_net_gethostinterfaces(netIfList);
	for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
		bindAddr = mupnp_net_interface_getaddress(netIf);
		if (mupnp_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
		{
			mupnp_upnp_device_announcefrom(dev, bindAddr);
		}
	}

	mupnp_net_interfacelist_delete(netIfList);

	mupnp_log_debug_l4("Leaving...\n");
}
	
/****************************************
* mupnp_upnp_device_byebyefrom
****************************************/

BOOL mupnp_upnp_device_byebyefrom(mUpnpUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	mUpnpUpnpServiceList *serviceList;
	mUpnpUpnpService *service;
	mUpnpUpnpDeviceList *devList;
	mUpnpUpnpDevice *childDev;
	mUpnpUpnpSSDPRequest *ssdpReq;
	mUpnpUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;

	mupnp_log_debug_l4("Entering...\n");

	ssdpSock = mupnp_upnp_ssdp_socket_new();
	ssdpReq = mupnp_upnp_ssdprequest_new();
	
	mupnp_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);

	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (mupnp_upnp_device_isrootdevice(dev) == TRUE) {
		mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	/**** root or embedded device UUID ****/
	mupnp_upnp_ssdprequest_setnt(ssdpReq, mupnp_upnp_device_getudn(dev));
	mupnp_upnp_ssdprequest_setusn(ssdpReq, mupnp_upnp_device_getudn(dev));
	sentResult = mupnp_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	mupnp_upnp_ssdprequest_delete(ssdpReq);
	mupnp_upnp_ssdp_socket_close(ssdpSock);
	mupnp_upnp_ssdp_socket_delete(ssdpSock);

	serviceList = mupnp_upnp_device_getservicelist(dev);
	for (service=mupnp_upnp_servicelist_gets(serviceList); service != NULL; service = mupnp_upnp_service_next(service))
		mupnp_upnp_service_byebyefrom(service, bindAddr);
	
	devList = mupnp_upnp_device_getdevicelist(dev);
	for (childDev = mupnp_upnp_devicelist_gets(devList); childDev != NULL; childDev = mupnp_upnp_device_next(childDev))
		mupnp_upnp_device_byebyefrom(childDev, bindAddr);

	mupnp_log_debug_l4("Leaving...\n");

	return sentResult;
}

/****************************************
* mupnp_upnp_device_byebye
****************************************/

void mupnp_upnp_device_byebye(mUpnpUpnpDevice *dev)
{
	mUpnpNetworkInterfaceList *netIfList;
	mUpnpNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_notifywait(dev);
	
	ssdpCount = mupnp_upnp_ssdp_getannouncecount();
	
	netIfList = mupnp_net_interfacelist_new();
	mupnp_net_gethostinterfaces(netIfList);
	for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
		bindAddr = mupnp_net_interface_getaddress(netIf);
		if (mupnp_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
			mupnp_upnp_device_byebyefrom(dev, bindAddr);
	}

	mupnp_net_interfacelist_delete(netIfList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_postsearchresponse
****************************************/

BOOL mupnp_upnp_device_postsearchresponse(mUpnpUpnpDevice *dev, mUpnpUpnpSSDPPacket *ssdpPkt, const char *st, const char *usn)
{
	mUpnpUpnpDevice *rootDev;
	mUpnpUpnpSSDPResponse *ssdpRes;
	char httpDateStr[CG_HTTP_DATE_MAXLEN];
	char *localAddr;
	char *remoteAddr;
	int remotePort;
	char rootDevLocation[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serverBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	int ssdpCount;
	mUpnpUpnpSSDPSocket *ssdpSock;
	int n;
	
	mupnp_log_debug_l4("Entering...\n");

	localAddr = mupnp_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
	mupnp_log_debug_s("Local address: <%s>\n", localAddr);
	rootDev = mupnp_upnp_device_getrootdevice(dev);

	mupnp_upnp_device_getlocationurl(rootDev, localAddr, rootDevLocation, sizeof(rootDevLocation));

	ssdpRes = mupnp_upnp_ssdpresponse_new();
	mupnp_upnp_ssdpresponse_setleasetime(ssdpRes, mupnp_upnp_device_getleasetime(dev));
	mupnp_upnp_ssdpresponse_setdate(ssdpRes, mupnp_http_getdate(mupnp_getcurrentsystemtime(), httpDateStr, sizeof(httpDateStr)));
	mupnp_upnp_ssdpresponse_setst(ssdpRes, st);
	mupnp_upnp_ssdpresponse_setbootid(ssdpRes, mupnp_upnp_device_getbootid(dev));

	mupnp_upnp_ssdpresponse_setext(ssdpRes);
	mupnp_upnp_getservername(serverBuf, CG_UPNP_SSDP_HEADER_LINE_MAXSIZE);
	mupnp_upnp_ssdpresponse_setserver(ssdpRes, serverBuf);

	mupnp_upnp_ssdpresponse_setusn(ssdpRes, usn);
	mupnp_upnp_ssdpresponse_setlocation(ssdpRes, rootDevLocation);

	remoteAddr = mupnp_upnp_ssdp_packet_getremoteaddress(ssdpPkt);
	remotePort = mupnp_upnp_ssdp_packet_getremoteport(ssdpPkt);
	mupnp_log_debug_s("Remote address: <%s>\n", remoteAddr);
	ssdpCount = mupnp_upnp_ssdp_getannouncecount();

	ssdpSock = mupnp_upnp_ssdp_socket_new();

	for (n=0; n<ssdpCount; n++) {
		mupnp_waitrandom(20);
		mupnp_upnp_ssdp_socket_postresponse(ssdpSock, ssdpRes, remoteAddr, remotePort);
	}
	
	mupnp_upnp_ssdp_socket_delete(ssdpSock);

	mupnp_upnp_ssdpresponse_delete(ssdpRes);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
*
* Start/Stop
*
****************************************/

/****************************************
* mupnp_upnp_device_start
****************************************/

BOOL mupnp_upnp_device_start(mUpnpUpnpDevice *dev)
{
	CG_HTTP_LISTENER httpListener;
	int httpPort;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_stop(dev);

	/* Create interface cache, if it does not exist and cache current */
	if (dev->ifCache == NULL)
		dev->ifCache = mupnp_net_interfacelist_new();
	mupnp_net_gethostinterfaces(dev->ifCache);
	
	/**** HTTP Server ****/
	httpPort = mupnp_upnp_device_gethttpport(dev);
	/* Opening HTTP server may fail, so try many ports */
	while(mupnp_http_serverlist_open(dev->httpServerList, httpPort) == FALSE)
	{
		mupnp_upnp_device_sethttpport(dev, httpPort + 1);
		httpPort = mupnp_upnp_device_gethttpport(dev);
	}
	mupnp_http_serverlist_setuserdata(dev->httpServerList, dev);
	httpListener = mupnp_upnp_device_gethttplistener(dev);
	if (httpListener == NULL)
		httpListener = mupnp_upnp_device_httprequestrecieved;
	mupnp_http_serverlist_setlistener(dev->httpServerList, httpListener);
	mupnp_http_serverlist_start(dev->httpServerList);

	/**** SSDP Server ****/
	if (mupnp_upnp_ssdp_serverlist_open(dev->ssdpServerList) == FALSE)
		return FALSE;
  mupnp_upnp_ssdp_serverlist_setlistener(dev->ssdpServerList, mupnp_upnp_device_ssdplistener);
	mupnp_upnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, dev);
	mupnp_upnp_ssdp_serverlist_start(dev->ssdpServerList);

	/**** Update BootId ****/
  mupnp_upnp_device_setbootid(dev, mupnp_upnp_createbootid());

	/**** Announce ****/
	mupnp_upnp_device_announce(dev);
	
	/**** Advertiser ****/
	mupnp_upnp_device_advertiser_start(dev);	

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}
	
/****************************************
* mupnp_upnp_device_ipchanged
****************************************/

BOOL mupnp_upnp_device_ipchanged(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_log_debug_l4("Leaving...\n");

	return FALSE;
}

/****************************************
* mupnp_upnp_device_stop
****************************************/

BOOL mupnp_upnp_device_stop(mUpnpUpnpDevice *dev)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_byebye(dev);
	
	/**** Advertiser ****/
	if (mupnp_upnp_device_advertiser_isrunning(dev))
		mupnp_upnp_device_advertiser_stop(dev);	

	/**** HTTP Server ****/
	if (0 < mupnp_http_headerlist_size(dev->httpServerList)) {
    mupnp_http_serverlist_setlistener(dev->httpServerList, NULL);
		mupnp_http_serverlist_stop(dev->httpServerList);
		mupnp_http_serverlist_close(dev->httpServerList);
		mupnp_http_serverlist_clear(dev->httpServerList);
	}
	
	/**** SSDP Server ****/
	if (0 < mupnp_upnp_ssdp_serverlist_size(dev->ssdpServerList)) {
    mupnp_upnp_ssdp_serverlist_setlistener(dev->ssdpServerList, NULL);
    mupnp_upnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, NULL);
		mupnp_upnp_ssdp_serverlist_stop(dev->ssdpServerList);
		mupnp_upnp_ssdp_serverlist_close(dev->ssdpServerList);
		mupnp_upnp_ssdp_serverlist_clear(dev->ssdpServerList);
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
 * mupnp_upnp_device_isrunning
 ****************************************/

BOOL mupnp_upnp_device_isrunning(mUpnpUpnpDevice *dev)
{
	return mupnp_upnp_device_advertiser_isrunning(dev);
}

/****************************************
*
* Embedded Service
*
****************************************/

/****************************************
* mupnp_upnp_device_initservicelist
****************************************/

static void mupnp_upnp_device_initservicelist(mUpnpUpnpDevice *dev)
{
	mUpnpXmlNode *devNode;
	mUpnpXmlNode *serviceListNode;
	mUpnpXmlNode *childNode;
	mUpnpUpnpService *childService;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_servicelist_clear(dev->serviceList);
	
	devNode = mupnp_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	serviceListNode = mupnp_xml_node_getchildnode(devNode, CG_UPNP_SERVICELIST_ELEM_NAME);
	if (serviceListNode == NULL)
		return;
		
	for (childNode = mupnp_xml_node_getchildnodes(serviceListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
		if (mupnp_upnp_service_isservicenode(childNode) == FALSE)
			continue;
		childService = mupnp_upnp_service_new();
		mupnp_upnp_service_setservicenode(childService, childNode);
		mupnp_upnp_servicelist_add(dev->serviceList, childService);
		mupnp_upnp_service_setdevice(childService, dev);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}


mUpnpUpnpService *mupnp_upnp_device_getservicebyserviceid(mUpnpUpnpDevice *dev, const char *serviceId)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(serviceId) <= 0 || dev == NULL)
	{
		return NULL;
	}

	for (service = mupnp_upnp_device_getservices(dev);
	     service != NULL;
	     service = mupnp_upnp_service_next(service))
	{
		if (mupnp_strcmp(mupnp_upnp_service_getserviceid(service),
			      serviceId) == 0)
		{
#ifdef CG_OPTIMIZED_CP_MODE
			if (mupnp_upnp_service_isparsed(service) == FALSE)
				mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
			return service;
		}
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev);
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		service = mupnp_upnp_device_getservicebyserviceid(childDev, serviceId);
		if (service != NULL)
		{
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
 * know the version of a service, use \ref mupnp_upnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 * 
 */
mUpnpUpnpService* mupnp_upnp_device_getservicebyexacttype(mUpnpUpnpDevice* dev, const char* type)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(type) <= 0 || dev == NULL)
	{
		return NULL;
	}

	for (service = mupnp_upnp_device_getservices(dev);
	     service != NULL;
	     service = mupnp_upnp_service_next(service))
	{
		if (mupnp_strcmp(mupnp_upnp_service_getservicetype(service),
			      type) == 0)
		{
#ifdef CG_OPTIMIZED_CP_MODE
		if (mupnp_upnp_service_isparsed(service) == FALSE)
			mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
		return service;
		}
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev);
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		service = mupnp_upnp_device_getservicebyexacttype(childDev, type);
		if (service != NULL)
		{
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
 * to know the version of a service, use \ref mupnp_upnp_servicetype_getversion
 *
 * \param dev Device in question
 * \param type Type of the service
 *
 */
mUpnpUpnpService *mupnp_upnp_device_getservicebytype(mUpnpUpnpDevice *dev, const char *type)
{
	mUpnpUpnpService *service = NULL;
	mUpnpUpnpDevice *childDev = NULL;
	const char* typeString = NULL;
	char* part = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(type) <= 0)
	{
		return NULL;
	}

	for (service = mupnp_upnp_device_getservices(dev);
	     service != NULL;
	     service = mupnp_upnp_service_next(service))
	{
		typeString = mupnp_upnp_service_getservicetype(service);
		if (typeString != NULL)
		{
			if (mupnp_strcmp(typeString, type) == 0)
				return service;
			part = mupnp_upnp_servicetype_getschematype(typeString);
			if (mupnp_strcmp(part, type) == 0)
			{
				free(part);
#ifdef CG_OPTIMIZED_CP_MODE
				if (mupnp_upnp_service_isparsed(service) == FALSE)
					mupnp_upnp_controlpoint_parsescservicescpd(service);
#endif
				return service;
			}
			else
			{
				free(part);
			}
		}
	}

	for (childDev = mupnp_upnp_device_getdevices(dev); 
	     childDev != NULL;
	     childDev = mupnp_upnp_device_next(childDev))
	{
		service = mupnp_upnp_device_getservicebytype(childDev, type);
		if (service != NULL)
		{
			return service;
		}
	}

	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* mupnp_upnp_device_getservicebyscpdurl
****************************************/

mUpnpUpnpService *mupnp_upnp_device_getservicebyscpdurl(mUpnpUpnpDevice *dev, const char *url)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(url) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		mupnp_log_debug_s("Child node v: %s\n", mupnp_xml_node_getchildnodevalue(mupnp_upnp_service_getservicenode(service), CG_UPNP_SERVICE_SCPDURL));
		if (mupnp_upnp_service_isscpdurl(service, url) == TRUE)
			return service;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		service = mupnp_upnp_device_getservicebyscpdurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* mupnp_upnp_device_getservicebycontrolurl
****************************************/

mUpnpUpnpService *mupnp_upnp_device_getservicebycontrolurl(mUpnpUpnpDevice *dev, const char *url)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	mUpnpNetURL* service_url;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(url) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		/* mupnp_log_debug_s("<%s> == <%s> ?\n", url, mupnp_net_url_getrequest(mupnp_upnp_service_getcontrolurl(service))); */
		/* MODIFICATION Fabrice Fontaine Orange 23/04/07
		if (mupnp_strstr(mupnp_net_url_getrequest(mupnp_upnp_service_getcontrolurl(service)), url) != -1)*/
		/* Memory leak correction : mupnp_upnp_service_getcontrolurl return a malloc */
		/* structure, this structure must be freed after use */
		service_url=mupnp_upnp_service_getcontrolurl(service);
		if (service_url) {
			if (mupnp_strstr(mupnp_net_url_getrequest(service_url), url) != -1)
			{
				mupnp_net_url_delete(service_url);
			/* MODIFICATION END Fabrice Fontaine Orange 23/04/07 */
				return service;
			/* ADD Fabrice Fontaine Orange 23/04/07 */
			}
			mupnp_net_url_delete(service_url);
		}
		/* ADD END Fabrice Fontaine Orange 23/04/07 */	
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		service = mupnp_upnp_device_getservicebycontrolurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* mupnp_upnp_device_getservicebysid
****************************************/

mUpnpUpnpService *mupnp_upnp_device_getservicebysid(mUpnpUpnpDevice *dev, const char *sid)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(sid) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_upnp_service_getsubscriberbysid(service, sid) != NULL)
		{
			return service;
		} else if (mupnp_upnp_service_issubscribed(service) == TRUE &&
			   mupnp_strcmp(mupnp_upnp_service_getsubscriptionsid(service), sid) == 0)
		{
			return service;
		}
			
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		service = mupnp_upnp_device_getservicebysid(childDev, sid);
		if (service != NULL)
			return service;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
* mupnp_upnp_device_setactionlistener
****************************************/

void mupnp_upnp_device_setactionlistener(mUpnpUpnpDevice *dev, CG_UPNP_ACTION_LISTNER actionListner)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service))
		mupnp_upnp_service_setactionlistener(service, actionListner);
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev))
		mupnp_upnp_device_setactionlistener(childDev, actionListner);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_setquerylistener
****************************************/

void mupnp_upnp_device_setquerylistener(mUpnpUpnpDevice *dev, CG_UPNP_STATEVARIABLE_LISTNER queryListner)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service))
		mupnp_upnp_service_setquerylistener(service, queryListner);
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev))
		mupnp_upnp_device_setquerylistener(childDev, queryListner);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_device_getservicebyeventsuburl
****************************************/

mUpnpUpnpService *mupnp_upnp_device_getservicebyeventsuburl(mUpnpUpnpDevice *dev, const char *url)
{
	mUpnpUpnpService *service;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(url) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		if (mupnp_streq(mupnp_net_url_getpath(mupnp_upnp_service_geteventsuburl(service)), url) == TRUE)
			return service;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		service = mupnp_upnp_device_getservicebyeventsuburl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	mupnp_log_debug_l4("Leaving...\n");

	return NULL;
}

/****************************************
 * mupnp_upnp_device_getsmallesticonbymimetype
 ****************************************/

mUpnpUpnpIcon *mupnp_upnp_device_getsmallesticonbymimetype(mUpnpUpnpDevice *dev, const char *mimeType)
{
	mUpnpUpnpIcon *icon;
	mUpnpUpnpIcon *smallestIcon;
	
	smallestIcon = NULL;
	for (icon = mupnp_upnp_device_geticons(dev); icon; icon = mupnp_upnp_icon_next(icon)) {
		if (0 < mupnp_strlen(mimeType)) {
			if (!mupnp_streq(mupnp_upnp_icon_getmimetype(icon), mimeType))
				continue;
		}
		if (!smallestIcon) {
			smallestIcon = icon;
			continue;
		}
		if (mupnp_upnp_icon_getwidth(icon) < mupnp_upnp_icon_getwidth(smallestIcon))
			smallestIcon = icon;			
	}
	
	return smallestIcon;
}

/****************************************
* mupnp_upnp_device_getsmallesticon
****************************************/
				
mUpnpUpnpIcon *mupnp_upnp_device_getsmallesticon(mUpnpUpnpDevice *dev)
{
	return mupnp_upnp_device_getsmallesticonbymimetype(dev, "");
}
				
/****************************************
 * mupnp_upnp_device_getabsoluteiconurl
 ****************************************/

BOOL mupnp_upnp_device_getabsoluteiconurl(mUpnpUpnpDevice *dev, mUpnpUpnpIcon *icon, mUpnpString *buf)
{
	mUpnpNetURI *uri;
	mUpnpNetURI *ssdpUri;
	mUpnpUpnpDevice *rootDev;
	const char *ssdplocation;
	
	uri = mupnp_net_uri_new();

	mupnp_net_uri_set(uri, mupnp_upnp_icon_geturl(icon));
	if (mupnp_net_uri_isabsolute(uri)) {
		mupnp_string_setvalue(buf, mupnp_net_uri_geturi(uri));
		mupnp_net_uri_delete(uri);
		return TRUE;
	}
	
	rootDev = mupnp_upnp_device_getrootdevice(dev);
	if (rootDev) {
		ssdplocation = mupnp_upnp_device_getlocationfromssdppacket(rootDev);
		ssdpUri = mupnp_net_uri_new();
		if (0 < mupnp_strlen(ssdplocation)) {
			mupnp_net_uri_set(uri, ssdplocation);
			mupnp_net_uri_setpath(uri, mupnp_upnp_icon_geturl(icon));
			mupnp_string_setvalue(buf, mupnp_net_uri_getvalue(uri));
			mupnp_net_uri_delete(uri);
			return TRUE;
		}
		mupnp_net_uri_delete(ssdpUri);
	}
	
	mupnp_net_uri_delete(uri);

	return FALSE;
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* mupnp_upnp_device_getactionbyname
****************************************/

mUpnpUpnpAction *mupnp_upnp_device_getactionbyname(mUpnpUpnpDevice *dev, const char *name)
{
	mUpnpUpnpService *service;
	mUpnpUpnpAction *action;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(name) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		action = mupnp_upnp_service_getactionbyname(service, name);
		if (action != NULL)
			return action;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		action = mupnp_upnp_device_getactionbyname(childDev, name);
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
* mupnp_upnp_device_getstatevariablebyname
****************************************/

mUpnpUpnpStateVariable *mupnp_upnp_device_getstatevariablebyname(mUpnpUpnpDevice *dev, const char *name)
{
	mUpnpUpnpService *service;
	mUpnpUpnpStateVariable *statVar;
	mUpnpUpnpDevice *childDev;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_strlen(name) <= 0)
		return NULL;
			
	for (service=mupnp_upnp_device_getservices(dev); service != NULL; service = mupnp_upnp_service_next(service)) {
		statVar = mupnp_upnp_service_getstatevariablebyname(service, name);
		if (statVar != NULL)
			return statVar;
	}
		
	for (childDev = mupnp_upnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_upnp_device_next(childDev)) {
		statVar = mupnp_upnp_device_getstatevariablebyname(childDev, name);
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
* mupnp_upnp_device_initiconlist
****************************************/

static void mupnp_upnp_device_initiconlist(mUpnpUpnpDevice *dev)
{
	mUpnpXmlNode *devNode;
	mUpnpXmlNode *iconListNode;
	mUpnpXmlNode *childNode;
	mUpnpUpnpIcon *childIcon;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_iconlist_clear(dev->iconList);
	
	devNode = mupnp_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	iconListNode = mupnp_xml_node_getchildnode(devNode, CG_UPNP_ICONLIST_ELEM_NAME);
	if (iconListNode == NULL)
		return;
		
	for (childNode = mupnp_xml_node_getchildnodes(iconListNode); childNode != NULL; childNode = mupnp_xml_node_next(childNode)) {
		if (mupnp_upnp_icon_isiconnode(childNode) == FALSE)
			continue;
		childIcon = mupnp_upnp_icon_new();
		mupnp_upnp_icon_seticonnode(childIcon, childNode);
		mupnp_upnp_iconlist_add(dev->iconList, childIcon);
	} 

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_upnp_device_addicon
 ****************************************/

BOOL mupnp_upnp_device_addicon(mUpnpUpnpDevice *dev, mUpnpUpnpIcon *icon)
{
	mUpnpXmlNode *devNode;
	mUpnpXmlNode *iconListNode;
	mUpnpXmlNode *iconNode;
	mUpnpXmlNode *copyIconNode;
	mUpnpUpnpIcon *copyIcon;

	iconNode = mupnp_upnp_icon_geticonnode(icon);
	if (iconNode == NULL)
		return FALSE;
	
	devNode = mupnp_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return FALSE;

	iconListNode = mupnp_xml_node_getchildnode(devNode, CG_UPNP_ICONLIST_ELEM_NAME);
	if (iconListNode == NULL) {
		iconListNode = mupnp_xml_node_new();
		mupnp_xml_node_setname(iconListNode, CG_UPNP_ICONLIST_ELEM_NAME);
		mupnp_xml_node_addchildnode(devNode, iconListNode);
	}
	
	copyIconNode = mupnp_xml_node_new();
	mupnp_xml_node_copy(copyIconNode, iconNode);
	mupnp_xml_node_addchildnode(iconListNode, copyIconNode);
	
	copyIcon = mupnp_upnp_icon_new();
	mupnp_upnp_icon_seticonnode(copyIcon, copyIconNode);
	mupnp_upnp_iconlist_add(dev->iconList, copyIcon);
	
	return TRUE;
}

/****************************************
 * mupnp_upnp_device_updateudn
 ****************************************/

void mupnp_upnp_device_updateudn(mUpnpUpnpDevice *dev)
{
	char uuid[CG_UPNP_UUID_MAX_LEN];
	mupnp_upnp_createuuid(uuid, sizeof(uuid));
	mupnp_upnp_device_setudn(dev, uuid);
}

/****************************************
 * mupnp_upnp_device_setpresentationlistener
 ****************************************/

void mupnp_upnp_device_setpresentationlistener(mUpnpUpnpDevice *dev, CG_UPNP_PRESENTATION_LISTNER func)
{
  mupnp_upnp_device_removepresentationurl(dev);
  if (func)
    mupnp_upnp_device_setpresentationurl(dev, CG_UPNP_DEVICE_DEFAULT_PRESENTATION_URI);
  
  dev->presentationListener = func;
}

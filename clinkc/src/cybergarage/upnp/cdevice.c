/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdevice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo (aapo.makela@nokia.com)
*		- Added cg_upnp_device_getservicebysid().
*
*	10/31/05
*		- cg_upnp_device_getdevicebyname: Changed dev to childDev in for
*		- cg_upnp_device_byebyefrom & _announcefrom: Added missing 
*		  advertisement and byebye messages
*		- Added EXT handling to postsearchresponse
*
*	11/17/05
*		- Fixed many wrong variable names in for loops (dev => childDev)
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>

#include <cybergarage/util/ctime.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/net/curl.h>
#include <cybergarage/upnp/cupnp_function.h>
#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/io/cfile.h>

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_device_initchildnodes(CgUpnpDevice *dev);
static void cg_upnp_device_initdevicelist(CgUpnpDevice *dev);
static void cg_upnp_device_initservicelist(CgUpnpDevice *dev);
static void cg_upnp_device_initiconlist(CgUpnpDevice *dev);

static void cg_upnp_device_notifywait(CgUpnpDevice *dev);

/****************************************
* cg_upnp_device_new
****************************************/

CgUpnpDevice *cg_upnp_device_new()
{
	CgUpnpDevice *dev = (CgUpnpDevice *)malloc(sizeof(CgUpnpDevice));
	cg_list_node_init((CgList *)dev);

	dev->rootNodeList = NULL;
	dev->deviceNode = NULL;

	dev->deviceList = cg_upnp_devicelist_new();
	dev->serviceList = cg_upnp_servicelist_new();
	dev->iconList = cg_upnp_iconlist_new();
	
	dev->mutex = cg_mutex_new();
	
	dev->httpServerList = cg_http_serverlist_new();
	dev->ssdpServerList = cg_upnp_ssdp_serverlist_new();
	
	dev->advertiser = cg_thread_new();
	
	dev->descriptionURI = cg_string_new();

	dev->ssdpPkt = cg_upnp_ssdp_packet_new();
	
	cg_upnp_device_setparentdevice(dev, NULL);
	cg_upnp_device_setdescriptionuri(dev, CG_UPNP_DEVICE_DEFAULT_DESCRIPTION_URI);
	cg_upnp_device_setleasetime(dev, CG_UPNP_DEVICE_DEFAULT_LEASE_TIME);
	cg_upnp_device_sethttpport(dev, CG_UPNP_DEVICE_HTTP_DEFAULT_PORT);
	cg_upnp_device_sethttplistener(dev, NULL);
	cg_upnp_device_setuserdata(dev, NULL);
	
	return dev;
}

/****************************************
* cg_upnp_device_delete
****************************************/

void cg_upnp_device_delete(CgUpnpDevice *dev)
{
	cg_list_remove((CgList *)dev);
	
	cg_upnp_device_clear(dev);
	
	cg_upnp_devicelist_delete(dev->deviceList);
	cg_upnp_servicelist_delete(dev->serviceList);
	cg_upnp_iconlist_delete(dev->iconList);

	cg_mutex_delete(dev->mutex);
	
	cg_http_serverlist_delete(dev->httpServerList);
	cg_upnp_ssdp_serverlist_delete(dev->ssdpServerList);
	cg_thread_delete(dev->advertiser);
	
	cg_string_delete(dev->descriptionURI);

	cg_upnp_ssdp_packet_delete(dev->ssdpPkt);
	
	free(dev);
}

/****************************************
* cg_upnp_device_clear
****************************************/

void cg_upnp_device_clear(CgUpnpDevice *dev)
{
	if (dev->rootNodeList != NULL) {
		cg_xml_nodelist_delete(dev->rootNodeList);
		dev->rootNodeList = NULL;
	}
	dev->deviceNode = NULL;
	
	cg_upnp_devicelist_clear(dev->deviceList);
	cg_upnp_servicelist_clear(dev->serviceList);
	cg_upnp_iconlist_clear(dev->iconList);
}

/****************************************
* cg_upnp_device_setdevicenode
****************************************/

void cg_upnp_device_setdevicenode(CgUpnpDevice *dev, CgXmlNode *node)
{
	dev->deviceNode = node;
	cg_upnp_device_initchildnodes(dev);
}

/****************************************
* cg_upnp_device_getrootdevice
****************************************/

CgUpnpDevice *cg_upnp_device_getrootdevice(CgUpnpDevice *dev)
{
	CgUpnpDevice *rootDev;
	
	rootDev = dev;
	while (cg_upnp_device_getparentdevice(rootDev) != NULL)
		rootDev = cg_upnp_device_getparentdevice(rootDev);
	
	return rootDev;
}

/****************************************
* cg_upnp_device_parsedescription
****************************************/

BOOL cg_upnp_device_parsedescription(CgUpnpDevice *dev, char *desciption, int descriptionLen)
{
	CgXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	CgXmlNode *rootNode;
	char uuidBuf[CG_UPNP_UUID_MAX_LEN];
	
	cg_upnp_device_clear(dev);
	dev->rootNodeList = cg_xml_nodelist_new();

	xmlParser = cg_xml_parser_new();
	xmlParseSuccess = cg_xml_parse(xmlParser, dev->rootNodeList, desciption, descriptionLen);
	cg_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
		return FALSE;

	if (cg_xml_nodelist_size(dev->rootNodeList) <= 0) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}

	rootNode = cg_upnp_device_getrootnode(dev);
	if (rootNode == NULL) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}
		
	dev->deviceNode = cg_xml_node_getchildnode(rootNode, CG_UPNP_DEVICE_ELEM_NAME);
	if (dev->deviceNode == NULL) {
		cg_upnp_device_clear(dev);
		return FALSE;
	}

	if (cg_upnp_device_hasudn(dev) == FALSE)
		cg_upnp_device_setudn(dev, cg_upnp_createuuid(uuidBuf));
	
	cg_upnp_device_initchildnodes(dev);
	
	return TRUE;
}

/****************************************
* cg_upnp_device_parsedescriptionurl
****************************************/

BOOL cg_upnp_device_parsedescriptionurl(CgUpnpDevice *dev, CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int statusCode;
	char *content;
	long contentLen;
	BOOL parseSuccess;
	
	if (cg_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	request = cg_net_url_getrequest(url);
	
	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, request);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, host, port);
	
	statusCode = cg_http_response_getstatuscode(httpRes);
	if (statusCode != CG_HTTP_STATUS_OK) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);

	parseSuccess = cg_upnp_device_parsedescription(dev, content, contentLen);

	cg_http_request_delete(httpReq);
	
	return parseSuccess;
}

/****************************************
* cg_upnp_device_loaddescriptionfile
****************************************/

#if !defined(TENGINE) && !defined(ITRON) && !defined(BTRON)

BOOL cg_upnp_device_loaddescriptionfile(CgUpnpDevice *dev, char *fileName)
{
	CgFile *file;
	char *description;
	int descriptionLen;
	BOOL parseSuccess;

	file = cg_file_new();
	cg_file_setname(file, fileName);
	if (cg_file_load(file) == FALSE) {
		cg_file_delete(file);
		return FALSE;
	}

	description = cg_file_getcontent(file);
	descriptionLen = cg_strlen(description);

	parseSuccess = cg_upnp_device_parsedescription(dev, description, descriptionLen);

	cg_file_delete(file);

	return parseSuccess;
}

#endif

/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_device_initchildnodes
****************************************/

static void cg_upnp_device_initchildnodes(CgUpnpDevice *dev)
{
	cg_upnp_device_initdevicelist(dev);
	cg_upnp_device_initservicelist(dev);
	cg_upnp_device_initiconlist(dev);
}

/****************************************
*
* Embedded Device
*
****************************************/

/****************************************
* cg_upnp_device_initdevicelist
****************************************/

static void cg_upnp_device_initdevicelist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *devListNode;
	CgXmlNode *childNode;
	CgUpnpDevice *childDev;
	
	cg_upnp_devicelist_clear(dev->deviceList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	devListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_DEVICELIST_ELEM_NAME);
	if (devListNode == NULL)
		return;
	
	for (childNode = cg_xml_node_getchildnodes(devListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_device_isdevicenode(childNode) == FALSE)
			continue;
		childDev = cg_upnp_device_new();
		cg_upnp_device_setdevicenode(childDev, childNode);
		cg_upnp_devicelist_add(dev->deviceList, childDev);
		cg_upnp_device_setparentdevice(childDev, dev);
	} 
}

/****************************************
* cg_upnp_device_hasname
****************************************/

BOOL cg_upnp_device_isname(CgUpnpDevice *dev, char *name)
{
	if (name == NULL)
		return FALSE;
	if (0 <= cg_strstr(cg_upnp_device_getudn(dev), name))
		return TRUE;
	if (0 <= cg_strstr(cg_upnp_device_getfriendlyname(dev), name))
		return TRUE;
	if (0 <= cg_strstr(cg_upnp_device_getdevicetype(dev), name))
		return TRUE;
	return FALSE;
}

/****************************************
* cg_upnp_device_getdevicebyname
****************************************/

CgUpnpDevice *cg_upnp_device_getdevicebyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpDevice *childDev;
	CgUpnpDevice *moreChildDev;
	
	if (cg_strlen(name) <= 0)
		return NULL;
	
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_device_isname(childDev, name) == TRUE)
			return childDev;
		moreChildDev = cg_upnp_device_getdevicebyname(childDev, name);
		if (moreChildDev != NULL)
			return moreChildDev;
	}
	return NULL;
}

/****************************************
* cg_upnp_device_getdevicebydescriptionuri
****************************************/

CgUpnpDevice *cg_upnp_device_getdevicebydescriptionuri(CgUpnpDevice *dev, char *url)
{
	CgUpnpDevice *childDev;
	CgUpnpDevice *moreChildDev;
	
	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_device_isdescriptionuri(dev, url) == TRUE)
			return dev;
		moreChildDev = cg_upnp_device_getdevicebydescriptionuri(childDev, url);
		if (moreChildDev != NULL)
			return moreChildDev;
	}
	return NULL;
}

/****************************************
*
* Notify
*
****************************************/

/****************************************
* cg_upnp_device_notifywait
****************************************/

static void cg_upnp_device_notifywait(CgUpnpDevice *dev)
{
	cg_waitrandom(CG_UPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME);
}

/****************************************
* cg_upnp_device_getlocationurl
****************************************/

char *cg_upnp_device_getlocationurl(CgUpnpDevice *dev, char *host, char *buf, int bufSize)
{
	return cg_net_gethosturl(
					host,
					cg_upnp_device_gethttpport(dev),
					cg_upnp_device_getdescriptionuri(dev),
					buf,
					bufSize);
}

/****************************************
* cg_upnp_device_getnotifydevicent
****************************************/

char *cg_upnp_device_getnotifydevicent(CgUpnpDevice *dev, char *buf, int bufSize)
{
	if (cg_upnp_device_isrootdevice(dev) == FALSE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", cg_upnp_device_getudn(dev));
#else
		cg_strcpy(buf, cg_upnp_device_getudn(dev));
#endif
	} else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		cg_strcpy(buf, CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	return buf;
}

/****************************************
* cg_upnp_device_getnotifydeviceusn
****************************************/

char *cg_upnp_device_getnotifydeviceusn(CgUpnpDevice *dev, char *buf, int bufSize)
{
	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#else
		sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), CG_UPNP_DEVICE_UPNP_ROOTDEVICE);
#endif
	}
	else {
#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s", cg_upnp_device_getudn(dev));
#else
		sprintf(buf, "%s", cg_upnp_device_getudn(dev));
#endif
	}
	return buf;
}

/****************************************
* cg_upnp_device_getnotifydevicetypent
****************************************/

char *cg_upnp_device_getnotifydevicetypent(CgUpnpDevice *dev, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s", cg_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s", cg_upnp_device_getdevicetype(dev));
#endif
	return buf;
}

/****************************************
* cg_upnp_device_getnotifydevicetypeusn
****************************************/

char *cg_upnp_device_getnotifydevicetypeusn(CgUpnpDevice *dev, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_device_getdevicetype(dev));
#else
	sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_device_getdevicetype(dev));
#endif
	return buf;
}

/****************************************
* cg_upnp_device_announcefrom
****************************************/

BOOL cg_upnp_device_announcefrom(CgUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpServiceList *serviceList;
	CgUpnpService *service;
	CgUpnpDeviceList *devList;
	CgUpnpDevice *childDev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
	
	ssdpSock = cg_upnp_ssdp_socket_new();
	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setserver(ssdpReq, cg_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setleasetime(ssdpReq, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdprequest_setlocation(ssdpReq, cg_upnp_device_getlocationurl(dev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	
	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
		cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	/**** root or embedded device UUID ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getudn(dev));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getudn(dev));
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	
	cg_upnp_ssdprequest_delete(ssdpReq);
	cg_upnp_ssdp_socket_close(ssdpSock);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	/**** child services ****/
	serviceList = cg_upnp_device_getservicelist(dev);
	for (service=cg_upnp_servicelist_gets(serviceList); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_announcefrom(service, bindAddr);
	
	/**** child deveices ****/
	devList = cg_upnp_device_getdevicelist(dev);
	for (childDev = cg_upnp_devicelist_gets(devList); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_announcefrom(childDev, bindAddr);
		
	return sentResult;
}

/****************************************
* cg_upnp_device_announce
****************************************/

void cg_upnp_device_announce(CgUpnpDevice *dev)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	cg_upnp_device_notifywait(dev);
	
	ssdpCount = cg_upnp_ssdp_getannouncecount();
	
	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf; netIf = cg_net_interface_next(netIf)) {
		bindAddr = cg_net_interface_getaddress(netIf);
		if (cg_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
			cg_upnp_device_announcefrom(dev, bindAddr);
	}

	cg_net_interfacelist_delete(netIfList);
}
	
/****************************************
* cg_upnp_device_byebyefrom
****************************************/

BOOL cg_upnp_device_byebyefrom(CgUpnpDevice *dev, char *bindAddr)
{
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpServiceList *serviceList;
	CgUpnpService *service;
	CgUpnpDeviceList *devList;
	CgUpnpDevice *childDev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;

	ssdpSock = cg_upnp_ssdp_socket_new();
	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);

	/**** uuid:device-UUID(::upnp:rootdevice) ****/
	if (cg_upnp_device_isrootdevice(dev) == TRUE) {
		cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydeviceusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
		cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	}

	/**** uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getnotifydevicetypent(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getnotifydevicetypeusn(dev, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	/**** root or embedded device UUID ****/
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_device_getudn(dev));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_device_getudn(dev));
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);

	cg_upnp_ssdprequest_delete(ssdpReq);
	cg_upnp_ssdp_socket_close(ssdpSock);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	serviceList = cg_upnp_device_getservicelist(dev);
	for (service=cg_upnp_servicelist_gets(serviceList); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_byebyefrom(service, bindAddr);
	
	devList = cg_upnp_device_getdevicelist(dev);
	for (childDev = cg_upnp_devicelist_gets(devList); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_byebyefrom(childDev, bindAddr);

	return sentResult;
}

/****************************************
* cg_upnp_device_byebye
****************************************/

void cg_upnp_device_byebye(CgUpnpDevice *dev)
{
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	char *bindAddr;
	int ssdpCount, i;
		
	cg_upnp_device_notifywait(dev);
	
	ssdpCount = cg_upnp_ssdp_getannouncecount();
	
	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	for (netIf = cg_net_interfacelist_gets(netIfList); netIf; netIf = cg_net_interface_next(netIf)) {
		bindAddr = cg_net_interface_getaddress(netIf);
		if (cg_strlen(bindAddr) <= 0)
			continue;
		for (i=0; i<ssdpCount; i++)
			cg_upnp_device_byebyefrom(dev, bindAddr);
	}

	cg_net_interfacelist_delete(netIfList);
}

/****************************************
* cg_upnp_device_postsearchresponse
****************************************/

BOOL cg_upnp_device_postsearchresponse(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, char *st, char *usn)
{
	CgUpnpDevice *rootDev;
	CgUpnpSSDPResponse *ssdpRes;
	char httpDateStr[CG_HTTP_DATE_MAXLEN];
	char *localAddr;
	char *remoteAddr;
	int remotePort;
	char rootDevLocation[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serverBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	int ssdpMx;
	int ssdpCount;
	CgUpnpSSDPSocket *ssdpSock;
	int n;
	
	localAddr = cg_upnp_ssdp_packet_getlocaladdress(ssdpPkt);
	rootDev = cg_upnp_device_getrootdevice(dev);
	cg_upnp_device_getlocationurl(rootDev, localAddr, rootDevLocation, sizeof(rootDevLocation));

	ssdpRes = cg_upnp_ssdpresponse_new();
	cg_upnp_ssdpresponse_setleasetime(ssdpRes, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdpresponse_setdate(ssdpRes, cg_http_getdate(cg_getcurrentsystemtime(), httpDateStr, sizeof(httpDateStr)));
	cg_upnp_ssdpresponse_setst(ssdpRes, st);

	cg_upnp_ssdpresponse_setext(ssdpRes);
	cg_upnp_getservername(serverBuf, CG_UPNP_SSDP_HEADER_LINE_MAXSIZE);
	cg_upnp_ssdpresponse_setserver(ssdpRes, serverBuf);

	cg_upnp_ssdpresponse_setusn(ssdpRes, usn);
	cg_upnp_ssdpresponse_setlocation(ssdpRes, rootDevLocation);

	ssdpMx = cg_upnp_ssdp_packet_getmx(ssdpPkt);
	cg_waitrandom(ssdpMx * 1000);
	
	remoteAddr = cg_upnp_ssdp_packet_getremoteaddress(ssdpPkt);
	remotePort = cg_upnp_ssdp_packet_getremoteport(ssdpPkt);
	ssdpCount = cg_upnp_ssdp_getannouncecount();

	ssdpSock = cg_upnp_ssdp_socket_new();
	for (n=0; n<ssdpCount; n++)
		cg_upnp_ssdp_socket_postresponse(ssdpSock, ssdpRes, remoteAddr, remotePort);
	cg_upnp_ssdp_socket_delete(ssdpSock);

	cg_upnp_ssdpresponse_delete(ssdpRes);

	return TRUE;
}

/****************************************
*
* Start/Stop
*
****************************************/

/****************************************
* cg_upnp_device_start
****************************************/

BOOL cg_upnp_device_start(CgUpnpDevice *dev)
{
	CG_HTTP_LISTENER httpListener;
	int httpPort;
	
	cg_upnp_device_stop(dev);

	/**** HTTP Server ****/
	httpPort = cg_upnp_device_gethttpport(dev);
	if (cg_http_serverlist_open(dev->httpServerList, httpPort) == FALSE)
		return FALSE;
	cg_http_serverlist_setuserdata(dev->httpServerList, dev);
	httpListener = cg_upnp_device_gethttplistener(dev);
	if (httpListener == NULL)
		httpListener = cg_upnp_device_httprequestrecieved;
	cg_http_serverlist_setlistener(dev->httpServerList, httpListener);
	cg_http_serverlist_start(dev->httpServerList);

	/**** SSDP Server ****/
	if (cg_upnp_ssdp_serverlist_open(dev->ssdpServerList) == FALSE)
		return FALSE;
	cg_upnp_ssdp_serverlist_setuserdata(dev->ssdpServerList, dev);
	cg_upnp_ssdp_serverlist_setlistener(dev->ssdpServerList, cg_upnp_device_ssdplistener);
	cg_upnp_ssdp_serverlist_start(dev->ssdpServerList);

	/**** Announce ****/
	cg_upnp_device_announce(dev);
	
	/**** Advertiser ****/
	cg_upnp_device_advertiser_start(dev);	

	return TRUE;
}
	
/****************************************
* cg_upnp_device_stop
****************************************/

BOOL cg_upnp_device_stop(CgUpnpDevice *dev)
{
	cg_upnp_device_byebye(dev);
	
	/**** HTTP Server ****/
	cg_http_serverlist_stop(dev->httpServerList);
	cg_http_serverlist_close(dev->httpServerList);
	cg_http_serverlist_clear(dev->httpServerList);

	/**** SSDP Server ****/
	cg_upnp_ssdp_serverlist_stop(dev->ssdpServerList);
	cg_upnp_ssdp_serverlist_close(dev->ssdpServerList);
	cg_upnp_ssdp_serverlist_clear(dev->ssdpServerList);

	/**** Advertiser ****/
	cg_upnp_device_advertiser_stop(dev);	
	
	return TRUE;
}

/****************************************
*
* Embedded Service
*
****************************************/

/****************************************
* cg_upnp_device_initservicelist
****************************************/

static void cg_upnp_device_initservicelist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *serviceListNode;
	CgXmlNode *childNode;
	CgUpnpService *childService;
	
	cg_upnp_servicelist_clear(dev->serviceList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	serviceListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_SERVICELIST_ELEM_NAME);
	if (serviceListNode == NULL)
		return;
		
	for (childNode = cg_xml_node_getchildnodes(serviceListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_service_isservicenode(childNode) == FALSE)
			continue;
		childService = cg_upnp_service_new();
		cg_upnp_service_setservicenode(childService, childNode);
		cg_upnp_servicelist_add(dev->serviceList, childService);
		cg_upnp_service_setdevice(childService, dev);
	} 
}

/****************************************
* cg_upnp_device_getservicebyname
****************************************/

CgUpnpService *cg_upnp_device_getservicebyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_service_isname(service, name) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebyname(childDev, name);
		if (service != NULL)
			return service;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_device_getservicebyscpdurl
****************************************/

CgUpnpService *cg_upnp_device_getservicebyscpdurl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_service_isscpdurl(service, url) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebyscpdurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_device_getservicebycontrolurl
****************************************/

CgUpnpService *cg_upnp_device_getservicebycontrolurl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_streq(cg_upnp_service_getcontrolurl(service), url) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebycontrolurl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_device_getservicebysid
****************************************/

CgUpnpService *cg_upnp_device_getservicebysid(CgUpnpDevice *dev, char *sid)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(sid) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_service_getsubscriberbysid(service, sid) != NULL)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebysid(childDev, sid);
		if (service != NULL)
			return service;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_device_setactionlistener
****************************************/

void cg_upnp_device_setactionlistener(CgUpnpDevice *dev, CG_UPNP_ACTION_LISTNER actionListner)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_setactionlistener(service, actionListner);
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_setactionlistener(childDev, actionListner);
}

/****************************************
* cg_upnp_device_setquerylistener
****************************************/

void cg_upnp_device_setquerylistener(CgUpnpDevice *dev, CG_UPNP_STATEVARIABLE_LISTNER queryListner)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_setquerylistener(service, queryListner);
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		cg_upnp_device_setquerylistener(childDev, queryListner);
}

/****************************************
* cg_upnp_device_getservicebyeventsuburl
****************************************/

CgUpnpService *cg_upnp_device_getservicebyeventsuburl(CgUpnpDevice *dev, char *url)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(url) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_streq(cg_upnp_service_geteventsuburl(service), url) == TRUE)
			return service;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		service = cg_upnp_device_getservicebyeventsuburl(childDev, url);
		if (service != NULL)
			return service;
	}
	
	return NULL;
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_device_getactionbyname
****************************************/

CgUpnpAction *cg_upnp_device_getactionbyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpService *service;
	CgUpnpAction *action;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		action = cg_upnp_service_getactionbyname(service, name);
		if (action != NULL)
			return action;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		action = cg_upnp_device_getactionbyname(childDev, name);
		if (action != NULL)
			return action;
	}
	
	return NULL;
}

/****************************************
*
* Embedded StateVariable
*
****************************************/

/****************************************
* cg_upnp_device_getstatevariablebyname
****************************************/

CgUpnpStateVariable *cg_upnp_device_getstatevariablebyname(CgUpnpDevice *dev, char *name)
{
	CgUpnpService *service;
	CgUpnpStateVariable *statVar;
	CgUpnpDevice *childDev;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		statVar = cg_upnp_service_getstatevariablebyname(service, name);
		if (statVar != NULL)
			return statVar;
	}
		
	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		statVar = cg_upnp_device_getstatevariablebyname(childDev, name);
		if (statVar != NULL)
			return statVar;
	}
	
	return NULL;
}

/****************************************
*
* Embedded Icon
*
****************************************/

/****************************************
* cg_upnp_device_initiconlist
****************************************/

static void cg_upnp_device_initiconlist(CgUpnpDevice *dev)
{
	CgXmlNode *devNode;
	CgXmlNode *iconListNode;
	CgXmlNode *childNode;
	CgUpnpIcon *childIcon;
	
	cg_upnp_iconlist_clear(dev->iconList);
	
	devNode = cg_upnp_device_getdevicenode(dev);
	if (devNode == NULL)
		return;
		
	iconListNode = cg_xml_node_getchildnode(devNode, CG_UPNP_ICONLIST_ELEM_NAME);
	if (iconListNode == NULL)
		return;
		
	for (childNode = cg_xml_node_getchildnodes(iconListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_icon_isiconnode(childNode) == FALSE)
			continue;
		childIcon = cg_upnp_icon_new();
		cg_upnp_icon_seticonnode(childIcon, childNode);
		cg_upnp_iconlist_add(dev->iconList, childIcon);
	} 
}

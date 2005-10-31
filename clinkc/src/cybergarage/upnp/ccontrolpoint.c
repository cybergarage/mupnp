/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontrolpoint.c
*
*	Revision:
*
*	05/26/05
*		- first revision
*	08/25/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_upnp_controlpoint_geteventsubcallbackurl() to add < and > around callbackurl per UPnP def.
*	09/06/06
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Added cg_upnp_controlpoint_setuserdata() and cg_upnp_controlpoint_getuserdatga().
*
*	10/31/05
*		- Fixed severe bug in getting child devices
*
******************************************************************/

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/cupnp_limit.h>
#include <cybergarage/upnp/control/ccontrol.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
* static function defines
****************************************/

static void cg_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt);
static void cg_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt);

/****************************************
* cg_upnp_controlpoint_new
****************************************/

CgUpnpControlPoint *cg_upnp_controlpoint_new()
{
	CgUpnpControlPoint *ctrlPoint = (CgUpnpControlPoint *)malloc(sizeof(CgUpnpControlPoint));
	
	ctrlPoint->mutex = cg_mutex_new();
	ctrlPoint->deviceRootNodeList = cg_xml_nodelist_new();
	ctrlPoint->deviceList = cg_upnp_devicelist_new();
	ctrlPoint->ssdpServerList = cg_upnp_ssdp_serverlist_new();
	ctrlPoint->ssdpResServerList = cg_upnp_ssdpresponse_serverlist_new();
	ctrlPoint->httpServerList = cg_http_serverlist_new();
	ctrlPoint->httpEventURI = cg_string_new();

	cg_upnp_controlpoint_setssdplistener(ctrlPoint, NULL);
	cg_upnp_controlpoint_setssdpresponselistener(ctrlPoint, NULL);
	cg_upnp_controlpoint_seteventlistener(ctrlPoint, NULL);
	cg_upnp_controlpoint_sethttplistener(ctrlPoint, NULL);

	cg_upnp_controlpoint_setssdpresponseport(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT);
	cg_upnp_controlpoint_setssdpsearchmx(ctrlPoint, CG_UPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX);
	cg_upnp_controlpoint_seteventport(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT);
	cg_upnp_controlpoint_seteventsuburi(ctrlPoint, CG_UPNP_CONTROLPOINT_HTTP_EVENTSUB_URI);

	cg_upnp_controlpoint_setuserdata(ctrlPoint, NULL);

	return ctrlPoint;
}

/****************************************
* cg_upnp_controlpoint_delete
****************************************/

void cg_upnp_controlpoint_delete(CgUpnpControlPoint *ctrlPoint)
{
	cg_mutex_delete(ctrlPoint->mutex);
	cg_xml_nodelist_delete(ctrlPoint->deviceRootNodeList);
	cg_upnp_devicelist_delete(ctrlPoint->deviceList);
	cg_upnp_ssdp_serverlist_delete(ctrlPoint->ssdpServerList);
	cg_upnp_ssdpresponse_serverlist_delete(ctrlPoint->ssdpResServerList);
	cg_http_serverlist_delete(ctrlPoint->httpServerList);
	cg_string_delete(ctrlPoint->httpEventURI);
	
	free(ctrlPoint);
}

/****************************************
* cg_upnp_controlpoint_lock
****************************************/

BOOL cg_upnp_controlpoint_lock(CgUpnpControlPoint *ctrlPoint)
{
	return cg_mutex_lock(ctrlPoint->mutex);
}

/****************************************
* cg_upnp_controlpoint_unlock
****************************************/

BOOL cg_upnp_controlpoint_unlock(CgUpnpControlPoint *ctrlPoint)
{
	return cg_mutex_unlock(ctrlPoint->mutex);
}

/****************************************
* cg_upnp_controlpoint_geteventsubcallbackurl
****************************************/

char *cg_upnp_controlpoint_geteventsubcallbackurl(CgUpnpControlPoint *ctrlPoint, char *ifaddr, char *buf, int bufLen)
{
	/**** Thanks for Theo Beisch (2005/08/25) ****/
	return cg_net_getmodifierhosturl(ifaddr, cg_upnp_controlpoint_geteventport(ctrlPoint), cg_upnp_controlpoint_geteventsuburi(ctrlPoint), buf, bufLen, "<", ">");
}

/****************************************
* cg_upnp_controlpoint_getdevicebyname
****************************************/

CgUpnpDevice *cg_upnp_controlpoint_getdevicebyname(CgUpnpControlPoint *ctrlPoint, char *name)
{
	CgUpnpDevice *dev, *findDev;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = cg_upnp_device_next(dev)) {
		if (cg_upnp_device_isname(dev, name) == TRUE)
			return dev;
		findDev = cg_upnp_device_getdevicebyname(dev, name);
		if (findDev != NULL)
			return findDev;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_device_parsedescriptionurl
****************************************/

static BOOL cg_upnp_controlpoint_parsescservicescpd(CgUpnpService *service, CgUpnpSSDPPacket *ssdpPkt)
{
	CgNetURL *scpdURL;
	CgUpnpDevice *rootDev;
	CgNetURL *locationURL;
	BOOL scpdParseSuccess;
		
	if (cg_strlen(cg_upnp_service_getscpdurl(service)) <= 0)
		return FALSE;
		
	/**** Parse using only SCPDURL ****/
	scpdURL = cg_net_url_new();
	cg_net_url_set(scpdURL, cg_upnp_service_getscpdurl(service));
	if (cg_net_url_isabsolute(scpdURL) == TRUE) {
		scpdParseSuccess = cg_upnp_service_parsedescriptionurl(service, scpdURL);
		if (scpdParseSuccess == TRUE) {
			cg_net_url_delete(scpdURL);
			return TRUE;
		}
	}
	
	/**** Parse using URLBase and SCPDURL ****/
	rootDev = cg_upnp_service_getrootdevice(service);
	if (rootDev != NULL) {
		if (0 < cg_strlen(cg_upnp_device_geturlbase(rootDev))) {
			locationURL = cg_net_url_new();
			cg_net_url_set(locationURL, cg_upnp_device_geturlbase(rootDev));
			cg_net_url_setpath(locationURL, cg_net_url_getpath(scpdURL));
			scpdParseSuccess = cg_upnp_service_parsedescriptionurl(service, locationURL);
			cg_net_url_delete(locationURL);
			if (scpdParseSuccess == TRUE) {
				cg_net_url_delete(scpdURL);
				return TRUE;
			}
		}
	}
	
	/**** Parse using Location and SCPDURL ****/
	if (cg_strlen(cg_upnp_ssdp_packet_getlocation(ssdpPkt)) <= 0) {
		cg_net_url_delete(scpdURL);
		return FALSE;
	}
	locationURL = cg_net_url_new();
	cg_net_url_set(locationURL, cg_upnp_ssdp_packet_getlocation(ssdpPkt));
	cg_net_url_setpath(locationURL, cg_net_url_getpath(scpdURL));
	scpdParseSuccess = cg_upnp_service_parsedescriptionurl(service, locationURL);
	cg_net_url_delete(locationURL);
	cg_net_url_delete(scpdURL);

	return scpdParseSuccess;
}

/****************************************
* cg_upnp_controlpoint_getdevice
****************************************/

static BOOL cg_upnp_controlpoint_parseservicesfordevice(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		if (cg_upnp_controlpoint_parsescservicescpd(service, ssdpPkt) == FALSE) {
			return FALSE;
		}
	}
	
	/* Now only root SCPDs for root services are parsed, but also child 
	   devices' services have to be parsed, so parse them */
	for (childDev=cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev)) {
		if (cg_upnp_controlpoint_parseservicesfordevice(childDev, ssdpPkt) == FALSE)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

static CgUpnpDevice *cg_upnp_controlpoint_createdevicefromssdkpacket(CgUpnpSSDPPacket *ssdpPkt)
{
	char *location;
	CgNetURL *url;
	CgUpnpDevice *dev;
	BOOL parseSuccess;
	
	location = cg_upnp_ssdp_packet_getlocation(ssdpPkt);
	if (cg_strlen(location) <= 0)
		return NULL;
			
	dev = cg_upnp_device_new();
	
	url = cg_net_url_new();
	cg_net_url_set(url, location);
	parseSuccess =  cg_upnp_device_parsedescriptionurl(dev, url);
	cg_net_url_delete(url);
	
	if (parseSuccess == FALSE) {
		cg_upnp_device_delete(dev);
		return NULL;
	}

	if (cg_upnp_controlpoint_parseservicesfordevice(dev, ssdpPkt) == FALSE)
	{
		cg_upnp_device_delete(dev);
		return NULL;
	}
	
	cg_upnp_device_setssdppacket(dev, ssdpPkt);
	
	return dev;
}

/****************************************
* cg_upnp_controlpoint_adddevicebyssdppacket
****************************************/

static void cg_upnp_controlpoint_adddevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	char *usn;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CgUpnpDevice *dev;
	
	if (cg_upnp_ssdp_packet_isrootdevice(ssdpPkt) == FALSE)
		return;
	
	usn = cg_upnp_ssdp_packet_getusn(ssdpPkt);
	cg_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	cg_upnp_controlpoint_lock(ctrlPoint);
	
	dev = cg_upnp_controlpoint_getdevicebyname(ctrlPoint, udn);

	if (dev != NULL) {
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
			
	dev = cg_upnp_controlpoint_createdevicefromssdkpacket(ssdpPkt);
	if (dev == NULL) {
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
			
	cg_upnp_controlpoint_adddevice(ctrlPoint, dev);
	
	cg_upnp_controlpoint_unlock(ctrlPoint);
}

/****************************************
* cg_upnp_controlpoint_removedevicebyssdppacket
****************************************/

static void cg_upnp_controlpoint_removedevicebyssdppacket(CgUpnpControlPoint *ctrlPoint, CgUpnpSSDPPacket *ssdpPkt)
{
	char *usn;
	char udn[CG_UPNP_UDN_LEN_MAX];
	CgUpnpDevice *dev;
	
	if (cg_upnp_ssdp_packet_isrootdevice(ssdpPkt) == FALSE)
		return;
	
	usn = cg_upnp_ssdp_packet_getusn(ssdpPkt);
	cg_upnp_usn_getudn(usn, udn, sizeof(udn));
	
	cg_upnp_controlpoint_lock(ctrlPoint);
	
	dev = cg_upnp_controlpoint_getdevicebyname(ctrlPoint, udn);

	if (dev == NULL) {
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
			
	cg_upnp_device_remove(dev);
	
	cg_upnp_controlpoint_unlock(ctrlPoint);
}

/****************************************
* cg_upnp_controlpoint_search
****************************************/

BOOL cg_upnp_controlpoint_search(CgUpnpControlPoint *ctrlPoint, char *target)
{
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	
	ssdpReq = cg_upnp_ssdprequest_new();
	cg_upnp_ssdprequest_setmethod(ssdpReq, CG_HTTP_MSEARCH);
	cg_upnp_ssdprequest_setst(ssdpReq, target);
	cg_upnp_ssdprequest_setmx(ssdpReq, cg_upnp_controlpoint_getssdpsearchmx(ctrlPoint));
	cg_upnp_ssdprequest_setman(ssdpReq, "\"" CG_UPNP_MAN_DISCOVER "\"");
	
	ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	cg_upnp_ssdpresponse_serverlist_post(ssdpResServerList, ssdpReq);
	
	cg_upnp_ssdprequest_delete(ssdpReq);

	return TRUE;
}

/****************************************
* cg_upnp_controlpoint_start
****************************************/

BOOL cg_upnp_controlpoint_start(CgUpnpControlPoint *ctrlPoint)
{
	CgHttpServerList *httpServerList;
	CG_HTTP_LISTENER httpListener;
	int httpEventPort;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	int ssdpResPort;
	
	cg_upnp_controlpoint_stop(ctrlPoint);

	/**** HTTP Server ****/
	httpEventPort = cg_upnp_controlpoint_geteventport(ctrlPoint);
	httpServerList = cg_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	if (cg_http_serverlist_open(httpServerList, httpEventPort) == FALSE)
		return FALSE;
	cg_http_serverlist_setuserdata(httpServerList, ctrlPoint);
	httpListener = cg_upnp_controlpoint_gethttplistener(ctrlPoint);
	if (httpListener == NULL)
		httpListener = cg_upnp_controlpoint_httprequestrecieved;
	cg_http_serverlist_setlistener(httpServerList, httpListener);
	cg_http_serverlist_start(httpServerList);

	/**** SSDP Server ****/
	ssdpServerList = cg_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	if (cg_upnp_ssdp_serverlist_open(ssdpServerList) == FALSE)
		return FALSE;
	cg_upnp_ssdp_serverlist_setlistener(ssdpServerList, cg_upnp_controlpoint_ssdplistner);
	cg_upnp_ssdp_serverlist_setuserdata(ssdpServerList, ctrlPoint);
	if (cg_upnp_ssdp_serverlist_start(ssdpServerList) == FALSE)
		return FALSE;

	/**** SSDP Response Server ****/
	ssdpResPort = cg_upnp_controlpoint_getssdpresponseport(ctrlPoint);
	ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	if (cg_upnp_ssdpresponse_serverlist_open(ssdpResServerList, ssdpResPort) == FALSE)
		return FALSE;
	cg_upnp_ssdpresponse_serverlist_setlistener(ssdpResServerList, cg_upnp_controlpoint_ssdpresponselistner);
	cg_upnp_ssdpresponse_serverlist_setuserdata(ssdpResServerList, ctrlPoint);
	if (cg_upnp_ssdpresponse_serverlist_start(ssdpResServerList) == FALSE)
		return FALSE;

	/**** search root devices ****/
	cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);

/*	
	////////////////////////////////////////
	// Disposer
	////////////////////////////////////////

	Disposer *disposer = new Disposer(this);
	setDeviceDisposer(disposer);
	disposer->start();

	////////////////////////////////////////
	// Subscriber
	////////////////////////////////////////
		
	if (isNMPRMode() == true) {
		RenewSubscriber *renewSub = new RenewSubscriber(this);
		setRenewSubscriber(renewSub);
		renewSub->start();
	}
*/
	return TRUE;
}

/****************************************
* cg_upnp_controlpoint_stop
****************************************/

BOOL cg_upnp_controlpoint_stop(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	CgHttpServerList *httpServerList;

/*
	unsubscribe();
*/

	/**** HTTP Server ****/
	ssdpServerList = cg_upnp_controlpoint_getssdpserverlist(ctrlPoint);
	cg_upnp_ssdp_serverlist_stop(ssdpServerList);
	cg_upnp_ssdp_serverlist_close(ssdpServerList);
	cg_upnp_ssdp_serverlist_clear(ssdpServerList);
	
	/**** SSDP Response Server ****/
	ssdpResServerList = cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint);
	cg_upnp_ssdpresponse_serverlist_stop(ssdpResServerList);
	cg_upnp_ssdpresponse_serverlist_close(ssdpResServerList);
	cg_upnp_ssdpresponse_serverlist_clear(ssdpResServerList);
	
	/**** HTTP Server ****/
	httpServerList = cg_upnp_controlpoint_gethttpserverlist(ctrlPoint);
	cg_http_serverlist_stop(httpServerList);
	cg_http_serverlist_close(httpServerList);
	cg_http_serverlist_clear(httpServerList);
/*		

	////////////////////////////////////////
	// Disposer
	////////////////////////////////////////

	Disposer *disposer = getDeviceDisposer();
	if (disposer != NULL) {
		disposer->stop();
		setDeviceDisposer(NULL);
	}
	
	////////////////////////////////////////
	// Subscriber
	////////////////////////////////////////
		
	RenewSubscriber *renewSub = getRenewSubscriber();
	if (renewSub != NULL) {
		renewSub->stop();
		setRenewSubscriber(NULL);
	}
*/
	return TRUE;
}

/****************************************
* cg_upnp_controlpoint_ssdplistner
****************************************/

static void cg_upnp_controlpoint_ssdplistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_LISTNER listener;

	ctrlPoint = (CgUpnpControlPoint *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	if (cg_upnp_ssdp_packet_isrootdevice(ssdpPkt) == TRUE) {
		if (cg_upnp_ssdp_packet_isalive(ssdpPkt) == TRUE)
			cg_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
		else if (cg_upnp_ssdp_packet_isbyebye(ssdpPkt) == TRUE)
			cg_upnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
	}
				
	listener = cg_upnp_controlpoint_getssdplistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);
}

/****************************************
* cg_upnp_controlpoint_ssdpresponselistner
****************************************/

static void cg_upnp_controlpoint_ssdpresponselistner(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpControlPoint *ctrlPoint;
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;

	ctrlPoint = (CgUpnpControlPoint *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	if (ctrlPoint == NULL)
		return;

	if (cg_upnp_ssdp_packet_isrootdevice(ssdpPkt) == TRUE)
		cg_upnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
				
	listener = cg_upnp_controlpoint_getssdpresponselistener(ctrlPoint);
	if (listener != NULL)
		listener(ssdpPkt);
}

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif


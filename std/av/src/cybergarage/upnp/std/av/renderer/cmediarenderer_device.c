/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediarenderer_device.c
*
*	Revision:
*		2009/06/11
*        - first release.
*
************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/std/av/cmediarenderer.h>

/****************************************
* Device Description
****************************************/

static char *CG_UPNPAV_DMR_DEVICE_DESCRIPTION = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <device>\n"
"      <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\n"
"      <friendlyName>Cyber Garage Media Renderer</friendlyName>\n"
"      <manufacturer>Cyber Garage</manufacturer>\n"
"      <manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
"      <modelDescription>CyberGarage</modelDescription>\n"
"      <modelName>Cyber Garage Media Renderer</modelName>\n"
"      <modelNumber>1.0</modelNumber>\n"
"      <UDN>uuid:BA2E90A0-3669-401a-B249-F85196ADFC44</UDN>\n"
"      <modelURL>http://www.cybergarage.org</modelURL>\n"
"      <dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMR-1.00</dlna:X_DLNADOC>\n"
"      <serviceList>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>\n"
"            <serviceId>urn:schemas-upnp-org:service:RenderingControl</serviceId>\n"
"            <SCPDURL>/service/RenderingControl1.xml</SCPDURL>\n"
"            <controlURL>/service/RenderingControl_control</controlURL>\n"
"            <eventSubURL>/service/RenderingControl_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\n"
"            <serviceId>urn:schemas-upnp-org:service:ConnectionManager</serviceId>\n"
"            <SCPDURL>/service/ConnectionManager1.xml</SCPDURL>\n"
"            <controlURL>/service/ConnectionManager_control</controlURL>\n"
"            <eventSubURL>/service/ConnectionManager_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>\n"
"            <serviceId>urn:schemas-upnp-org:service:AVTransport</serviceId>\n"
"            <SCPDURL>/service/AVTransport1.xml</SCPDURL>\n"
"            <controlURL>/service/AVTransport_control</controlURL>\n"
"            <eventSubURL>/service/AVTransport_event</eventSubURL>\n"
"         </service>\n"
"      </serviceList>\n"
"   </device>\n"
"</root>\n";

/****************************************
* Functions
****************************************/

BOOL cg_upnpav_dmr_conmgr_init(CgUpnpAvRenderer *dmr);
BOOL cg_upnpav_dmr_avtransport_init(CgUpnpAvRenderer *dmr);
BOOL cg_upnpav_dmr_renderingctrl_init(CgUpnpAvRenderer *dmr);

BOOL cg_upnpav_dmr_conmgr_actionreceived(CgUpnpAction *action);
BOOL cg_upnpav_dmr_avtransport_actionreceived(CgUpnpAction *action);
BOOL cg_upnpav_dmr_renderingctrl_actionreceived(CgUpnpAction *action);

BOOL cg_upnpav_dmr_conmgr_queryreceived(CgUpnpStateVariable *statVar);
BOOL cg_upnpav_dmr_avtransport_queryreceived(CgUpnpStateVariable *statVar);
BOOL cg_upnpav_dmr_renderingctrl_queryreceived(CgUpnpStateVariable *statVar);

/****************************************
* cg_upnpav_dmr_actionreceived
****************************************/

BOOL cg_upnpav_dmr_actionreceived(CgUpnpAction *action)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAvRenderer *dmr;
	CG_UPNPAV_ACTION_LISTNER userActionListener;

	service = cg_upnp_action_getservice(action);
	if (!service)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(service);
	if (!dev) 
		return FALSE;

	dmr = (CgUpnpAvRenderer *)cg_upnp_device_getuserdata(dev);
	if (!dmr)
		return FALSE;

	userActionListener = cg_upnpav_dmr_getactionlistener(dmr);
	if (userActionListener) {
		if (userActionListener(action))
			return TRUE;
	}

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE))
		return cg_upnpav_dmr_avtransport_actionreceived(action);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE))
		return cg_upnpav_dmr_renderingctrl_actionreceived(action);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE))
		return cg_upnpav_dmr_conmgr_actionreceived(action);

	return FALSE;
}

/****************************************
* cg_upnpav_dmr_queryreceived
****************************************/

BOOL cg_upnpav_dmr_queryreceived(CgUpnpStateVariable *statVar)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAvRenderer *dmr;
	CG_UPNPAV_STATEVARIABLE_LISTNER userQueryListener;

	service = cg_upnp_statevariable_getservice(statVar);
	if (!service)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(service);
	if (!dev) 
		return FALSE;

	dmr = (CgUpnpAvRenderer *)cg_upnp_device_getuserdata(dev);
	if (!dmr)
		return FALSE;

	userQueryListener = cg_upnpav_dmr_getquerylistener(dmr);
	if (userQueryListener) {
		if (userQueryListener(statVar))
			return TRUE;
	}
	
	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE))
		return cg_upnpav_dmr_avtransport_queryreceived(statVar);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE))
		return cg_upnpav_dmr_renderingctrl_queryreceived(statVar);

	if (cg_streq(cg_upnp_service_getservicetype(service), CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE))
		return cg_upnpav_dmr_conmgr_queryreceived(statVar);

	return FALSE;
}

/****************************************
* cg_upnpav_dmr_device_httprequestrecieved
****************************************/

void cg_upnpav_dmr_device_httprequestrecieved(CgHttpRequest *httpReq)
{
	CgUpnpAvRenderer *dmr;
	CgUpnpDevice *dev;
	CG_UPNPAV_HTTP_LISTENER userHttpListener;

	dev = (CgUpnpDevice *)cg_http_request_getuserdata(httpReq);
	if (!dev) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	dmr = (CgUpnpAvRenderer *)cg_upnp_device_getuserdata(dev);
	if (!dmr) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	userHttpListener = cg_upnpav_dmr_gethttplistener(dmr);
	if (userHttpListener) {
		if (userHttpListener(httpReq))
			return;
	}
	
	cg_upnp_device_httprequestrecieved(httpReq);
}

/****************************************
* cg_upnpav_dmr_new
****************************************/

CgUpnpAvRenderer *cg_upnpav_dmr_new()
{
	CgUpnpAvRenderer *dmr;
	
	dmr = (CgUpnpAvRenderer *)malloc(sizeof(CgUpnpAvRenderer));

	dmr->dev = cg_upnp_device_new();
	if (!dmr->dev) {
		free(dmr);
		return NULL;
	}

	if (cg_upnp_device_parsedescription(dmr->dev, CG_UPNPAV_DMR_DEVICE_DESCRIPTION, cg_strlen(CG_UPNPAV_DMR_DEVICE_DESCRIPTION)) == FALSE) {
		cg_upnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	if (cg_upnpav_dmr_conmgr_init(dmr) == FALSE) {
		cg_upnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	if (cg_upnpav_dmr_renderingctrl_init(dmr) == FALSE) {
		cg_upnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}
	
	if (cg_upnpav_dmr_avtransport_init(dmr) == FALSE) {
		cg_upnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	dmr->mutex = cg_mutex_new();
	if (!dmr->mutex) {
		cg_upnpav_dmr_delete(dmr);
		return NULL;
	}

	cg_upnp_device_setactionlistener(dmr->dev, cg_upnpav_dmr_actionreceived);
	cg_upnp_device_setquerylistener(dmr->dev, cg_upnpav_dmr_queryreceived);
	cg_upnp_device_sethttplistener(dmr->dev, cg_upnpav_dmr_device_httprequestrecieved);

	cg_upnpav_dmr_setactionlistener(dmr, NULL);
	cg_upnpav_dmr_setquerylistener(dmr, NULL);
	cg_upnpav_dmr_sethttplistener(dmr, NULL);

	cg_upnp_device_setuserdata(dmr->dev, dmr);
	
	dmr->protocolInfoList = cg_upnpav_protocolinfolist_new();

	return dmr;
}

/****************************************
* cg_upnpav_dmr_delete
****************************************/

void cg_upnpav_dmr_delete(CgUpnpAvRenderer *dmr)
{
	if (dmr == NULL)
		return;

	if (dmr->mutex)
		cg_mutex_delete(dmr->mutex);

	if (dmr->protocolInfoList)
		cg_upnpav_protocolinfolist_delete(dmr->protocolInfoList);

	cg_upnp_device_delete(dmr->dev);

	free(dmr);
}

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

#include <mupnp/std/av/cmediarenderer.h>

/****************************************
* Device Description
****************************************/

static char *CG_UPNPAV_DMR_DEVICE_DESCRIPTION =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\n"
" <specVersion>\n"
"    <major>1</major>\n"
"    <minor>0</minor>\n"
" </specVersion>\n"
" <device>\n"
"    <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\n"
"    <friendlyName>Cyber Garage Media Renderer</friendlyName>\n"
"    <manufacturer>Cyber Garage</manufacturer>\n"
"    <manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
"    <modelDescription>CyberGarage</modelDescription>\n"
"    <modelName>Cyber Garage Media Renderer</modelName>\n"
"    <modelNumber>1.0</modelNumber>\n"
"    <UDN>uuid:BA2E90A0-3669-401a-B249-F85196ADFC44</UDN>\n"
"    <modelURL>http://www.cybergarage.org</modelURL>\n"
"    <dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMR-1.50</dlna:X_DLNADOC>\n"
"    <serviceList>\n"
"       <service>\n"
"          <serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>\n"
"          <serviceId>urn:schemas-upnp-org:service:RenderingControl</serviceId>\n"
"          <SCPDURL>/service/RenderingControl1.xml</SCPDURL>\n"
"          <controlURL>/service/RenderingControl_control</controlURL>\n"
"          <eventSubURL>/service/RenderingControl_event</eventSubURL>\n"
"       </service>\n"
"       <service>\n"
"          <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\n"
"          <serviceId>urn:schemas-upnp-org:service:ConnectionManager</serviceId>\n"
"          <SCPDURL>/service/ConnectionManager1.xml</SCPDURL>\n"
"          <controlURL>/service/ConnectionManager_control</controlURL>\n"
"          <eventSubURL>/service/ConnectionManager_event</eventSubURL>\n"
"       </service>\n"
"       <service>\n"
"          <serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>\n"
"          <serviceId>urn:schemas-upnp-org:service:AVTransport</serviceId>\n"
"          <SCPDURL>/service/AVTransport1.xml</SCPDURL>\n"
"          <controlURL>/service/AVTransport_control</controlURL>\n"
"          <eventSubURL>/service/AVTransport_event</eventSubURL>\n"
"       </service>\n"
"    </serviceList>\n"
" </device>\n"
"</root>\n";

/****************************************
* Functions
****************************************/

bool mupnp_upnpav_dmr_conmgr_init(mUpnpAvRenderer *dmr);
bool mupnp_upnpav_dmr_avtransport_init(mUpnpAvRenderer *dmr);
bool mupnp_upnpav_dmr_renderingctrl_init(mUpnpAvRenderer *dmr);

bool mupnp_upnpav_dmr_conmgr_actionreceived(mUpnpAction *action);
bool mupnp_upnpav_dmr_avtransport_actionreceived(mUpnpAction *action);
bool mupnp_upnpav_dmr_renderingctrl_actionreceived(mUpnpAction *action);

bool mupnp_upnpav_dmr_conmgr_queryreceived(mUpnpStateVariable *statVar);
bool mupnp_upnpav_dmr_avtransport_queryreceived(mUpnpStateVariable *statVar);
bool mupnp_upnpav_dmr_renderingctrl_queryreceived(mUpnpStateVariable *statVar);

/****************************************
 * mupnp_upnpav_dmr_addprotocolinfo
 ****************************************/

void mupnp_upnpav_dmr_addprotocolinfo(mUpnpAvRenderer *dmr, mUpnpAvProtocolInfo *info)
{
	mUpnpString *protocolInfos;
	mUpnpAvProtocolInfo *protocolInfo;
	mUpnpService *service;
	mUpnpStateVariable *stateVar;

	mupnp_upnpav_protocolinfolist_add(dmr->protocolInfoList, info);

	protocolInfos = mupnp_string_new();
	for (protocolInfo = mupnp_upnpav_dmr_getprotocolinfos(dmr); protocolInfo; protocolInfo = mupnp_upnpav_protocolinfo_next(protocolInfo)) {
		if (0 < mupnp_string_length(protocolInfos))
			mupnp_string_addvalue(protocolInfos, ",");
		mupnp_string_addvalue(protocolInfos, mupnp_upnpav_protocolinfo_getstring(protocolInfo));
	}

	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SINKPROTOCOLINFO);
	mupnp_statevariable_setvalue(stateVar, mupnp_string_getvalue(protocolInfos));

	mupnp_string_delete(protocolInfos);
}

/****************************************
 * mupnp_upnpav_dmr_setcurrentconnectionids
 ****************************************/

void mupnp_upnpav_dmr_setcurrentconnectionids(mUpnpAvRenderer *dmr, char *value)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;

	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_CURRENTCONNECTIONIDS);
	mupnp_statevariable_setvalue(stateVar, value);
}

/****************************************
 * mupnp_upnpav_dmr_getcurrentconnectionids
 ****************************************/

char *mupnp_upnpav_dmr_getcurrentconnectionids(mUpnpAvRenderer *dmr)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;

	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_CURRENTCONNECTIONIDS);
	return mupnp_statevariable_getvalue(stateVar);
}

/****************************************
* mupnp_upnpav_dmr_actionreceived
****************************************/

bool mupnp_upnpav_dmr_actionreceived(mUpnpAction *action)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAvRenderer *dmr;
	CG_UPNPAV_ACTION_LISTNER userActionListener;

	service = mupnp_action_getservice(action);
	if (!service)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(service);
	if (!dev)
		return false;

	dmr = (mUpnpAvRenderer *)mupnp_device_getuserdata(dev);
	if (!dmr)
		return false;

	userActionListener = mupnp_upnpav_dmr_getactionlistener(dmr);
	if (userActionListener) {
		if (userActionListener(action))
			return true;
	}

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE))
		return mupnp_upnpav_dmr_avtransport_actionreceived(action);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE))
		return mupnp_upnpav_dmr_renderingctrl_actionreceived(action);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE))
		return mupnp_upnpav_dmr_conmgr_actionreceived(action);

	return false;
}

/****************************************
* mupnp_upnpav_dmr_queryreceived
****************************************/

bool mupnp_upnpav_dmr_queryreceived(mUpnpStateVariable *statVar)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAvRenderer *dmr;
	CG_UPNPAV_STATEVARIABLE_LISTNER userQueryListener;

	service = mupnp_statevariable_getservice(statVar);
	if (!service)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(service);
	if (!dev)
		return false;

	dmr = (mUpnpAvRenderer *)mupnp_device_getuserdata(dev);
	if (!dmr)
		return false;

	userQueryListener = mupnp_upnpav_dmr_getquerylistener(dmr);
	if (userQueryListener) {
		if (userQueryListener(statVar))
			return true;
	}

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE))
		return mupnp_upnpav_dmr_avtransport_queryreceived(statVar);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE))
		return mupnp_upnpav_dmr_renderingctrl_queryreceived(statVar);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE))
		return mupnp_upnpav_dmr_conmgr_queryreceived(statVar);

	return false;
}

/****************************************
* mupnp_upnpav_dmr_device_httprequestrecieved
****************************************/

void mupnp_upnpav_dmr_device_httprequestrecieved(mUpnpHttpRequest *httpReq)
{
	mUpnpAvRenderer *dmr;
	mUpnpDevice *dev;
	CG_UPNPAV_HTTP_LISTENER userHttpListener;

	dev = (mUpnpDevice *)mupnp_http_request_getuserdata(httpReq);
	if (!dev) {
		mupnp_device_httprequestrecieved(httpReq);
		return;
	}

	dmr = (mUpnpAvRenderer *)mupnp_device_getuserdata(dev);
	if (!dmr) {
		mupnp_device_httprequestrecieved(httpReq);
		return;
	}

	userHttpListener = mupnp_upnpav_dmr_gethttplistener(dmr);
	if (userHttpListener) {
		if (userHttpListener(httpReq))
			return;
	}

	mupnp_device_httprequestrecieved(httpReq);
}

/****************************************
* mupnp_upnpav_dmr_new
****************************************/

mUpnpAvRenderer *mupnp_upnpav_dmr_new()
{
	mUpnpAvRenderer *dmr;
	char *lastChange;

	dmr = (mUpnpAvRenderer *)malloc(sizeof(mUpnpAvRenderer));

	dmr->dev = mupnp_device_new();
	if (!dmr->dev) {
		free(dmr);
		return NULL;
	}

	if (mupnp_device_parsedescription(dmr->dev, CG_UPNPAV_DMR_DEVICE_DESCRIPTION, mupnp_strlen(CG_UPNPAV_DMR_DEVICE_DESCRIPTION)) == false) {
		mupnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	if (mupnp_upnpav_dmr_conmgr_init(dmr) == false) {
		mupnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	if (mupnp_upnpav_dmr_renderingctrl_init(dmr) == false) {
		mupnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	if (mupnp_upnpav_dmr_avtransport_init(dmr) == false) {
		mupnp_device_delete(dmr->dev);
		free(dmr);
		return NULL;
	}

	dmr->mutex = mupnp_mutex_new();
	if (!dmr->mutex) {
		mupnp_upnpav_dmr_delete(dmr);
		return NULL;
	}

	mupnp_device_setactionlistener(dmr->dev, mupnp_upnpav_dmr_actionreceived);
	mupnp_device_setquerylistener(dmr->dev, mupnp_upnpav_dmr_queryreceived);
	mupnp_device_sethttplistener(dmr->dev, mupnp_upnpav_dmr_device_httprequestrecieved);

	mupnp_upnpav_dmr_setactionlistener(dmr, NULL);
	mupnp_upnpav_dmr_setquerylistener(dmr, NULL);
	mupnp_upnpav_dmr_sethttplistener(dmr, NULL);

	mupnp_device_setuserdata(dmr->dev, dmr);
	mupnp_device_updateudn(dmr->dev);

	dmr->protocolInfoList = mupnp_upnpav_protocolinfolist_new();

	mupnp_upnpav_dmr_setavtransportlastchange(dmr,  "&lt;Event xmlns = &quot;urn:schemas-upnp-org:metadata-1-0/AVT/&quot;/&gt;");

	mupnp_upnpav_dmr_setsourceprotocolinfo(dmr, "");
	mupnp_upnpav_dmr_setcurrentconnectionids(dmr, "0");

	lastChange = "&lt;Event xmlns = &quot;urn:schemas-upnp-org:metadata-1-0/RCS/&quot;&gt;"
	"&lt;InstanceID val=&quot;0&quot;&gt;"
	"&lt;Volume val=&quot;100&quot; channel=&quot;RF&quot;/&gt;"
	"&lt;Volume val=&quot;100&quot; channel=&quot;LF&quot;/&gt;"
	"&lt;/InstanceID&gt;"
	"&lt;/Event&gt;";

	mupnp_upnpav_dmr_setrenderingcontrollastchange(dmr, lastChange);

	return dmr;
}

/****************************************
* mupnp_upnpav_dmr_delete
****************************************/

void mupnp_upnpav_dmr_delete(mUpnpAvRenderer *dmr)
{
	if (dmr == NULL)
		return;

	if (dmr->mutex)
		mupnp_mutex_delete(dmr->mutex);

	if (dmr->protocolInfoList)
		mupnp_upnpav_protocolinfolist_delete(dmr->protocolInfoList);

	mupnp_device_delete(dmr->dev);

	free(dmr);
}

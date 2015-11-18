/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediaserver_devicer.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <mupnp/std/av/cmediaserver.h>

/****************************************
* Device Description
****************************************/

static char *CG_UPNPAV_DMS_DEVICE_DESCRIPTION =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <device>\n"
"      <deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>\n"
"      <friendlyName>Cyber Garage Media Server (MediaServer) : 1</friendlyName>\n"
"      <manufacturer>Cyber Garage</manufacturer>\n"
"      <manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
"      <modelDescription>CyberGarage</modelDescription>\n"
"      <modelName>CyberGarage Media Server</modelName>\n"
"      <modelNumber>1.0</modelNumber>\n"
"      <UDN>uuid:BA2E90A0-3669-401a-B249-F85196ADFC44</UDN>\n"
"      <modelURL>http://www.cybergarage.org</modelURL>\n"
"      <dlna:X_DLNADOC>DMS-1.00</dlna:X_DLNADOC>\n"
"      <serviceList>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>\n"
"            <serviceId>urn:upnp-org:serviceId:urn:schemas-upnp-org:service:ContentDirectory</serviceId>\n"
"            <SCPDURL>/service/ContentDirectory1.xml</SCPDURL>\n"
"            <controlURL>/service/ContentDirectory_control</controlURL>\n"
"            <eventSubURL>/service/ContentDirectory_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\n"
"            <serviceId>urn:upnp-org:serviceId:urn:schemas-upnp-org:service:ConnectionManager</serviceId>\n"
"            <SCPDURL>/service/ConnectionManager1.xml</SCPDURL>\n"
"            <controlURL>/service/ConnectionManager_control</controlURL>\n"
"            <eventSubURL>/service/ConnectionManager_event</eventSubURL>\n"
"         </service>\n"
"         <service>\n"
"            <serviceType>urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1</serviceType>\n"
"            <serviceId>urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</serviceId>\n"
"            <SCPDURL>/service/MediaReceiverRegistrar1.xml</SCPDURL>\n"
"            <controlURL>/service/MediaReceiverRegistrar_control</controlURL>\n"
"            <eventSubURL>/service/MediaReceiverRegistrar_event</eventSubURL>\n"
"         </service>\n"
"      </serviceList>\n"
"   </device>\n"
"</root>\n";

/****************************************
* Functions
****************************************/

bool mupnp_upnpav_dms_condir_actionreceived(mUpnpAction *action);
bool mupnp_upnpav_dms_conmgr_actionreceived(mUpnpAction *action);
bool mupnp_upnpav_dms_medrec_actionreceived(mUpnpAction *action);
bool mupnp_upnpav_dms_condir_queryreceived(mUpnpStateVariable *var);

/****************************************
* mupnp_upnpav_dms_actionreceived
****************************************/

bool mupnp_upnpav_dms_actionreceived(mUpnpAction *action)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAvServer *dms;
	CG_UPNPAV_ACTION_LISTNER userActionListener;

	service = mupnp_action_getservice(action);
	if (!service)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(service);
	if (!dev)
		return false;

	dms = (mUpnpAvServer *)mupnp_device_getuserdata(dev);
	if (!dms)
		return false;

	userActionListener = mupnp_upnpav_dms_getactionlistener(dms);
	if (userActionListener) {
		if (userActionListener(action))
			return true;
	}

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE))
		return mupnp_upnpav_dms_condir_actionreceived(action);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_TYPE))
		return mupnp_upnpav_dms_medrec_actionreceived(action);

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_TYPE))
		return mupnp_upnpav_dms_conmgr_actionreceived(action);

	return false;
}

/****************************************
* mupnp_upnpav_dms_queryreceived
****************************************/

bool mupnp_upnpav_dms_queryreceived(mUpnpStateVariable *statVar)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAvServer *dms;
	CG_UPNPAV_STATEVARIABLE_LISTNER userQueryListener;

	service = mupnp_statevariable_getservice(statVar);
	if (!service)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(service);
	if (!dev)
		return false;

	dms = (mUpnpAvServer *)mupnp_device_getuserdata(dev);
	if (!dms)
		return false;

	userQueryListener = mupnp_upnpav_dms_getquerylistener(dms);
	if (userQueryListener) {
		if (userQueryListener(statVar))
			return true;
	}

	if (mupnp_streq(mupnp_service_getservicetype(service), CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE))
		return mupnp_upnpav_dms_condir_queryreceived(statVar);

	return false;
}

/****************************************
* mupnp_upnpav_dms_device_httprequestrecieved
****************************************/

void mupnp_upnpav_dms_device_httprequestrecieved(mUpnpHttpRequest *httpReq)
{
	mUpnpAvServer *dms;
	mUpnpDevice *dev;
	CG_UPNPAV_HTTP_LISTENER userHttpListener;

	dev = (mUpnpDevice *)mupnp_http_request_getuserdata(httpReq);
	if (!dev) {
		mupnp_device_httprequestrecieved(httpReq);
		return;
	}

	dms = (mUpnpAvServer *)mupnp_device_getuserdata(dev);
	if (!dms) {
		mupnp_device_httprequestrecieved(httpReq);
		return;
	}

	userHttpListener = mupnp_upnpav_dms_gethttplistener(dms);
	if (userHttpListener) {
		if (userHttpListener(httpReq))
			return;
	}

	mupnp_device_httprequestrecieved(httpReq);
}

/****************************************
* mupnp_upnpav_dms_new
****************************************/

mUpnpAvServer *mupnp_upnpav_dms_new()
{
	mUpnpAvServer *dms;

	dms = (mUpnpAvServer *)malloc(sizeof(mUpnpAvServer));

	dms->dev = mupnp_device_new();
	if (!dms->dev) {
		free(dms);
		return NULL;
	}

	if (mupnp_device_parsedescription(dms->dev, CG_UPNPAV_DMS_DEVICE_DESCRIPTION, mupnp_strlen(CG_UPNPAV_DMS_DEVICE_DESCRIPTION)) == false) {
		mupnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (mupnp_upnpav_dms_conmgr_init(dms) == false) {
		mupnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (mupnp_upnpav_dms_condir_init(dms) == false) {
		mupnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	if (mupnp_upnpav_dms_medrec_init(dms) == false) {
		mupnp_device_delete(dms->dev);
		free(dms);
		return NULL;
	}

	dms->rootContent = mupnp_upnpav_content_new();
	mupnp_upnpav_content_settype(dms->rootContent, CG_UPNPAV_CONTENT_CONTAINER);
    mupnp_upnpav_content_settitle(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_TITLE);
	mupnp_upnpav_content_setid(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_ID);
	mupnp_upnpav_content_setparentid(dms->rootContent, CG_UPNPAV_ROOT_CONTENT_PARENTID);

	if (!dms->rootContent) {
		mupnp_upnpav_dms_delete(dms);
		return NULL;
	}

	dms->mutex = mupnp_mutex_new();
	if (!dms->mutex) {
		mupnp_upnpav_dms_delete(dms);
		return NULL;
	}

	dms->networkInterfaceList = mupnp_net_interfacelist_new();
	if (!dms->networkInterfaceList) {
		mupnp_upnpav_dms_delete(dms);
		return NULL;
	}

	mupnp_device_setactionlistener(dms->dev, mupnp_upnpav_dms_actionreceived);
	mupnp_device_setquerylistener(dms->dev, mupnp_upnpav_dms_queryreceived);
	mupnp_device_sethttplistener(dms->dev, mupnp_upnpav_dms_device_httprequestrecieved);

	mupnp_upnpav_dms_setactionlistener(dms, NULL);
	mupnp_upnpav_dms_setquerylistener(dms, NULL);
	mupnp_upnpav_dms_sethttplistener(dms, NULL);

	mupnp_device_setuserdata(dms->dev, dms);
	mupnp_device_updateudn(dms->dev);

	dms->protocolInfoList = mupnp_upnpav_protocolinfolist_new();

	return dms;
}

/****************************************
* mupnp_upnpav_dms_delete
****************************************/

void mupnp_upnpav_dms_delete(mUpnpAvServer *dms)
{
	if (dms == NULL)
		return;

	if (dms->rootContent)
		mupnp_upnpav_content_delete(dms->rootContent);

	if (dms->mutex)
		mupnp_mutex_delete(dms->mutex);

	if (dms->protocolInfoList)
		mupnp_upnpav_protocolinfolist_delete(dms->protocolInfoList);

	if (dms->networkInterfaceList)
		mupnp_net_interfacelist_delete(dms->networkInterfaceList);

	mupnp_device_delete(dms->dev);

	free(dms);
}

/****************************************
 * mupnp_upnpav_dms_updatenetworkinterfaces
 ****************************************/

bool mupnp_upnpav_dms_updatenetworkinterfaces(mUpnpAvServer *dms)
{
	mupnp_upnpav_dms_lock(dms);
	mupnp_net_interfacelist_clear(dms->networkInterfaceList);
	mupnp_net_gethostinterfaces(dms->networkInterfaceList);
	mupnp_upnpav_dms_unlock(dms);
	return (0 < mupnp_net_interfacelist_size(dms->networkInterfaceList)) ? true : false;
}

/****************************************
 * mupnp_upnpav_dms_getnetworkinterface
 ****************************************/

mUpnpNetworkInterface *mupnp_upnpav_dms_getnetworkinterface(mUpnpAvServer *dms)
{
	mUpnpNetworkInterface *netIf = mupnp_upnpav_dms_getnetworkinterfaces(dms);
	if (netIf)
		return netIf;
	return NULL;
}

/****************************************
* mupnp_upnpav_dms_findcontentbyname
****************************************/

mUpnpAvContent *mupnp_upnpav_dms_findcontentbytitle(mUpnpAvServer *dms, char *title)
{
	return mupnp_upnpav_content_getbytitle(mupnp_upnpav_dms_getrootcontent(dms), title);
}

/****************************************
* mupnp_upnpav_dms_findcontentbyobjectid
****************************************/

mUpnpAvContent *mupnp_upnpav_dms_findcontentbyid(mUpnpAvServer *dms, char *objID)
{
	return mupnp_upnpav_content_getbyid(mupnp_upnpav_dms_getrootcontent(dms), objID);
}


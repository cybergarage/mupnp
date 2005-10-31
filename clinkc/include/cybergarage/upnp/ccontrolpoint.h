/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontrolpoint.h
*
*	Revision:
*
*	05/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CCONTROLPOINT_H_
#define _CG_UPNP_CCONTROLPOINT_H_

#include <cybergarage/util/cstring.h>
#include <cybergarage/util/cmutex.h>
#include <cybergarage/xml/cxml.h>

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cargument.h>
#include <cybergarage/upnp/cupnp_status.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/event/cevent.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_CONTROLPOINT_SSDP_DEFAULT_SEARCH_MX 3

#define CG_UPNP_CONTROLPOINT_SSDP_RESPONSE_DEFAULT_PORT 39400
#define CG_UPNP_CONTROLPOINT_HTTP_EVENT_DEFAULT_PORT 39500
#define CG_UPNP_CONTROLPOINT_HTTP_EVENTSUB_URI "/eventSub"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpControlPoint {
	CgMutex *mutex;
	CgXmlNodeList *deviceRootNodeList;
	CgUpnpDeviceList *deviceList;
	CgUpnpSSDPServerList *ssdpServerList;
	CgUpnpSSDPResponseServerList *ssdpResServerList;
	CgHttpServerList *httpServerList;
	CG_HTTP_LISTENER httpListener;
	CG_UPNP_SSDP_LISTNER ssdpListener;
	CG_UPNP_SSDP_RESPONSE_LISTNER ssdpResListener;
	CG_UPNP_EVENT_LISTENER eventListener;
	int ssdpResPort;
	CgString *httpEventURI;
	int httpEventPort;
	int ssdpSearchMx;
	void *userData;
} CgUpnpControlPoint;

/****************************************
* Function 
****************************************/

CgUpnpControlPoint *cg_upnp_controlpoint_new();
void cg_upnp_controlpoint_delete(CgUpnpControlPoint *ctrlPoint);

BOOL cg_upnp_controlpoint_start(CgUpnpControlPoint *ctrlPoint);
BOOL cg_upnp_controlpoint_stop(CgUpnpControlPoint *ctrlPoint);

CgUpnpDevice *cg_upnp_controlpoint_getdevicebyname(CgUpnpControlPoint *ctrlPoint, char *name);

#define cg_upnp_controlpoint_getssdpserverlist(ctrlPoint) (ctrlPoint->ssdpServerList)
#define cg_upnp_controlpoint_getssdpresponseserverlist(ctrlPoint) (ctrlPoint->ssdpResServerList)
#define cg_upnp_controlpoint_gethttpserverlist(ctrlPoint) (ctrlPoint->httpServerList)

#define cg_upnp_controlpoint_setssdplistener(ctrlPoint, func) (ctrlPoint->ssdpListener = func)
#define cg_upnp_controlpoint_getssdplistener(ctrlPoint) (ctrlPoint->ssdpListener)

#define cg_upnp_controlpoint_setssdpresponselistener(ctrlPoint, func) (ctrlPoint->ssdpResListener = func)
#define cg_upnp_controlpoint_getssdpresponselistener(ctrlPoint) (ctrlPoint->ssdpResListener)

#define cg_upnp_controlpoint_seteventlistener(ctrlPoint, func) (ctrlPoint->eventListener = func)
#define cg_upnp_controlpoint_geteventlistener(ctrlPoint) (ctrlPoint->eventListener)

BOOL cg_upnp_controlpoint_lock(CgUpnpControlPoint *ctrlPoint);
BOOL cg_upnp_controlpoint_unlock(CgUpnpControlPoint *ctrlPoint);

#define cg_upnp_controlpoint_setssdpresponseport(ctrlPoint, port) (ctrlPoint->ssdpResPort = port)
#define cg_upnp_controlpoint_getssdpresponseport(ctrlPoint) (ctrlPoint->ssdpResPort)

#define cg_upnp_controlpoint_seteventport(ctrlPoint, port) (ctrlPoint->httpEventPort = port)
#define cg_upnp_controlpoint_geteventport(ctrlPoint) (ctrlPoint->httpEventPort)

#define cg_upnp_controlpoint_seteventsuburi(ctrlPoint, uri) cg_string_setvalue(ctrlPoint->httpEventURI, uri)
#define cg_upnp_controlpoint_geteventsuburi(ctrlPoint) cg_string_getvalue(ctrlPoint->httpEventURI)

BOOL cg_upnp_controlpoint_search(CgUpnpControlPoint *ctrlPoint, char *target);
#define cg_upnp_controlpoint_setssdpsearchmx(ctrlPoint, value) (ctrlPoint->ssdpSearchMx = value)
#define cg_upnp_controlpoint_getssdpsearchmx(ctrlPoint) (ctrlPoint->ssdpSearchMx)

#define cg_upnp_controlpoint_sethttplistener(ctrlPoint, func) (ctrlPoint->httpListener = func)
#define cg_upnp_controlpoint_gethttplistener(ctrlPoint) (ctrlPoint->httpListener)
void cg_upnp_controlpoint_httprequestrecieved(CgHttpRequest *httpReq);

/**** User Data****/
#define cg_upnp_controlpoint_setuserdata(ctrlPoint, value) (ctrlPoint->userData = value)
#define cg_upnp_controlpoint_getuserdata(dev) (ctrlPoint->userData)

/****************************************
* Function (Subscription)
****************************************/

BOOL cg_upnp_controlpoint_subscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout);
BOOL cg_upnp_controlpoint_resubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service, long timeout);
BOOL cg_upnp_controlpoint_unsubscribe(CgUpnpControlPoint *ctrlPoint, CgUpnpService *service);

BOOL cg_upnp_controlpoint_subscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout);
BOOL cg_upnp_controlpoint_resubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev, long timeout);
BOOL cg_upnp_controlpoint_unsubscribeall(CgUpnpControlPoint *ctrlPoint, CgUpnpDevice *dev);

/****************************************
* Function (DeviceList)
****************************************/

#define cg_upnp_controlpoint_getdevices(ctrlPoint) cg_upnp_devicelist_gets(ctrlPoint->deviceList)
#define cg_upnp_controlpoint_getndevices(ctrlPoint) cg_upnp_devicelist_size(ctrlPoint->deviceList)
#define cg_upnp_controlpoint_adddevice(ctrlPoint, dev) cg_upnp_devicelist_add(ctrlPoint->deviceList, dev)

#ifdef  __cplusplus
}
#endif

#endif

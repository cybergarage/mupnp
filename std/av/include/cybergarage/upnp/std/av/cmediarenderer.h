/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediarenderer.h
*
*	Revision:
*		2009/06/11
*        - first release.
*
************************************************************/

#ifndef _CG_CLINKC_AV_MEDIARENDERER_H_
#define _CG_CLINKC_AV_MEDIARENDERER_H_

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/std/av/cupnpav.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Struct
****************************************/

typedef struct _CgUpnpAvRenderer {
CgMutex *mutex;
CgUpnpDevice *dev;
CG_UPNPAV_HTTP_LISTENER httplistener;
CG_UPNPAV_ACTION_LISTNER actionListner;
CG_UPNPAV_STATEVARIABLE_LISTNER queryListner;
CgUpnpAvProtocolInfoList *protocolInfoList;
void *userData;
} CgUpnpAvRenderer;

/****************************************
* Constants (Media Server)
****************************************/

#define CG_UPNPAV_DMR_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaRenderer:1"
#define CG_UPNPAV_DMR_DEFAULT_HTTP_PORT 38520

/****************************************
* Constants (Rendering Control)
****************************************/

#define CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE "urn:schemas-upnp-org:service:RenderingControl:1"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_GETVOLUME "GetVolume"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_GETMUTE "GetMute"

/****************************************
* Constants (Connection Manager)
****************************************/

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_HTTP_GET "http-get"
	
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_PROTOCOL_INFO "GetProtocolInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SOURCE "Source"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SINK "Sink"
	
/****************************************
* Constants (AVTransport)
****************************************/

#define CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE "urn:schemas-upnp-org:service:AVTransport:1"

#define CG_UPNPAV_DMR_AVTRANSPORT_GETTRANSPORTINFO "GetTransportInfo"
#define CG_UPNPAV_DMR_AVTRANSPORT_STOP "Stop"

/****************************************
* Public Functions
****************************************/

CgUpnpAvRenderer *cg_upnpav_dmr_new();
void cg_upnpav_dmr_delete(CgUpnpAvRenderer *dmr);

#define cg_upnpav_dmr_getdevice(dmr) (dmr->dev)

#define cg_upnpav_dmr_start(dmr) cg_upnpav_device_start(dmr->dev)
#define cg_upnpav_dmr_stop(dmr) cg_upnpav_device_stop(dmr->dev)

#define cg_upnpav_dmr_lock(dmr) cg_mutex_lock(dmr->mutex)
#define cg_upnpav_dmr_unlock(dmr) cg_mutex_unlock(dmr->mutex)

#define cg_upnpav_dmr_setfriendlyname(dmr, value) cg_upnpav_device_setfriendlyname(dmr->dev, value)
#define cg_upnpav_dmr_getfriendlyname(dmr) cg_upnpav_device_getfriendlyname(dmr->dev)

#define cg_upnpav_dmr_setudn(dmr, value) cg_upnpav_device_setudn(dmr->dev, value)
#define cg_upnpav_dmr_getudn(dmr) cg_upnpav_device_getudn(dmr->dev)

#define cg_upnpav_dmr_sethttplistener(dmr,func) (dmr->httplistener = func)
#define cg_upnpav_dmr_gethttplistener(dmr) (dmr->httplistener)

#define cg_upnpav_dmr_setactionlistener(dmr,func) (dmr->actionListner = func)
#define cg_upnpav_dmr_getactionlistener(dmr) (dmr->actionListner)

#define cg_upnpav_dmr_setquerylistener(dmr,func) (dmr->queryListner = func)
#define cg_upnpav_dmr_getquerylistener(dmr) (dmr->queryListner)

#define cg_upnpav_dmr_setuserdata(dmr,data) (dmr->userData = data)
#define cg_upnpav_dmr_getuserdata(dmr) (dmr->userData)

#define cg_upnpav_dmr_addprotocolinfo(dms, info) cg_upnpav_protocolinfolist_add(dms->protocolInfoList, info)
#define cg_upnpav_dmr_getprotocolinfos(dms) cg_upnpav_protocolinfolist_gets(dms->protocolInfoList)

#ifdef  __cplusplus
}
#endif

#endif

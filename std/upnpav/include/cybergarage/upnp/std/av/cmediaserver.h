/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediaserver.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKC_MEDIASERVER_H_
#define _CG_CLINKC_MEDIASERVER_H_

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/std/av/cdidl.h>
#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/upnp/std/av/cmd5.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Struct
****************************************/

typedef struct _CgUpnpMediaServer {
CgMutex *mutex;
CgUpnpDevice *dev;
CgUpnpMediaContent *rootContent;
int systemUpdateID;
CG_HTTP_LISTENER httplistener;
CG_UPNP_ACTION_LISTNER actionListner;
CG_UPNP_STATEVARIABLE_LISTNER queryListner;
void *userData;
} CgUpnpMediaServer;

/****************************************
* Constants (Media Server)
****************************************/

#define CG_UPNP_DMS_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define CG_UPNP_DMS_DEFAULT_HTTP_PORT 38520

/****************************************
* Constants (Content Directory)
****************************************/

#define CG_UPNP_DMS_CONTENTDIRECTORY_SERVICE_TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"

#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE "Browse"
	
#define CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH "Search"

#define CG_UPNP_DMS_CONTENTDIRECTORY_GET_SEARCH_CAPABILITIES "GetSearchCapabilities"
#define CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH_CAPS "SearchCaps"
	
#define CG_UPNP_DMS_CONTENTDIRECTORY_GET_SORT_CAPABILITIES "GetSortCapabilities"
#define CG_UPNP_DMS_CONTENTDIRECTORY_SORT_CAPS "SortCaps"
	
#define CG_UPNP_DMS_CONTENTDIRECTORY_GET_SYSTEM_UPDATE_ID "GetSystemUpdateID"
#define CG_UPNP_DMS_CONTENTDIRECTORY_ID "Id"

#define CG_UPNP_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID "SystemUpdateID"
#define CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH_CAPABILITIES "SearchCapabilities"
#define CG_UPNP_DMS_CONTENTDIRECTORY_SORT_CAPABILITIES "SortCapabilities"
	
#define CG_UPNP_DMS_CONTENTDIRECTORY_CONTENT_EXPORT_URI "/ExportContent"
#define CG_UPNP_DMS_CONTENTDIRECTORY_CONTENT_IMPORT_URI "/ImportContent"
#define CG_UPNP_DMS_CONTENTDIRECTORY_CONTENT_ID "id"

#define CG_UPNP_DMS_CONTENTDIRECTORY_DEFAULT_SYSTEMUPDATEID_INTERVAL 2000
#define CG_UPNP_DMS_CONTENTDIRECTORY_DEFAULT_CONTENTUPDATE_INTERVAL 60000

#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID "ObjectID"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_FLAG "BrowseFlag"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_FILTER "Filter"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_STARTING_INDEX "StartingIndex"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_REQUESTED_COUNT "RequestedCount"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_SORT_CRITERIA "SortCriteria"
	
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_METADATA "BrowseMetadata"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_DIRECT_CHILDREN "BrowseDirectChildren"

#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_RESULT "Result"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED "NumberReturned"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES "TotalMatches"
#define CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID "UpdateID"

/****************************************
* Constants (Connection Manager)
****************************************/

#define CG_UPNP_DMS_CONNECTIONMANAGER_SERVICE_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"

#define CG_UPNP_DMS_CONNECTIONMANAGER_HTTP_GET "http-get"
	
#define CG_UPNP_DMS_CONNECTIONMANAGER_GET_PROTOCOL_INFO "GetProtocolInfo"
#define CG_UPNP_DMS_CONNECTIONMANAGER_SOURCE "Source"
#define CG_UPNP_DMS_CONNECTIONMANAGER_SINK "Sink"

#define CG_UPNP_DMS_CONNECTIONMANAGER_PREPARE_FOR_CONNECTION "PrepareForConnection"
#define CG_UPNP_DMS_CONNECTIONMANAGER_REMOTE_PROTOCOL_INFO "RemoteProtocolInfo"
#define CG_UPNP_DMS_CONNECTIONMANAGER_PEER_CONNECTION_MANAGER "PeerConnectionManager"
#define CG_UPNP_DMS_CONNECTIONMANAGER_PEER_CONNECTION_ID "PeerConnectionID"
#define CG_UPNP_DMS_CONNECTIONMANAGER_DIRECTION "Direction"
#define CG_UPNP_DMS_CONNECTIONMANAGER_CONNECTION_ID "ConnectionID"
#define CG_UPNP_DMS_CONNECTIONMANAGER_AV_TRNSPORT_ID "AVTransportID"
#define CG_UPNP_DMS_CONNECTIONMANAGER_RCS_ID "RcsID"
	
#define CG_UPNP_DMS_CONNECTIONMANAGER_CONNECTION_COMPLETE "ConnectionComplete"

#define CG_UPNP_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_IDS "GetCurrentConnectionIDs"
#define CG_UPNP_DMS_CONNECTIONMANAGER_CONNECTION_IDS "ConnectionIDs"

#define CG_UPNP_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_INFO "GetCurrentConnectionInfo"
#define CG_UPNP_DMS_CONNECTIONMANAGER_PROTOCOL_INFO "ProtocolInfo"
#define CG_UPNP_DMS_CONNECTIONMANAGER_STATUS "Status"
#define CG_UPNP_DMS_CONNECTIONMANAGER_INPUT "Input"
#define CG_UPNP_DMS_CONNECTIONMANAGER_OUTPUT "Output"
#define CG_UPNP_DMS_CONNECTIONMANAGER_OK "OK"

/****************************************
* Public Functions
****************************************/

CgUpnpMediaServer *cg_upnp_dms_new();
void cg_upnp_dms_delete(CgUpnpMediaServer *dms);

#define cg_upnp_dms_getdevice(dms) (dms->dev)
#define cg_upnp_dms_getrootcontent(dms) (dms->rootContent)

#define cg_upnp_dms_start(dms) cg_upnp_device_start(dms->dev)
#define cg_upnp_dms_stop(dms) cg_upnp_device_stop(dms->dev)

#define cg_upnp_dms_lock(dms) cg_mutex_lock(dms->mutex)
#define cg_upnp_dms_unlock(dms) cg_mutex_unlock(dms->mutex)

#define cg_upnp_dms_setfriendlyname(dms, value) cg_upnp_device_setfriendlyname(dms->dev, value)
#define cg_upnp_dms_getfriendlyname(dms) cg_upnp_device_getfriendlyname(dms->dev)

#define cg_upnp_dms_setudn(dms, value) cg_upnp_device_setudn(dms->dev, value)
#define cg_upnp_dms_getudn(dms) cg_upnp_device_getudn(dms->dev)

CgUpnpMediaContent *cg_upnp_dms_findcontentbytitle(CgUpnpMediaServer *dms, char *name);
CgUpnpMediaContent *cg_upnp_dms_findcontentbyid(CgUpnpMediaServer *dms, char *objectID);

#define cg_upnp_dms_sethttplistener(dms,func) (dms->httplistener = func)
#define cg_upnp_dms_gethttplistener(dms) (dms->httplistener)

#define cg_upnp_dms_setactionlistener(dms,func) (dms->actionListner = func)
#define cg_upnp_dms_getactionlistener(dms) (dms->actionListner)

#define cg_upnp_dms_setquerylistener(dms,func) (dms->queryListner = func)
#define cg_upnp_dms_getquerylistener(dms) (dms->queryListner)

#define cg_upnp_dms_setuserdata(dms,data) (dms->userData = data)
#define cg_upnp_dms_getuserdata(dms) (dms->userData)

/****************************************
* Private Functions
****************************************/

BOOL cg_upnp_dms_conmgr_init(CgUpnpMediaServer *dms);
BOOL cg_upnp_dms_condir_init(CgUpnpMediaServer *dms);
void cg_upnp_dms_condir_updatesystemupdateid(CgUpnpMediaServer *dms);

#ifdef  __cplusplus
}
#endif

#endif

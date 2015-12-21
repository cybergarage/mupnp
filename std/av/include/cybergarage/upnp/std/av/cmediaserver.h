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

#ifndef _CG_CLINKCAV_SERVER_H_
#define _CG_CLINKCAV_SERVER_H_

#include <mupnp/upnp.h>
#include <mupnp/std/av/cupnpav.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Struct
****************************************/

typedef struct _mUpnpAvServer {
  mUpnpMutex* mutex;
  mUpnpDevice* dev;
  mUpnpAvContent* rootContent;
  int systemUpdateID;
  CG_UPNPAV_HTTP_LISTENER httplistener;
  CG_UPNPAV_ACTION_LISTNER actionListner;
  CG_UPNPAV_STATEVARIABLE_LISTNER queryListner;
  mUpnpAvProtocolInfoList* protocolInfoList;
  mUpnpNetworkInterfaceList* networkInterfaceList;
  void* userData;
} mUpnpAvServer;

/****************************************
* Constants (Media Server)
****************************************/

#define CG_UPNPAV_DMS_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define CG_UPNPAV_DMS_DEFAULT_HTTP_PORT 38520

/****************************************
* Constants (Content Directory)
****************************************/

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE "Browse"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH "Search"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SEARCH_CAPABILITIES "GetSearchCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPS "SearchCaps"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SORT_CAPABILITIES "GetSortCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPS "SortCaps"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SYSTEM_UPDATE_ID "GetSystemUpdateID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_ID "Id"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID "SystemUpdateID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPABILITIES "SearchCapabilities"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPABILITIES "SortCapabilities"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_EXPORT_URI "/ExportContent"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_IMPORT_URI "/ImportContent"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_CONTENT_ID "id"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_DEFAULT_SYSTEMUPDATEID_INTERVAL 2000
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_DEFAULT_CONTENTUPDATE_INTERVAL 60000

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID "ObjectID"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_FLAG "BrowseFlag"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_FILTER "Filter"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_STARTING_INDEX "StartingIndex"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_REQUESTED_COUNT "RequestedCount"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_SORT_CRITERIA "SortCriteria"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_METADATA "BrowseMetadata"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_DIRECT_CHILDREN "BrowseDirectChildren"

#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_RESULT "Result"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED "NumberReturned"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES "TotalMatches"
#define CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID "UpdateID"

/****************************************
* Constants (Connection Manager)
****************************************/

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SERVICE_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_HTTP_GET "http-get"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_PROTOCOL_INFO "GetProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SOURCE "Source"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_SINK "Sink"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PREPARE_FOR_CONNECTION "PrepareForConnection"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_REMOTE_PROTOCOL_INFO "RemoteProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PEER_CONNECTION_MANAGER "PeerConnectionManager"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PEER_CONNECTION_ID "PeerConnectionID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_DIRECTION "Direction"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_ID "ConnectionID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_AV_TRNSPORT_ID "AVTransportID"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_RCS_ID "RcsID"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_COMPLETE "ConnectionComplete"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_IDS "GetCurrentConnectionIDs"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_CONNECTION_IDS "ConnectionIDs"

#define CG_UPNPAV_DMS_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_INFO "GetCurrentConnectionInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_PROTOCOL_INFO "ProtocolInfo"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_STATUS "Status"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_INPUT "Input"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_OUTPUT "Output"
#define CG_UPNPAV_DMS_CONNECTIONMANAGER_OK "OK"

/****************************************
* Constants (Media Receiver, Microsoft)
****************************************/

#define CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_TYPE "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1"
#define CG_UPNPAV_DMS_MEDIARECEIVER_REGISTRATION_RESP_MSG "RegistrationRespMsg"
#define CG_UPNPAV_DMS_MEDIARECEIVER_REGISTER_DEVICE "RegisterDevice"
#define CG_UPNPAV_DMS_MEDIARECEIVER_IS_AUTHORIZED "IsAuthorized"
#define CG_UPNPAV_DMS_MEDIARECEIVER_IS_VALIDATED "IsValidated"
#define CG_UPNPAV_DMS_MEDIARECEIVER_RESULT "Result"

/****************************************
* Public Functions
****************************************/

mUpnpAvServer* mupnp_upnpav_dms_new();
void mupnp_upnpav_dms_delete(mUpnpAvServer* dms);

#define mupnp_upnpav_dms_getdevice(dms) (dms->dev)
#define mupnp_upnpav_dms_getrootcontent(dms) (dms->rootContent)

#define mupnp_upnpav_dms_start(dms) mupnp_device_start(dms->dev)
#define mupnp_upnpav_dms_stop(dms) mupnp_device_stop(dms->dev)

#define mupnp_upnpav_dms_lock(dms) mupnp_mutex_lock(dms->mutex)
#define mupnp_upnpav_dms_unlock(dms) mupnp_mutex_unlock(dms->mutex)

#define mupnp_upnpav_dms_setfriendlyname(dms, value) mupnp_device_setfriendlyname(dms->dev, value)
#define mupnp_upnpav_dms_getfriendlyname(dms) mupnp_device_getfriendlyname(dms->dev)

#define mupnp_upnpav_dms_setudn(dms, value) mupnp_device_setudn(dms->dev, value)
#define mupnp_upnpav_dms_getudn(dms) mupnp_device_getudn(dms->dev)

mUpnpAvContent* mupnp_upnpav_dms_findcontentbytitle(mUpnpAvServer* dms, char* name);
mUpnpAvContent* mupnp_upnpav_dms_findcontentbyid(mUpnpAvServer* dms, char* objectID);

#define mupnp_upnpav_dms_sethttplistener(dms, func) (dms->httplistener = func)
#define mupnp_upnpav_dms_gethttplistener(dms) (dms->httplistener)

#define mupnp_upnpav_dms_setactionlistener(dms, func) (dms->actionListner = func)
#define mupnp_upnpav_dms_getactionlistener(dms) (dms->actionListner)

#define mupnp_upnpav_dms_setquerylistener(dms, func) (dms->queryListner = func)
#define mupnp_upnpav_dms_getquerylistener(dms) (dms->queryListner)

#define mupnp_upnpav_dms_setuserdata(dms, data) (dms->userData = data)
#define mupnp_upnpav_dms_getuserdata(dms) (dms->userData)

#define mupnp_upnpav_dms_addprotocolinfo(dms, info) mupnp_upnpav_protocolinfolist_add(dms->protocolInfoList, info)
#define mupnp_upnpav_dms_getprotocolinfos(dms) mupnp_upnpav_protocolinfolist_gets(dms->protocolInfoList)

/****************************************
* Connection Manager
****************************************/

BOOL mupnp_upnpav_dms_conmgr_init(mUpnpAvServer* dms);

/****************************************
* Network Interface
****************************************/

#define mupnp_upnpav_dms_getnetworkinterfaces(dms) mupnp_net_interfacelist_gets(dms->networkInterfaceList)
#define mupnp_upnpav_dms_getnnetworkinterfaces(dms) mupnp_net_interfacelist_size(dms->networkInterfaceList)

BOOL mupnp_upnpav_dms_updatenetworkinterfaces(mUpnpAvServer* dms);
mUpnpNetworkInterface* mupnp_upnpav_dms_getnetworkinterface(mUpnpAvServer* dms);

/****************************************
* Content Directory
****************************************/

BOOL mupnp_upnpav_dms_condir_init(mUpnpAvServer* dms);

void mupnp_upnpav_dms_condir_setsystemupdateid(mUpnpAvServer* dms, int id);
int mupnp_upnpav_dms_condir_getsystemupdateid(mUpnpAvServer* dms);
void mupnp_upnpav_dms_condir_updatesystemupdateid(mUpnpAvServer* dms);

/****************************************
* Media Receiveer
****************************************/

BOOL mupnp_upnpav_dms_medrec_init(mUpnpAvServer* dms);

#ifdef __cplusplus
}
#endif

#endif

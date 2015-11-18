/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2009
*
*	File: cconnectionmgr_service.c
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
#include <mupnp/std/av/ccontent.h>
#include <mupnp/std/av/cresource.h>

/****************************************
* Service Description (Connection Manager)
****************************************/

static char *CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION = 
#if defined(CG_CLINKCAV_USE_UPNPSTD_XML)
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <actionList>\n"
"      <action>\n"
"         <name>GetCurrentConnectionIDs</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ConnectionIDs</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetCurrentConnectionInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ConnectionID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RcsID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>AVTransportID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>ProtocolInfo</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PeerConnectionManager</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>PeerConnectionID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Direction</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Status</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetProtocolInfo</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>Source</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Sink</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"   </actionList>\n"
"   <serviceStateTable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionStatus</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>OK</allowedValue>\n"
"            <allowedValue>ContentFormatMismatch</allowedValue>\n"
"            <allowedValue>InsufficientBandwidth</allowedValue>\n"
"            <allowedValue>UnreliableChannel</allowedValue>\n"
"            <allowedValue>Unknown</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_AVTransportID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_RcsID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionID</name>\n"
"         <dataType>i4</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_ConnectionManager</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>SourceProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>SinkProtocolInfo</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"no\">\n"
"         <name>A_ARG_TYPE_Direction</name>\n"
"         <dataType>string</dataType>\n"
"         <allowedValueList>\n"
"            <allowedValue>Input</allowedValue>\n"
"            <allowedValue>Output</allowedValue>\n"
"         </allowedValueList>\n"
"      </stateVariable>\n"
"      <stateVariable sendEvents=\"yes\">\n"
"         <name>CurrentConnectionIDs</name>\n"
"         <dataType>string</dataType>\n"
"      </stateVariable>\n"
"   </serviceStateTable>\n"
"</scpd>\n";
#else
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"    <specVersion>\n"
"       <major>1</major>\n"
"       <minor>0</minor>\n"
"    </specVersion>\n"
"    <actionList>\n"
"       <action>\n"
"          <name>GetCurrentConnectionIDs</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>ConnectionIDs</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>GetCurrentConnectionInfo</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>ConnectionID</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>RcsID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>AVTransportID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>ProtocolInfo</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>PeerConnectionManager</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>PeerConnectionID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Direction</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Status</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>GetProtocolInfo</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>Source</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Sink</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"    </actionList>\n"
"    <serviceStateTable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_ProtocolInfo</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_ConnectionStatus</name>\n"
"          <dataType>string</dataType>\n"
"          <allowedValueList>\n"
"             <allowedValue>OK</allowedValue>\n"
"             <allowedValue>ContentFormatMismatch</allowedValue>\n"
"             <allowedValue>InsufficientBandwidth</allowedValue>\n"
"             <allowedValue>UnreliableChannel</allowedValue>\n"
"             <allowedValue>Unknown</allowedValue>\n"
"          </allowedValueList>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_AVTransportID</name>\n"
"          <dataType>i4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_RcsID</name>\n"
"          <dataType>i4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_ConnectionID</name>\n"
"          <dataType>i4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_ConnectionManager</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>SourceProtocolInfo</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>SinkProtocolInfo</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_Direction</name>\n"
"          <dataType>string</dataType>\n"
"          <allowedValueList>\n"
"             <allowedValue>Input</allowedValue>\n"
"             <allowedValue>Output</allowedValue>\n"
"          </allowedValueList>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>CurrentConnectionIDs</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"    </serviceStateTable>\n"
" </scpd>\n";
#endif

/****************************************
* mupnp_upnpav_dmr_conmgr_actionreceived
****************************************/

bool mupnp_upnpav_dmr_conmgr_actionreceived(mUpnpAction *action)
{
	mUpnpAvRenderer *dmr;
	mUpnpDevice *dev;
	char *actionName;
	mUpnpArgument *arg;
	mUpnpString *protocolInfos;
	mUpnpAvProtocolInfo *protocolInfo;
	
	actionName = (char*)mupnp_action_getname(action);
	if (mupnp_strlen(actionName) <= 0)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(mupnp_action_getservice(action));
	if (!dev) 
		return false;

	dmr = (mUpnpAvRenderer *)mupnp_device_getuserdata(dev);
	if (!dmr)
		return false;
	
	/* GetProtocolInfo*/
	if (mupnp_streq(actionName, CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_PROTOCOL_INFO)) {
		arg = mupnp_action_getargumentbyname(action, CG_UPNPAV_DMR_CONNECTIONMANAGER_SINK);
		if (!arg)
			return false;
		protocolInfos = mupnp_string_new();
		for (protocolInfo = mupnp_upnpav_dmr_getprotocolinfos(dmr); protocolInfo; protocolInfo = mupnp_upnpav_protocolinfo_next(protocolInfo)) {
			if (0 < mupnp_string_length(protocolInfos))
				mupnp_string_addvalue(protocolInfos, ",");
			mupnp_string_addvalue(protocolInfos, mupnp_upnpav_protocolinfo_getstring(protocolInfo));
		}
		mupnp_argument_setvalue(arg, mupnp_string_getvalue(protocolInfos));
		mupnp_string_delete(protocolInfos);
		return true;
	}

	return false;
}

/****************************************
 * mupnp_upnpav_dmr_conmgr_queryreceived
 ****************************************/

bool mupnp_upnpav_dmr_conmgr_queryreceived(mUpnpStateVariable *statVar)
{
	return false;
}

/****************************************
* mupnp_upnpav_dmr_conmgr_init
****************************************/

bool mupnp_upnpav_dmr_conmgr_init(mUpnpAvRenderer *dmr)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAction *action;

	dev = mupnp_upnpav_dmr_getdevice(dmr);
	if (!dev)
		return false;

	service = mupnp_device_getservicebytype(dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	if (!service)
		return false;
	
	if (mupnp_service_parsedescription(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION, mupnp_strlen(CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION)) == false)
		return false;

	mupnp_service_setuserdata(service, dmr);
	for (action=mupnp_service_getactions(service); action; action=mupnp_action_next(action))
		mupnp_action_setuserdata(action, dmr);

	return true;
}


/****************************************
 * mupnp_upnpav_dmr_setsinkprotocolinfo
 ****************************************/

void mupnp_upnpav_dmr_setsinkprotocolinfo(mUpnpAvRenderer *dmr, char *value)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;
	
	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SINKPROTOCOLINFO);
	mupnp_statevariable_setvalue(stateVar, value);
}

/****************************************
 * mupnp_upnpav_dmr_getsinkprotocolinfo
 ****************************************/

char *mupnp_upnpav_dmr_getsinkprotocolinfo(mUpnpAvRenderer *dmr)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;
	
	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SINKPROTOCOLINFO);
	return mupnp_statevariable_getvalue(stateVar);
}

/****************************************
 * mupnp_upnpav_dmr_setsourceprotocolinfo
 ****************************************/

void mupnp_upnpav_dmr_setsourceprotocolinfo(mUpnpAvRenderer *dmr, char *value)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;
	
	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SOURCEPROTOCOLINFO);
	mupnp_statevariable_setvalue(stateVar, value);
}

/****************************************
 * mupnp_upnpav_dmr_getsourceprotocolinfo
 ****************************************/

char *mupnp_upnpav_dmr_getsourceprotocolinfo(mUpnpAvRenderer *dmr)
{
	mUpnpService *service;
	mUpnpStateVariable *stateVar;
	
	service = mupnp_device_getservicebyexacttype(dmr->dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	stateVar = mupnp_service_getstatevariablebyname(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SOURCEPROTOCOLINFO);
	return mupnp_statevariable_getvalue(stateVar);
}

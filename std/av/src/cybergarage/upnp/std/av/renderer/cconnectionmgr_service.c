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

#include <cybergarage/upnp/std/av/cmediarenderer.h>
#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/upnp/std/av/cresource.h>

/****************************************
* Service Description (Connection Manager)
****************************************/

static char *CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"     <serviceStateTable> \n"
"         <stateVariable> \n"
"             <name>SourceProtocolInfo</name> \n"
"             <sendEventsAttribute>yes</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>SinkProtocolInfo</name> \n"
"             <sendEventsAttribute>yes</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>CurrentConnectionIDs</name> \n"
"             <sendEventsAttribute>yes</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_ConnectionStatus</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"             <allowedValueList> \n"
"                 <allowedValue>OK</allowedValue> \n"
"                 <allowedValue>ContentFormatMismatch</allowedValue> \n"
"                 <allowedValue>InsufficientBandwidth</allowedValue> \n"
"                 <allowedValue>UnreliableChannel</allowedValue> \n"
"                 <allowedValue>Unknown</allowedValue> \n"
"             </allowedValueList> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_ConnectionManager</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_Direction</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"             <allowedValueList> \n"
"                 <allowedValue>Input</allowedValue> \n"
"                 <allowedValue>Output</allowedValue> \n"
"             </allowedValueList> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_ProtocolInfo</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>string</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_ConnectionID</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>i4</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_AVTransportID</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>i4</dataType> \n"
"         </stateVariable> \n"
"         <stateVariable> \n"
"             <name>A_ARG_TYPE_RcsID</name> \n"
"             <sendEventsAttribute>no</sendEventsAttribute> \n"
"             <dataType>i4</dataType> \n"
"         </stateVariable> \n"
"     </serviceStateTable> \n"
" <actionList> \n"
"         <action> \n"
"             <name>GetProtocolInfo</name> \n"
"             <argumentList> \n"
"                 <argument> \n"
"                     <name>Source</name> \n"
"                     <direction>out</direction>\n"
"                     <relatedStateVariable>SourceProtocolInfo</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>Sink</name> \n"
"                     <direction>out</direction>\n"
"	               <relatedStateVariable>SinkProtocolInfo</relatedStateVariable> \n"
"                 </argument> \n"
"             </argumentList> \n"
"         </action> \n"
" 	  <action> \n"
" 	  <Optional/> \n"
"             <name>PrepareForConnection</name> \n"
"             <argumentList> \n"
"                 <argument> \n"
"                     <name>RemoteProtocolInfo</name> \n"
"                     <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>PeerConnectionManager</name> \n"
"                     <direction>in</direction>\n"
"	                  <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>PeerConnectionID</name> \n"
"                     <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>Direction</name> \n"
"                     <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>ConnectionID</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>AVTransportID</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>RcsID</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable> \n"
"                 </argument> \n"
"             </argumentList> \n"
"         </action> \n"
"         <action> \n"
" 	  <Optional/> \n"
"             <name>ConnectionComplete</name> \n"
"             <argumentList> \n"
"                 <argument> \n"
"                     <name>ConnectionID</name> \n"
"                     <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable> \n"
"                 </argument> \n"
"             </argumentList> \n"
"         </action> \n"
"         <action> \n"
"             <name>GetCurrentConnectionIDs</name> \n"
"             <argumentList> \n"
"                 <argument> \n"
"                     <name>ConnectionIDs</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>CurrentConnectionIDs</relatedStateVariable> \n"
"                 </argument> \n"
"             </argumentList> \n"
"         </action> \n"
"         <action> \n"
"             <name>GetCurrentConnectionInfo</name> \n"
"             <argumentList> \n"
"                 <argument> \n"
"                     <name>ConnectionID</name> \n"
"                     <direction>in</direction>\n"
"                   <relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>RcsID</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>AVTransportID</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>ProtocolInfo</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>PeerConnectionManager</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable> \n"
"                 </argument> \n"
"              <argument> \n"
"                     <name>PeerConnectionID</name> \n"
"                     <direction>out</direction>\n"
"						<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>Direction</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable> \n"
"                 </argument> \n"
"                 <argument> \n"
"                     <name>Status</name> \n"
"                     <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable> \n"
"                 </argument> \n"
"             </argumentList> \n"
"         </action> \n"
"     </actionList> \n"
" </scpd>\n";

/****************************************
* cg_upnpav_dmr_conmgr_actionreceived
****************************************/

BOOL cg_upnpav_dmr_conmgr_actionreceived(CgUpnpAction *action)
{
	CgUpnpAvRenderer *dmr;
	CgUpnpDevice *dev;
	char *actionName;
	CgUpnpArgument *arg;
	CgString *protocolInfos;
	int protocolInfoCnt;
	int n;
	char protoInfoBuf[CG_UPNPAV_PROTOCOLINFO_MAXLEN];
	
	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
	if (!dev) 
		return FALSE;

	dmr = (CgUpnpAvRenderer *)cg_upnp_device_getuserdata(dev);
	if (!dmr)
		return FALSE;
	
	/* GetProtocolInfo*/
	if (cg_streq(actionName, CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_PROTOCOL_INFO)) {
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNPAV_DMR_CONNECTIONMANAGER_SINK);
		if (!arg)
			return FALSE;
		protocolInfos = cg_string_new();
		protocolInfoCnt = cg_upnpav_resource_getnprotocolinfos();
		for (n=0; n<protocolInfoCnt; n++) {
			if (0 < cg_string_length(protocolInfos))
				cg_string_addvalue(protocolInfos, ",");
			cg_upnpav_resource_getprotocolinfo(n, protoInfoBuf, sizeof(protoInfoBuf)-1);
			cg_string_addvalue(protocolInfos, protoInfoBuf);
		}
		cg_upnp_argument_setvalue(arg, cg_string_getvalue(protocolInfos));
		cg_string_delete(protocolInfos);
		return TRUE;
	}

	return FALSE;
}

/****************************************
 * cg_upnpav_dmr_conmgr_queryreceived
 ****************************************/

BOOL cg_upnpav_dmr_conmgr_queryreceived(CgUpnpStateVariable *statVar)
{
	return FALSE;
}

/****************************************
* cg_upnpav_dmr_conmgr_init
****************************************/

BOOL cg_upnpav_dmr_conmgr_init(CgUpnpAvRenderer *dmr)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAction *action;

	dev = cg_upnpav_dmr_getdevice(dmr);
	if (!dev)
		return FALSE;

	service = cg_upnp_device_getservicebytype(dev, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE);
	if (!service)
		return FALSE;
	
	if (cg_upnp_service_parsedescription(service, CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION, cg_strlen(CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_DESCRIPTION)) == FALSE)
		return FALSE;

	cg_upnp_service_setuserdata(service, dmr);
	for (action=cg_upnp_service_getactions(service); action; action=cg_upnp_action_next(action))
		cg_upnp_action_setuserdata(action, dmr);

	return TRUE;
}

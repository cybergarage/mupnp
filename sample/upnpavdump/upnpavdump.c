/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: upnpdump.c
*
*	Revision:
*       05/11/05
*               - first release.
*
*	10/31/05
*		- Added some M-SEARCH sending actions
*
************************************************************/

#include <cybergarage/upnp/cupnp.h>

#define UPNPAVDUMP_DMS_DEVICETYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define UPNPAVDUMP_DMS_BROWSE_ACTIONNAME "Browse"

/////////////////////////////////////////////////////////////////////////////////
// Print Device
/////////////////////////////////////////////////////////////////////////////////

void PrintContentDirectory(CgUpnpAction *browseAction, int indent, char *objectId)
{
	int n;
	char indentStr[128];
	char *resultXml;
	CgXmlParser *xmlParser;
	CgXmlNodeList *rootNode;

	for (n=0; n<indent && n<(sizeof(indentStr)-1); n++)
		indentStr[n] = ' ';
	indentStr[n] = '\0';

	cg_upnp_action_setargumentvaluebyname(browseAction, "ObjectID", objectId);
	cg_upnp_action_setargumentvaluebyname(browseAction, "BrowseFlag", "BrowseDirectChildren");
	cg_upnp_action_setargumentvaluebyname(browseAction, "Filter", "*");
	cg_upnp_action_setargumentvaluebyname(browseAction, "StartingIndex", "0");
	cg_upnp_action_setargumentvaluebyname(browseAction, "RequestedCount", "0");
	cg_upnp_action_setargumentvaluebyname(browseAction, "SortCriteria", "");
	
	if (!cg_upnp_action_post(browseAction))
		return;

	resultXml = cg_upnp_action_getargumentvaluebyname(browseAction, "Result");
	if (cg_strlen(resultXml) <= 0)
		return;

	rootNode = cg_xml_nodelist_new();
	xmlParser = cg_xml_parser_new();
	if (cg_xml_parse(xmlParser, rootNode, resultXml, cg_strlen(resultXml))) {
	}
	cg_xml_nodelist_delete(rootNode);
	cg_xml_parser_delete(xmlParser);

}

void PrintDMSInfo(CgUpnpDevice *dev, int dmsNum)
{
	CgUpnpService *conDirService;
	CgUpnpAction *browseAction;
	
	if (!cg_upnp_device_isdevicetype(dev, UPNPAVDUMP_DMS_DEVICETYPE))
		return;
	
	printf("[%d] : %s\n", dmsNum, cg_upnp_device_getfriendlyname(dev));

	conDirService = cg_upnp_device_getservicebytype(dev, UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE);
	if (!conDirService)
		return;

	browseAction = cg_upnp_service_getactionbyname(conDirService, UPNPAVDUMP_DMS_BROWSE_ACTIONNAME);
	if (!browseAction)
		return;

	PrintContentDirectory(browseAction, 0, "0");
}

void PrintDMSInfos(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev;
	int dmsNum;
		
	printf("Device Num = %d\n", cg_upnp_controlpoint_getndevices(ctrlPoint));
	
	dmsNum = 0;
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = cg_upnp_device_next(dev)) {
		if (cg_upnp_device_isdevicetype(dev, UPNPAVDUMP_DMS_DEVICETYPE))
			PrintDMSInfo(dev, ++dmsNum);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

#if defined(TENGINE)
MBEG
#else
int main( int argc, char* argv[] )
#endif
{
	CgUpnpControlPoint *ctrlPoint;

	ctrlPoint = cg_upnp_controlpoint_new();
	if (cg_upnp_controlpoint_start(ctrlPoint) == FALSE) {
		printf("Couldn't start this control point !!");
		exit(1);
	}
	
	cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);

	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(ctrlPoint) * 1000);

	PrintDMSInfos(ctrlPoint);
	
	cg_upnp_controlpoint_stop(ctrlPoint);
	cg_upnp_controlpoint_delete(ctrlPoint);
	
	return(0);
}

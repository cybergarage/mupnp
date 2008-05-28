#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnp.h>

void PrintContentDirectory(CgUpnpAction *browseAction, int indent, char *objectId)
{
	int n;
	char indentStr[128];
	char *resultXml;
	CgXmlParser *xmlParser;
	CgXmlNodeList *rootNode;
	CgXmlNode *didlNode;
	CgXmlNode *cnode;
	char *id;
	char *title;
	char *url;

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
		didlNode = cg_xml_nodelist_getbyname(rootNode, "DIDL-Lite");
		if (didlNode) {
			for (cnode=cg_xml_node_getchildnodes(didlNode); cnode; cnode=cg_xml_node_next(cnode)) {
				id = cg_xml_node_getattributevalue(cnode, "id");
				title = cg_xml_node_getchildnodevalue(cnode, "dc:title");
				if (cg_xml_node_isname(cnode, "container")) {
					printf(" %s[%s]%s\n", 
						indentStr, 
						id, 
						((0 < cg_strlen(title)) ? title : ""));
					PrintContentDirectory(browseAction, (indent+1), id);
				}
				else {
					url = cg_xml_node_getchildnodevalue(cnode, "res");
					printf(" %s[%s]%s (%s)\n", 
						indentStr, 
						id, 
						((0 < cg_strlen(title)) ? title : ""),
						((0 < cg_strlen(url)) ? url: ""));
				}
			}
		}
	}
	cg_xml_nodelist_delete(rootNode);
	cg_xml_parser_delete(xmlParser);
}

void PrintDmsInfo(CGUpnpDevice *dev, int dmsNum) 
{
	int serviceNum = 0;
	int actionNum = 0;
	int argNum = 0;
	int statVarNum = 0;
	
	NSLog(@"[%d] %@", ++dmsNum, [dev friendlyName]);
	NSLog(@"  deviceType = %@", [dev deviceType]);

	CGUpnpService *conDirService = [dev getServiceByType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return;

	CGUpnpStateVariable *searchCap = [conDirService getStateVariableByName:@"SearchCapabilities"];
	if (searchCap) {
		if ([searchCap query])
			NSLog(@" SearchCapabilities = %@\n", [searchCap value]);
	}

	CGUpnpStateVariable *sorpCap = [conDirService getStateVariableByName:@"SortCapabilities"];
	if (sorpCap) {
		if ([sorpCap query])
			NSLog(@" SortCapabilities = %@\n", [sorpCap value]);
	}

	CGUpnpAction *browseAction = cg_upnp_service_getactionbyname(conDirService, UPNPAVDUMP_DMS_BROWSE_ACTIONNAME);
	if (!browseAction)
		return;

	PrintContentDirectory(browseAction, 0, "0");
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];

	[ctrlPoint search];

	int dmsNum = 0;
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray) {
		if ([dev isDeviceType:@"urn:schemas-upnp-org:device:MediaServer:1"])
			PrintDmsInfo(dev, ++dmsNum);
	}

	if (dmsNum <= 0)
		printf("Media Server is not found !!\n");

	[pool drain];
	return 0;
}

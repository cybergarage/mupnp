/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2008
*
*	File: upnpavdump.c
*
*	Revision:
*       08/05/24
*               - first release.
*
************************************************************/

#include <mupnp/upnp.h>

#define UPNPAVDUMP_DMS_DEVICETYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define UPNPAVDUMP_DMS_BROWSE_ACTIONNAME "Browse"

/////////////////////////////////////////////////////////////////////////////////
// PrintContentDirectory
/////////////////////////////////////////////////////////////////////////////////

void PrintContentDirectory(mUpnpAction* browseAction, int indent, const char* objectId)
{
  int n;
  char indentStr[128];
  char* resultXml;
  mUpnpXmlParser* xmlParser;
  mUpnpXmlNodeList* rootNode;
  mUpnpXmlNode* didlNode;
  mUpnpXmlNode* cnode;
  const char* id;
  const char* title;
  const char* url;

  for (n = 0; n < indent && n < (sizeof(indentStr) - 1); n++)
    indentStr[n] = ' ';
  indentStr[n] = '\0';

  mupnp_action_setargumentvaluebyname(browseAction, "ObjectID", objectId);
  mupnp_action_setargumentvaluebyname(browseAction, "BrowseFlag", "BrowseDirectChildren");
  mupnp_action_setargumentvaluebyname(browseAction, "Filter", "*");
  mupnp_action_setargumentvaluebyname(browseAction, "StartingIndex", "0");
  mupnp_action_setargumentvaluebyname(browseAction, "RequestedCount", "0");
  mupnp_action_setargumentvaluebyname(browseAction, "SortCriteria", "");

  if (!mupnp_action_post(browseAction))
    return;

  resultXml = mupnp_action_getargumentvaluebyname(browseAction, "Result");
  if (mupnp_strlen(resultXml) <= 0)
    return;

  rootNode = mupnp_xml_nodelist_new();
  xmlParser = mupnp_xml_parser_new();
  if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
    didlNode = mupnp_xml_nodelist_getbyname(rootNode, "DIDL-Lite");
    if (didlNode) {
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          PrintContentDirectory(browseAction, (indent + 1), id);
        }
        else {
          url = mupnp_xml_node_getchildnodevalue(cnode, "res");
          printf(" %s[%s]%s (%s)\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""),
              ((0 < mupnp_strlen(url)) ? url : ""));
        }
      }
    }
  }
  mupnp_xml_nodelist_delete(rootNode);
  mupnp_xml_parser_delete(xmlParser);
}

/////////////////////////////////////////////////////////////////////////////////
// PrintDMSInfo
/////////////////////////////////////////////////////////////////////////////////

void PrintDMSInfo(mUpnpDevice* dev, int dmsNum)
{
  mUpnpService* conDirService;
  mUpnpAction* browseAction;
  mUpnpStateVariable* searchCap;
  mUpnpStateVariable* sorpCap;

  if (!mupnp_device_isdevicetype(dev, UPNPAVDUMP_DMS_DEVICETYPE))
    return;

  printf("[%d] : %s\n", dmsNum, mupnp_device_getfriendlyname(dev));

  conDirService = mupnp_device_getservicebytype(dev, UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE);
  if (!conDirService)
    return;

  searchCap = mupnp_service_getstatevariablebyname(conDirService, "SearchCapabilities");
  if (searchCap) {
    if (mupnp_statevariable_post(searchCap))
      printf(" SearchCapabilities = %s\n", mupnp_statevariable_getvalue(searchCap));
  }

  sorpCap = mupnp_service_getstatevariablebyname(conDirService, "SortCapabilities");
  if (sorpCap) {
    if (mupnp_statevariable_post(sorpCap))
      printf(" SortCapabilities = %s\n", mupnp_statevariable_getvalue(sorpCap));
  }

  browseAction = mupnp_service_getactionbyname(conDirService, UPNPAVDUMP_DMS_BROWSE_ACTIONNAME);
  if (!browseAction)
    return;

  PrintContentDirectory(browseAction, 0, "0");
}

/////////////////////////////////////////////////////////////////////////////////
// PrintDMSInfos
/////////////////////////////////////////////////////////////////////////////////

void PrintDMSInfos(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev;
  int dmsNum;

  dmsNum = 0;
  for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_device_next(dev)) {
    if (mupnp_device_isdevicetype(dev, UPNPAVDUMP_DMS_DEVICETYPE))
      PrintDMSInfo(dev, ++dmsNum);
  }

  if (dmsNum <= 0)
    printf("Media Server is not found !!\n");
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

#if defined(TENGINE)
MBEG
#else
int main(int argc, char* argv[])
#endif
{
  mUpnpControlPoint* ctrlPoint;

  ctrlPoint = mupnp_controlpoint_new();
  if (!mupnp_controlpoint_start(ctrlPoint)) {
    printf("Couldn't start this control point !!");
    exit(1);
  }

  mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE);

  mupnp_sleep(mupnp_controlpoint_getssdpsearchmx(ctrlPoint) * 1000);

  PrintDMSInfos(ctrlPoint);

  mupnp_controlpoint_stop(ctrlPoint);
  mupnp_controlpoint_delete(ctrlPoint);

  return (0);
}

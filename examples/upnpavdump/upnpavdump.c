/************************************************************
 *
 *	mUPnP for C
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

#include "../common/content_directory.h"
#include <mupnp/upnp.h>

#define UPNPAVDUMP_DMS_DEVICETYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define UPNPAVDUMP_DMS_BROWSE_ACTIONNAME "Browse"

/////////////////////////////////////////////////////////////////////////////////
// PrintContentDirectory
/////////////////////////////////////////////////////////////////////////////////

void print_content_directory(mUpnpAction* action, int indent, const char* objectId)
{
  mupnp_example_print_content_directory(action, indent, objectId);
}

/////////////////////////////////////////////////////////////////////////////////
// PrintDMSInfo
/////////////////////////////////////////////////////////////////////////////////

void print_dms_info(mUpnpDevice* dev, int dmsNum)
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

  print_content_directory(browseAction, 0, "0");
}

/////////////////////////////////////////////////////////////////////////////////
// PrintDMSInfos
/////////////////////////////////////////////////////////////////////////////////

void print_dms_infos(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev;
  int dmsNum;

  dmsNum = 0;
  for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_device_next(dev)) {
    if (mupnp_device_isdevicetype(dev, UPNPAVDUMP_DMS_DEVICETYPE))
      print_dms_info(dev, ++dmsNum);
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

  print_dms_infos(ctrlPoint);

  mupnp_controlpoint_stop(ctrlPoint);
  mupnp_controlpoint_delete(ctrlPoint);

  return (0);
}

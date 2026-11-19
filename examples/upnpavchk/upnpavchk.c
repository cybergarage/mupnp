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
#include <mupnp/std/av/cmediarenderer.h>
#include <mupnp/upnp.h>
#include <signal.h>
#include <string.h>

#define UPNPAVDUMP_DMS_DEVICETYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define UPNPAVDUMP_DMS_CONTENTDIR_SERVICETYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define UPNPAVDUMP_DMS_BROWSE_ACTIONNAME "Browse"

/////////////////////////////////////////////////////////////////////////////////
// PrintContentDirectory
/////////////////////////////////////////////////////////////////////////////////

void PrintContentDirectory(mUpnpAction* action, int indent, const char* objectId)
{
  mupnp_example_print_content_directory(action, indent, objectId);
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

static volatile sig_atomic_t interrupted;
static void stop_requested(int signalNumber)
{
  interrupted = signalNumber;
}

#if defined(TENGINE)
MBEG
#else
int main(int argc, char* argv[])
#endif
{
  bool withRenderer = false;
  if (argc == 2 && strcmp(argv[1], "--with-renderer") == 0)
    withRenderer = true;
  else if (argc != 1) {
    printf("Usage: upnpavchk [--with-renderer]\n");
    return argc == 2 && strcmp(argv[1], "--help") == 0 ? 0 : 2;
  }
  interrupted = 0;
  void (*oldInt)(int) = signal(SIGINT, stop_requested);
  void (*oldTerm)(int) = signal(SIGTERM, stop_requested);
  CgUpnpAvRenderer* dmr = NULL;
  mUpnpControlPoint* ctrlPoint = NULL;
  int result = 1;
  if (withRenderer) {
    dmr = cg_upnpav_dmr_new();
    if (!dmr || !cg_upnpav_dmr_start(dmr)) {
      fputs("Couldn't start this media renderer !!\n", stderr);
      goto cleanup;
    }
  }
  if (interrupted)
    goto cleanup;
  ctrlPoint = mupnp_controlpoint_new();
  if (!ctrlPoint || !mupnp_controlpoint_start(ctrlPoint)) {
    fputs("Couldn't start this control point !!\n", stderr);
    goto cleanup;
  }
  if (interrupted || !mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE))
    goto cleanup;
  for (int remaining = mupnp_controlpoint_getssdpsearchmx(ctrlPoint) * 10; remaining > 0 && !interrupted; remaining--)
    mupnp_sleep(100);
  if (interrupted)
    goto cleanup;
  PrintDMSInfos(ctrlPoint);
  result = 0;
cleanup:
  if (ctrlPoint) {
    mupnp_controlpoint_stop(ctrlPoint);
    mupnp_controlpoint_delete(ctrlPoint);
  }
  if (dmr) {
    cg_upnpav_dmr_stop(dmr);
    cg_upnpav_dmr_delete(dmr);
  }
  if (oldInt != SIG_ERR)
    signal(SIGINT, oldInt);
  if (oldTerm != SIG_ERR)
    signal(SIGTERM, oldTerm);
  return interrupted ? 128 + interrupted : result;
}

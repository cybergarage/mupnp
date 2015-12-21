/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2008
*
*	File: upnpigddump.c
*
*	Revision:
*       08/05/24
*               - first release.
*
************************************************************/

#include <mupnp/upnp.h>

#define UPNPAVDUMP_IGD_DEVICETYPE "urn:schemas-upnp-org:device:InternetGatewayDevice:1"

#define UPNPAVDUMP_IGD_WANIPCON_SERVICETYPE "urn:schemas-upnp-org:service:WANIPConnection:1"
#define UPNPAVDUMP_IGD_WANCOMIFCFG_SERVICETYPE "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"

/////////////////////////////////////////////////////////////////////////////////
// PrintIGDInfo
/////////////////////////////////////////////////////////////////////////////////

void PrintIGDInfo(mUpnpDevice* dev, int igdNum)
{
  mUpnpService* ipConService;
  mUpnpAction* extIpAddrAction;
  mUpnpService* wanComIfCfgService;
  mUpnpAction* totalBytesSentAction;
  mUpnpAction* totalBytesRecvAction;

  if (!mupnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
    return;

  printf("[%d] : %s\n", igdNum, mupnp_device_getfriendlyname(dev));

  ipConService = mupnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANIPCON_SERVICETYPE);
  if (ipConService) {
    extIpAddrAction = mupnp_service_getactionbyname(ipConService, "GetExternalIPAddress");
    if (extIpAddrAction) {
      if (mupnp_action_post(extIpAddrAction))
        printf("  GetExternalIPAddress = %s\n", mupnp_action_getargumentvaluebyname(extIpAddrAction, "NewExternalIPAddress"));
    }
  }

  wanComIfCfgService = mupnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANCOMIFCFG_SERVICETYPE);
  if (wanComIfCfgService) {
    totalBytesSentAction = mupnp_service_getactionbyname(ipConService, "GetTotalBytesSent");
    if (totalBytesSentAction) {
      if (mupnp_action_post(totalBytesSentAction))
        printf("  GetTotalBytesSent = %s\n", mupnp_action_getargumentvaluebyname(totalBytesSentAction, "NewTotalBytesSent"));
    }
    totalBytesRecvAction = mupnp_service_getactionbyname(ipConService, "GetTotalBytesReceived");
    if (totalBytesRecvAction) {
      if (mupnp_action_post(totalBytesRecvAction))
        printf("  GetTotalBytesSent = %s\n", mupnp_action_getargumentvaluebyname(totalBytesRecvAction, "NewTotalBytesReceived"));
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
// PrintIGDInfos
/////////////////////////////////////////////////////////////////////////////////

void PrintIGDInfos(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev;
  int igdNum;

  igdNum = 0;
  for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_device_next(dev)) {
    if (mupnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
      PrintIGDInfo(dev, ++igdNum);
  }

  if (igdNum <= 0)
    printf("IGD is not found !!\n");
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

  PrintIGDInfos(ctrlPoint);

  mupnp_controlpoint_stop(ctrlPoint);
  mupnp_controlpoint_delete(ctrlPoint);

  return (0);
}

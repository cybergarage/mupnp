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

void PrintIGDInfo(mUpnpUpnpDevice *dev, int igdNum)
{
	mUpnpUpnpService *ipConService;
	mUpnpUpnpAction *extIpAddrAction;
	mUpnpUpnpService *wanComIfCfgService;
	mUpnpUpnpAction *totalBytesSentAction;
	mUpnpUpnpAction *totalBytesRecvAction;

	if (!mupnp_upnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
		return;
	
	printf("[%d] : %s\n", igdNum, mupnp_upnp_device_getfriendlyname(dev));

	ipConService = mupnp_upnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANIPCON_SERVICETYPE);
	if (ipConService) {
		extIpAddrAction = mupnp_upnp_service_getactionbyname(ipConService, "GetExternalIPAddress");
		if (extIpAddrAction) {
			if (mupnp_upnp_action_post(extIpAddrAction))
				printf("  GetExternalIPAddress = %s\n", mupnp_upnp_action_getargumentvaluebyname(extIpAddrAction, "NewExternalIPAddress"));
		}
	}

	wanComIfCfgService = mupnp_upnp_device_getservicebytype(dev, UPNPAVDUMP_IGD_WANCOMIFCFG_SERVICETYPE);
	if (wanComIfCfgService) {
		totalBytesSentAction = mupnp_upnp_service_getactionbyname(ipConService, "GetTotalBytesSent");
		if (totalBytesSentAction) {
			if (mupnp_upnp_action_post(totalBytesSentAction))
				printf("  GetTotalBytesSent = %s\n", mupnp_upnp_action_getargumentvaluebyname(totalBytesSentAction, "NewTotalBytesSent"));
		}
		totalBytesRecvAction = mupnp_upnp_service_getactionbyname(ipConService, "GetTotalBytesReceived");
		if (totalBytesRecvAction) {
			if (mupnp_upnp_action_post(totalBytesRecvAction))
				printf("  GetTotalBytesSent = %s\n", mupnp_upnp_action_getargumentvaluebyname(totalBytesRecvAction, "NewTotalBytesReceived"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// PrintIGDInfos
/////////////////////////////////////////////////////////////////////////////////

void PrintIGDInfos(mUpnpUpnpControlPoint *ctrlPoint)
{
	mUpnpUpnpDevice *dev;
	int igdNum;
		
	igdNum = 0;
	for (dev = mupnp_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_upnp_device_next(dev)) {
		if (mupnp_upnp_device_isdevicetype(dev, UPNPAVDUMP_IGD_DEVICETYPE))
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
int main( int argc, char* argv[] )
#endif
{
	mUpnpUpnpControlPoint *ctrlPoint;

	ctrlPoint = mupnp_upnp_controlpoint_new();
	if (mupnp_upnp_controlpoint_start(ctrlPoint) == FALSE) {
		printf("Couldn't start this control point !!");
		exit(1);
	}
	
	mupnp_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);

	mupnp_sleep(mupnp_upnp_controlpoint_getssdpsearchmx(ctrlPoint) * 1000);

	PrintIGDInfos(ctrlPoint);
	
	mupnp_upnp_controlpoint_stop(ctrlPoint);
	mupnp_upnp_controlpoint_delete(ctrlPoint);
	
	return(0);
}

/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/device.h>
#include <mupnp/service.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_service_ssdpmessagereceived
****************************************/

void mupnp_upnp_service_ssdpmessagereceived(mUpnpUpnpService *service, mUpnpUpnpSSDPPacket *ssdpPkt)
{
	mUpnpUpnpDevice *dev;
	const char *ssdpST;
	const char *serviceType;
	char serviceNT[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serviceUSN[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
	
	mupnp_log_debug_l4("Entering...\n");

	ssdpST = mupnp_upnp_ssdp_packet_getst(ssdpPkt);
	if (mupnp_strlen(ssdpST) <= 0)
		return;

	dev = mupnp_upnp_service_getdevice(service);

	mupnp_upnp_service_getnotifyservicetypent(service, serviceNT, sizeof(serviceNT));
	mupnp_upnp_service_getnotifyservicetypeusn(service, serviceUSN, sizeof(serviceUSN));
	
	if (mupnp_upnp_st_isalldevice(ssdpST) == TRUE) {
			mupnp_upnp_device_postsearchresponse(dev, ssdpPkt, serviceNT, serviceUSN);
	}
	else if (mupnp_upnp_st_isurn(ssdpST)  == TRUE) {
		serviceType = mupnp_upnp_service_getservicetype(service);
		if (mupnp_streq(ssdpST, serviceType) == TRUE)
			mupnp_upnp_device_postsearchresponse(dev, ssdpPkt, serviceType, serviceUSN);
	}

	mupnp_log_debug_l4("Leaving...\n");
}

/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cservice_ssdp_server.c
*
*	Revision:
*
*	06/08/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/control/ccontrol.h>

/****************************************
* cg_upnp_service_getnotifyservicetypent
****************************************/

char *cg_upnp_service_getnotifyservicetypent(CgUpnpService *service, char *buf, int bufSize)
{
	cg_strncpy(buf, cg_upnp_service_getservicetype(service), bufSize);
	return buf;
}

/****************************************
* cg_upnp_service_getnotifyservicetypent
****************************************/

char *cg_upnp_service_getnotifyservicetypeusn(CgUpnpService *service, char *buf, int bufSize)
{
	CgUpnpDevice *dev;
	
	dev = cg_upnp_service_getdevice(service);

#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_service_getservicetype(service));
#else
	sprintf(buf, "%s::%s", cg_upnp_device_getudn(dev), cg_upnp_service_getservicetype(service));
#endif

	return buf;
}

/****************************************
* cg_upnp_service_ssdpmessagereceived
****************************************/

void cg_upnp_service_ssdpmessagereceived(CgUpnpService *service, CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev;
	char *ssdpST;
	char serviceNT[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char serviceUSN[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char *serviceType;
	
	ssdpST = cg_upnp_ssdp_packet_getst(ssdpPkt);
	if (cg_strlen(ssdpST) <= 0)
		return;

	dev = cg_upnp_service_getdevice(service);

	cg_upnp_service_getnotifyservicetypent(service, serviceNT, sizeof(serviceNT));
	cg_upnp_service_getnotifyservicetypeusn(service, serviceUSN, sizeof(serviceUSN));
	
	if (cg_upnp_st_isalldevice(ssdpST) == TRUE) {
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, serviceNT, serviceUSN);
	}
	else if (cg_upnp_st_isurnservice(ssdpST)  == TRUE) {
		serviceType = cg_upnp_service_getservicetype(service);
		if (cg_streq(ssdpST, serviceType) == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, serviceType, serviceUSN);
	}
}

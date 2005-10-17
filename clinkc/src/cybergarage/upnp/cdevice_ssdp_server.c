/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdevice_ssdp_server.c
*
*	Revision:
*
*	03/22/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/control/ccontrol.h>

/****************************************
* cg_upnp_device_ssdpmessagereceived
****************************************/

void cg_upnp_device_ssdpmessagereceived(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt)
{
	BOOL isRootDev;
	char *ssdpST;
	int ssdpRepeatCnt;
	CgString *devUSN;
	char *devUDN, *devType;
	char ssdpMsg[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	int n;
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	ssdpST = cg_upnp_ssdp_packet_getst(ssdpPkt);
	if (cg_strlen(ssdpST) <= 0)
		return;
		
	isRootDev = cg_upnp_device_isrootdevice(dev);
		
	devUSN = cg_string_new();
	cg_string_setvalue(devUSN, cg_upnp_device_getudn(dev));
	if (isRootDev == TRUE) {
		cg_string_addvalue(devUSN, "::");
		cg_string_addvalue(devUSN, CG_UPNP_USN_ROOTDEVICE);
	}
	
	if (cg_upnp_st_isalldevice(ssdpST) == TRUE) {
		cg_upnp_device_getnotifydevicent(dev, ssdpMsg, sizeof(ssdpMsg));			
		ssdpRepeatCnt = (isRootDev == TRUE) ? 3 : 2;
		for (n=0; n<ssdpRepeatCnt; n++)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, cg_string_getvalue(devUSN));
	}
	else if (cg_upnp_st_isrootdevice(ssdpST)  == TRUE) {
		if (isRootDev == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, CG_UPNP_ST_ROOT_DEVICE, cg_string_getvalue(devUSN));
	}
	else if (cg_upnp_st_isuuiddevice(ssdpST)  == TRUE) {
		devUDN = cg_upnp_device_getudn(dev);
		if (cg_streq(ssdpST, devUDN) == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devUDN, cg_string_getvalue(devUSN));
	}
	else if (cg_upnp_st_isurndevice(ssdpST)  == TRUE) {
		devType = cg_upnp_device_getdevicetype(dev);
		if (cg_streq(ssdpST, devType) == TRUE) {
			cg_string_setvalue(devUSN, cg_upnp_device_getudn(dev));
			cg_string_addvalue(devUSN, "::");
			cg_string_addvalue(devUSN, devType);
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devType, cg_string_getvalue(devUSN));
		}
	}

	cg_string_delete(devUSN);
	
	for (service=cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service))
		cg_upnp_service_ssdpmessagereceived(service, ssdpPkt);

	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(dev))
		cg_upnp_device_ssdpmessagereceived(childDev, ssdpPkt);
	
}

/****************************************
* cg_upnp_device_ssdplistener
****************************************/

void cg_upnp_device_ssdplistener(CgUpnpSSDPPacket *ssdpPkt)
{
	CgUpnpDevice *dev;
	
	dev = (CgUpnpDevice *)cg_upnp_ssdp_packet_getuserdata(ssdpPkt);
	cg_upnp_device_ssdpmessagereceived(dev, ssdpPkt);
}

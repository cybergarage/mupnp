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
*	10/31/05
*		- Corrected response to MSearch message for all ST
*		  (search target) types.
*	12/07/05
*		- No longer responds if MX header is empty
*		  or non-integer.
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
	char *devUDN, *devType;
	char ssdpMsg[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	char deviceUSN[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	int n;
	CgUpnpService *service;
	CgUpnpDevice *childDev;
	
	/****************************************
	 * Request line
	 * Check the request line for errors, this is not ideal as it currently only
	 * checks for the presence of the strings and not the order.
	 ***************************************/
	/**** check for M-SEARCH and return if not found ****/
	if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data), CG_HTTP_MSEARCH) < 0)
		return;
	/**** check for * and return if not found ****/
	if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data), "*") < 0)
		return;
	/**** check HTTP version and return if not found ****/
	if (cg_strstr(cg_string_getvalue(ssdpPkt->dgmPkt->data),  CG_HTTP_VER11) < 0)
		return;

	/****************************************
	 * check HOST header, should always be 239.255.255.250:1900, return if incorrect
	 ***************************************/
	if (cg_strcmp(cg_upnp_ssdp_packet_gethost(ssdpPkt), CG_UPNP_SSDP_MULTICAST_ADDRESS) != 0)
		return;

	/****************************************
	 * check MAN header, return if incorrect
	 ***************************************/
	if (cg_upnp_ssdp_packet_isdiscover(ssdpPkt) == FALSE)
		return;

	/****************************************
	 * check MX header, return if incorrect
	 ***************************************/
	char *mxvalue = cg_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_MX);
	if (mxvalue == NULL || strlen(mxvalue)==0)
		/* return if the MX value does not exist or is empty */
		return;
	/* check if MX value is not an integer */
	for (n=0; n<strlen(mxvalue); n++) {
		if (isdigit(mxvalue[n]) == 0)
			/* MX value contains a non-digit so is invalid */
			return;
	}

	/****************************************
	 * check ST header and if empty return
	 ***************************************/
	ssdpST = cg_upnp_ssdp_packet_getst(ssdpPkt);
	if (cg_strlen(ssdpST) <= 0)
		return;

	isRootDev = cg_upnp_device_isrootdevice(dev);
	
	if (cg_upnp_st_isalldevice(ssdpST) == TRUE) {
		/* for root device only */
		if (isRootDev == TRUE) {
			cg_upnp_device_getnotifydevicent(dev, ssdpMsg, sizeof(ssdpMsg));
			cg_upnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
		}
		/* for all devices send */
		/* device type : device version */
		cg_upnp_device_getnotifydevicetypent(dev, ssdpMsg, sizeof(ssdpMsg));
		cg_upnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
		cg_upnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
		/* device UUID */
		cg_upnp_device_postsearchresponse(dev, ssdpPkt, cg_upnp_device_getudn(dev), cg_upnp_device_getudn(dev));
	}
	else if (cg_upnp_st_isrootdevice(ssdpST)  == TRUE) {
		if (isRootDev == TRUE) {
			cg_upnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, CG_UPNP_ST_ROOT_DEVICE, deviceUSN);
		}
	}
	else if (cg_upnp_st_isuuiddevice(ssdpST)  == TRUE) {
		devUDN = cg_upnp_device_getudn(dev);
		if (cg_streq(ssdpST, devUDN) == TRUE)
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devUDN, devUDN);
	}
	else if (cg_upnp_st_isurndevice(ssdpST)  == TRUE) {
		devType = cg_upnp_device_getdevicetype(dev);
		if (cg_streq(ssdpST, devType) == TRUE) {
			cg_upnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
			cg_upnp_device_postsearchresponse(dev, ssdpPkt, devType, deviceUSN);
		}
	}

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

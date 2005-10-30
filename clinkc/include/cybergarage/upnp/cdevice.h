/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdevice.h
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CDEVICE_H_
#define _CG_UPNP_CDEVICE_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/cmutex.h>
#include <cybergarage/util/cthread.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/net/curl.h>

#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/cicon.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_DEVICE_ELEM_NAME "device"
#define CG_UPNP_DEVICELIST_ELEM_NAME "deviceList"

#define CG_UPNP_DEVICE_UPNP_ROOTDEVICE "upnp:rootdevice"
#define CG_UPNP_DEVICE_DEFAULT_STARTUP_WAIT_TIME 1000
#define CG_UPNP_DEVICE_DEFAULT_DISCOVERY_WAIT_TIME 500
#define CG_UPNP_DEVICE_DEFAULT_LEASE_TIME (30 * 60)
#define CG_UPNP_DEVICE_HTTP_DEFAULT_PORT 38400
#define CG_UPNP_DEVICE_DEFAULT_DESCRIPTION_URI "/description.xml"
#define CG_UPNP_DEVICE_URLBASE_MAXLEN 64

#define CG_UPNP_DEVICE_URLBASE_NAME "URLBase"
#define CG_UPNP_DEVICE_DEVICE_TYPE "deviceType"
#define CG_UPNP_DEVICE_FRIENDLY_NAME "friendlyName"
#define CG_UPNP_DEVICE_MANUFACTURE "manufacture"
#define CG_UPNP_DEVICE_MANUFACTURE_URL "manufactureURL"
#define CG_UPNP_DEVICE_MODEL_DESCRIPTION "modelDescription"
#define CG_UPNP_DEVICE_MODEL_NAME "modelName"
#define CG_UPNP_DEVICE_MODEL_NUMBER "modelNumber"
#define CG_UPNP_DEVICE_MODEL_URL "modelURL"
#define CG_UPNP_DEVICE_SERIAL_NUMBER "serialNumber"
#define CG_UPNP_DEVICE_UDN "UDN"
#define CG_UPNP_DEVICE_UPC "UPC"
#define CG_UPNP_DEVICE_PRESENTATION_URL "presentationURL"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpDevice {
	BOOL headFlag;
	struct _CgUpnpDevice *prev;
	struct _CgUpnpDevice *next;
	
	CgXmlNodeList *rootNodeList;
	CgXmlNode *deviceNode;
	
	struct _CgUpnpDevice *parentDevice;
	struct _CgUpnpDevice *deviceList;
	struct _CgUpnpService *serviceList;
	struct _CgUpnpIcon *iconList;
	
	CgMutex *mutex;
	CgHttpServerList *httpServerList;
	CgUpnpSSDPServer *ssdpServerList;
	CgThread *advertiser;
	
	CG_HTTP_LISTENER httpListener;

	/**** Execution Data ****/
	CgString *descriptionURI;
	int leaseTime;
	int httpPort;
	CgUpnpSSDPPacket *ssdpPkt;
	void *userData;
	
} CgUpnpDevice, CgUpnpDeviceList;

/****************************************
* Function (Device)
****************************************/

CgUpnpDevice *cg_upnp_device_new();
void cg_upnp_device_delete(CgUpnpDevice *dev);
void cg_upnp_device_clear(CgUpnpDevice *dev);

#define cg_upnp_device_next(dev) (CgUpnpDevice *)cg_list_next((CgList *)dev)
#define cg_upnp_device_remove(dev) cg_list_remove((CgList *)dev)

#define cg_upnp_device_isdevicenode(node) cg_xml_node_isname(node, CG_UPNP_DEVICE_ELEM_NAME)

#define cg_upnp_device_getrootnode(dev) cg_xml_nodelist_gets(dev->rootNodeList)
void cg_upnp_device_setdevicenode(CgUpnpDevice *dev, CgXmlNode *node);
#define cg_upnp_device_getdevicenode(dev) (dev->deviceNode)

#define cg_upnp_device_isrootdevice(dev) ((dev->rootNodeList != NULL) ? TRUE : FALSE)

#define cg_upnp_device_setparentdevice(dev, pDev) (dev->parentDevice = pDev)
#define cg_upnp_device_getparentdevice(dev) (dev->parentDevice)

CgUpnpDevice *cg_upnp_device_getrootdevice(CgUpnpDevice *dev);

BOOL cg_upnp_device_parsedescription(CgUpnpDevice *dev, char *desciption, int descriptionLen);
BOOL cg_upnp_device_parsedescriptionurl(CgUpnpDevice *dev, CgNetURL *url);

#if !defined(TENGINE) && !defined(ITRON) && !defined(BTRON)
BOOL cg_upnp_device_loaddescriptionfile(CgUpnpDevice *dev, char *fileName);
#endif

/**** deviceType ****/
#define cg_upnp_device_setdevicetype(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_DEVICE_TYPE, value)
#define cg_upnp_device_getdevicetype(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_DEVICE_TYPE)
#define cg_upnp_device_isdevicetype(dev, value) cg_streq(cg_upnp_device_getdevicetype(dev), value)

/**** friendlyName ****/
#define cg_upnp_device_setfriendlyname(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_FRIENDLY_NAME, value)
#define cg_upnp_device_getfriendlyname(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_FRIENDLY_NAME)

/**** manufacture ****/
#define cg_upnp_device_setmanufacture(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURE, value)
#define cg_upnp_device_getmanufacture(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURE)

/**** manufactureURL ****/
#define cg_upnp_device_setmanufactureurl(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURE_URL, value)
#define cg_upnp_device_getmanufactureurl(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MANUFACTURE_URL)

/**** modelDescription ****/
#define cg_upnp_device_setmodeldescription(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_DESCRIPTION, value)
#define cg_upnp_device_getmodeldescription(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_DESCRIPTION)
	
/**** modelName ****/
#define cg_upnp_device_setmodelname(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NAME, value)
#define cg_upnp_device_getmodelname(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NAME)
	
/**** modelNumber ****/
#define cg_upnp_device_setmodelnumber(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NUMBER, value)
#define cg_upnp_device_getmodelnumber(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_NUMBER)
	
/**** modelURL ****/
#define cg_upnp_device_setmodelurl(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_URL, value)
#define cg_upnp_device_getmodelurl(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_MODEL_URL)
	
/**** serialNumber ****/
#define cg_upnp_device_setserialnumber(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_SERIAL_NUMBER, value)
#define cg_upnp_device_getserialnumber(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_SERIAL_NUMBER)
	
/****  UDN ****/
#define cg_upnp_device_setudn(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UDN, value)
#define cg_upnp_device_getudn(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UDN)
#define cg_upnp_device_hasudn(dev) ((0 < cg_strlen(cg_upnp_device_getudn(dev))) ? TRUE : FALSE)

/**** UPC ****/
#define cg_upnp_device_setupc(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UPC, value)
#define cg_upnp_device_getupc(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_UPC)
	
/**** PresentationURL ****/
#define cg_upnp_device_setpresentationurl(dev, value) cg_xml_node_setchildnode(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_PRESENTATION_URL, value)
#define cg_upnp_device_getpresentationurl(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(dev), CG_UPNP_DEVICE_PRESENTATION_URL)

/**** URLBase ****/
void cg_upnp_device_seturlbase(CgUpnpDevice *dev, char *value);
#define cg_upnp_device_geturlbase(dev) cg_xml_node_getchildnodevalue(cg_upnp_device_getdevicenode(cg_upnp_device_getrootdevice(dev)), CG_UPNP_DEVICE_URLBASE_NAME)

/**** Strart/Stop ****/
BOOL cg_upnp_device_start(CgUpnpDevice *dev);
BOOL cg_upnp_device_stop(CgUpnpDevice *dev);

/*** URL ****/
char *cg_upnp_device_getlocationurl(CgUpnpDevice *dev, char *host, char *buf, int bufSize);

/**** Notify ****/
void cg_upnp_device_announce(CgUpnpDevice *dev);
void cg_upnp_device_byebye(CgUpnpDevice *dev);

/**** Mutex ****/
#define cg_upnp_device_lock(dev) cg_mutex_lock(dev->mutex)
#define cg_upnp_device_unlock(dev) cg_mutex_unlock(dev->mutex)

/**** Listner ****/
void cg_upnp_device_setactionlistener(CgUpnpDevice *dev, CG_UPNP_ACTION_LISTNER actionListner);
void cg_upnp_device_setquerylistener(CgUpnpDevice *dev, CG_UPNP_STATEVARIABLE_LISTNER queryListner);

/**** User Data****/
#define cg_upnp_device_setuserdata(dev, value) (dev->userData = value)
#define cg_upnp_device_getuserdata(dev) (dev->userData)
 
/****************************************
* Function
****************************************/

/**** DescriptionURL ****/
#define cg_upnp_device_setdescriptionuri(dev,url) cg_string_setvalue(dev->descriptionURI, url)
#define cg_upnp_device_getdescriptionuri(dev) cg_string_getvalue(dev->descriptionURI)
#define cg_upnp_device_isdescriptionuri(dev, url) cg_streq(cg_string_getvalue(dev->descriptionURI), url)

/**** Lease Time ****/
#define cg_upnp_device_setleasetime(dev,value) (dev->leaseTime = value)
#define cg_upnp_device_getleasetime(dev) (dev->leaseTime)

/****************************************
* Function (Embedded)
****************************************/

/**** Embedded DeviceList ****/
#define cg_upnp_device_getdevicelist(dev) (dev->deviceList)
#define cg_upnp_device_getdevice(dev,idx) ((CgUpnpDevice *)cg_list_get(dev->deviceList,idx))
#define cg_upnp_device_getdevices(dev) ((CgUpnpDevice *)cg_list_gets((CgList *)dev->deviceList))
BOOL cg_upnp_device_isname(CgUpnpDevice *dev, char *name);
CgUpnpDevice *cg_upnp_device_getdevicebyname(CgUpnpDevice *dev, char *name);
CgUpnpDevice *cg_upnp_device_getdevicebydescriptionuri(CgUpnpDevice *dev, char *url);

/**** Embedded ServiceList ****/
#define cg_upnp_device_getservicelist(dev) (dev->serviceList)
#define cg_upnp_device_getservice(dev,idx) ((CgUpnpService *)cg_list_get(dev->serviceList,idx))
#define cg_upnp_device_getservices(dev) ((CgUpnpService *)cg_list_gets((CgList *)dev->serviceList))
CgUpnpService *cg_upnp_device_getservicebyname(CgUpnpDevice *dev, char *name);
CgUpnpService *cg_upnp_device_getservicebyscpdurl(CgUpnpDevice *dev, char *url);
CgUpnpService *cg_upnp_device_getservicebycontrolurl(CgUpnpDevice *dev, char *url);
CgUpnpService *cg_upnp_device_getservicebyeventsuburl(CgUpnpDevice *dev, char *url);
CgUpnpService *cg_upnp_device_getservicebysid(CgUpnpDevice *dev, char *sid);

/**** Embedded IconList ****/
#define cg_upnp_device_geticonlist(dev) (dev->iconList)
#define cg_upnp_device_geticon(dev,idx) ((CgUpnpIcon *)cg_list_get((CgList *)dev->iconList,idx))
#define cg_upnp_device_geticons(dev) ((CgUpnpIcon *)cg_list_gets((CgList *)dev->iconList))

/**** Embedded Acton ****/
CgUpnpAction *cg_upnp_device_getactionbyname(CgUpnpDevice *dev, char *name);

/**** Embedded StateVariable ****/
CgUpnpStateVariable *cg_upnp_device_getstatevariablebyname(CgUpnpDevice *dev, char *name);

/****************************************
* Function (DeviceList)
****************************************/

CgUpnpDeviceList *cg_upnp_devicelist_new();
void cg_upnp_devicelist_delete(CgUpnpDeviceList *devList);

#define cg_upnp_devicelist_clear(devList) cg_list_clear((CgList *)devList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_device_delete)
#define cg_upnp_devicelist_size(devList) cg_list_size((CgList *)devList)
#define cg_upnp_devicelist_gets(devList) (CgUpnpDevice *)cg_list_next((CgList *)devList)
#define cg_upnp_devicelist_add(devList, dev) cg_list_add((CgList *)devList, (CgList *)dev)

/****************************************
* Function (Service)
****************************************/

#define cg_upnp_service_getdevice(service) ((CgUpnpDevice *)service->parentDevice)
#define cg_upnp_service_getrootdevice(service) cg_upnp_device_getrootdevice((CgUpnpDevice *)service->parentDevice)

/****************************************
* Function (HTTP)
****************************************/

/**** HTTP Port ****/
#define cg_upnp_device_sethttpport(dev,value) (dev->httpPort = value)
#define cg_upnp_device_gethttpport(dev) (dev->httpPort)

#define cg_upnp_device_sethttplistener(dev, func) (dev->httpListener = func)
#define cg_upnp_device_gethttplistener(dev) (dev->httpListener)
void cg_upnp_device_httprequestrecieved(CgHttpRequest *httpReq);

/****************************************
* Function (SSDP)
****************************************/

#define cg_upnp_device_getssdpserverlist(dev) (dev->ssdpServerList)

void cg_upnp_device_ssdplistener(CgUpnpSSDPPacket *ssdpPkt);

char *cg_upnp_device_getnotifydevicent(CgUpnpDevice *dev, char *buf, int bufSize);
char *cg_upnp_device_getnotifydevicetypent(CgUpnpDevice *dev, char *buf, int bufSize);
char *cg_upnp_device_getnotifydevicetypeusn(CgUpnpDevice *dev, char *buf, int bufSize);

void cg_upnp_device_ssdpmessagereceived(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt);
BOOL cg_upnp_device_postsearchresponse(CgUpnpDevice *dev, CgUpnpSSDPPacket *ssdpPkt, char *st, char *usn);

/****************************************
* Function (SSDPPacket)
****************************************/

#define cg_upnp_device_getssdppacket(dev) (dev->ssdpPkt)
#define cg_upnp_device_setssdppacket(dev, srcSsdpPkt) cg_upnp_ssdp_packet_copy(dev->ssdpPkt, srcSsdpPkt)
#define cg_upnp_device_getlocationfromssdppacket(dev) cg_upnp_ssdp_packet_getlocation(dev->ssdpPkt)
#define cg_upnp_device_getinterfaceaddressfromssdppacket(dev) cg_upnp_ssdp_packet_getlocaladdress(dev->ssdpPkt)

/****************************************
* Function (Advertiser)
****************************************/

BOOL cg_upnp_device_advertiser_start(CgUpnpDevice *dev);
BOOL cg_upnp_device_advertiser_stop(CgUpnpDevice *dev);

#define cg_upnp_device_getadvertiserthead(dev) (dev->advertiser)

#ifdef  __cplusplus
}
#endif

#endif

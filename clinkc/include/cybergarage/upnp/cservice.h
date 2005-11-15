/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cservice.h
*
*	Revision:
*
*	02/14/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CSERVICE_H_
#define _CG_UPNP_CSERVICE_H_

#include <cybergarage/typedef.h>
#include <cybergarage/xml/cxml.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/cmutex.h>
#include <cybergarage/net/curl.h>

#include <cybergarage/upnp/caction.h>
#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/upnp/ssdp/cssdp_server.h>
#include <cybergarage/upnp/event/csubscriber.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_SERVICE_ELEM_NAME "service"
#define CG_UPNP_SERVICELIST_ELEM_NAME "serviceList"

#define CG_UPNP_SERVICE_TYPE "serviceType"
#define CG_UPNP_SERVICE_ID "serviceId"
#define CG_UPNP_SERVICE_SCPDURL "SCPDURL"
#define CG_UPNP_SERVICE_CONTROL_URL "controlURL"
#define CG_UPNP_SERVICE_EVENT_SUB_URL "eventSubURL"

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpService {
	BOOL headFlag;
	struct _CgUpnpService *prev;
	struct _CgUpnpService *next;
	CgXmlNodeList *scpdNodeList;
	CgXmlNode *serviceNode;
	CgUpnpActionList *actionList;
	CgUpnpServiceStateTable *serviceStateTable;
	void *parentDevice;
	CgMutex *mutex;
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	CgUpnpSubscriberList *subscriberList;
#endif
	BOOL parsed;
	/**** Execution Data ****/
	CgString *subscriptionSid;
	long subscriptionTimeout;
	void *userData;
} CgUpnpService, CgUpnpServiceList;

/****************************************
* Function (Service)
****************************************/

CgUpnpService *cg_upnp_service_new();
void cg_upnp_service_delete(CgUpnpService *service);
void cg_upnp_service_clear(CgUpnpService *service);

#define cg_upnp_service_next(service) (CgUpnpService *)cg_list_next((CgList *)service)

#define cg_upnp_service_isservicenode(node) cg_xml_node_isname(node, CG_UPNP_SERVICE_ELEM_NAME)

#define cg_upnp_service_getscpdnode(service) cg_xml_nodelist_gets(service->scpdNodeList)
#define cg_upnp_service_setservicenode(service,node) (service->serviceNode = node)
#define cg_upnp_service_getservicenode(service) (service->serviceNode)
#define cg_upnp_service_getdevicenode(service) cg_xml_node_getparentnode(cg_xml_node_getparentnode(service->serviceNode))

BOOL cg_upnp_service_isname(CgUpnpService *service, char *name);

BOOL cg_upnp_service_parsedescription(CgUpnpService *service, char *desciption, int descriptionLen);
BOOL cg_upnp_service_parsedescriptionurl(CgUpnpService *service, CgNetURL *url);
char *cg_upnp_service_getdescription(CgUpnpService *service, CgString *descStr);
#define cg_upnp_service_isparsed(service) (service->parsed)

/**** serviceType ****/
#define cg_upnp_service_setservicetype(service, value) cg_xml_node_setchildnode(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_TYPE, value)
#define cg_upnp_service_getservicetype(service) cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_TYPE)

/**** serviceID ****/
#define cg_upnp_service_setserviceid(service, value) cg_xml_node_setchildnode(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_ID, value)
#define cg_upnp_service_getserviceid(service) cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_ID)

/**** SCPDURL ****/
#define cg_upnp_service_setscpdurl(service, value) cg_xml_node_setchildnode(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_SCPDURL, value)
#define cg_upnp_service_getscpdurl(service) cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_SCPDURL)
#define cg_upnp_service_isscpdurl(service, url) cg_streq(cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_SCPDURL), url)

/**** controlURL ****/
#define cg_upnp_service_setcontrolurl(service, value) cg_xml_node_setchildnode(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_CONTROL_URL, value)
#define cg_upnp_service_getcontrolurl(service) cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_CONTROL_URL)

/**** eventSubURL ****/
#define cg_upnp_service_seteventsuburl(service, value) cg_xml_node_setchildnode(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_EVENT_SUB_URL, value)
#define cg_upnp_service_geteventsuburl(service) cg_xml_node_getchildnodevalue(cg_upnp_service_getservicenode(service), CG_UPNP_SERVICE_EVENT_SUB_URL)

/**** Action ****/
CgUpnpActionList *cg_upnp_service_getactionlist(CgUpnpService *service);
CgUpnpAction *cg_upnp_service_getactions(CgUpnpService *service);
CgUpnpAction *cg_upnp_service_getactionbyname(CgUpnpService *service, char *name);
#define cg_upnp_service_hasactionbyname(service, name) ((cg_upnp_service_getactionbyname(service,name) != NULL) ? TRUE : FALSE)

/**** ServiceStateTable ****/
CgUpnpServiceStateTable *cg_upnp_service_getservicestatetable(CgUpnpService *service);
CgUpnpStateVariable *cg_upnp_service_getstatevariables(CgUpnpService *service);
CgUpnpStateVariable *cg_upnp_service_getstatevariablebyname(CgUpnpService *service, char *name);
#define cg_upnp_service_hasstatevariablebyname(service, name) ((cg_upnp_service_getstatevariablebyname(service,name) != NULL) ? TRUE : FALSE)

BOOL cg_upnp_service_announcefrom(CgUpnpService *service, char *bindAddr);
BOOL cg_upnp_service_byebyefrom(CgUpnpService *service, char *bindAddr);

/**** Listner ****/
void cg_upnp_service_setactionlistener(CgUpnpService *service, CG_UPNP_ACTION_LISTNER actionListener);
void cg_upnp_service_setquerylistener(CgUpnpService *service, CG_UPNP_STATEVARIABLE_LISTNER queryListener);

/**** Parent Device ****/
#define cg_upnp_service_setdevice(service, dev) (service->parentDevice = dev)
#define cg_upnp_service_getdevice(service) ((CgUpnpDevice *)service->parentDevice)

/**** Root Device ****/
#define cg_upnp_service_getrootdevice(service) cg_upnp_device_getrootdevice((CgUpnpDevice *)service->parentDevice)

/**** Subscriber ****/
#define cg_upnp_service_getsubscribers(service) cg_upnp_subscriberlist_gets(service->subscriberList)
#define cg_upnp_service_getnsubscribers(service) cg_upnp_subscriberlist_size(service->subscriberList)
BOOL cg_upnp_service_addsubscriber(CgUpnpService *service, CgUpnpSubscriber *sub);
BOOL cg_upnp_service_removesubscriber(CgUpnpService *service, CgUpnpSubscriber *sub);
CgUpnpSubscriber *cg_upnp_service_getsubscriberbysid(CgUpnpService *service, char *sid);

/**** Notify ****/
BOOL cg_upnp_service_notify(CgUpnpService *service, CgUpnpStateVariable *statVar);
BOOL cg_upnp_service_notifyallstatevariables(CgUpnpService *service);

/**** Mutex ****/
#define cg_upnp_service_lock(service) cg_mutex_lock(service->mutex)
#define cg_upnp_service_unlock(service) cg_mutex_unlock(service->mutex)

/**** SID ****/
#define cg_upnp_service_setsubscriptionsid(service, value) cg_string_setvalue(service->subscriptionSid, value)
#define cg_upnp_service_getsubscriptionsid(service) cg_string_getvalue(service->subscriptionSid)
#define cg_upnp_service_issubscribed(service) ((0 < cg_string_length(service->subscriptionSid)) ? TRUE : FALSE)
#define cg_upnp_service_clearsubscriptionsid(service) cg_string_setvalue(service->subscriptionSid, "")

/**** Timeout ****/
#define cg_upnp_service_setsubscriptiontimeout(service, value) (service->subscriptionTimeout = value)
#define cg_upnp_service_getsubscriptiontimeout(service) (service->subscriptionTimeout)

/**** User Data****/
#define cg_upnp_service_setuserdata(service, value) (service->userData = value)
#define cg_upnp_service_getuserdata(service) (service->userData)

/****************************************
* Function (ServiceList)
****************************************/

CgUpnpServiceList *cg_upnp_servicelist_new();
void cg_upnp_servicelist_delete(CgUpnpServiceList *serviceList);

#define cg_upnp_servicelist_clear(serviceList) cg_list_clear((CgList *)serviceList, (CG_LIST_DESTRUCTORFUNC)cg_upnp_service_delete)
#define cg_upnp_servicelist_size(serviceList) cg_list_size((CgList *)serviceList)
#define cg_upnp_servicelist_gets(serviceList) (CgUpnpService *)cg_list_next((CgList *)serviceList)
#define cg_upnp_servicelist_add(serviceList, service) cg_list_add((CgList *)serviceList, (CgList *)service)

/****************************************
* Function (SSDP)
****************************************/

void cg_upnp_service_ssdpmessagereceived(CgUpnpService *service, CgUpnpSSDPPacket *ssdpPkt);

#ifdef  __cplusplus
}
#endif

#endif

/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cservice.c
*
*	Revision:
*
*	02/14/05
*		- first revision
*	08/16/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_upnp_service_parsedescriptionurl() to set "/" as the default path when the url has no path.
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/cupnp_function.h>
#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/ssdp/cssdp.h>

#ifdef CG_OPTIMIZED_CP_MODE
#include <cybergarage/upnp/ccontrolpoint.h>
#define cg_upnp_service_parseifnotparsed(service)  \
			do { \
				if (cg_upnp_service_isparsed(service) == FALSE) \
					cg_upnp_controlpoint_parsescservicescpd(service); \
			} while(0)
#endif

/****************************************
* prototype define for static functions
****************************************/

static void cg_upnp_service_initchildnodes(CgUpnpService *service);
static void cg_upnp_service_initactionlist(CgUpnpService *service);
static void cg_upnp_service_initservicestatetable(CgUpnpService *service);

static char *cg_upnp_service_getnotifyservicetypent(CgUpnpService *service, char *buf, int bufSize);
static char *cg_upnp_service_getnotifyservicetypeusn(CgUpnpService *service,  char *buf, int bufSize);

/****************************************
* cg_upnp_service_new
****************************************/

CgUpnpService *cg_upnp_service_new()
{
	CgUpnpService *service = (CgUpnpService *)malloc(sizeof(CgUpnpService));
	cg_list_node_init((CgList *)service);

	service->scpdNodeList = NULL;
	service->serviceNode = NULL;

	service->actionList = cg_upnp_actionlist_new();
	service->serviceStateTable = cg_upnp_servicestatetable_new();

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	service->subscriberList = cg_upnp_subscriberlist_new();
#endif
	
#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = FALSE;
#endif

	service->mutex = cg_mutex_new();
	service->subscriptionSid = cg_string_new();
	
	cg_upnp_service_setdevice(service, NULL);
	cg_upnp_service_setsubscriptiontimeout(service, 0);
	
	cg_upnp_service_setuserdata(service, NULL);
	
	
	return service;
}

/****************************************
* cg_upnp_service_delete
****************************************/

void cg_upnp_service_delete(CgUpnpService *service)
{
	cg_list_remove((CgList *)service);
	
	cg_upnp_service_clear(service);

	cg_upnp_actionlist_delete(service->actionList);
	cg_upnp_servicestatetable_delete(service->serviceStateTable);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	cg_upnp_subscriberlist_delete(service->subscriberList);
#endif
	
	cg_mutex_delete(service->mutex);
	cg_string_delete(service->subscriptionSid);
	
	free(service);
}

/****************************************
* cg_upnp_service_clear
****************************************/

void cg_upnp_service_clear(CgUpnpService *service)
{
	/* Don't clear the service node */
	/* service->serviceNode = NULL; */
	
	if (service->scpdNodeList != NULL) {
		cg_xml_nodelist_delete(service->scpdNodeList);
		service->scpdNodeList = NULL;
	}

	cg_upnp_actionlist_clear(service->actionList);
	cg_upnp_servicestatetable_clear(service->serviceStateTable);
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	cg_upnp_subscriberlist_clear(service->subscriberList);
#endif
	
	cg_upnp_service_setsubscriptionsid(service, NULL);
	cg_upnp_service_setsubscriptiontimeout(service, 0);
}	


/****************************************
* cg_upnp_service_isname
****************************************/

BOOL cg_upnp_service_isname(CgUpnpService *service, char *name)
{
	if (name == NULL)
		return FALSE;
	if (0 <= cg_strstr(cg_upnp_service_getservicetype(service), name))
	{
#ifdef CG_OPTIMIZED_CP_MODE
		cg_upnp_service_parseifnotparsed(service);
#endif
		return TRUE;
	}
	if (0  <= cg_strstr(cg_upnp_service_getserviceid(service), name))
	{
#ifdef CG_OPTIMIZED_CP_MODE
		cg_upnp_service_parseifnotparsed(service);
#endif
		return TRUE;
	}
	return FALSE;
}

/****************************************
* cg_upnp_service_parsedescription
****************************************/

BOOL cg_upnp_service_parsedescription(CgUpnpService *service, char *desciption, int descriptionLen)
{
	CgXmlParser *xmlParser;
	BOOL xmlParseSuccess;
	CgXmlNode *scpdNode;
	
	cg_upnp_service_lock(service);
	cg_upnp_service_clear(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = FALSE;
#endif	
	service->scpdNodeList = cg_xml_nodelist_new();

	xmlParser = cg_xml_parser_new();
	xmlParseSuccess = cg_xml_parse(xmlParser, service->scpdNodeList, desciption, descriptionLen);
	cg_xml_parser_delete(xmlParser);
	if (xmlParseSuccess == FALSE)
	{
		cg_upnp_service_unlock(service);
		return FALSE;
	}
	
	if (cg_xml_nodelist_size(service->scpdNodeList) <= 0) {
		cg_upnp_service_clear(service);
		cg_upnp_service_unlock(service);
		return FALSE;
	}

	scpdNode = cg_upnp_service_getscpdnode(service);
	if (scpdNode == NULL) {
		cg_upnp_service_clear(service);
		cg_upnp_service_unlock(service);
		return FALSE;
	}
		
	cg_upnp_service_initchildnodes(service);

#ifdef CG_OPTIMIZED_CP_MODE
	service->parsed = TRUE;
#endif
	cg_upnp_service_unlock(service);
	return TRUE;
}

/****************************************
* cg_upnp_service_parsedescriptionurl
****************************************/

BOOL cg_upnp_service_parsedescriptionurl(CgUpnpService *service, CgNetURL *url)
{
	char *host;
	int port;
	char *request;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int statusCode;
	char *content;
	long contentLen;
	BOOL parseSuccess;
	
	CgNetURL *fixedUrl;
	
	if (cg_net_url_ishttpprotocol(url) == FALSE)
		return FALSE;
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;
	/**** Thanks for Theo Beisch (08/16/05) ****/
	request = cg_net_url_getrequest(url);
	if (cg_strlen(request) <= 0)
		request = "/";

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, request);
	cg_http_request_setcontentlength(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, host, port);
	
	statusCode = cg_http_response_getstatuscode(httpRes);
	if (statusCode != CG_HTTP_STATUS_OK) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	
	content = cg_http_response_getcontent(httpRes);
	contentLen = cg_http_response_getcontentlength(httpRes);

	parseSuccess = cg_upnp_service_parsedescription(service, content, contentLen);

	cg_http_request_delete(httpReq);
	
	/* Fix broken event URL */
	request = cg_upnp_service_geteventsuburl(service);
	if (cg_strlen(request) > 0 && request[0] != '/')
	{
		fixedUrl = cg_net_url_new();
		cg_net_url_set(fixedUrl, request);
		cg_upnp_service_seteventsuburl(service, cg_net_url_getrequest(fixedUrl));
		cg_net_url_delete(fixedUrl);
	}
	
	/* Fix broken control URL */
	request = cg_upnp_service_getcontrolurl(service);
	if (cg_strlen(request) > 0 && request[0] != '/')
	{
		fixedUrl = cg_net_url_new();
		cg_net_url_set(fixedUrl, request);
		cg_upnp_service_setcontrolurl(service, cg_net_url_getrequest(fixedUrl));
		cg_net_url_delete(fixedUrl);
	}
	
	return parseSuccess;
}

/****************************************
* cg_upnp_service_getdescription
****************************************/

char *cg_upnp_service_getdescription(CgUpnpService *service, CgString *descStr)
{
	CgXmlNode *scpdNode;
	
	scpdNode = cg_upnp_service_getscpdnode(service);
	if (scpdNode != NULL) {
		cg_string_addvalue(descStr, CG_UPNP_XML_DECLARATION);
		cg_string_addvalue(descStr, "\n");
		cg_xml_node_tostring(scpdNode, TRUE, descStr);
	}
	
	return cg_string_getvalue(descStr);
}

/****************************************
*
* Announce
*
****************************************/

static char *cg_upnp_service_getnotifyservicetypent(CgUpnpService *service, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%s", cg_upnp_service_getservicetype(service));
#else
	sprintf(buf, "%s", cg_upnp_service_getservicetype(service));
#endif
	return buf;
}

static char *cg_upnp_service_getnotifyservicetypeusn(CgUpnpService *service, char *buf, int bufSize)
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

BOOL cg_upnp_service_announcefrom(CgUpnpService *service, char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpDevice *rootDev;
	CgUpnpDevice *dev;
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
	
	rootDev = cg_upnp_service_getrootdevice(service);
	dev = cg_upnp_service_getdevice(service);
	
	ssdpReq = cg_upnp_ssdprequest_new();
	cg_upnp_ssdprequest_setserver(ssdpReq, cg_upnp_getservername(ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setleasetime(ssdpReq, cg_upnp_device_getleasetime(dev));
	cg_upnp_ssdprequest_setlocation(ssdpReq, cg_upnp_device_getlocationurl(rootDev, bindAddr, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_ALIVE);
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	
	ssdpSock = cg_upnp_ssdp_socket_new();
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_upnp_ssdp_socket_delete(ssdpSock);
	
	cg_upnp_ssdprequest_delete(ssdpReq);
	
	return sentResult;
}

BOOL cg_upnp_service_byebyefrom(CgUpnpService *service, char *bindAddr)
{
	/**** uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v ****/
	char ssdpLineBuf[CG_UPNP_SSDP_HEADER_LINE_MAXSIZE];
	CgUpnpSSDPRequest *ssdpReq;
	CgUpnpSSDPSocket *ssdpSock;
	BOOL sentResult;
		
	ssdpReq = cg_upnp_ssdprequest_new();
	
	cg_upnp_ssdprequest_setnts(ssdpReq, CG_UPNP_SSDP_NTS_BYEBYE);
	cg_upnp_ssdprequest_setnt(ssdpReq, cg_upnp_service_getnotifyservicetypent(service, ssdpLineBuf, sizeof(ssdpLineBuf)));
	cg_upnp_ssdprequest_setusn(ssdpReq, cg_upnp_service_getnotifyservicetypeusn(service, ssdpLineBuf, sizeof(ssdpLineBuf)));

	ssdpSock = cg_upnp_ssdp_socket_new();
	sentResult = cg_upnp_ssdp_socket_notifyfrom(ssdpSock, ssdpReq, bindAddr);
	cg_upnp_ssdp_socket_delete(ssdpSock);
	
	cg_upnp_ssdprequest_delete(ssdpReq);

	return sentResult;
}


/****************************************
*
* Child Node
*
****************************************/

/****************************************
* cg_upnp_service_initchildnodes
****************************************/

static void cg_upnp_service_initchildnodes(CgUpnpService *service)
{
	cg_upnp_service_initactionlist(service);
	cg_upnp_service_initservicestatetable(service);
}

/****************************************
*
* Embedded Action
*
****************************************/

/****************************************
* cg_upnp_service_initactionlist
****************************************/

static void cg_upnp_service_initactionlist(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *actionListNode;
	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpAction *action;
	
	cg_upnp_actionlist_clear(service->actionList);
	
	scdpNode = cg_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	actionListNode = cg_xml_node_getchildnode(scdpNode, CG_UPNP_ACTIONLIST_ELEM_NAME);
	if (actionListNode == NULL)
		return;
		
	serviceNode = cg_upnp_service_getservicenode(service);
	for (childNode = cg_xml_node_getchildnodes(actionListNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_action_isactionnode(childNode) == FALSE)
			continue;
		action = cg_upnp_action_new();
		cg_upnp_action_setservice(action, service);
		cg_upnp_action_setactionnode(action, childNode);
		cg_upnp_actionlist_add(service->actionList, action);
	} 
}

/****************************************
* cg_upnp_service_getactionbyname
****************************************/

CgUpnpAction *cg_upnp_service_getactionbyname(CgUpnpService *service, char *name)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	cg_upnp_service_parseifnotparsed(service);
#endif
	actionList = cg_upnp_service_getactionlist(service);
	for (action=cg_upnp_actionlist_gets(actionList); action != NULL; action = cg_upnp_action_next(action)) {
		if (cg_upnp_action_isname(action, name) == TRUE)
			return action;
	}
	
	return NULL;
}

/****************************************
*
* Embedded ServiceStateTable
*
****************************************/

/****************************************
* cg_upnp_service_initservicestatetable
****************************************/

static void cg_upnp_service_initservicestatetable(CgUpnpService *service)
{
	CgXmlNode *scdpNode;
	CgXmlNode *stateTableNode;
	CgXmlNode *serviceNode;
	CgXmlNode *childNode;
	CgUpnpStateVariable *statVar;
	
	cg_upnp_servicestatetable_clear(service->serviceStateTable);
	
	scdpNode = cg_upnp_service_getscpdnode(service);
	if (scdpNode == NULL)
		return;
		
	stateTableNode = cg_xml_node_getchildnode(scdpNode, CG_UPNP_SERVICESTATETALBE_ELEM_NAME);
	if (stateTableNode == NULL)
		return;
		
	serviceNode = cg_upnp_service_getservicenode(service);
	for (childNode = cg_xml_node_getchildnodes(stateTableNode); childNode != NULL; childNode = cg_xml_node_next(childNode)) {
		if (cg_upnp_statevariable_isstatevariablenode(childNode) == FALSE)
			continue;
		statVar = cg_upnp_statevariable_new();
		cg_upnp_statevariable_setservice(statVar, service);
		cg_upnp_statevariable_setstatevariablenode(statVar, childNode);
		cg_upnp_servicestatetable_add(service->serviceStateTable, statVar);
	} 
}

/****************************************
* cg_upnp_service_getstatevariablebyname
****************************************/

CgUpnpStateVariable *cg_upnp_service_getstatevariablebyname(CgUpnpService *service, char *name)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	if (cg_strlen(name) <= 0)
		return NULL;
			
#ifdef CG_OPTIMIZED_CP_MODE
	cg_upnp_service_parseifnotparsed(service);
#endif
	stateTable = cg_upnp_service_getservicestatetable(service);
	for (stateVar=cg_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) {
		if (cg_upnp_statevariable_isname(stateVar, name) == TRUE)
			return stateVar;
	}
	
	return NULL;
}

/****************************************
* cg_upnp_service_setactionlistener
****************************************/

void cg_upnp_service_setactionlistener(CgUpnpService *service, CG_UPNP_ACTION_LISTNER actionListener)
{
	CgUpnpActionList *actionList;
	CgUpnpAction *action;
	
	actionList = cg_upnp_service_getactionlist(service);
	for (action=cg_upnp_actionlist_gets(actionList); action != NULL; action = cg_upnp_action_next(action))
		cg_upnp_action_setlistner(action, actionListener);
}

/****************************************
* cg_upnp_service_setquerylistener
****************************************/

void cg_upnp_service_setquerylistener(CgUpnpService *service, CG_UPNP_STATEVARIABLE_LISTNER queryListener)
{
	CgUpnpServiceStateTable *stateTable;
	CgUpnpStateVariable *stateVar;
	
	stateTable = cg_upnp_service_getservicestatetable(service);
	for (stateVar=cg_upnp_servicestatetable_gets(stateTable); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) 
		cg_upnp_statevariable_setlistener(stateVar, queryListener);
}

CgUpnpActionList *cg_upnp_service_getactionlist(CgUpnpService *service)
{
	cg_upnp_service_parseifnotparsed(service);
	return service->actionList;
}

CgUpnpAction *cg_upnp_service_getactions(CgUpnpService *service)
{
	cg_upnp_service_parseifnotparsed(service);
	return cg_upnp_actionlist_gets(service->actionList);
}

CgUpnpServiceStateTable *cg_upnp_service_getservicestatetable(CgUpnpService *service)
{
	cg_upnp_service_parseifnotparsed(service);
	return service->serviceStateTable;
}

CgUpnpStateVariable *cg_upnp_service_getstatevariables(CgUpnpService *service)
{
	cg_upnp_service_parseifnotparsed(service);
	return cg_upnp_servicestatetable_gets(service->serviceStateTable);
}

/****************************************
*
*	Subscription
*
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_service_addsubscriber
****************************************/

BOOL cg_upnp_service_addsubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	cg_upnp_service_lock(service);
	cg_upnp_subscriberlist_add(service->subscriberList, sub);
	cg_upnp_service_unlock(service);

	return TRUE;
}

/****************************************
* cg_upnp_service_removesubscriber
****************************************/

BOOL cg_upnp_service_removesubscriber(CgUpnpService *service, CgUpnpSubscriber *sub) 
{
	cg_upnp_service_lock(service);
	cg_upnp_subscriber_remove(sub);
	cg_upnp_service_unlock(service);

	return TRUE;
}

/****************************************
* cg_upnp_service_getsubscriberbysid
****************************************/

CgUpnpSubscriber *cg_upnp_service_getsubscriberbysid(CgUpnpService *service, char *sid)
{
	CgUpnpSubscriber *sub;
	int uuidIdx;

	if (cg_strlen(sid) <= 0)
		return NULL;
	
	uuidIdx = cg_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (0 <= uuidIdx)
		sid += cg_strlen(CG_UPNP_ST_UUID_DEVICE) + 1;

	for (sub = cg_upnp_service_getsubscribers(service); sub != NULL; sub = cg_upnp_subscriber_next(sub)) {
		if (cg_streq(sid, cg_upnp_subscriber_getsid(sub)) == TRUE)
			return sub;
	}
	
	return NULL;
}

#endif

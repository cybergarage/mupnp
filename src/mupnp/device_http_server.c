/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdevice_http_server.c
*
*	Revision:
*
*	03/22/05
*		- first revision
*
*	18-Jan-06 Aapo Makela
*		- Added HEAD-method support
*		- Fixed to handle "Not Found" case for GET-request
*		- Fixed to unescape URIs
******************************************************************/

#include <mupnp/device.h>
#include <mupnp/upnp.h>
#include <mupnp/http/http.h>
#include <mupnp/net/url.h>
#include <mupnp/control/control.h>
#include <mupnp/event/event.h>
#include <mupnp/util/log.h>

/****************************************
* prototype define for static functions
****************************************/

static BOOL mupnp_upnp_device_ispresentationrequest(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq);
static void mupnp_upnp_device_getrequestrecieved(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq);
static void mupnp_upnp_device_postrequestrecieved(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq);
static void mupnp_upnp_device_soapactionrecieved(mUpnpUpnpDevice *dev, mUpnpSoapRequest *soapReq);

static void mupnp_upnp_device_controlrequestrecieved(mUpnpUpnpService *service, mUpnpSoapRequest *soapReq);
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
static void mupnp_upnp_device_actioncontrolrequestrecieved(mUpnpUpnpService *service, mUpnpUpnpActionRequest *actionReq);
#endif
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
static void mupnp_upnp_device_querycontrolrequestrecieved(mUpnpUpnpService *service, mUpnpUpnpQueryRequest *queryReq);
#endif

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
static void mupnp_upnp_device_subscriptionrecieved(mUpnpUpnpDevice *dev, mUpnpUpnpSubscriptionRequest *subReq);
static void mupnp_upnp_device_newsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq);
static void mupnp_upnp_device_renewsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq);
static void mupnp_upnp_device_unsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq);
#endif

/****************************************
* mupnp_upnp_device_httprequestrecieved
****************************************/

void mupnp_upnp_device_httprequestrecieved(mUpnpHttpRequest *httpReq)
{
	mUpnpUpnpDevice *dev;
	mUpnpString *unescapedUrl;
	char *url;
	
	mupnp_log_debug_l4("Entering...\n");

	dev = (mUpnpUpnpDevice *)mupnp_http_request_getuserdata(httpReq);
	
	/* Unescape URI */
	url = mupnp_http_request_geturi(httpReq);
	if (0 < mupnp_strlen(url)) {
		unescapedUrl = mupnp_string_new();
		mupnp_net_uri_unescapestring(url, 0, unescapedUrl);
		if (0 < mupnp_string_length(unescapedUrl))
			mupnp_http_request_seturi(httpReq, mupnp_string_getvalue(unescapedUrl));
		mupnp_string_delete(unescapedUrl);
	}
	
  if (mupnp_upnp_device_ispresentationrequest(dev, httpReq) == TRUE) {
    CG_UPNP_PRESENTATION_LISTNER presentationListener = mupnp_upnp_device_getpresentationlistener(dev);
    if (presentationListener) {
      presentationListener(httpReq);
      return;
    }
  }
  
	if (mupnp_http_request_isgetrequest(httpReq) == TRUE ||
	    mupnp_http_request_isheadrequest(httpReq) == TRUE) {
		mupnp_upnp_device_getrequestrecieved(dev, httpReq);
		return;
	}

	if (mupnp_http_request_ispostrequest(httpReq) == TRUE) {
		mupnp_upnp_device_postrequestrecieved(dev, httpReq);
		return;
	}

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	if (mupnp_http_request_issubscriberequest(httpReq) == TRUE || mupnp_http_request_isunsubscriberequest(httpReq) == TRUE) {
		mupnp_upnp_device_subscriptionrecieved(dev, httpReq);
		return;
	}
#endif

	mupnp_http_request_postbadrequest(httpReq);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_upnp_device_ispresentationrequest
 ****************************************/

static BOOL mupnp_upnp_device_ispresentationrequest(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq)
{
  const char *presentationURL;
  const char *requestURI;
  
	if (!mupnp_http_request_isgetrequest(httpReq))
    return FALSE;
  
  presentationURL = mupnp_upnp_device_getpresentationurl(dev);
  if (!presentationURL)
    return FALSE;
  
  requestURI = mupnp_http_request_geturi(httpReq);
  if (!requestURI)
    return FALSE;
  
  return (0 < mupnp_strstr(requestURI, presentationURL)) ? TRUE : FALSE;
}


/****************************************
* 
* HTTP GET REQUEST
*
****************************************/

/****************************************
* mupnp_upnp_device_updateurlbase
****************************************/

void mupnp_upnp_device_seturlbase(mUpnpUpnpDevice *dev, char *value)
{
	mUpnpXmlNode *rootNode;
	mUpnpXmlNode *node;

	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_device_isrootdevice(dev) == FALSE)
		return;
	
	rootNode = mupnp_upnp_device_getrootnode(dev);
	if (rootNode == NULL)
		return;

	node = mupnp_xml_node_getchildnode(rootNode, CG_UPNP_DEVICE_URLBASE_NAME);
	if (node != NULL) {
		mupnp_xml_node_setvalue(node, value);
		return;
	}

	node = mupnp_xml_node_new();
	mupnp_xml_node_setname(node, CG_UPNP_DEVICE_URLBASE_NAME);
	mupnp_xml_node_setvalue(node, value);

	mupnp_xml_node_addchildnode(rootNode, node);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_updateurlbase(mUpnpUpnpDevice *dev, char *host)
{
	char urlBase[CG_UPNP_DEVICE_URLBASE_MAXLEN];

	mupnp_log_debug_l4("Entering...\n");

	mupnp_net_gethosturl(host, mupnp_upnp_device_gethttpport(dev), "", urlBase, sizeof(urlBase));
	mupnp_upnp_device_seturlbase(dev, urlBase);

	mupnp_log_debug_l4("Leaving...\n");
}

static char *mupnp_upnp_device_getdescription(mUpnpUpnpDevice *dev, char *ifAddr, mUpnpString *descStr)
{
	mUpnpXmlNode *rootNode;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_device_lock(dev);
	
	if (mupnp_upnp_isnmprmode() == FALSE)
		mupnp_upnp_device_updateurlbase(dev, ifAddr);
	
	rootNode = mupnp_upnp_device_getrootnode(dev);
	
	if (rootNode != NULL) {
		mupnp_string_addvalue(descStr, CG_UPNP_XML_DECLARATION);
		mupnp_string_addvalue(descStr, "\n");
		mupnp_xml_node_tostring(rootNode, TRUE, descStr);	
	}
	
	mupnp_upnp_device_unlock(dev);
	
	return mupnp_string_getvalue(descStr);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_getrequestrecieved(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq)
{
	mUpnpString *descStr;
	char *url;
	char *ifAddr;
	mUpnpUpnpService *embService;
	mUpnpUpnpDevice *embDev;
	mUpnpHttpResponse *httpRes;
	
	mupnp_log_debug_l4("Entering...\n");

	url = mupnp_http_request_geturi(httpReq);
	if (mupnp_strlen(url) <= 0) {
		mupnp_http_request_postbadrequest(httpReq);
		return;
	}

	descStr = mupnp_string_new();	
	ifAddr = mupnp_http_request_getlocaladdress(httpReq);

	mupnp_log_debug_s("Requested: |%s|, description: |%s|\n", url, mupnp_string_getvalue(dev->descriptionURI));
	if (mupnp_upnp_device_isdescriptionuri(dev, url) == TRUE) {
		mupnp_upnp_device_getdescription(dev, ifAddr, descStr);
	}
	else if ((embService = mupnp_upnp_device_getservicebyscpdurl(dev, url)) != NULL) {
		mupnp_upnp_service_getdescription(embService, descStr);
	}
	else if ((embDev = mupnp_upnp_device_getdevicebydescriptionuri(dev, url)) != NULL) {
		mupnp_upnp_device_getdescription(embDev, ifAddr, descStr);
	} else {
		/* Here we should handle Not Found case */
		mupnp_http_request_poststatuscode(httpReq, CG_HTTP_STATUS_NOT_FOUND);
		mupnp_string_delete(descStr);
		return;
	}
	
	httpRes = mupnp_http_response_new();
	mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	mupnp_http_response_setcontenttype(httpRes, CG_XML_CONTENT_TYPE);
	mupnp_http_response_setcontent(httpRes, mupnp_string_getvalue(descStr));
	mupnp_http_response_setcontentlength(httpRes, mupnp_string_length(descStr));
	
	if (mupnp_http_request_isheadrequest(httpReq) == TRUE)
	{
		/* If the request is head request, then clear message body */
		mupnp_http_response_setcontent(httpRes, NULL);
	}
	
	mupnp_http_response_print(httpRes);
	
	mupnp_http_request_postresponse(httpReq, httpRes);
	mupnp_http_response_delete(httpRes);	

	mupnp_string_delete(descStr);	

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_postrequestrecieved(mUpnpUpnpDevice *dev, mUpnpHttpRequest *httpReq)
{
	mUpnpSoapRequest *soapReq;
	
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_http_request_issoapaction(httpReq) == TRUE) {
		soapReq = mupnp_soap_request_new();
		mupnp_soap_request_sethttprequest(soapReq, httpReq);
		mupnp_upnp_device_soapactionrecieved(dev, soapReq);
		mupnp_soap_request_delete(soapReq);
		return;
	}
	mupnp_http_request_postbadrequest(httpReq);

	mupnp_log_debug_l4("Leaving...\n");
}


/****************************************
* 
* SOAP REQUEST
*
****************************************/

static void mupnp_upnp_device_badsoapactionrecieved(mUpnpHttpRequest *httpReq)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_request_postbadrequest(httpReq);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_invalidcontrolrecieved(mUpnpSoapRequest *soapReq, int code)
{
	mUpnpHttpRequest *httpReq;
	mUpnpSoapResponse *soapRes;
	mUpnpHttpResponse *httpRes;

	mupnp_log_debug_l4("Entering...\n");

	httpReq = mupnp_soap_request_gethttprequest(soapReq);
	
	soapRes = mupnp_soap_response_new();
	mupnp_upnp_control_soap_response_setfaultresponse(soapRes, code, mupnp_upnp_status_code2string(code));
	httpRes = mupnp_soap_response_gethttpresponse(soapRes);
	mupnp_http_request_postresponse(httpReq, httpRes);
	mupnp_soap_response_delete(soapRes);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_soapactionrecieved(mUpnpUpnpDevice *dev, mUpnpSoapRequest *soapReq)
{
	mUpnpHttpRequest *httpReq;
	mUpnpUpnpService *ctrlService;
	char *url;
	
	mupnp_log_debug_l4("Entering...\n");

	httpReq = mupnp_soap_request_gethttprequest(soapReq);
	
	url = mupnp_http_request_geturi(httpReq);
	mupnp_log_debug_s("Control url in device: %s\n", url);
	ctrlService = mupnp_upnp_device_getservicebycontrolurl(dev, url);
	
	if (ctrlService != NULL) {
		mupnp_upnp_device_controlrequestrecieved(ctrlService, soapReq);
		return;
	}
	
	mupnp_upnp_device_badsoapactionrecieved(httpReq);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_controlrequestrecieved(mUpnpUpnpService *service, mUpnpSoapRequest *soapReq)
{
	mUpnpHttpRequest *httpReq;
	mUpnpUpnpActionRequest *actionReq;
	mUpnpUpnpQueryRequest *queryReq;
	
	mupnp_log_debug_l4("Entering...\n");

	httpReq = mupnp_soap_request_gethttprequest(soapReq);
	
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
	if (mupnp_upnp_control_isqueryrequest(soapReq) == TRUE) {
		queryReq = mupnp_upnp_control_query_request_new();
		mupnp_upnp_control_query_request_setsoaprequest(queryReq, soapReq);
		mupnp_upnp_device_querycontrolrequestrecieved(service, queryReq);
		mupnp_upnp_control_query_request_delete(queryReq);
		return;
	}
#endif
	
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
	if (mupnp_upnp_control_isactionrequest(soapReq) == TRUE) {
		actionReq = mupnp_upnp_control_action_request_new();
		mupnp_upnp_control_action_request_setsoaprequest(actionReq, soapReq);
		mupnp_upnp_device_actioncontrolrequestrecieved(service, actionReq);
		mupnp_upnp_control_action_request_delete(actionReq);
		return;
	}
#endif
	
	mupnp_upnp_device_badsoapactionrecieved(httpReq);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* 
* SOAP REQUEST (Action)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

#define mupnp_upnp_device_invalidactioncontrolrecieved(actionReq) mupnp_upnp_device_invalidcontrolrecieved(mupnp_upnp_control_action_request_getsoaprequest(actionReq), CG_UPNP_STATUS_INVALID_ACTION)

static void mupnp_upnp_device_actioncontrolrequestrecieved(mUpnpUpnpService *service, mUpnpUpnpActionRequest *actionReq)
{
	char *actionName;
	mUpnpUpnpAction *action;
	mUpnpUpnpArgumentList *actionArgList;
	mUpnpUpnpArgumentList *actionReqArgList;
	
	mupnp_log_debug_l4("Entering...\n");

	actionName = mupnp_upnp_control_action_request_getactionname(actionReq);
	action = mupnp_upnp_service_getactionbyname(service, actionName);
	if (action == NULL) {
		mupnp_upnp_device_invalidactioncontrolrecieved(actionReq);
		return;
	}
	
	actionArgList = mupnp_upnp_action_getargumentlist(action);
	actionReqArgList = mupnp_upnp_control_action_request_getargumentlist(actionReq);
	mupnp_upnp_argumentlist_set(actionArgList, actionReqArgList);
	if (mupnp_upnp_action_performlistner(action, actionReq) == FALSE)
		mupnp_upnp_device_invalidactioncontrolrecieved(actionReq);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (Query)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

#define mupnp_upnp_device_invalidquerycontrolrecieved(queryReq) mupnp_upnp_device_invalidcontrolrecieved(mupnp_upnp_control_query_request_getsoaprequest(queryReq), CG_UPNP_STATUS_INVALID_VAR)

static void mupnp_upnp_device_querycontrolrequestrecieved(mUpnpUpnpService *service, mUpnpUpnpQueryRequest *queryReq)
{
	char *varName;
	mUpnpUpnpStateVariable *stateVar;
	
	mupnp_log_debug_l4("Entering...\n");

	varName = mupnp_upnp_control_query_request_getvarname(queryReq);
	if (mupnp_upnp_service_hasstatevariablebyname(service, varName) == FALSE) {
		mupnp_upnp_device_invalidquerycontrolrecieved(queryReq);
		return;
	}

	stateVar = mupnp_upnp_service_getstatevariablebyname(service, varName);
	if (mupnp_upnp_statevariable_performlistner(stateVar, queryReq) == FALSE)
		mupnp_upnp_device_invalidquerycontrolrecieved(queryReq);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (SubScribe)
*
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

static void mupnp_upnp_device_badsubscriptionrecieved(mUpnpUpnpSubscriptionRequest *subReq, int code)
{
	mUpnpUpnpSubscriptionResponse *subRes;
	
	mupnp_log_debug_l4("Entering...\n");

	subRes = mupnp_upnp_event_subscription_response_new();
	mupnp_upnp_event_subscription_response_setstatuscode(subRes, code);
	mupnp_upnp_event_subscription_request_postresponse(subReq, subRes);
	mupnp_upnp_event_subscription_response_delete(subRes);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_subscriptionrecieved(mUpnpUpnpDevice *dev, mUpnpUpnpSubscriptionRequest *subReq)
{
	char *uri;
	mUpnpUpnpService *service;
	
	mupnp_log_debug_l4("Entering...\n");

	uri = mupnp_http_request_geturi(subReq);
	if (mupnp_strlen(uri) <= 0) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
	service = mupnp_upnp_device_getservicebyeventsuburl(dev, uri);
	if (service == NULL) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
		
	if (mupnp_upnp_event_subscription_request_hascallback(subReq) == FALSE && mupnp_upnp_event_subscription_request_hassid(subReq) == FALSE) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}

        if (mupnp_upnp_event_subscription_request_hascallback(subReq) &&
            mupnp_strlen(mupnp_upnp_event_subscription_request_getcallback(subReq)) <= 0) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}

        if (mupnp_upnp_event_subscription_request_hassid(subReq) && 
            (mupnp_upnp_event_subscription_request_hascallback(subReq) ||
             mupnp_upnp_event_subscription_request_hasnt(subReq))) {
                mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_BAD_REQUEST);
		return;
        }

	if (mupnp_upnp_event_subscription_request_hasnt(subReq) &&
            (mupnp_strcmp(mupnp_upnp_event_subscription_request_getnt(subReq), CG_UPNP_NT_EVENT) != 0)) {
                mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
        }

	/**** UNSUBSCRIBE ****/
	if (mupnp_upnp_event_subscription_isunsubscriberequest(subReq) == TRUE) {
		mupnp_upnp_device_unsubscriptionrecieved(service, subReq);
		return;
	}

	/**** SUBSCRIBE (NEW) ****/
	if (mupnp_upnp_event_subscription_request_hascallback(subReq) == TRUE) {
		mupnp_upnp_device_newsubscriptionrecieved(service, subReq);
		return;
	}
		
	/**** SUBSCRIBE (RENEW) ****/
	if (mupnp_upnp_event_subscription_request_hassid(subReq) == TRUE) {
		mupnp_upnp_device_renewsubscriptionrecieved(service, subReq);
		return;
	}

	mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_newsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq)
{
	char *callback;
	char *aux;
	mUpnpTime timeout;
	char sid[CG_UPNP_SUBSCRIPTION_SID_SIZE];
	mUpnpUpnpSubscriber *sub;
	mUpnpUpnpSubscriptionResponse *subRes;
	
	mupnp_log_debug_l4("Entering...\n");

	aux = mupnp_strdup(mupnp_upnp_event_subscription_request_getcallback(subReq));
	if (aux == NULL)
		return;
	callback = mupnp_strltrim(aux, CG_UPNP_SUBSCRIPTION_CALLBACK_START_WITH, 1);
	mupnp_strrtrim(callback, CG_UPNP_SUBSCRIPTION_CALLBACK_END_WITH, 1);
	
	timeout = mupnp_upnp_event_subscription_request_gettimeout(subReq);
	/* Limit timeout to the given maximum */
	if (CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT > 0)
	{
		if (timeout > CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT ||
		    timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE)
		{
			timeout = CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT;
		}
	}
	
	mupnp_upnp_event_subscription_createsid(sid, sizeof(sid));

	sub = mupnp_upnp_subscriber_new();
	mupnp_upnp_subscriber_setdeliveryurl(sub, callback);
	free(aux);
	mupnp_upnp_subscriber_settimeout(sub, timeout);
	mupnp_upnp_subscriber_setsid(sub, sid);
	mupnp_upnp_service_addsubscriber(service, sub);

	subRes = mupnp_upnp_event_subscription_response_new();
	mupnp_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	mupnp_upnp_event_subscription_response_setsid(subRes, sid);
	mupnp_upnp_event_subscription_response_settimeout(subRes, timeout);
	mupnp_upnp_event_subscription_request_postresponse(subReq, subRes);
	mupnp_upnp_event_subscription_response_delete(subRes);

	mupnp_upnp_service_createnotifyallthread(service, CG_UPNP_SERVICE_NOTIFY_WAITTIME);

	mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_upnp_device_renewsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq)
{
	mUpnpUpnpSubscriber *sub;
	const char *sid;
	long timeout;
	mUpnpUpnpSubscriptionResponse *subRes;
	
	mupnp_log_debug_l4("Entering...\n");

	sid = mupnp_upnp_event_subscription_request_getsid(subReq);
	sub = mupnp_upnp_service_getsubscriberbysid(service, sid);

	if (sub == NULL) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		return;
	}
	
	timeout = mupnp_upnp_event_subscription_request_gettimeout(subReq);
	/* Limit timeout to the given maximum */
	if (CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT > 0)
	{
		if (timeout > CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT ||
		    timeout == CG_UPNP_SUBSCRIPTION_INFINITE_VALUE)
		{
			timeout = CG_UPNP_SUBSCRIPTION_MAX_TIMEOUT;
		}
	}
	
	mupnp_upnp_subscriber_settimeout(sub, timeout);
	mupnp_upnp_subscriber_renew(sub);

	subRes = mupnp_upnp_event_subscription_response_new();
	mupnp_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	mupnp_upnp_event_subscription_response_setsid(subRes, sid);
	mupnp_upnp_event_subscription_response_settimeout(subRes, timeout);
	mupnp_upnp_event_subscription_request_postresponse(subReq, subRes);
	mupnp_upnp_event_subscription_response_delete(subRes);

	mupnp_log_debug_l4("Leaving...\n");
}		

static void mupnp_upnp_device_unsubscriptionrecieved(mUpnpUpnpService *service, mUpnpUpnpSubscriptionRequest *subReq)
{
	mUpnpUpnpSubscriber *sub;
	const char *sid;
	mUpnpUpnpSubscriptionResponse *subRes;
	
	mupnp_log_debug_l4("Entering...\n");

	sid = mupnp_upnp_event_subscription_request_getsid(subReq);
	mupnp_upnp_service_lock(service);
	sub = mupnp_upnp_service_getsubscriberbysid(service, sid);

	if (sub == NULL) {
		mupnp_upnp_device_badsubscriptionrecieved(subReq, CG_HTTP_STATUS_PRECONDITION_FAILED);
		mupnp_upnp_service_unlock(service);
		return;
	}
	
	mupnp_upnp_service_removesubscriber(service, sub);
	mupnp_upnp_service_unlock(service);
	
	subRes = mupnp_upnp_event_subscription_response_new();
	mupnp_upnp_event_subscription_subscriberesponse_setresponse(subRes, CG_HTTP_STATUS_OK);
	mupnp_upnp_event_subscription_request_postresponse(subReq, subRes);
	mupnp_upnp_event_subscription_response_delete(subRes);

	mupnp_log_debug_l4("Leaving...\n");
}		

#endif

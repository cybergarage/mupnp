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
#include <mupnp/upnp.h>
#include <mupnp/http/http.h>
#include <mupnp/net/url.h>
#include <mupnp/control/control.h>
#include <mupnp/event/event.h>
#include <mupnp/util/log.h>

/****************************************
* prototype define for static functions
****************************************/

static bool mupnp_device_ispresentationrequest(mUpnpDevice* dev, mUpnpHttpRequest* httpReq);
static void mupnp_device_getrequestrecieved(mUpnpDevice* dev, mUpnpHttpRequest* httpReq);
static void mupnp_device_postrequestrecieved(mUpnpDevice* dev, mUpnpHttpRequest* httpReq);
static void mupnp_device_soapactionrecieved(mUpnpDevice* dev, mUpnpSoapRequest* soapReq);

static void mupnp_device_controlrequestrecieved(mUpnpService* service, mUpnpSoapRequest* soapReq);
#if !defined(MUPNP_NOUSE_ACTIONCTRL)
static void mupnp_device_actioncontrolrequestrecieved(mUpnpService* service, mUpnpActionRequest* actionReq);
#endif
#if !defined(MUPNP_NOUSE_QUERYCTRL)
static void mupnp_device_querycontrolrequestrecieved(mUpnpService* service, mUpnpQueryRequest* queryReq);
#endif

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
static void mupnp_device_subscriptionrecieved(mUpnpDevice* dev, mUpnpSubscriptionRequest* subReq);
static void mupnp_device_newsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq);
static void mupnp_device_renewsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq);
static void mupnp_device_unsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq);
#endif

/****************************************
* mupnp_device_httprequestrecieved
****************************************/

void mupnp_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  mUpnpDevice* dev;
  mUpnpString* unescapedUrl;
  char* url;

  mupnp_log_debug_l4("Entering...\n");

  dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  /* Unescape URI */
  url = mupnp_http_request_geturi(httpReq);
  if (0 < mupnp_strlen(url)) {
    unescapedUrl = mupnp_string_new();
    mupnp_net_uri_unescapestring(url, 0, unescapedUrl);
    if (0 < mupnp_string_length(unescapedUrl))
      mupnp_http_request_seturi(httpReq, mupnp_string_getvalue(unescapedUrl));
    mupnp_string_delete(unescapedUrl);
  }

  if (mupnp_device_ispresentationrequest(dev, httpReq) == true) {
    MUPNP_PRESENTATION_LISTNER presentationListener = mupnp_device_getpresentationlistener(dev);
    if (presentationListener) {
      presentationListener(httpReq);
      return;
    }
  }

  if (mupnp_http_request_isgetrequest(httpReq) == true || mupnp_http_request_isheadrequest(httpReq) == true) {
    mupnp_device_getrequestrecieved(dev, httpReq);
    return;
  }

  if (mupnp_http_request_ispostrequest(httpReq) == true) {
    mupnp_device_postrequestrecieved(dev, httpReq);
    return;
  }

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  if (mupnp_http_request_issubscriberequest(httpReq) == true || mupnp_http_request_isunsubscriberequest(httpReq) == true) {
    mupnp_device_subscriptionrecieved(dev, httpReq);
    return;
  }
#endif

  mupnp_http_request_postbadrequest(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_device_ispresentationrequest
 ****************************************/

static bool mupnp_device_ispresentationrequest(mUpnpDevice* dev, mUpnpHttpRequest* httpReq)
{
  const char* presentationURL;
  const char* requestURI;

  if (!mupnp_http_request_isgetrequest(httpReq))
    return false;

  presentationURL = mupnp_device_getpresentationurl(dev);
  if (!presentationURL)
    return false;

  requestURI = mupnp_http_request_geturi(httpReq);
  if (!requestURI)
    return false;

  return (0 < mupnp_strstr(requestURI, presentationURL)) ? true : false;
}

/****************************************
* 
* HTTP GET REQUEST
*
****************************************/

/****************************************
* mupnp_device_updateurlbase
****************************************/

void mupnp_device_seturlbase(mUpnpDevice* dev, char* value)
{
  mUpnpXmlNode* rootNode;
  mUpnpXmlNode* node;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_device_isrootdevice(dev) == false)
    return;

  rootNode = mupnp_device_getrootnode(dev);
  if (rootNode == NULL)
    return;

  node = mupnp_xml_node_getchildnode(rootNode, MUPNP_DEVICE_URLBASE_NAME);
  if (node != NULL) {
    mupnp_xml_node_setvalue(node, value);
    return;
  }

  node = mupnp_xml_node_new();
  mupnp_xml_node_setname(node, MUPNP_DEVICE_URLBASE_NAME);
  mupnp_xml_node_setvalue(node, value);

  mupnp_xml_node_addchildnode(rootNode, node);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_updateurlbase(mUpnpDevice* dev, char* host)
{
  char urlBase[MUPNP_DEVICE_URLBASE_MAXLEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_net_gethosturl(host, mupnp_device_gethttpport(dev), "", urlBase, sizeof(urlBase));
  mupnp_device_seturlbase(dev, urlBase);

  mupnp_log_debug_l4("Leaving...\n");
}

static char* mupnp_device_getdescription(mUpnpDevice* dev, char* ifAddr, mUpnpString* descStr)
{
  mUpnpXmlNode* rootNode;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_device_lock(dev);

  if (mupnp_isnmprmode() == false)
    mupnp_device_updateurlbase(dev, ifAddr);

  rootNode = mupnp_device_getrootnode(dev);

  if (rootNode != NULL) {
    mupnp_string_addvalue(descStr, MUPNP_XML_DECLARATION);
    mupnp_string_addvalue(descStr, "\n");
    mupnp_xml_node_tostring(rootNode, true, descStr);
  }

  mupnp_device_unlock(dev);

  return mupnp_string_getvalue(descStr);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_getrequestrecieved(mUpnpDevice* dev, mUpnpHttpRequest* httpReq)
{
  mUpnpString* descStr;
  char* url;
  char* ifAddr;
  mUpnpService* embService;
  mUpnpDevice* embDev;
  mUpnpHttpResponse* httpRes;

  mupnp_log_debug_l4("Entering...\n");

  url = mupnp_http_request_geturi(httpReq);
  if (mupnp_strlen(url) <= 0) {
    mupnp_http_request_postbadrequest(httpReq);
    return;
  }

  descStr = mupnp_string_new();
  ifAddr = mupnp_http_request_getlocaladdress(httpReq);

  mupnp_log_debug_s("Requested: |%s|, description: |%s|\n", url, mupnp_string_getvalue(dev->descriptionURI));
  if (mupnp_device_isdescriptionuri(dev, url) == true) {
    mupnp_device_getdescription(dev, ifAddr, descStr);
  }
  else if ((embService = mupnp_device_getservicebyscpdurl(dev, url)) != NULL) {
    mupnp_service_getdescription(embService, descStr);
  }
  else if ((embDev = mupnp_device_getdevicebydescriptionuri(dev, url)) != NULL) {
    mupnp_device_getdescription(embDev, ifAddr, descStr);
  }
  else {
    /* Here we should handle Not Found case */
    mupnp_http_request_poststatuscode(httpReq, MUPNP_HTTP_STATUS_NOT_FOUND);
    mupnp_string_delete(descStr);
    return;
  }

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontenttype(httpRes, MUPNP_XML_CONTENT_TYPE);
  mupnp_http_response_setcontent(httpRes, mupnp_string_getvalue(descStr));
  mupnp_http_response_setcontentlength(httpRes, mupnp_string_length(descStr));

  if (mupnp_http_request_isheadrequest(httpReq) == true) {
    /* If the request is head request, then clear message body */
    mupnp_http_response_setcontent(httpRes, NULL);
  }

  mupnp_http_response_print(httpRes);

  mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);

  mupnp_string_delete(descStr);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_postrequestrecieved(mUpnpDevice* dev, mUpnpHttpRequest* httpReq)
{
  mUpnpSoapRequest* soapReq;

  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_http_request_issoapaction(httpReq) == true) {
    soapReq = mupnp_soap_request_new();
    mupnp_soap_request_sethttprequest(soapReq, httpReq);
    mupnp_device_soapactionrecieved(dev, soapReq);
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

static void mupnp_device_badsoapactionrecieved(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_postbadrequest(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_invalidcontrolrecieved(mUpnpSoapRequest* soapReq, int code)
{
  mUpnpHttpRequest* httpReq;
  mUpnpSoapResponse* soapRes;
  mUpnpHttpResponse* httpRes;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = mupnp_soap_request_gethttprequest(soapReq);

  soapRes = mupnp_soap_response_new();
  mupnp_control_soap_response_setfaultresponse(soapRes, code, mupnp_status_code2string(code));
  httpRes = mupnp_soap_response_gethttpresponse(soapRes);
  mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_soap_response_delete(soapRes);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_soapactionrecieved(mUpnpDevice* dev, mUpnpSoapRequest* soapReq)
{
  mUpnpHttpRequest* httpReq;
  mUpnpService* ctrlService;
  char* url;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = mupnp_soap_request_gethttprequest(soapReq);

  url = mupnp_http_request_geturi(httpReq);
  mupnp_log_debug_s("Control url in device: %s\n", url);
  ctrlService = mupnp_device_getservicebycontrolurl(dev, url);

  if (ctrlService != NULL) {
    mupnp_device_controlrequestrecieved(ctrlService, soapReq);
    return;
  }

  mupnp_device_badsoapactionrecieved(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_controlrequestrecieved(mUpnpService* service, mUpnpSoapRequest* soapReq)
{
  mUpnpHttpRequest* httpReq;
  mUpnpActionRequest* actionReq;
  mUpnpQueryRequest* queryReq;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = mupnp_soap_request_gethttprequest(soapReq);

#if !defined(MUPNP_NOUSE_QUERYCTRL)
  if (mupnp_control_isqueryrequest(soapReq) == true) {
    queryReq = mupnp_control_query_request_new();
    mupnp_control_query_request_setsoaprequest(queryReq, soapReq);
    mupnp_device_querycontrolrequestrecieved(service, queryReq);
    mupnp_control_query_request_delete(queryReq);
    return;
  }
#endif

#if !defined(MUPNP_NOUSE_ACTIONCTRL)
  if (mupnp_control_isactionrequest(soapReq) == true) {
    actionReq = mupnp_control_action_request_new();
    mupnp_control_action_request_setsoaprequest(actionReq, soapReq);
    mupnp_device_actioncontrolrequestrecieved(service, actionReq);
    mupnp_control_action_request_delete(actionReq);
    return;
  }
#endif

  mupnp_device_badsoapactionrecieved(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* 
* SOAP REQUEST (Action)
*
****************************************/

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

#define mupnp_device_invalidactioncontrolrecieved(actionReq) mupnp_device_invalidcontrolrecieved(mupnp_control_action_request_getsoaprequest(actionReq), MUPNP_STATUS_INVALID_ACTION)

static void mupnp_device_actioncontrolrequestrecieved(mUpnpService* service, mUpnpActionRequest* actionReq)
{
  char* actionName;
  mUpnpAction* action;
  mUpnpArgumentList* actionArgList;
  mUpnpArgumentList* actionReqArgList;

  mupnp_log_debug_l4("Entering...\n");

  actionName = mupnp_control_action_request_getactionname(actionReq);
  action = mupnp_service_getactionbyname(service, actionName);
  if (action == NULL) {
    mupnp_device_invalidactioncontrolrecieved(actionReq);
    return;
  }

  actionArgList = mupnp_action_getargumentlist(action);
  actionReqArgList = mupnp_control_action_request_getargumentlist(actionReq);
  mupnp_argumentlist_set(actionArgList, actionReqArgList);
  if (mupnp_action_performlistner(action, actionReq) == false)
    mupnp_device_invalidactioncontrolrecieved(actionReq);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (Query)
*
****************************************/

#if !defined(MUPNP_NOUSE_QUERYCTRL)

#define mupnp_device_invalidquerycontrolrecieved(queryReq) mupnp_device_invalidcontrolrecieved(mupnp_control_query_request_getsoaprequest(queryReq), MUPNP_STATUS_INVALID_VAR)

static void mupnp_device_querycontrolrequestrecieved(mUpnpService* service, mUpnpQueryRequest* queryReq)
{
  char* varName;
  mUpnpStateVariable* stateVar;

  mupnp_log_debug_l4("Entering...\n");

  varName = mupnp_control_query_request_getvarname(queryReq);
  if (mupnp_service_hasstatevariablebyname(service, varName) == false) {
    mupnp_device_invalidquerycontrolrecieved(queryReq);
    return;
  }

  stateVar = mupnp_service_getstatevariablebyname(service, varName);
  if (mupnp_statevariable_performlistner(stateVar, queryReq) == false)
    mupnp_device_invalidquerycontrolrecieved(queryReq);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* 
* SOAP REQUEST (SubScribe)
*
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

static void mupnp_device_badsubscriptionrecieved(mUpnpSubscriptionRequest* subReq, int code)
{
  mUpnpSubscriptionResponse* subRes;

  mupnp_log_debug_l4("Entering...\n");

  subRes = mupnp_event_subscription_response_new();
  mupnp_event_subscription_response_setstatuscode(subRes, code);
  mupnp_event_subscription_request_postresponse(subReq, subRes);
  mupnp_event_subscription_response_delete(subRes);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_subscriptionrecieved(mUpnpDevice* dev, mUpnpSubscriptionRequest* subReq)
{
  char* uri;
  mUpnpService* service;

  mupnp_log_debug_l4("Entering...\n");

  uri = mupnp_http_request_geturi(subReq);
  if (mupnp_strlen(uri) <= 0) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }
  service = mupnp_device_getservicebyeventsuburl(dev, uri);
  if (service == NULL) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }

  if (mupnp_event_subscription_request_hascallback(subReq) == false && mupnp_event_subscription_request_hassid(subReq) == false) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }

  if (mupnp_event_subscription_request_hascallback(subReq) && mupnp_strlen(mupnp_event_subscription_request_getcallback(subReq)) <= 0) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }

  if (mupnp_event_subscription_request_hassid(subReq) && (mupnp_event_subscription_request_hascallback(subReq) || mupnp_event_subscription_request_hasnt(subReq))) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_BAD_REQUEST);
    return;
  }

  if (mupnp_event_subscription_request_hasnt(subReq) && (mupnp_strcmp(mupnp_event_subscription_request_getnt(subReq), MUPNP_NT_EVENT) != 0)) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }

  /**** UNSUBSCRIBE ****/
  if (mupnp_event_subscription_isunsubscriberequest(subReq) == true) {
    mupnp_device_unsubscriptionrecieved(service, subReq);
    return;
  }

  /**** SUBSCRIBE (NEW) ****/
  if (mupnp_event_subscription_request_hascallback(subReq) == true) {
    mupnp_device_newsubscriptionrecieved(service, subReq);
    return;
  }

  /**** SUBSCRIBE (RENEW) ****/
  if (mupnp_event_subscription_request_hassid(subReq) == true) {
    mupnp_device_renewsubscriptionrecieved(service, subReq);
    return;
  }

  mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_newsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq)
{
  char* callback;
  char* aux;
  mUpnpTime timeout;
  char sid[MUPNP_SUBSCRIPTION_SID_SIZE];
  mUpnpSubscriber* sub;
  mUpnpSubscriptionResponse* subRes;

  mupnp_log_debug_l4("Entering...\n");

  aux = mupnp_strdup(mupnp_event_subscription_request_getcallback(subReq));
  if (aux == NULL)
    return;
  callback = mupnp_strltrim(aux, MUPNP_SUBSCRIPTION_CALLBACK_START_WITH, 1);
  mupnp_strrtrim(callback, MUPNP_SUBSCRIPTION_CALLBACK_END_WITH, 1);

  timeout = mupnp_event_subscription_request_gettimeout(subReq);
  /* Limit timeout to the given maximum */
  if (MUPNP_SUBSCRIPTION_MAX_TIMEOUT > 0) {
    if (timeout > MUPNP_SUBSCRIPTION_MAX_TIMEOUT || timeout == MUPNP_SUBSCRIPTION_INFINITE_VALUE) {
      timeout = MUPNP_SUBSCRIPTION_MAX_TIMEOUT;
    }
  }

  mupnp_event_subscription_createsid(sid, sizeof(sid));

  sub = mupnp_subscriber_new();
  mupnp_subscriber_setdeliveryurl(sub, callback);
  free(aux);
  mupnp_subscriber_settimeout(sub, timeout);
  mupnp_subscriber_setsid(sub, sid);
  mupnp_service_addsubscriber(service, sub);

  subRes = mupnp_event_subscription_response_new();
  mupnp_event_subscription_subscriberesponse_setresponse(subRes, MUPNP_HTTP_STATUS_OK);
  mupnp_event_subscription_response_setsid(subRes, sid);
  mupnp_event_subscription_response_settimeout(subRes, timeout);
  mupnp_event_subscription_request_postresponse(subReq, subRes);
  mupnp_event_subscription_response_delete(subRes);

  mupnp_service_createnotifyallthread(service, MUPNP_SERVICE_NOTIFY_WAITTIME);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_renewsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq)
{
  mUpnpSubscriber* sub;
  const char* sid;
  long timeout;
  mUpnpSubscriptionResponse* subRes;

  mupnp_log_debug_l4("Entering...\n");

  sid = mupnp_event_subscription_request_getsid(subReq);
  sub = mupnp_service_getsubscriberbysid(service, sid);

  if (sub == NULL) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    return;
  }

  timeout = mupnp_event_subscription_request_gettimeout(subReq);
  /* Limit timeout to the given maximum */
  if (MUPNP_SUBSCRIPTION_MAX_TIMEOUT > 0) {
    if (timeout > MUPNP_SUBSCRIPTION_MAX_TIMEOUT || timeout == MUPNP_SUBSCRIPTION_INFINITE_VALUE) {
      timeout = MUPNP_SUBSCRIPTION_MAX_TIMEOUT;
    }
  }

  mupnp_subscriber_settimeout(sub, timeout);
  mupnp_subscriber_renew(sub);

  subRes = mupnp_event_subscription_response_new();
  mupnp_event_subscription_subscriberesponse_setresponse(subRes, MUPNP_HTTP_STATUS_OK);
  mupnp_event_subscription_response_setsid(subRes, sid);
  mupnp_event_subscription_response_settimeout(subRes, timeout);
  mupnp_event_subscription_request_postresponse(subReq, subRes);
  mupnp_event_subscription_response_delete(subRes);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_device_unsubscriptionrecieved(mUpnpService* service, mUpnpSubscriptionRequest* subReq)
{
  mUpnpSubscriber* sub;
  const char* sid;
  mUpnpSubscriptionResponse* subRes;

  mupnp_log_debug_l4("Entering...\n");

  sid = mupnp_event_subscription_request_getsid(subReq);
  mupnp_service_lock(service);
  sub = mupnp_service_getsubscriberbysid(service, sid);

  if (sub == NULL) {
    mupnp_device_badsubscriptionrecieved(subReq, MUPNP_HTTP_STATUS_PRECONDITION_FAILED);
    mupnp_service_unlock(service);
    return;
  }

  mupnp_service_removesubscriber(service, sub);
  mupnp_service_unlock(service);

  subRes = mupnp_event_subscription_response_new();
  mupnp_event_subscription_subscriberesponse_setresponse(subRes, MUPNP_HTTP_STATUS_OK);
  mupnp_event_subscription_request_postresponse(subReq, subRes);
  mupnp_event_subscription_response_delete(subRes);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

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

#include <errno.h>
#include <limits.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>
#include <string.h>

/****************************************
 * MUPNP_NOUSE_ACTIONCTRL (Begin)
 ****************************************/

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

/****************************************
 * mupnp_action_clearoutputargumentvalues
 ****************************************/

void mupnp_action_clearoutputargumentvalues(mUpnpAction* action)
{
  mUpnpArgumentList* argList;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  argList = mupnp_action_getargumentlist(action);
  for (arg = mupnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isoutdirection(arg) == true)
      mupnp_argument_setvalue(arg, "");
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_action_performlistener
 ****************************************/

bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq)
{
  MUPNP_ACTION_LISTNER listener;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* actionReqHttpReq;
  mUpnpHttpResponse* actionResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_action_getlistner(action);
  if (listener == NULL)
    return false;

  actionRes = mupnp_control_action_response_new();

  mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ACTION);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));

  mupnp_action_clearoutputargumentvalues(action);

  if (listener(action) == true)
    mupnp_control_action_response_setresponse(actionRes, action);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_action_response_getsoapresponse(actionRes), mupnp_action_getstatuscode(action), mupnp_action_getstatusdescription(action));

  actionReqHttpReq = mupnp_soap_request_gethttprequest(mupnp_control_action_request_getsoaprequest(actionReq));
  actionResHttpRes = mupnp_soap_response_gethttpresponse(mupnp_control_action_response_getsoapresponse(actionRes));
  mupnp_http_request_postresponse(actionReqHttpReq, actionResHttpRes);

  mupnp_control_action_response_delete(actionRes);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_action_post
 ****************************************/

bool mupnp_action_post(mUpnpAction* action)
{
  mUpnpActionRequest* actionReq;
  mUpnpActionResponse* actionRes;
  bool actionSuccess;

  mupnp_log_debug_l4("Entering...\n");

  actionReq = mupnp_control_action_request_new();

  mupnp_control_action_request_setaction(actionReq, action);
  actionRes = mupnp_control_action_request_post(actionReq);
  actionSuccess = mupnp_control_action_response_issuccessful(actionRes);
  if (actionSuccess == true) {
    /* Reset status code to 0 (otherwise latest error stays in action) */
    mupnp_action_setstatuscode(action, 0);
    if (mupnp_control_action_response_getresult(actionRes, action) == false) {
      actionSuccess = false;
    }
  }
  else {
    /* Action was unsuccesful, but put the error to the action */
    mupnp_control_action_response_geterror(actionRes, action);
  }

  mupnp_control_action_request_delete(actionReq);

  mupnp_log_debug_l4("Leaving...\n");

  return actionSuccess;
}

/****************************************
 * Independent strict action invocations
 ****************************************/

struct _mUpnpActionResult {
  mUpnpActionResultCode code;
  int httpStatusCode;
  int upnpErrorCode;
  mUpnpDictionary* outputs;
  mUpnpString* faultCode;
  mUpnpString* faultString;
  mUpnpString* errorDescription;
};

static mUpnpActionResult* mupnp_action_result_new(void)
{
  mUpnpActionResult* result = calloc(1, sizeof(mUpnpActionResult));
  if (!result)
    return NULL;
  result->code = MUPNP_ACTION_RESULT_PARSE_ERROR;
  result->outputs = mupnp_dictionary_new();
  result->faultCode = mupnp_string_new();
  result->faultString = mupnp_string_new();
  result->errorDescription = mupnp_string_new();
  if (!result->outputs || !result->faultCode || !result->faultString || !result->errorDescription) {
    mupnp_action_result_delete(result);
    return NULL;
  }
  return result;
}

void mupnp_action_result_delete(mUpnpActionResult* result)
{
  if (!result)
    return;
  if (result->outputs)
    mupnp_dictionary_delete(result->outputs);
  if (result->faultCode)
    mupnp_string_delete(result->faultCode);
  if (result->faultString)
    mupnp_string_delete(result->faultString);
  if (result->errorDescription)
    mupnp_string_delete(result->errorDescription);
  free(result);
}

mUpnpActionResultCode mupnp_action_result_getcode(const mUpnpActionResult* result) { return result->code; }
int mupnp_action_result_gethttpstatuscode(const mUpnpActionResult* result) { return result->httpStatusCode; }
int mupnp_action_result_getupnperrorcode(const mUpnpActionResult* result) { return result->upnpErrorCode; }
const char* mupnp_action_result_getfaultcode(const mUpnpActionResult* result) { return mupnp_string_getvalue(result->faultCode); }
const char* mupnp_action_result_getfaultstring(const mUpnpActionResult* result) { return mupnp_string_getvalue(result->faultString); }
const char* mupnp_action_result_geterrordescription(const mUpnpActionResult* result) { return mupnp_string_getvalue(result->errorDescription); }
mUpnpDictionary* mupnp_action_result_getoutputs(const mUpnpActionResult* result) { return result->outputs; }
const char* mupnp_action_result_getoutput(const mUpnpActionResult* result, const char* name)
{
  return mupnp_dictionary_getvalue(result->outputs, name);
}

static const char* mupnp_action_localname(mUpnpXmlNode* node)
{
  const char* name = node ? mupnp_xml_node_getname(node) : NULL;
  const char* colon = name ? strchr(name, ':') : NULL;
  return colon ? colon + 1 : name;
}

/* Resolve the element's namespace, including inherited/default bindings. */
static bool mupnp_action_node_is(mUpnpXmlNode* node, const char* name, const char* ns)
{
  const char* full = node ? mupnp_xml_node_getname(node) : NULL;
  const char* colon = full ? strchr(full, ':') : NULL;
  char attribute[256] = "xmlns";
  if (!mupnp_streq(mupnp_action_localname(node), name))
    return false;
  if (colon) {
    size_t size = (size_t)(colon - full);
    if (size > sizeof(attribute) - 7)
      return false;
    memcpy(attribute + 5, ":", 1);
    memcpy(attribute + 6, full, size);
    attribute[6 + size] = '\0';
  }
  for (; node; node = mupnp_xml_node_getparentnode(node)) {
    const char* value = mupnp_xml_node_getattributevalue(node, attribute);
    if (value)
      return mupnp_streq(value, ns);
  }
  return false;
}

static bool mupnp_action_validschema(mUpnpAction* action)
{
  if (!action || !mupnp_action_getservice(action) || mupnp_strlen(mupnp_action_getname(action)) <= 0)
    return false;
  for (mUpnpArgument* arg = mupnp_action_getarguments(action); arg; arg = mupnp_argument_next(arg)) {
    const char* name = mupnp_argument_getname(arg);
    if (mupnp_strlen(name) <= 0 || (!mupnp_streq(mupnp_argument_getdirection(arg), "in") && !mupnp_streq(mupnp_argument_getdirection(arg), "out")))
      return false;
    for (mUpnpArgument* previous = mupnp_action_getarguments(action); previous != arg; previous = mupnp_argument_next(previous)) {
      if (mupnp_streq(mupnp_argument_getname(previous), name))
        return false;
    }
  }
  return true;
}

static bool mupnp_action_validinputs(mUpnpAction* action, mUpnpDictionary* inputs)
{
  if (!mupnp_action_validschema(action))
    return false;
  for (mUpnpArgument* arg = mupnp_action_getarguments(action); arg; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isindirection(arg) && (!inputs || !mupnp_dictionary_getvalue(inputs, mupnp_argument_getname(arg))))
      return false;
  }
  for (mUpnpDictionaryElement* entry = inputs ? mupnp_dictionary_gets(inputs) : NULL; entry; entry = mupnp_dictionary_element_next(entry)) {
    mUpnpArgument* arg = mupnp_action_getargumentbyname(action, mupnp_dictionary_element_getkey(entry));
    if (!arg || !mupnp_argument_isindirection(arg) || !mupnp_dictionary_element_getvalue(entry))
      return false;
    for (mUpnpDictionaryElement* previous = mupnp_dictionary_gets(inputs); previous != entry; previous = mupnp_dictionary_element_next(previous)) {
      if (mupnp_streq(mupnp_dictionary_element_getkey(previous), mupnp_dictionary_element_getkey(entry)))
        return false;
    }
  }
  return true;
}

static mUpnpXmlNode* mupnp_action_responsecontent(mUpnpXmlNode* envelope)
{
  mUpnpXmlNode* body = NULL;
  mUpnpXmlNode* response;
  if (!mupnp_action_node_is(envelope, "Envelope", MUPNP_SOAP_XMLNS_URL) || mupnp_xml_node_next(envelope))
    return NULL;
  for (mUpnpXmlNode* node = mupnp_xml_node_getchildnodes(envelope); node; node = mupnp_xml_node_next(node)) {
    if (mupnp_action_node_is(node, "Body", MUPNP_SOAP_XMLNS_URL)) {
      if (body)
        return NULL;
      body = node;
    }
  }
  response = body ? mupnp_xml_node_getchildnodes(body) : NULL;
  return response && !mupnp_xml_node_next(response) ? response : NULL;
}

static int mupnp_action_parseerrorcode(mUpnpXmlNode* node)
{
  const char* value = node ? mupnp_xml_node_getvalue(node) : NULL;
  char* end = NULL;
  long code;
  if (!value)
    return 0;
  errno = 0;
  code = strtol(value, &end, 10);
  return end != value && !*end && errno == 0 && code > 0 && code <= INT_MAX ? (int)code : 0;
}

static void mupnp_action_result_setfault(mUpnpActionResult* result, mUpnpXmlNode* response)
{
  mUpnpXmlNode* node;
  mUpnpXmlNode* detail;
  mUpnpXmlNode* error;
  result->code = MUPNP_ACTION_RESULT_SOAP_FAULT;
  node = mupnp_xml_node_getchildnodewithnamespace(response, "faultcode", NULL, true);
  if (node)
    mupnp_string_setvalue(result->faultCode, mupnp_xml_node_getvalue(node));
  node = mupnp_xml_node_getchildnodewithnamespace(response, "faultstring", NULL, true);
  if (node)
    mupnp_string_setvalue(result->faultString, mupnp_xml_node_getvalue(node));
  detail = mupnp_xml_node_getchildnodewithnamespace(response, "detail", NULL, true);
  error = detail ? mupnp_xml_node_getchildnodewithnamespace(detail, "UPnPError", NULL, true) : NULL;
  if (!mupnp_action_node_is(error, "UPnPError", "urn:schemas-upnp-org:control-1-0"))
    return;
  node = mupnp_xml_node_getchildnodewithnamespace(error, "errorCode", NULL, true);
  result->upnpErrorCode = mupnp_action_parseerrorcode(node);
  node = mupnp_xml_node_getchildnodewithnamespace(error, "errorDescription", NULL, true);
  if (node)
    mupnp_string_setvalue(result->errorDescription, mupnp_xml_node_getvalue(node));
}

static void mupnp_action_result_setoutputs(mUpnpActionResult* result, mUpnpAction* action, mUpnpXmlNode* response)
{
  mUpnpString* responseName = mupnp_string_new();
  bool matches;
  if (!responseName)
    return;
  mupnp_string_setvalue(responseName, mupnp_action_getname(action));
  mupnp_string_addvalue(responseName, "Response");
  matches = mupnp_action_node_is(response, mupnp_string_getvalue(responseName), mupnp_service_getservicetype(mupnp_action_getservice(action)));
  mupnp_string_delete(responseName);
  if (!matches)
    return;
  for (mUpnpXmlNode* node = mupnp_xml_node_getchildnodes(response); node; node = mupnp_xml_node_next(node)) {
    const char* name = mupnp_action_localname(node);
    const char* value = mupnp_xml_node_getvalue(node);
    mUpnpArgument* arg = mupnp_action_getargumentbyname(action, name);
    if (!arg || !mupnp_argument_isoutdirection(arg) || mupnp_dictionary_getvalue(result->outputs, name) || mupnp_xml_node_haschildnodes(node)) {
      mupnp_dictionary_clear(result->outputs);
      return;
    }
    mupnp_dictionary_setvalue(result->outputs, name, value ? value : "");
  }
  result->code = MUPNP_ACTION_RESULT_SUCCESS;
  for (mUpnpArgument* arg = mupnp_action_getarguments(action); arg; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isoutdirection(arg) && !mupnp_dictionary_getvalue(result->outputs, mupnp_argument_getname(arg)))
      result->code = MUPNP_ACTION_RESULT_INCOMPLETE;
  }
}

mUpnpActionResult* mupnp_action_result_fromresponse(mUpnpAction* action, int httpStatusCode, const char* content, size_t length)
{
  mUpnpActionResult* result = mupnp_action_result_new();
  mUpnpXmlParser* parser;
  mUpnpXmlNodeList* nodes;
  mUpnpXmlNode* response;
  bool parsed;
  if (!result)
    return NULL;
  result->httpStatusCode = httpStatusCode;
  if (httpStatusCode == 0) {
    result->code = MUPNP_ACTION_RESULT_TRANSPORT_ERROR;
    return result;
  }
  /* Preserve HTTP failures even when a proxy supplies non-SOAP content. */
  if (httpStatusCode < 200 || httpStatusCode >= 300)
    result->code = MUPNP_ACTION_RESULT_HTTP_ERROR;
  if (!action || !mupnp_action_getservice(action) || !content || !length)
    return result;
  nodes = mupnp_xml_nodelist_new();
  parser = mupnp_xml_parser_new();
  if (!nodes || !parser) {
    if (nodes)
      mupnp_xml_nodelist_delete(nodes);
    if (parser)
      mupnp_xml_parser_delete(parser);
    mupnp_action_result_delete(result);
    return NULL;
  }
  parsed = mupnp_xml_parse(parser, nodes, content, length);
  mupnp_xml_parser_delete(parser);
  response = parsed ? mupnp_action_responsecontent(mupnp_xml_nodelist_gets(nodes)) : NULL;
  if (response) {
    if (mupnp_action_node_is(response, "Fault", MUPNP_SOAP_XMLNS_URL))
      mupnp_action_result_setfault(result, response);
    else if (result->code != MUPNP_ACTION_RESULT_HTTP_ERROR)
      mupnp_action_result_setoutputs(result, action, response);
  }
  mupnp_xml_nodelist_delete(nodes);
  return result;
}

mUpnpActionResult* mupnp_action_postwitharguments(mUpnpAction* action, mUpnpDictionary* inputs)
{
  mUpnpActionResult* result;
  mUpnpAction* invocation;
  mUpnpActionRequest* request;
  mUpnpSoapRequest* soap;
  mUpnpHttpRequest* http;
  mUpnpHttpResponse* response;
  mUpnpNetURL* url;
  bool received = false;
  if (!mupnp_action_validinputs(action, inputs)) {
    result = mupnp_action_result_new();
    if (result)
      result->code = MUPNP_ACTION_RESULT_INVALID_ARGUMENTS;
    return result;
  }
  invocation = mupnp_action_new();
  if (!invocation)
    return NULL;
  /* Argument values are independent; the SCPD node is borrowed read-only. */
  mupnp_action_setactionnode(invocation, mupnp_action_getactionnode(action));
  mupnp_action_setservice(invocation, mupnp_action_getservice(action));
  for (mUpnpArgument* arg = mupnp_action_getarguments(invocation); arg; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isindirection(arg))
      mupnp_argument_setvalue(arg, mupnp_dictionary_getvalue(inputs, mupnp_argument_getname(arg)));
  }
  request = mupnp_control_action_request_new();
  if (!request) {
    mupnp_action_delete(invocation);
    return NULL;
  }
  mupnp_control_action_request_setaction(request, invocation);
  soap = mupnp_control_action_request_getsoaprequest(request);
  http = mupnp_soap_request_gethttprequest(soap);
  url = mupnp_http_request_getposturl(http);
  /* Deliberately bypass SOAP's 405 -> M-POST automatic retry. */
  response = mupnp_http_request_postonce(http, mupnp_net_url_gethost(url), mupnp_net_url_getport(url), &received);
  result = mupnp_action_result_fromresponse(action, received && response ? mupnp_http_response_getstatuscode(response) : 0, response ? mupnp_http_response_getcontent(response) : NULL, response ? mupnp_string_length(response->content) : 0);
  mupnp_control_action_request_delete(request);
  mupnp_action_delete(invocation);
  return result;
}

/****************************************
 * MUPNP_NOUSE_ACTIONCTRL (End)
 ****************************************/

#endif

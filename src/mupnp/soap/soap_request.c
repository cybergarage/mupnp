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

#include <mupnp/soap/soap.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>

/****************************************
* mupnp_soap_request_new
****************************************/

mUpnpSoapRequest* mupnp_soap_request_new()
{
  mUpnpSoapRequest* soapReq;

  mupnp_log_debug_l4("Entering...\n");

  soapReq = (mUpnpSoapRequest*)malloc(sizeof(mUpnpSoapRequest));

  if (NULL != soapReq) {
    soapReq->rootNodeList = mupnp_xml_nodelist_new();
    soapReq->soapRes = mupnp_soap_response_new();

    soapReq->httpReq = mupnp_http_request_new();
    soapReq->isHttpReqCreated = true;
    mupnp_http_request_setcontenttype(soapReq->httpReq, MUPNP_SOAP_CONTENT_TYPE);
    mupnp_http_request_setmethod(soapReq->httpReq, MUPNP_HTTP_POST);

    mupnp_soap_request_setuserdata(soapReq, NULL);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return soapReq;
}

/****************************************
* mupnp_soap_request_delete
****************************************/

void mupnp_soap_request_delete(mUpnpSoapRequest* soapReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_soap_request_clear(soapReq);
  mupnp_xml_nodelist_delete(soapReq->rootNodeList);

  if (soapReq->isHttpReqCreated == true)
    mupnp_http_request_delete(soapReq->httpReq);

  mupnp_soap_response_delete(soapReq->soapRes);

  free(soapReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_request_clear
****************************************/

void mupnp_soap_request_clear(mUpnpSoapRequest* soapReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_xml_nodelist_clear(soapReq->rootNodeList);

  if (soapReq->isHttpReqCreated == true)
    mupnp_http_request_delete(soapReq->httpReq);

  soapReq->httpReq = mupnp_http_request_new();
  soapReq->isHttpReqCreated = true;
  mupnp_http_request_setcontenttype(soapReq->httpReq, MUPNP_SOAP_CONTENT_TYPE);
  mupnp_http_request_setmethod(soapReq->httpReq, MUPNP_HTTP_POST);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_soap_request_getbodynode
****************************************/

mUpnpXmlNode* mupnp_soap_request_getbodynode(mUpnpSoapRequest* soapReq)
{
  mUpnpXmlNode* envNode;
  mUpnpXmlNode* bodyNode = NULL;
  mUpnpXmlAttribute* attr;
  char* name;
  mUpnpStringTokenizer* tok;
  char* nsPrefix;
  size_t bodyLen;
  char* body;

  mupnp_log_debug_l4("Entering...\n");

  envNode = mupnp_soap_request_getenvelopenode(soapReq);
  if (envNode == NULL)
    return NULL;
  if (mupnp_xml_node_haschildnodes(envNode) == false)
    return NULL;

  /* We cannot assume the namespace prefix for Body is 's'. 
           According to spec, it could be anything... */
  for (attr = mupnp_xml_node_getattributes(envNode); attr != NULL;
       attr = mupnp_xml_attribute_next(attr)) {
    /* First, find the namespace declaration attribute. */
    /* Note: We must take a copy of the attr name. 
                   Tokenizer doesn't do it (by default) */
    name = mupnp_strdup(mupnp_xml_attribute_getname(attr));
    tok = mupnp_string_tokenizer_new(name, ":");

    nsPrefix = mupnp_string_tokenizer_nexttoken(tok);
    if (-1 != mupnp_strstr(nsPrefix, "xmlns")) {
      /* This attribute is a namespace declaration. Check is 
                           it the one defined for SOAP. */
      if (mupnp_strcmp(mupnp_xml_attribute_getvalue(attr), MUPNP_SOAP_XMLNS_URL) == 0) {
        /* This namespace declaration is correct. 
                                   Use it to find the body node... */
        if (mupnp_string_tokenizer_hasmoretoken(tok)) {
          /* There is a prefix */
          nsPrefix = mupnp_string_tokenizer_nexttoken(tok);
          bodyLen = mupnp_strlen(nsPrefix) + mupnp_strlen(MUPNP_SOAP_DELIM) + mupnp_strlen(MUPNP_SOAP_BODY) + 1; /* +1 for trailing '\0'*/
          body = (char*)malloc(bodyLen);

          if (NULL == body) {
            mupnp_log_debug_s("Memory allocation failure!\n");
            return NULL;
          }
#if defined(HAVE_SNPRINTF)
          snprintf(body, bodyLen, "%s%s%s", nsPrefix, MUPNP_SOAP_DELIM, MUPNP_SOAP_BODY);
#else
          sprintf(body, "%s%s%s", nsPrefix, MUPNP_SOAP_DELIM, MUPNP_SOAP_BODY);
#endif
          bodyNode = mupnp_xml_node_getchildnode(envNode, body);
          free(body);
        }
        else {
          /* No prefix */
          bodyNode = mupnp_xml_node_getchildnode(envNode, MUPNP_SOAP_BODY);
        }
        /* Free memory before leaving the loop */
        mupnp_string_tokenizer_delete(tok);
        free(name);
        break;
      }
    }
    mupnp_string_tokenizer_delete(tok);
    free(name);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return bodyNode;
}

/****************************************
* mupnp_soap_request_sethttprequest
****************************************/

bool mupnp_soap_request_sethttprequest(mUpnpSoapRequest* soapReq, mUpnpHttpRequest* httpReq)
{
  char* content;
  size_t contentLen;

  mupnp_log_debug_l4("Entering...\n");

  if (soapReq->isHttpReqCreated == true)
    mupnp_http_request_delete(soapReq->httpReq);
  soapReq->httpReq = httpReq;
  soapReq->isHttpReqCreated = false;

  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
}

/****************************************
* mupnp_soap_request_parsemessage
****************************************/

bool mupnp_soap_request_parsemessage(mUpnpSoapRequest* soapReq, char* msg, size_t msgLen)
{
  mUpnpXmlParser* xmlParser;
  bool parseRet;

  mupnp_log_debug_l4("Entering...\n");

  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);

  mupnp_log_debug_l4("Leaving...\n");

  return parseRet;
}

/****************************************
* mupnp_soap_request_post
****************************************/

mUpnpSoapResponse* mupnp_soap_request_post(mUpnpSoapRequest* soapReq, const char* ipaddr, int port)
{
  mUpnpHttpResponse* httpRes;
  char* content;
  size_t contentLen;
  mUpnpXmlParser* xmlParser;
  mUpnpHttpHeader* header = NULL;

  mupnp_log_debug_l4("Entering...\n");

  httpRes = mupnp_http_request_post(soapReq->httpReq, ipaddr, port);

  /* Check for HTTP response 405 Method Not Allowed */
  if (mupnp_http_response_getstatuscode(httpRes) == MUPNP_HTTP_STATUS_METHOD_NOT_ALLOWED) {
    /* Status code implies that we need to use M-POST */
    mupnp_http_request_setmethod(soapReq->httpReq, MUPNP_HTTP_MPOST);

    /* Add MAN header */
    header = mupnp_http_header_new();
    mupnp_http_header_setname(header, MUPNP_HTTP_MAN);
    mupnp_http_header_setvalue(header, MUPNP_HTTP_SOAP_MAN_VALUE);
    mupnp_http_packet_addheader((mUpnpHttpPacket*)soapReq->httpReq, header);

    /* Change soapaction header name to include namespace */
    header = mupnp_http_packet_getheader((mUpnpHttpPacket*)soapReq->httpReq,
        MUPNP_HTTP_SOAP_ACTION);
    if (header != NULL) {
      mupnp_http_header_setname(header, MUPNP_HTTP_SOAP_ACTION_WITH_NS);
    }

    /* New attempt */
    httpRes = mupnp_http_request_post(soapReq->httpReq, ipaddr, port);
  }

  mupnp_soap_response_sethttpresponse(soapReq->soapRes, httpRes);

  content = mupnp_http_response_getcontent(httpRes);
  contentLen = mupnp_http_response_getcontentlength(httpRes);
  if (content == NULL || contentLen <= 0)
    return soapReq->soapRes;

  xmlParser = mupnp_xml_parser_new();
  mupnp_xml_parse(xmlParser, soapReq->soapRes->rootNodeList, content, contentLen);
  mupnp_xml_parser_delete(xmlParser);

  mupnp_log_debug_l4("Leaving...\n");

  return soapReq->soapRes;
}

/****************************************
* mupnp_soap_request_setcontent
****************************************/

void mupnp_soap_request_setcontent(mUpnpSoapRequest* soapReq, mUpnpXmlNode* node)
{
  mUpnpHttpRequest* httpReq;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = mupnp_soap_request_gethttprequest(soapReq);

  /**** content type ****/
  mupnp_http_request_setcontenttype(httpReq, MUPNP_XML_CONTENT_TYPE);

  /**** content ****/
  mupnp_http_request_appendncontent(httpReq, MUPNP_SOAP_VERSION_HEADER, mupnp_strlen(MUPNP_SOAP_VERSION_HEADER));
  mupnp_http_request_appendncontent(httpReq, MUPNP_XML_CONTENT_LF, mupnp_strlen(MUPNP_XML_CONTENT_LF));
  mupnp_xml_node_tostring(node, true, httpReq->content);

  /**** content length ****/
  mupnp_http_request_setcontentlength(httpReq,
      mupnp_string_length(httpReq->content));

  mupnp_log_debug_l4("Leaving...\n");
}

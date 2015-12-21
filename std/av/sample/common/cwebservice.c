/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: rss_func.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <mupnp/http/http.h>
#include <mupnp/xml/xml.h>

/************************************************************
* mupnp_http_getrestresponse
************************************************************/

#define CG_HTTP_URLLEN_MAX 512

BOOL mupnp_http_getrestresponse(char* url, mUpnpString* str)
{
  mUpnpNetURL* netUrl;
  mUpnpHttpRequest* httpReq;
  mUpnpHttpResponse* httpRes;
  int statusCode;
  char* content;
  long contentLen;
  char path[CG_HTTP_URLLEN_MAX];

  netUrl = mupnp_net_url_new();
  mupnp_net_url_set(netUrl, url);

  httpReq = mupnp_http_request_new();
  mupnp_http_request_setmethod(httpReq, CG_HTTP_GET);

  mupnp_strncpy(path, mupnp_net_url_getpath(netUrl), (CG_HTTP_URLLEN_MAX - 1));
  if (mupnp_net_url_getquery(netUrl) != NULL) {
    mupnp_strncat(path, "?", (CG_HTTP_URLLEN_MAX - mupnp_strlen(path) - 1));
    mupnp_strncat(path, mupnp_net_url_getquery(netUrl), (CG_HTTP_URLLEN_MAX - mupnp_strlen(path) - 1));
  }

  mupnp_http_request_seturi(httpReq, path);
  mupnp_http_request_setcontentlength(httpReq, 0);
  httpRes = mupnp_http_request_post(httpReq, mupnp_net_url_gethost(netUrl), mupnp_net_url_getport(netUrl));

  statusCode = mupnp_http_response_getstatuscode(httpRes);

  if (statusCode != CG_HTTP_STATUS_OK) {
    mupnp_net_url_delete(netUrl);
    mupnp_http_request_delete(httpReq);
    return FALSE;
  }

  content = mupnp_http_response_getcontent(httpRes);
  contentLen = (long)mupnp_http_response_getcontentlength(httpRes);

  mupnp_string_setvalue(str, content);

  mupnp_http_request_delete(httpReq);
  mupnp_net_url_delete(netUrl);

  return TRUE;
}

/************************************************************
* mupnp_http_getrestresponse
************************************************************/

BOOL mupnp_http_getredirecturl(char* orgURL, char* redirectURL, int redirectURLSize)
{
  mUpnpNetURL* netUrl;
  mUpnpString* postUri;
  mUpnpHttpRequest* httpReq;
  mUpnpHttpResponse* httpRes;
  int statusCode;
  mUpnpHttpHeader* locationHeader;
  char* locationVal;

  /**** Redirect URL ****/
  netUrl = mupnp_net_url_new();
  mupnp_net_url_set(netUrl, orgURL);
  httpReq = mupnp_http_request_new();
  mupnp_http_request_setmethod(httpReq, CG_HTTP_GET);
  postUri = mupnp_string_new();
  mupnp_string_setvalue(postUri, mupnp_net_url_getpath(netUrl));
  if (mupnp_net_url_getquery(netUrl) != NULL) {
    mupnp_string_addvalue(postUri, "?");
    mupnp_string_addvalue(postUri, mupnp_net_url_getquery(netUrl));
  }
  mupnp_http_request_seturi(httpReq, mupnp_string_getvalue(postUri));
  mupnp_string_delete(postUri);
  mupnp_http_request_setcontentlength(httpReq, 0);
  httpRes = mupnp_http_request_post(httpReq, mupnp_net_url_gethost(netUrl), mupnp_net_url_getport(netUrl));

  statusCode = mupnp_http_response_getstatuscode(httpRes);
  if (statusCode != 303) {
    mupnp_net_url_delete(netUrl);
    mupnp_http_request_delete(httpReq);
    return FALSE;
  }

  locationHeader = mupnp_http_response_getheader(httpRes, CG_HTTP_LOCATION);
  if (!locationHeader) {
    mupnp_net_url_delete(netUrl);
    mupnp_http_request_delete(httpReq);
    return FALSE;
  }
  locationVal = mupnp_http_header_getvalue(locationHeader);
  if (mupnp_strlen(locationVal) <= 0) {
    mupnp_net_url_delete(netUrl);
    mupnp_http_request_delete(httpReq);
    return FALSE;
  }
  if (mupnp_strstr(locationVal, "http://") < 0) {
    mupnp_strncpy(redirectURL, "http://", redirectURLSize);
    mupnp_strncat(redirectURL, mupnp_net_url_gethost(netUrl), (redirectURLSize - mupnp_strlen(redirectURL)));
    mupnp_strncat(redirectURL, locationVal, (redirectURLSize - mupnp_strlen(redirectURL)));
  }
  else
    mupnp_strncpy(redirectURL, locationVal, redirectURLSize);

  mupnp_net_url_delete(netUrl);
  mupnp_http_request_delete(httpReq);

  return TRUE;
}

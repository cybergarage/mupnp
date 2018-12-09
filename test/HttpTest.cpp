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

#include <boost/test/unit_test.hpp>

#include <mupnp/http/http.h>

////////////////////////////////////////
// testHttpServer
////////////////////////////////////////

#define MUPNP_TESTCASE_HTTP_PORT 38192
#define MUPNP_TESTCASE_HTTP_PAGE "<HTML><BODY>Hello World</BODY></HTML>"
#define MUPNP_TESTCASE_HTTP_URL "/index.html"
#define MUPNP_TESTCASE_HTTP_LOOP 100

void ClinkTestcaseHttpRequestRecieved(mUpnpHttpRequest* httpReq)
{
  mUpnpHttpResponse* httpRes;

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, MUPNP_TESTCASE_HTTP_PAGE);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(MUPNP_TESTCASE_HTTP_PAGE));
  mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);
}

BOOST_AUTO_TEST_CASE(HttpServer)
{
  /**** HTTP Server ****/
  mUpnpHttpServer* httpServer = mupnp_http_server_new();
  BOOST_CHECK(httpServer);
  BOOST_CHECK(mupnp_http_server_open(httpServer, MUPNP_TESTCASE_HTTP_PORT, NULL));
  mupnp_http_server_setlistener(httpServer, ClinkTestcaseHttpRequestRecieved);
  mupnp_http_server_start(httpServer);

  /**** HTTP Client ****/
  for (int n = 0; n < MUPNP_TESTCASE_HTTP_LOOP; n++) {
    mUpnpHttpRequest* httpReq = mupnp_http_request_new();
    BOOST_CHECK(httpReq);
    mupnp_http_request_setmethod(httpReq, MUPNP_HTTP_GET);
    mupnp_http_request_seturi(httpReq, MUPNP_TESTCASE_HTTP_URL);
    mupnp_http_request_setcontentlength(httpReq, 0);
    mUpnpHttpResponse* httpRes = mupnp_http_request_post(httpReq, "localhost", MUPNP_TESTCASE_HTTP_PORT);
    BOOST_CHECK(httpRes);
    BOOST_CHECK(mupnp_http_response_issuccessful(httpRes));
    BOOST_CHECK(mupnp_streq(mupnp_http_response_getcontent(httpRes), MUPNP_TESTCASE_HTTP_PAGE));
    BOOST_CHECK(mupnp_http_response_getcontentlength(httpRes) == mupnp_strlen(MUPNP_TESTCASE_HTTP_PAGE));
    mupnp_http_request_delete(httpReq);
  }

  /**** HTTP Server ****/
  mupnp_http_server_stop(httpServer);
}

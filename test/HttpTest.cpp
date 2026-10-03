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

// Use the same address family for both ends; localhost may resolve to ::1.
#define MUPNP_TESTCASE_HTTP_ADDR "127.0.0.1"
#define MUPNP_TESTCASE_HTTP_PORT 38192
#define MUPNP_TESTCASE_HTTP_PAGE "<HTML><BODY>Hello World</BODY></HTML>"
#define MUPNP_TESTCASE_HTTP_URL "/index.html"
#define MUPNP_TESTCASE_HTTP_LOOP 100

void clink_testcase_http_request_recieved(mUpnpHttpRequest* httpReq)
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
  BOOST_REQUIRE(httpServer);
  BOOST_REQUIRE(mupnp_http_server_open(httpServer, MUPNP_TESTCASE_HTTP_PORT, MUPNP_TESTCASE_HTTP_ADDR));
  mupnp_http_server_setlistener(httpServer, clink_testcase_http_request_recieved);
  BOOST_REQUIRE(mupnp_http_server_start(httpServer));

  /**** HTTP Client ****/
  for (int n = 0; n < MUPNP_TESTCASE_HTTP_LOOP; n++) {
    mUpnpHttpRequest* httpReq = mupnp_http_request_new();
    BOOST_REQUIRE(httpReq);
    mupnp_http_request_setmethod(httpReq, MUPNP_HTTP_GET);
    mupnp_http_request_seturi(httpReq, MUPNP_TESTCASE_HTTP_URL);
    mupnp_http_request_setcontentlength(httpReq, 0);
    mUpnpHttpResponse* httpRes = mupnp_http_request_post(httpReq, MUPNP_TESTCASE_HTTP_ADDR, MUPNP_TESTCASE_HTTP_PORT);
    BOOST_REQUIRE(httpRes);
    BOOST_REQUIRE(mupnp_http_response_issuccessful(httpRes));
    BOOST_REQUIRE(mupnp_streq(mupnp_http_response_getcontent(httpRes), MUPNP_TESTCASE_HTTP_PAGE));
    BOOST_REQUIRE(mupnp_http_response_getcontentlength(httpRes) == mupnp_strlen(MUPNP_TESTCASE_HTTP_PAGE));
    mupnp_http_request_delete(httpReq);
  }

  /**** HTTP Server ****/
  mupnp_http_server_stop(httpServer);
}

#if !defined(WIN32)
#include <sys/socket.h>
#include <unistd.h>

// Feed a complete or truncated wire response without depending on LAN devices.
static bool read_wire_response(const char* wire, const char* expectedContent = NULL)
{
  int pair[2];
  BOOST_REQUIRE_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, pair), 0);
  BOOST_REQUIRE_EQUAL(write(pair[1], wire, strlen(wire)), (ssize_t)strlen(wire));
  shutdown(pair[1], SHUT_WR);

  mUpnpSocket* sock = mupnp_socket_stream_new();
  mupnp_socket_setid(sock, pair[0]);
  mUpnpHttpResponse* response = mupnp_http_response_new();
  bool result = mupnp_http_response_read(response, sock, false);
  if (result && expectedContent)
    BOOST_CHECK(mupnp_streq(mupnp_http_response_getcontent(response), expectedContent));
  mupnp_http_response_delete(response);
  mupnp_socket_delete(sock);
  close(pair[1]);
  return result;
}

BOOST_AUTO_TEST_CASE(HttpFixedBodyComplete)
{
  BOOST_CHECK(read_wire_response("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhello", "hello"));
}

BOOST_AUTO_TEST_CASE(HttpFixedBodyTruncated)
{
  BOOST_CHECK(!read_wire_response("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhe"));
}

BOOST_AUTO_TEST_CASE(HttpFixedBodyEmpty)
{
  BOOST_CHECK(!read_wire_response("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\n"));
}

BOOST_AUTO_TEST_CASE(HttpChunkedBodyComplete)
{
  BOOST_CHECK(read_wire_response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n2\r\nhe\r\n3\r\nllo\r\n0\r\n\r\n", "hello"));
}

BOOST_AUTO_TEST_CASE(HttpChunkedBodyTruncated)
{
  BOOST_CHECK(!read_wire_response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhe"));
}

BOOST_AUTO_TEST_CASE(HttpChunkedBodyMissingTerminator)
{
  BOOST_CHECK(!read_wire_response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n"));
}

BOOST_AUTO_TEST_CASE(HttpChunkedBodyBadLength)
{
  BOOST_CHECK(!read_wire_response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\ninvalid\r\n"));
}

BOOST_AUTO_TEST_CASE(HttpRequestBodyTruncated)
{
  int pair[2];
  const char* wire = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhe";
  BOOST_REQUIRE_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, pair), 0);
  BOOST_REQUIRE_EQUAL(write(pair[1], wire, strlen(wire)), (ssize_t)strlen(wire));
  shutdown(pair[1], SHUT_WR);
  mUpnpSocket* sock = mupnp_socket_stream_new();
  mupnp_socket_setid(sock, pair[0]);
  mUpnpHttpRequest* request = mupnp_http_request_new();
  BOOST_CHECK(!mupnp_http_request_read(request, sock));
  mupnp_http_request_delete(request);
  mupnp_socket_delete(sock);
  close(pair[1]);
}
#endif

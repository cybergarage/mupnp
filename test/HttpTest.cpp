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

#if !defined(WIN32)
#include "TestDevice.h"
#include <chrono>
#include <fcntl.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#endif

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

#if !defined(WIN32)

/* Exercise signed recv results through the public request/response boundary. */
static bool read_wire_message(const char* wire, bool response, bool nonblocking, std::string* content = nullptr)
{
  int descriptors[2];
  BOOST_REQUIRE_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, descriptors), 0);
  mUpnpSocket* socket = mupnp_socket_stream_new();
  mupnp_socket_setid(socket, descriptors[0]);
  BOOST_REQUIRE_EQUAL(write(descriptors[1], wire, strlen(wire)), (ssize_t)strlen(wire));
  if (nonblocking)
    fcntl(descriptors[0], F_SETFL, O_NONBLOCK);
  else
    shutdown(descriptors[1], SHUT_WR);
  bool result;
  if (response) {
    mUpnpHttpResponse* message = mupnp_http_response_new();
    result = mupnp_http_response_read(message, socket, false);
    if (content && result)
      *content = mupnp_http_response_getcontent(message);
    mupnp_http_response_delete(message);
  }
  else {
    mUpnpHttpRequest* message = mupnp_http_request_new();
    result = mupnp_http_request_read(message, socket);
    mupnp_http_request_delete(message);
  }
  mupnp_socket_delete(socket);
  close(descriptors[1]);
  return result;
}

BOOST_AUTO_TEST_CASE(HttpTruncatedBodies)
{
  const char* prefix = "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nx";
  BOOST_CHECK(!read_wire_message(prefix, true, false));
  BOOST_CHECK(!read_wire_message(prefix, true, true));
  BOOST_CHECK(!read_wire_message("POST / HTTP/1.1\r\nContent-Length: 4\r\n\r\nx", false, false));
  BOOST_CHECK(!read_wire_message("POST / HTTP/1.1\r\nContent-Length: 4\r\n\r\nx", false, true));
  BOOST_CHECK(!read_wire_message("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nx", true, false));
  BOOST_CHECK(!read_wire_message("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nx", true, true));
  BOOST_CHECK(!read_wire_message("HTTP/1.1 200 OK\r\nContent-Length: 18446744073709551616\r\n\r\n", true, false));
  BOOST_CHECK(!read_wire_message("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\ngarbage\r\n", true, false));
}

BOOST_AUTO_TEST_CASE(HttpCompleteBodies)
{
  std::string content;
  BOOST_CHECK(read_wire_message("HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nbody", true, false, &content));
  BOOST_CHECK_EQUAL(content, "body");
  BOOST_CHECK(read_wire_message("HTTP/1.1 200 OK\r\nContent-Length: \t4\t\r\n\r\nbody", true, false, &content));
  BOOST_CHECK_EQUAL(content, "body");
  BOOST_CHECK(read_wire_message("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4;extension=value\r\nbody\r\n0\r\nTrailer: value\r\n\r\n", true, false, &content));
  BOOST_CHECK_EQUAL(content, "body");
  BOOST_CHECK(read_wire_message("HTTP/1.0 200 OK\r\n\r\nbody", true, false, &content));
  BOOST_CHECK_EQUAL(content, "body");
  BOOST_CHECK(read_wire_message("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n", true, false));
  std::string largeBody(2048, 'x');
  std::string wire = "HTTP/1.0 200 OK\r\n\r\n" + largeBody;
  BOOST_CHECK(read_wire_message(wire.c_str(), true, false, &content));
  BOOST_CHECK_EQUAL(content, largeBody);
  wire = "HTTP/1.0 200 OK\r\nLong-Header: " + largeBody + "\r\n\r\nbody";
  BOOST_CHECK(read_wire_message(wire.c_str(), true, false, &content));
  BOOST_CHECK_EQUAL(content, "body");
}
#endif

#if !defined(WIN32)

static void truncated_chunk_listener(mUpnpHttpRequest* request)
{
  const char* wire = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4\r\n<r/>\r\n";
  mupnp_socket_write(mupnp_http_request_getsocket(request), wire, strlen(wire));
}

BOOST_AUTO_TEST_CASE(HttpPostRejectsIncompleteChunkedResponse)
{
  mUpnpHttpServer* server = mupnp_http_server_new();
  BOOST_REQUIRE(mupnp_http_server_open(server, MUPNP_TESTCASE_HTTP_PORT + 1, "127.0.0.1"));
  mupnp_http_server_setlistener(server, truncated_chunk_listener);
  BOOST_REQUIRE(mupnp_http_server_start(server));
  mUpnpHttpRequest* request = mupnp_http_request_new();
  mupnp_http_request_setmethod(request, MUPNP_HTTP_GET);
  mupnp_http_request_seturi(request, "/");
  mupnp_http_request_setversion(request, MUPNP_HTTP_VER10);
  mUpnpHttpResponse* response = mupnp_http_request_post(request, "127.0.0.1", MUPNP_TESTCASE_HTTP_PORT + 1);
  BOOST_CHECK(!mupnp_http_response_issuccessful(response));
  BOOST_CHECK_EQUAL(mupnp_strlen(mupnp_http_response_getcontent(response)), 0);
  mupnp_http_request_delete(request);
  mupnp_http_server_delete(server);
}

static void self_delete_listener(mUpnpHttpRequest* request)
{
  mupnp_http_server_delete(static_cast<mUpnpHttpServer*>(mupnp_http_request_getuserdata(request)));
}

BOOST_AUTO_TEST_CASE(HttpListenerCanDeleteItsServer)
{
  mUpnpHttpServer* server = mupnp_http_server_new();
  BOOST_REQUIRE(mupnp_http_server_open(server, MUPNP_TESTCASE_HTTP_PORT + 2, "127.0.0.1"));
  mupnp_http_server_setuserdata(server, server);
  mupnp_http_server_setlistener(server, self_delete_listener);
  BOOST_REQUIRE(mupnp_http_server_start(server));
  mUpnpHttpRequest* request = mupnp_http_request_new();
  mupnp_http_request_setmethod(request, MUPNP_HTTP_GET);
  mupnp_http_request_seturi(request, "/");
  mUpnpHttpResponse* response = mupnp_http_request_post(request, "127.0.0.1", MUPNP_TESTCASE_HTTP_PORT + 2);
  BOOST_CHECK(!mupnp_http_response_issuccessful(response));
  mupnp_http_request_delete(request);
  /* Allow the worker trampoline to complete; ASan checks its final accesses. */
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

BOOST_AUTO_TEST_CASE(TestPresentationIsDeterministic)
{
  mUpnpHttpServer* server = mupnp_http_server_new();
  BOOST_REQUIRE(mupnp_http_server_open(server, MUPNP_TESTCASE_HTTP_PORT + 3, "127.0.0.1"));
  mupnp_http_server_setlistener(server, upnp_test_device_httprequestrecieved);
  BOOST_REQUIRE(mupnp_http_server_start(server));
  mUpnpHttpRequest* request = mupnp_http_request_new();
  mupnp_http_request_setmethod(request, MUPNP_HTTP_GET);
  mupnp_http_request_seturi(request, "/presentation");
  mUpnpHttpResponse* response = mupnp_http_request_post(request, "127.0.0.1", MUPNP_TESTCASE_HTTP_PORT + 3);
  BOOST_CHECK(mupnp_http_response_issuccessful(response));
  BOOST_CHECK_EQUAL(mupnp_http_response_getcontent(response), "<HTML><BODY>UPnP test device</BODY></HTML>");
  mupnp_http_request_delete(request);
  mupnp_http_server_delete(server);
}
#endif

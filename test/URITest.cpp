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
#include <mupnp/net/interface.h>
#include <mupnp/net/uri.h>
#include <mupnp/net/url.h>
#include <string>
#include <strings.h>

////////////////////////////////////////
// testURIParameter
////////////////////////////////////////

#define MUPNP_TESTCASE_NET_URI_PROTOCOL "http"
#define MUPNP_TESTCASE_NET_URI_PROTOCOL_FTP "ftp"
#define MUPNP_TESTCASE_NET_URI_HOST "www.cybergarage.org"
#define MUPNP_TESTCASE_NET_URI_PORT "8080"
#define MUPNP_TESTCASE_NET_URI_PATH "/index.html"

#define MUPNP_TESTCASE_NET_URI_FRAGMENT "fragment"

BOOST_AUTO_TEST_CASE(URI)
{
  mUpnpNetURI* uri;

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST ":" MUPNP_TESTCASE_NET_URI_PORT
          MUPNP_TESTCASE_NET_URI_PATH);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_REQUIRE(mupnp_net_uri_getport(uri) == atoi(MUPNP_TESTCASE_NET_URI_PORT));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST
          MUPNP_TESTCASE_NET_URI_PATH);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_REQUIRE(mupnp_net_uri_getport(uri) == MUPNP_NET_URI_DEFAULT_HTTP_PORT);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL_FTP "://" MUPNP_TESTCASE_NET_URI_HOST
          MUPNP_TESTCASE_NET_URI_PATH);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL_FTP));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_REQUIRE(mupnp_net_uri_getport(uri) == MUPNP_NET_URI_DEFAULT_FTP_PORT);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));
  mupnp_net_uri_delete(uri);

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_REQUIRE(!mupnp_net_uri_haspath(uri));
  mupnp_net_uri_delete(uri);

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST "/");
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getpath(uri), "/"));
  mupnp_net_uri_delete(uri);
}

////////////////////////////////////////
// testURIParameter
////////////////////////////////////////

#define MUPNP_TESTCASE_NET_URI_PARAM_PATH "/test.cgi"
#define MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME "abc"
#define MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME "defgh"
#define MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "123"
#define MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE "45678"

#define MUPNP_TESTCASE_NET_URI_FRAGMENT "fragment"

BOOST_AUTO_TEST_CASE(URIParameter)
{
  mUpnpNetURI* uri;

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_PARAM_PATH "?" MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE "#" MUPNP_TESTCASE_NET_URI_FRAGMENT);
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PARAM_PATH));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getquery(uri),
      MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getfragment(uri), MUPNP_TESTCASE_NET_URI_FRAGMENT));
  mUpnpDictionary* paramDir = mupnp_net_uri_getquerydictionary(uri);
  mUpnpDictionaryElement* paramElem;
  paramElem = mupnp_dictionary_gets(paramDir);
  BOOST_REQUIRE(paramElem != NULL);
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getkey(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME));
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getvalue(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE));
  paramElem = mupnp_dictionary_element_next(paramElem);
  BOOST_REQUIRE(paramElem != NULL);
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getkey(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME));
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getvalue(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));

  mupnp_net_uri_delete(uri);
}

////////////////////////////////////////
// testURISecurity (regression tests for buffer overflows)
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(URISecurity)
{
  mUpnpNetURI* uri;

  /* User/password parsing must not underflow the length when a protocol
     prefix is present (previously a heap-buffer-overflow). */
  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, "http://username@example.com/path");
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_getuser(uri), "username"));
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), "example.com"));
  mupnp_net_uri_delete(uri);

  /* IPv6 host with an explicit port must strip the brackets without
     reading past the truncated host buffer. */
  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, "https://[fe80::1234:5678]:8080/path");
  BOOST_REQUIRE(mupnp_streq(mupnp_net_uri_gethost(uri), "fe80::1234:5678"));
  BOOST_REQUIRE(mupnp_net_uri_getport(uri) == 8080);
  mupnp_net_uri_delete(uri);

  /* A single trailing query parameter must not walk past the end of the
     query string when building the dictionary. */
  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, "/test.cgi?only=value");
  mUpnpDictionary* dir = mupnp_net_uri_getquerydictionary(uri);
  mUpnpDictionaryElement* elem = mupnp_dictionary_gets(dir);
  BOOST_REQUIRE(elem != NULL);
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getkey(elem), "only"));
  BOOST_REQUIRE(mupnp_streq(mupnp_dictionary_element_getvalue(elem), "value"));
  mupnp_net_uri_delete(uri);
}

BOOST_AUTO_TEST_CASE(URIQueryFragmentBoundaries)
{
  const struct {
    const char* value;
    const char* path;
    const char* query;
    const char* fragment;
  } cases[] = {
    { "/path#?x", "/path", "", "?x" },
    { "/path#fragment?query", "/path", "", "fragment?query" },
    { "http://example.com/path#?x", "/path", "", "?x" },
    { "/path?key=value#fragment?data", "/path", "key=value", "fragment?data" },
    { "/path?#?", "/path", "", "?" },
    { "/path?key=value", "/path", "key=value", "" },
    { "/path#", "/path", "", "" },
    { "/path?", "/path", "", "" },
    { "#?x", "", "", "?x" },
    { "?key=value#fragment", "", "key=value", "fragment" },
    { "/path", "/path", "", "" },
  };

  mUpnpNetURI* uri = mupnp_net_uri_new();
  BOOST_REQUIRE(uri != NULL);
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    BOOST_TEST_CONTEXT(cases[i].value)
    {
      mupnp_net_uri_setvalue(uri, cases[i].value);
      BOOST_CHECK(cases[i].path[0] ? mupnp_streq(mupnp_net_uri_getpath(uri), cases[i].path) : !mupnp_net_uri_haspath(uri));
      BOOST_CHECK(cases[i].query[0] ? mupnp_streq(mupnp_net_uri_getquery(uri), cases[i].query) : !mupnp_net_uri_hasquery(uri));
      BOOST_CHECK(cases[i].fragment[0] ? mupnp_streq(mupnp_net_uri_getfragment(uri), cases[i].fragment) : !mupnp_net_uri_hasfragment(uri));
    }
  }
  mupnp_net_uri_delete(uri);
}

////////////////////////////////////////
// testURIAdd
////////////////////////////////////////

#define MUPNP_TESTCASE_NET_URI_ADD_BASEPATH "http://192.168.100.10:80"
#define MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH "index.html"
#define MUPNP_TESTCASE_NET_URI_ADD_FULLPATH MUPNP_TESTCASE_NET_URI_ADD_BASEPATH "/" MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH

BOOST_AUTO_TEST_CASE(URIAdd)
{
  mUpnpNetURI* uri;
  const char* uriStr;

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_ADD_BASEPATH "/");
  mupnp_net_uri_addpath(uri, MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = mupnp_net_uri_getvalue(uri);
  BOOST_REQUIRE(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_ADD_BASEPATH);
  mupnp_net_uri_addpath(uri, "/" MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = mupnp_net_uri_getvalue(uri);
  BOOST_REQUIRE(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_ADD_BASEPATH);
  mupnp_net_uri_addpath(uri, MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = mupnp_net_uri_getvalue(uri);
  BOOST_REQUIRE(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);
}

/* Issue #17: IPv6 link-local addresses carry a zone index. URLs built from
   them encode it as "%25" (RFC 6874) and parse back to the numeric zone that
   mupnp_net_getipv6scopeid() and getaddrinfo() expect. */
BOOST_AUTO_TEST_CASE(URIIPv6ZoneRoundTrip)
{
  char buf[256];
  mupnp_net_gethosturl("fe80::1%2", 4004, "/description.xml", buf, sizeof(buf));
  BOOST_CHECK_EQUAL(std::string(buf), "http://[fe80::1%252]:4004/description.xml");

  mupnp_net_gethosturl("2001:db8::1", 80, "/", buf, sizeof(buf));
  BOOST_CHECK_EQUAL(std::string(buf), "http://[2001:db8::1]:80/");
  mupnp_net_gethosturl("192.168.0.1", 80, "/", buf, sizeof(buf));
  BOOST_CHECK_EQUAL(std::string(buf), "http://192.168.0.1:80/");

  mUpnpNetURI* uri = mupnp_net_uri_new();
  mupnp_net_uri_set(uri, "http://[fe80::1%252]:4004/description.xml");
  BOOST_CHECK_EQUAL(std::string(mupnp_net_uri_gethost(uri)), "fe80::1%2");
  BOOST_CHECK_EQUAL(mupnp_net_uri_getport(uri), 4004);
  BOOST_CHECK_EQUAL(std::string(mupnp_net_uri_getpath(uri)), "/description.xml");
  BOOST_CHECK_EQUAL(mupnp_net_getipv6scopeid(mupnp_net_uri_gethost(uri)), 2);

  mupnp_net_uri_set(uri, "http://[2001:db8::1]:8080/a");
  BOOST_CHECK_EQUAL(std::string(mupnp_net_uri_gethost(uri)), "2001:db8::1");
  BOOST_CHECK_EQUAL(mupnp_net_uri_getport(uri), 8080);
  mupnp_net_uri_delete(uri);

  /* The HTTP Host header never carries the zone. */
  mUpnpHttpRequest* req = mupnp_http_request_new();
  mupnp_http_request_sethost(req, "fe80::1%2", 4004);
  BOOST_CHECK_EQUAL(std::string(mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)req, MUPNP_HTTP_HOST)), "[fe80::1]:4004");
  mupnp_http_request_sethost(req, "2001:db8::1", 4004);
  BOOST_CHECK_EQUAL(std::string(mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)req, MUPNP_HTTP_HOST)), "[2001:db8::1]:4004");
  mupnp_http_request_delete(req);
}

BOOST_AUTO_TEST_CASE(NetIPv6OptIn)
{
  BOOST_CHECK(!mupnp_net_isipv6enabled());

  /* With IPv6 disabled (the default) no IPv6 address is returned. */
  mUpnpNetworkInterfaceList* ifList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(ifList);
  for (mUpnpNetworkInterface* netIf = mupnp_net_interfacelist_gets(ifList); netIf; netIf = mupnp_net_interface_next(netIf))
    BOOST_CHECK(!mupnp_net_isipv6address(mupnp_net_interface_getaddress(netIf)));

  /* When enabled, any IPv6 address must be link-local with a numeric zone. */
  mupnp_net_setipv6enabled(true);
  BOOST_CHECK(mupnp_net_isipv6enabled());
  mupnp_net_gethostinterfaces(ifList);
  for (mUpnpNetworkInterface* netIf = mupnp_net_interfacelist_gets(ifList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    const char* addr = mupnp_net_interface_getaddress(netIf);
    if (!mupnp_net_isipv6address(addr))
      continue;
    BOOST_TEST_MESSAGE("IPv6 interface address: " << addr);
    BOOST_CHECK(strncasecmp(addr, "fe80:", 5) == 0);
    BOOST_CHECK(0 < mupnp_net_getipv6scopeid(addr));
  }
  mupnp_net_setipv6enabled(false);
  mupnp_net_interfacelist_delete(ifList);
}

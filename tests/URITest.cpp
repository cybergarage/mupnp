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

#include <mupnp/net/uri.h>

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
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_CHECK(mupnp_net_uri_getport(uri) == atoi(MUPNP_TESTCASE_NET_URI_PORT));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST
          MUPNP_TESTCASE_NET_URI_PATH);
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_CHECK(mupnp_net_uri_getport(uri) == MUPNP_NET_URI_DEFAULT_HTTP_PORT);
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL_FTP "://" MUPNP_TESTCASE_NET_URI_HOST
          MUPNP_TESTCASE_NET_URI_PATH);
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL_FTP));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_CHECK(mupnp_net_uri_getport(uri) == MUPNP_NET_URI_DEFAULT_FTP_PORT);
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PATH));
  mupnp_net_uri_delete(uri);

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST);
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_CHECK(!mupnp_net_uri_haspath(uri));
  mupnp_net_uri_delete(uri);

  //////////////////////////////////////////////////

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri,
      MUPNP_TESTCASE_NET_URI_PROTOCOL "://" MUPNP_TESTCASE_NET_URI_HOST "/");
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getprotocol(uri), MUPNP_TESTCASE_NET_URI_PROTOCOL));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_gethost(uri), MUPNP_TESTCASE_NET_URI_HOST));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getpath(uri), "/"));
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
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getpath(uri), MUPNP_TESTCASE_NET_URI_PARAM_PATH));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getquery(uri),
      MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));
  BOOST_CHECK(mupnp_streq(mupnp_net_uri_getfragment(uri), MUPNP_TESTCASE_NET_URI_FRAGMENT));
  mUpnpDictionary* paramDir = mupnp_net_uri_getquerydictionary(uri);
  mUpnpDictionaryElement* paramElem;
  paramElem = mupnp_dictionary_gets(paramDir);
  BOOST_CHECK(paramElem != NULL);
  BOOST_CHECK(mupnp_streq(mupnp_dictionary_element_getkey(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_NAME));
  BOOST_CHECK(mupnp_streq(mupnp_dictionary_element_getvalue(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM1_VALUE));
  paramElem = mupnp_dictionary_element_next(paramElem);
  BOOST_CHECK(paramElem != NULL);
  BOOST_CHECK(mupnp_streq(mupnp_dictionary_element_getkey(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_NAME));
  BOOST_CHECK(mupnp_streq(mupnp_dictionary_element_getvalue(paramElem), MUPNP_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));

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
  BOOST_CHECK(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_ADD_BASEPATH);
  mupnp_net_uri_addpath(uri, "/" MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = mupnp_net_uri_getvalue(uri);
  BOOST_CHECK(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);

  uri = mupnp_net_uri_new();
  mupnp_net_uri_setvalue(uri, MUPNP_TESTCASE_NET_URI_ADD_BASEPATH);
  mupnp_net_uri_addpath(uri, MUPNP_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = mupnp_net_uri_getvalue(uri);
  BOOST_CHECK(mupnp_streq(uriStr, MUPNP_TESTCASE_NET_URI_ADD_FULLPATH));
  mupnp_net_uri_delete(uri);
}

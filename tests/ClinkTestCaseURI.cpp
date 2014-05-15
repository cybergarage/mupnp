/************************************************************
 *
 *	CyberLink for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ************************************************************/

#include <boost/test/unit_test.hpp>

#include <cybergarage/net/curi.h>

////////////////////////////////////////
// testURIParameter
////////////////////////////////////////

#define CLINK_TESTCASE_NET_URI_PROTOCOL "http"
#define CLINK_TESTCASE_NET_URI_PROTOCOL_FTP "ftp"
#define CLINK_TESTCASE_NET_URI_HOST "www.cybergarage.org"
#define CLINK_TESTCASE_NET_URI_PORT "8080"
#define CLINK_TESTCASE_NET_URI_PATH "/index.html"

#define CLINK_TESTCASE_NET_URI_FRAGMENT "fragment"

BOOST_AUTO_TEST_CASE(URI)
{
	CgNetURI *uri;
	
  //////////////////////////////////////////////////
    
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST ":"
                        CLINK_TESTCASE_NET_URI_PORT
                        CLINK_TESTCASE_NET_URI_PATH);
	BOOST_CHECK(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	BOOST_CHECK(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	BOOST_CHECK(cg_net_uri_getport(uri) == atoi(CLINK_TESTCASE_NET_URI_PORT));
	BOOST_CHECK(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
    
  //////////////////////////////////////////////////
    
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST
                        CLINK_TESTCASE_NET_URI_PATH);
	BOOST_CHECK(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	BOOST_CHECK(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	BOOST_CHECK(cg_net_uri_getport(uri) == CG_NET_URI_DEFAULT_HTTP_PORT);
	BOOST_CHECK(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
    
  //////////////////////////////////////////////////
    
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL_FTP "://"
                        CLINK_TESTCASE_NET_URI_HOST
                        CLINK_TESTCASE_NET_URI_PATH);
	BOOST_CHECK(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL_FTP));
	BOOST_CHECK(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	BOOST_CHECK(cg_net_uri_getport(uri) == CG_NET_URI_DEFAULT_FTP_PORT);
	BOOST_CHECK(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
	cg_net_uri_delete(uri);

  //////////////////////////////////////////////////
    
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST);
	BOOST_CHECK(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	BOOST_CHECK(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	BOOST_CHECK(!cg_net_uri_haspath(uri));
	cg_net_uri_delete(uri);

  //////////////////////////////////////////////////
    
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST "/");
	BOOST_CHECK(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	BOOST_CHECK(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	BOOST_CHECK(cg_streq(cg_net_uri_getpath(uri), "/"));
	cg_net_uri_delete(uri);
}


////////////////////////////////////////
// testURIParameter
////////////////////////////////////////

#define CLINK_TESTCASE_NET_URI_PARAM_PATH "/test.cgi"
#define CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME "abc"
#define CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME "defgh"
#define CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "123"
#define CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE "45678"

#define CLINK_TESTCASE_NET_URI_FRAGMENT "fragment"

BOOST_AUTO_TEST_CASE(URIParameter)
{
	CgNetURI *uri;
	
  uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_PARAM_PATH "?" 
		CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE
		"#" CLINK_TESTCASE_NET_URI_FRAGMENT);
	BOOST_CHECK(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PARAM_PATH));
	BOOST_CHECK(cg_streq(cg_net_uri_getquery(uri), 
		CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE
		));
	BOOST_CHECK(cg_streq(cg_net_uri_getfragment(uri), CLINK_TESTCASE_NET_URI_FRAGMENT));
	CgDictionary *paramDir = cg_net_uri_getquerydictionary(uri);
	CgDictionaryElement *paramElem;
	paramElem = cg_dictionary_gets(paramDir);
	BOOST_CHECK(paramElem != NULL);
	BOOST_CHECK(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME));
	BOOST_CHECK(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE));
	paramElem = cg_dictionary_element_next(paramElem);
	BOOST_CHECK(paramElem != NULL);
	BOOST_CHECK(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME));
	BOOST_CHECK(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));

	cg_net_uri_delete(uri);
}

////////////////////////////////////////
// testURIAdd
////////////////////////////////////////

#define CLINK_TESTCASE_NET_URI_ADD_BASEPATH "http://192.168.100.10:80"
#define CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH "index.html"
#define CLINK_TESTCASE_NET_URI_ADD_FULLPATH CLINK_TESTCASE_NET_URI_ADD_BASEPATH "/" CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH

BOOST_AUTO_TEST_CASE(URIAdd)
{
	CgNetURI *uri;
  const char *uriStr;
    
  uri = cg_net_uri_new();
  cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH "/");
  cg_net_uri_addpath(uri, CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = cg_net_uri_getvalue(uri);
	BOOST_CHECK(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    

  uri = cg_net_uri_new();
  cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH);
  cg_net_uri_addpath(uri, "/" CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = cg_net_uri_getvalue(uri);
	BOOST_CHECK(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    
    
  uri = cg_net_uri_new();
  cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH);
  cg_net_uri_addpath(uri, CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
  uriStr = cg_net_uri_getvalue(uri);
	BOOST_CHECK(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    
}

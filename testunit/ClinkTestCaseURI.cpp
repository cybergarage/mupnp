#include "ClinkTestCase.h"

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

void ClinkTestCase::testURI()
{
	CgNetURI *uri;
	
    //////////////////////////////////////////////////
    
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST ":"
                        CLINK_TESTCASE_NET_URI_PORT
                        CLINK_TESTCASE_NET_URI_PATH);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	CPPUNIT_ASSERT(cg_net_uri_getport(uri) == atoi(CLINK_TESTCASE_NET_URI_PORT));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
    
    //////////////////////////////////////////////////
    
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST
                        CLINK_TESTCASE_NET_URI_PATH);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	CPPUNIT_ASSERT(cg_net_uri_getport(uri) == CG_NET_URI_DEFAULT_HTTP_PORT);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
    
    //////////////////////////////////////////////////
    
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL_FTP "://"
                        CLINK_TESTCASE_NET_URI_HOST
                        CLINK_TESTCASE_NET_URI_PATH);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL_FTP));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	CPPUNIT_ASSERT(cg_net_uri_getport(uri) == CG_NET_URI_DEFAULT_FTP_PORT);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
	cg_net_uri_delete(uri);

    //////////////////////////////////////////////////
    
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	CPPUNIT_ASSERT(!cg_net_uri_haspath(uri));
	cg_net_uri_delete(uri);

    //////////////////////////////////////////////////
    
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, 
                        CLINK_TESTCASE_NET_URI_PROTOCOL "://"
                        CLINK_TESTCASE_NET_URI_HOST "/");
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getprotocol(uri), CLINK_TESTCASE_NET_URI_PROTOCOL));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_gethost(uri), CLINK_TESTCASE_NET_URI_HOST));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), "/"));
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

void ClinkTestCase::testURIParameter()
{
	CgNetURI *uri;
	
    uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_PARAM_PATH "?" 
		CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE
		"#" CLINK_TESTCASE_NET_URI_FRAGMENT);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PARAM_PATH));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getquery(uri), 
		CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE
		));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getfragment(uri), CLINK_TESTCASE_NET_URI_FRAGMENT));
	CgDictionary *paramDir = cg_net_uri_getquerydictionary(uri);
	CgDictionaryElement *paramElem;
	paramElem = cg_dictionary_gets(paramDir);
	CPPUNIT_ASSERT(paramElem != NULL);
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM1_NAME));
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM1_VALUE));
	paramElem = cg_dictionary_element_next(paramElem);
	CPPUNIT_ASSERT(paramElem != NULL);
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM2_NAME));
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM_PARAM2_VALUE));

	cg_net_uri_delete(uri);
}

////////////////////////////////////////
// testURIAdd
////////////////////////////////////////

#define CLINK_TESTCASE_NET_URI_ADD_BASEPATH "http://192.168.100.10:80"
#define CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH "index.html"
#define CLINK_TESTCASE_NET_URI_ADD_FULLPATH CLINK_TESTCASE_NET_URI_ADD_BASEPATH "/" CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH

void ClinkTestCase::testURIAdd()
{
	CgNetURI *uri;
    char *uriStr;
    
    uri = cg_net_uri_new();
    cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH "/");
    cg_net_uri_addpath(uri, CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
    uriStr = cg_net_uri_getvalue(uri);
	CPPUNIT_ASSERT(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    

    uri = cg_net_uri_new();
    cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH);
    cg_net_uri_addpath(uri, "/" CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
    uriStr = cg_net_uri_getvalue(uri);
	CPPUNIT_ASSERT(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    
    
    uri = cg_net_uri_new();
    cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_ADD_BASEPATH);
    cg_net_uri_addpath(uri, CLINK_TESTCASE_NET_URI_ADD_EXTRAPATH);
    uriStr = cg_net_uri_getvalue(uri);
	CPPUNIT_ASSERT(cg_streq(uriStr, CLINK_TESTCASE_NET_URI_ADD_FULLPATH));
	cg_net_uri_delete(uri);    
}

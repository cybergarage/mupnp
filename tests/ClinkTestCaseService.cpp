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

#include <cybergarage/upnp/cupnp.h>

////////////////////////////////////////
// testDevice`
////////////////////////////////////////

extern "C" {
CgNetURL *cg_upnp_service_mangleabsoluteurl(const char *serviceURLStr, const char *baseURLStr, const char *locationURLStr);
}

BOOST_AUTO_TEST_CASE(ServiceAbsoluteURL)
{
    CgNetURL *url;
    const char *urlStr;

    /********************************************************************************
     * O:serviceURLStr ?:baseURLStr ?:locationURLStr
     ********************************************************************************/
     
    /* O:serviceURLStr -:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", NULL, NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* O:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", "http://192.168.0.2:80/", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
    cg_net_url_delete(url);
    
    /* O:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", NULL, "http://192.168.0.3:80/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* O:serviceURLStr O:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", "http://192.168.0.2:80/", "http://192.168.0.3:80/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /********************************************************************************
     * X:serviceURLStr X:baseURLStr X:locationURLStr
     ********************************************************************************/

    /* O:serviceURLStr X:baseURLStr X:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", NULL, NULL);
    urlStr = cg_net_url_getvalue(url);
    // FIXME
    //BOOST_CHECK(strcmp(urlStr, "/serviceURL") == 0);
    cg_net_url_delete(url);
    
    /********************************************************************************
     * X:serviceURLStr O:baseURLStr -:locationURLStr (CASE01)
     ********************************************************************************/
    
    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80/", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80/", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /********************************************************************************
     * X:serviceURLStr O:baseURLStr -:locationURLStr (CASE02)
     ********************************************************************************/
    
    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80/device/", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
    cg_net_url_delete(url);
    
    /* X:serviceURLStr O:baseURLStr -:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80/device/", NULL);
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/device/serviceURL") == 0);
    cg_net_url_delete(url);
    
    /********************************************************************************
     * X:serviceURLStr -:baseURLStr O:locationURLStr (CASE01)
     ********************************************************************************/
    
    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /********************************************************************************
     * X:serviceURLStr -:baseURLStr O:locationURLStr (CASE02)
     ********************************************************************************/
    
    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80/device/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
    cg_net_url_delete(url);

    /* X:serviceURLStr -:baseURLStr O:locationURLStr */
    url = cg_upnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80/device/");
    urlStr = cg_net_url_getvalue(url);
    BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/device/serviceURL") == 0);
    cg_net_url_delete(url);
}

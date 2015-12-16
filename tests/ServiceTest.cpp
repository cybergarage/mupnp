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

#include <mupnp/upnp.h>

////////////////////////////////////////
// testDevice`
////////////////////////////////////////

extern "C" {
mUpnpNetURL* mupnp_service_mangleabsoluteurl(const char* serviceURLStr, const char* baseURLStr, const char* locationURLStr);
}

BOOST_AUTO_TEST_CASE(ServiceAbsoluteURL)
{
  mUpnpNetURL* url;
  const char* urlStr;

  /********************************************************************************
     * O:serviceURLStr ?:baseURLStr ?:locationURLStr
     ********************************************************************************/

  /* O:serviceURLStr -:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", NULL, NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* O:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", "http://192.168.0.2:80/", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* O:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", NULL, "http://192.168.0.3:80/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* O:serviceURLStr O:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("http://192.168.0.1:80/serviceURL", "http://192.168.0.2:80/", "http://192.168.0.3:80/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.1:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /********************************************************************************
     * X:serviceURLStr X:baseURLStr X:locationURLStr
     ********************************************************************************/

  /* O:serviceURLStr X:baseURLStr X:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", NULL, NULL);
  urlStr = mupnp_net_url_getvalue(url);
  // FIXME
  //BOOST_CHECK(strcmp(urlStr, "/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /********************************************************************************
     * X:serviceURLStr O:baseURLStr -:locationURLStr (CASE01)
     ********************************************************************************/

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80/", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80/", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /********************************************************************************
     * X:serviceURLStr O:baseURLStr -:locationURLStr (CASE02)
     ********************************************************************************/

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", "http://192.168.0.2:80/device/", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr O:baseURLStr -:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", "http://192.168.0.2:80/device/", NULL);
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.2:80/device/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /********************************************************************************
     * X:serviceURLStr -:baseURLStr O:locationURLStr (CASE01)
     ********************************************************************************/

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /********************************************************************************
     * X:serviceURLStr -:baseURLStr O:locationURLStr (CASE02)
     ********************************************************************************/

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("/serviceURL", NULL, "http://192.168.0.3:80/device/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/serviceURL") == 0);
  mupnp_net_url_delete(url);

  /* X:serviceURLStr -:baseURLStr O:locationURLStr */
  url = mupnp_service_mangleabsoluteurl("serviceURL", NULL, "http://192.168.0.3:80/device/");
  urlStr = mupnp_net_url_getvalue(url);
  BOOST_CHECK(strcmp(urlStr, "http://192.168.0.3:80/device/serviceURL") == 0);
  mupnp_net_url_delete(url);
}

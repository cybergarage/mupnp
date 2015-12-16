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
#include <string.h>

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE "1234"
#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

static bool ClinkTestCaseTestSubscriptionFlag;

static void ClinkTestCaseTestSubscription(mUpnpProperty* prop)
{
  char* propName = mupnp_property_getname(prop);
  BOOST_CHECK(propName != NULL);
  BOOST_CHECK(mupnp_streq(propName, TEST_DEVICE_STATEVARIABLE_STATUS));

  char* sid = mupnp_property_getsid(prop);
  BOOST_CHECK(sid != NULL);

  size_t seq = mupnp_property_getseq(prop);
  BOOST_CHECK(sid != NULL);

  char* propValue = mupnp_property_getvalue(prop);
  BOOST_CHECK(propValue != NULL);

  if (seq == 0) {
    BOOST_CHECK(mupnp_streq(propValue, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE));
  }
  else {
    BOOST_CHECK(mupnp_streq(propValue, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));
  }

  ClinkTestCaseTestSubscriptionFlag = true;
}

BOOST_AUTO_TEST_CASE(Subscription)
{
  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_CHECK(testDev);
  BOOST_CHECK(mupnp_device_start(testDev));

  mUpnpControlPoint* testCp = mupnp_controlpoint_new();
  BOOST_CHECK(testCp);
  BOOST_CHECK(mupnp_controlpoint_start(testCp));
  BOOST_CHECK(mupnp_controlpoint_search(testCp, MUPNP_ST_ROOT_DEVICE));
  mupnp_controlpoint_addeventlistener(testCp, ClinkTestCaseTestSubscription);

  // Find Device
  mupnp_sleep(mupnp_controlpoint_getssdpsearchmx(testCp) * 1000);
  int devCnt = mupnp_controlpoint_getndevices(testCp);
  BOOST_CHECK(0 < devCnt);
  mUpnpDevice* testCpDev = NULL;
  for (int n = 0; n < devCnt; n++) {
    mUpnpDevice* dev = mupnp_controlpoint_getdevice(testCp, n);
    if (strcmp(mupnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
      testCpDev = dev;
      break;
    }
  }
  BOOST_CHECK(testCpDev != NULL);

  // Get Target Service
  mUpnpService* testDevService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_CHECK(testDevService != NULL);
  mUpnpStateVariable* testDevState = mupnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_CHECK(testDevState != NULL);

  // Set Initial Value
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE);

  // Subscribe
  mUpnpService* testCpDevService = mupnp_device_getservicebyexacttype(testCpDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_CHECK(testCpDevService != NULL);
  BOOST_CHECK(mupnp_controlpoint_subscribe(testCp, testCpDevService, 300));
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  // Update State Variable
  ClinkTestCaseTestSubscriptionFlag = false;
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);
  BOOST_CHECK(ClinkTestCaseTestSubscriptionFlag);

  // Unscribe
  BOOST_CHECK(mupnp_controlpoint_unsubscribe(testCp, testCpDevService));

  // Finish
  BOOST_CHECK(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
  BOOST_CHECK(mupnp_controlpoint_stop(testCp));
  mupnp_controlpoint_delete(testCp);
}

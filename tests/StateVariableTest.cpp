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

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

BOOST_AUTO_TEST_CASE(StateVariable)
{
  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_CHECK(testDev);
  BOOST_CHECK(mupnp_device_start(testDev));

  // Update State Variable
  mUpnpService* testDevService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_CHECK(testDevService != NULL);
  mUpnpStateVariable* testDevState = mupnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_CHECK(testDevState != NULL);
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
  BOOST_CHECK(mupnp_streq(mupnp_statevariable_getvalue(testDevState), TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));

  BOOST_CHECK(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
}

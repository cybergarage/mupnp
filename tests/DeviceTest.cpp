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

BOOST_AUTO_TEST_CASE(DeviceTest)
{
  mUpnpDevice* testDev;

  testDev = upnp_test_device_new();
  BOOST_CHECK(testDev);
  BOOST_CHECK(strcmp(mupnp_device_getfriendlyname(testDev), TEST_DEVICE_FREINDLYNAME) == 0);
  BOOST_CHECK(strcmp(mupnp_device_getdevicetype(testDev), TEST_DEVICE_DEVICE_TYPE) == 0);
  BOOST_CHECK(0 < mupnp_device_getbootid(testDev));

  // Embedded Devices
  int ebdDeviceCnt = 0;
  for (mUpnpDevice* ebdDev = mupnp_device_getdevices(testDev); ebdDev; ebdDev = mupnp_device_next(ebdDev)) {
    ebdDeviceCnt++;
    if (ebdDeviceCnt == 1)
      BOOST_CHECK(strcmp(mupnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE1) == 0);
    if (ebdDeviceCnt == 2)
      BOOST_CHECK(strcmp(mupnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE2) == 0);
  }
  BOOST_CHECK(ebdDeviceCnt == 2);

  BOOST_CHECK(mupnp_device_start(testDev));

  BOOST_CHECK(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
}

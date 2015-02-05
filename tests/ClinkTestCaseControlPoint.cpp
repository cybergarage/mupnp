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
// testControlPoint
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(ControlPointTest)
{
	mUpnpUpnpControlPoint *testCp = mupnp_upnp_controlpoint_new();
	BOOST_CHECK(testCp);
	BOOST_CHECK(mupnp_upnp_controlpoint_start(testCp));
	BOOST_CHECK(mupnp_upnp_controlpoint_search(testCp, MUPNP_ST_ROOT_DEVICE));

	mUpnpUpnpDevice *testDev = upnp_test_device_new();
	BOOST_CHECK(testDev);
	BOOST_CHECK(mupnp_upnp_device_start(testDev));

	mupnp_sleep(mupnp_upnp_controlpoint_getssdpsearchmx(testCp) * 1000 * 2);

	int devCnt = mupnp_upnp_controlpoint_getndevices(testCp);
	BOOST_CHECK(0 < devCnt);

	BOOL testDevFound = FALSE;
	for (int n=0; n<devCnt; n++) {
		mUpnpUpnpDevice *dev = mupnp_upnp_controlpoint_getdevice(testCp, n);
		if (strcmp(mupnp_upnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
			testDevFound = TRUE;
			break;
		}
	}
	BOOST_CHECK(testDevFound);

	BOOST_CHECK(mupnp_upnp_device_stop(testDev));
	mupnp_upnp_device_delete(testDev);

	BOOST_CHECK(mupnp_upnp_controlpoint_stop(testCp));
	mupnp_upnp_controlpoint_delete(testCp);
}

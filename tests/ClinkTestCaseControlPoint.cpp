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
#include <string.h>

#include "TestDevice.h"

////////////////////////////////////////
// testControlPoint
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(ControlPointTest)
{
	CgUpnpControlPoint *testCp = cg_upnp_controlpoint_new();
	BOOST_CHECK(testCp);
	BOOST_CHECK(cg_upnp_controlpoint_start(testCp));
	BOOST_CHECK(cg_upnp_controlpoint_search(testCp, CG_UPNP_ST_ROOT_DEVICE));

	CgUpnpDevice *testDev = upnp_test_device_new();
	BOOST_CHECK(testDev);
	BOOST_CHECK(cg_upnp_device_start(testDev));

	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(testCp) * 1000 * 2);

	int devCnt = cg_upnp_controlpoint_getndevices(testCp);
	BOOST_CHECK(0 < devCnt);

	BOOL testDevFound = FALSE;
	for (int n=0; n<devCnt; n++) {
		CgUpnpDevice *dev = cg_upnp_controlpoint_getdevice(testCp, n);
		if (strcmp(cg_upnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
			testDevFound = TRUE;
			break;
		}
	}
	BOOST_CHECK(testDevFound);

	BOOST_CHECK(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);

	BOOST_CHECK(cg_upnp_controlpoint_stop(testCp));
	cg_upnp_controlpoint_delete(testCp);
}

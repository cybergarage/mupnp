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

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(DeviceTest)
{
	CgUpnpDevice *testDev;

	testDev = upnp_test_device_new();
	BOOST_CHECK(testDev);
	BOOST_CHECK(strcmp(cg_upnp_device_getfriendlyname(testDev), TEST_DEVICE_FREINDLYNAME) == 0);
	BOOST_CHECK(strcmp(cg_upnp_device_getdevicetype(testDev), TEST_DEVICE_DEVICE_TYPE) == 0);
	BOOST_CHECK(0 < cg_upnp_device_getbootid(testDev));

	// Embedded Devices
	int ebdDeviceCnt = 0;
	for (CgUpnpDevice *ebdDev = cg_upnp_device_getdevices(testDev); ebdDev; ebdDev = cg_upnp_device_next(ebdDev)) {
		ebdDeviceCnt++;
		if (ebdDeviceCnt == 1)
			BOOST_CHECK(strcmp(cg_upnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE1) == 0);
		if (ebdDeviceCnt == 2)
			BOOST_CHECK(strcmp(cg_upnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE2) == 0);
	}
	BOOST_CHECK(ebdDeviceCnt == 2);

	BOOST_CHECK(cg_upnp_device_start(testDev));

	BOOST_CHECK(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
}

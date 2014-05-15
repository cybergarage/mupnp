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

#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

BOOST_AUTO_TEST_CASE(StateVariable)
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	BOOST_CHECK(testDev);
	BOOST_CHECK(cg_upnp_device_start(testDev));

	// Update State Variable
	CgUpnpService *testDevService = cg_upnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
	BOOST_CHECK(testDevService != NULL);
	CgUpnpStateVariable *testDevState = cg_upnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
	BOOST_CHECK(testDevState != NULL);
	cg_upnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
	BOOST_CHECK(cg_streq(cg_upnp_statevariable_getvalue(testDevState), TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));

	BOOST_CHECK(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
}

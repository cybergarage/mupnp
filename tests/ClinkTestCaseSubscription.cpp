/************************************************************
 *
 *	CyberLink for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ************************************************************/

#include <string.h>

#include <boost/test/unit_test.hpp>
#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE "1234"
#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

static bool ClinkTestCaseTestSubscriptionFlag;

static void ClinkTestCaseTestSubscription(CgUpnpProperty *prop)
{
	char *propName = cg_upnp_property_getname(prop);
	BOOST_CHECK(propName != NULL);
	BOOST_CHECK(cg_streq(propName, TEST_DEVICE_STATEVARIABLE_STATUS));

	char *sid = cg_upnp_property_getsid(prop);
	BOOST_CHECK(sid != NULL);

	long seq = cg_upnp_property_getseq(prop);
	BOOST_CHECK(sid != NULL);

	char *propValue = cg_upnp_property_getvalue(prop);
	BOOST_CHECK(propValue != NULL);
	switch (seq) {
	case 0:
		{
			BOOST_CHECK(cg_streq(propValue, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE));
		}
		break;
	default:
		{
			BOOST_CHECK(cg_streq(propValue, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));
		}
	}

	ClinkTestCaseTestSubscriptionFlag = true;
}

BOOST_AUTO_TEST_CASE(Subscription)
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	BOOST_CHECK(testDev);
	BOOST_CHECK(cg_upnp_device_start(testDev));

	CgUpnpControlPoint *testCp = cg_upnp_controlpoint_new();
	BOOST_CHECK(testCp);
	BOOST_CHECK(cg_upnp_controlpoint_start(testCp));
	BOOST_CHECK(cg_upnp_controlpoint_search(testCp, CG_UPNP_ST_ROOT_DEVICE));
	cg_upnp_controlpoint_addeventlistener(testCp, ClinkTestCaseTestSubscription);
	
	// Find Device
	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(testCp) * 1000);
	int devCnt = cg_upnp_controlpoint_getndevices(testCp);
	BOOST_CHECK(0 < devCnt);
	CgUpnpDevice *testCpDev = NULL;
	for (int n=0; n<devCnt; n++) {
		CgUpnpDevice *dev = cg_upnp_controlpoint_getdevice(testCp, n);
		if (strcmp(cg_upnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
			testCpDev = dev;
			break;
		}
	}
	BOOST_CHECK(testCpDev != NULL);

	// Get Target Service
	CgUpnpService *testDevService = cg_upnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
	BOOST_CHECK(testDevService != NULL);
	CgUpnpStateVariable *testDevState = cg_upnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
	BOOST_CHECK(testDevState != NULL);

	// Set Initial Value
	cg_upnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE);

	// Subscribe
	CgUpnpService *testCpDevService = cg_upnp_device_getservicebyexacttype(testCpDev, TEST_DEVICE_SERVICE_TYPE);
	BOOST_CHECK(testCpDevService != NULL);
	BOOST_CHECK(cg_upnp_controlpoint_subscribe(testCp, testCpDevService, 300) );

	// Update State Variable
	ClinkTestCaseTestSubscriptionFlag = false;
	cg_upnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
	cg_sleep(1000);
	BOOST_CHECK(ClinkTestCaseTestSubscriptionFlag);

	// Unscribe
	BOOST_CHECK(cg_upnp_controlpoint_unsubscribe(testCp, testCpDevService));

	// Finish
	BOOST_CHECK(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
	BOOST_CHECK(cg_upnp_controlpoint_stop(testCp));
	cg_upnp_controlpoint_delete(testCp);
}

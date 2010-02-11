#include <string.h>

#include <cybergarage/upnp/std/av/cmediarenderer.h>

#include "ClinkAVTestCase.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_VALUE "4649"

static bool ClinkTestCaseTestSubscriptionFlag;

	/*
static void ClinkTestCaseTestSubscription(CgUpnpProperty *prop)
{
//	CPPUNIT_ASSERT(cg_streq(cg_upnp_property_getname(prop), TEST_DEVICE_STATEVARIABLE_STATUS));
//	CPPUNIT_ASSERT(cg_streq(cg_upnp_property_getvalue(prop), TEST_UPDATE_STATEVARIABLE_VALUE));
	ClinkTestCaseTestSubscriptionFlag = true;
}
	*/

void ClinkAVTestCase::testRenderer()
{
	CgUpnpAvRenderer *dmr = cg_upnpav_dmr_new();

	cg_upnpav_dms_start(dmr);
	cg_upnpav_dms_stop(dmr);

	/*
	CgUpnpDevice *testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(cg_upnp_device_start(testDev));

	CgUpnpControlPoint *testCp = cg_upnp_controlpoint_new();
	CPPUNIT_ASSERT(testCp);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_start(testCp));
	CPPUNIT_ASSERT(cg_upnp_controlpoint_search(testCp, CG_UPNP_ST_ROOT_DEVICE));
	cg_upnp_controlpoint_addeventlistener(testCp, ClinkTestCaseTestSubscription);
	
	// Find Device
	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(testCp) * 1000);
	int devCnt = cg_upnp_controlpoint_getndevices(testCp);
	CPPUNIT_ASSERT(0 < devCnt);
	CgUpnpDevice *testCpDev = NULL;
	for (int n=0; n<devCnt; n++) {
		CgUpnpDevice *dev = cg_upnp_controlpoint_getdevice(testCp, n);
		if (strcmp(cg_upnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
			testCpDev = dev;
			break;
		}
	}
	CPPUNIT_ASSERT(testCpDev != NULL);

	// Subscribe
	CgUpnpService *testCpDevService = cg_upnp_device_getservicebyexacttype(testCpDev, TEST_DEVICE_SERVICE_TYPE);
	CPPUNIT_ASSERT(testCpDevService != NULL);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_subscribe(testCp, testCpDevService, 300) );

	// Update State Variable
	ClinkTestCaseTestSubscriptionFlag = false;
	CgUpnpService *testDevService = cg_upnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
	CPPUNIT_ASSERT(testDevService != NULL);
	CgUpnpStateVariable *testDevState = cg_upnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
	CPPUNIT_ASSERT(testDevState != NULL);
	cg_upnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_VALUE);
	cg_sleep(1000);
	CPPUNIT_ASSERT(ClinkTestCaseTestSubscriptionFlag);

	// Unscribe
	CPPUNIT_ASSERT(cg_upnp_controlpoint_unsubscribe(testCp, testCpDevService) );

	// Finish
	CPPUNIT_ASSERT(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_stop(testCp));
	cg_upnp_controlpoint_delete(testCp);
	*/
}

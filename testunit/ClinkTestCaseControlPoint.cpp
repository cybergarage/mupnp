#include "ClinkTestCase.h"

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

void ClinkTestCase::testControlPoint()
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(cg_upnp_device_start(testDev));

	CgUpnpControlPoint *testCp = cg_upnp_controlpoint_new();
	CPPUNIT_ASSERT(testCp);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_start(testCp));
	CPPUNIT_ASSERT(cg_upnp_controlpoint_search(testCp, CG_UPNP_ST_ROOT_DEVICE));

	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(testCp) * 1000);

	CPPUNIT_ASSERT(cg_upnp_controlpoint_stop(testCp));
	cg_upnp_controlpoint_delete(testCp);

	CPPUNIT_ASSERT(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
}

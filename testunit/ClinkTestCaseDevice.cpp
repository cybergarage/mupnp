#include "ClinkTestCase.h"

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

void ClinkTestCase::testDevice()
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(strcmp(cg_upnp_device_getfriendlyname(testDev), TEST_DEVICE_FREINDLYNAME) == 0);
	CPPUNIT_ASSERT(strcmp(cg_upnp_device_getdevicetype(testDev), TEST_DEVICE_DEVICE_TYPE) == 0);
	cg_upnp_device_delete(testDev);
}

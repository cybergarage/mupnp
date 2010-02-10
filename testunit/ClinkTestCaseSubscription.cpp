#include <string.h>

#include "ClinkTestCase.h"
#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

static void ClinkTestCaseTestSubscription(CgUpnpProperty *)
{
	printf("Hello");
}

void ClinkTestCase::testSubscription()
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(cg_upnp_device_start(testDev));

	CgUpnpControlPoint *testCp = cg_upnp_controlpoint_new();
	CPPUNIT_ASSERT(testCp);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_start(testCp));
	CPPUNIT_ASSERT(cg_upnp_controlpoint_search(testCp, CG_UPNP_ST_ROOT_DEVICE));
	cg_upnp_controlpoint_addeventlistener(testCp, ClinkTestCaseTestSubscription);
	
	cg_sleep(cg_upnp_controlpoint_getssdpsearchmx(testCp) * 1000 * 2);

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

	CgUpnpService *testCpDevService = cg_upnp_device_getservicebyexacttype(testCpDev, TEST_DEVICE_SERVICE_TYPE);
	CPPUNIT_ASSERT(testCpDevService != NULL);
	CPPUNIT_ASSERT(cg_upnp_controlpoint_subscribe(testCp, testCpDevService, 300) );

	CgUpnpService *testDevService = cg_upnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
	CPPUNIT_ASSERT(testDevService != NULL);
	CgUpnpStateVariable *testDevState = cg_upnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
	CPPUNIT_ASSERT(testDevState != NULL);
	cg_upnp_statevariable_setvalue(testDevState, "1");
	
	CPPUNIT_ASSERT(cg_upnp_controlpoint_unsubscribe(testCp, testCpDevService) );
	
	CPPUNIT_ASSERT(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);

	CPPUNIT_ASSERT(cg_upnp_controlpoint_stop(testCp));
	cg_upnp_controlpoint_delete(testCp);
}

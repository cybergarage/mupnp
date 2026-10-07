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

#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

BOOST_AUTO_TEST_CASE(StateVariable)
{
  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_REQUIRE(testDev);
  BOOST_REQUIRE(mupnp_device_start(testDev));

  // Update State Variable
  mUpnpService* testDevService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(testDevService != NULL);
  mUpnpStateVariable* testDevState = mupnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_REQUIRE(testDevState != NULL);
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
  BOOST_REQUIRE(mupnp_streq(mupnp_statevariable_getvalue(testDevState), TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));

  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
}

/* Issue #9: the declared dataType and defaultValue must be readable even
   though the current value stays NULL until the variable is queried. */
BOOST_AUTO_TEST_CASE(StateVariableDefaultValueAndDataType)
{
  const char* scpd = "<?xml version=\"1.0\"?>\n"
                     "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
                     "<specVersion><major>1</major><minor>0</minor></specVersion>\n"
                     "<serviceStateTable>\n"
                     "<stateVariable sendEvents=\"no\">\n"
                     "<name>HI_UPNP_VAR_VinpuServerURI</name>\n"
                     "<dataType>uri</dataType>\n"
                     "<defaultValue>Vinput://0.0.0.0:8822/</defaultValue>\n"
                     "</stateVariable>\n"
                     "<stateVariable sendEvents=\"no\">\n"
                     "<name>NoDefault</name>\n"
                     "<dataType>string</dataType>\n"
                     "</stateVariable>\n"
                     "</serviceStateTable>\n"
                     "</scpd>\n";

  mUpnpService* service = mupnp_service_new();
  BOOST_REQUIRE(service);
  BOOST_REQUIRE(mupnp_service_parsedescription(service, scpd, strlen(scpd)));

  mUpnpStateVariable* uriVar = mupnp_service_getstatevariablebyname(service, "HI_UPNP_VAR_VinpuServerURI");
  BOOST_REQUIRE(uriVar);
  BOOST_CHECK(mupnp_streq(mupnp_statevariable_getdatatype(uriVar), "uri"));
  BOOST_CHECK(mupnp_streq(mupnp_statevariable_getdefaultvalue(uriVar), "Vinput://0.0.0.0:8822/"));
  BOOST_CHECK(mupnp_statevariable_getvalue(uriVar) == NULL);

  mUpnpStateVariable* plainVar = mupnp_service_getstatevariablebyname(service, "NoDefault");
  BOOST_REQUIRE(plainVar);
  BOOST_CHECK(mupnp_statevariable_getdefaultvalue(plainVar) == NULL);

  mupnp_statevariable_setdefaultvalue(plainVar, "abc");
  BOOST_CHECK(mupnp_streq(mupnp_statevariable_getdefaultvalue(plainVar), "abc"));

  mupnp_service_delete(service);
}

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

#if !defined(WIN32)
#include <arpa/inet.h>
#include <array>
#include <chrono>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#endif

#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE "1234"
#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

static bool clinkTestCaseTestSubscriptionFlag;

static void clink_test_case_test_subscription(mUpnpProperty* prop)
{
  char* propName = mupnp_property_getname(prop);
  BOOST_REQUIRE(propName != NULL);
  BOOST_REQUIRE(mupnp_streq(propName, TEST_DEVICE_STATEVARIABLE_STATUS));

  char* sid = mupnp_property_getsid(prop);
  BOOST_REQUIRE(sid != NULL);

  size_t seq = mupnp_property_getseq(prop);
  BOOST_REQUIRE(sid != NULL);

  char* propValue = mupnp_property_getvalue(prop);
  BOOST_REQUIRE(propValue != NULL);

  if (seq == 0) {
    BOOST_REQUIRE(mupnp_streq(propValue, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE));
  }
  else {
    BOOST_REQUIRE(mupnp_streq(propValue, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));
  }

  clinkTestCaseTestSubscriptionFlag = true;
}

BOOST_AUTO_TEST_CASE(Subscription)
{
  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_REQUIRE(testDev);
  BOOST_REQUIRE(mupnp_device_start(testDev));

  mUpnpControlPoint* testCp = mupnp_controlpoint_new();
  BOOST_REQUIRE(testCp);
  BOOST_REQUIRE(mupnp_controlpoint_start(testCp));
  BOOST_REQUIRE(mupnp_controlpoint_search(testCp, MUPNP_ST_ROOT_DEVICE));
  mupnp_controlpoint_addeventlistener(testCp, clink_test_case_test_subscription);

  // Find Device
  mupnp_sleep(mupnp_controlpoint_getssdpsearchmx(testCp) * 1000);
  int devCnt = mupnp_controlpoint_getndevices(testCp);
  BOOST_REQUIRE(0 < devCnt);
  mUpnpDevice* testCpDev = NULL;
  for (int n = 0; n < devCnt; n++) {
    mUpnpDevice* dev = mupnp_controlpoint_getdevice(testCp, n);
    if (strcmp(mupnp_device_getdevicetype(dev), TEST_DEVICE_DEVICE_TYPE) == 0) {
      testCpDev = dev;
      break;
    }
  }
  BOOST_REQUIRE(testCpDev != NULL);

  // Get Target Service
  mUpnpService* testDevService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(testDevService != NULL);
  mUpnpStateVariable* testDevState = mupnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_REQUIRE(testDevState != NULL);

  // Set Initial Value
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_DEFAULTVALUE);

  // Subscribe
  mUpnpService* testCpDevService = mupnp_device_getservicebyexacttype(testCpDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(testCpDevService != NULL);
  BOOST_REQUIRE(mupnp_controlpoint_subscribe(testCp, testCpDevService, 300));
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  // Update State Variable
  clinkTestCaseTestSubscriptionFlag = false;
  mupnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);
  BOOST_REQUIRE(clinkTestCaseTestSubscriptionFlag);

  // Unscribe
  BOOST_REQUIRE(mupnp_controlpoint_unsubscribe(testCp, testCpDevService));

  // Finish
  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
  BOOST_REQUIRE(mupnp_controlpoint_stop(testCp));
  mupnp_controlpoint_delete(testCp);
}

#if !defined(WIN32)

/* Reads up to the first CRLF and returns that line. */
static std::string read_status_line(int fd)
{
  std::string received;
  std::array<char, 256> buf {};
  while (received.find("\r\n") == std::string::npos) {
    ssize_t n = read(fd, buf.data(), buf.size());
    if (n <= 0)
      break;
    received.append(buf.data(), (size_t)n);
  }
  return received.substr(0, received.find("\r\n"));
}

/* Sends a raw GENA SUBSCRIBE for the test device's SwitchPower service and
   returns the HTTP status line. */
static std::string send_raw_subscribe(mUpnpDevice* dev, const char* callbackHost, int callbackPort)
{
  auto* service = mupnp_device_getservicebyexacttype(dev, TEST_DEVICE_SERVICE_TYPE);
  if (!service)
    return "";
  auto* eventSubURL = mupnp_service_geteventsuburl(service);
  if (!eventSubURL)
    return "";
  std::string path = mupnp_net_url_getpath(eventSubURL);
  mupnp_net_url_delete(eventSubURL);

  auto* ifList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(ifList);
  auto* netIf = mupnp_net_interfacelist_gets(ifList);
  std::string host = netIf ? mupnp_net_interface_getaddress(netIf) : "127.0.0.1";
  mupnp_net_interfacelist_delete(ifList);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return "";
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(mupnp_device_gethttpport(dev));
  inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
  std::string req = "SUBSCRIBE " + path + " HTTP/1.1\r\n"
      + "HOST: " + host + ":" + std::to_string(mupnp_device_gethttpport(dev)) + "\r\n"
      + "CALLBACK: <http://" + callbackHost + ":" + std::to_string(callbackPort) + "/cb>\r\n"
      + "NT: upnp:event\r\nTIMEOUT: Second-60\r\nContent-Length: 0\r\n\r\n";
  std::string status;
  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0 && write(fd, req.data(), req.size()) == (ssize_t)req.size())
    status = read_status_line(fd);
  close(fd);
  return status;
}

/* A device deleted right after accepting a SUBSCRIBE must not send the
   initial event from freed service data afterwards (use-after-free). The
   repeated SUBSCRIBE lookups must not leak either (run under LeakSanitizer). */
BOOST_AUTO_TEST_CASE(DeviceDeleteRightAfterSubscribe)
{
  /* A callback listener that accepts nothing in particular. */
  int listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    BOOST_FAIL("Could not create the callback listener");
    return;
  }
  struct sockaddr_in cbAddr = {};
  cbAddr.sin_family = AF_INET;
  cbAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  socklen_t cbLen = sizeof(cbAddr);
  BOOST_REQUIRE(bind(listener, (struct sockaddr*)&cbAddr, sizeof(cbAddr)) == 0);
  BOOST_REQUIRE(listen(listener, 8) == 0);
  BOOST_REQUIRE(getsockname(listener, (struct sockaddr*)&cbAddr, &cbLen) == 0);
  int cbPort = ntohs(cbAddr.sin_port);

  for (int n = 0; n < 2; n++) {
    mUpnpDevice* dev = upnp_test_device_new();
    BOOST_REQUIRE(dev);
    BOOST_REQUIRE(mupnp_device_start(dev));
    std::string status = send_raw_subscribe(dev, "127.0.0.1", cbPort);
    BOOST_CHECK_MESSAGE(status.find(" 200 ") != std::string::npos, "SUBSCRIBE status: " << status);
    /* Delete while the initial event is still pending. */
    mupnp_device_stop(dev);
    mupnp_device_delete(dev);
  }

  /* Give a detached initial-event sender time to touch freed memory. */
  std::this_thread::sleep_for(std::chrono::milliseconds(2500));
  close(listener);
}
#endif

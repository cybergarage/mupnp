/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

/*
 * A known device announced with a different LOCATION (for example a host
 * that advertises on several interfaces) must keep its GENA subscription:
 * renew, NOTIFY delivery and unsubscribe have to keep working, and an
 * unreachable LOCATION must not wipe the cached device.
 */

#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <string.h>
#include <string>

#include "TestDevice.h"

#define LOCATION_TEST_DEFAULTVALUE "1234"
#define LOCATION_TEST_UPDATEVALUE "4649"

static std::string locationTestLastSid;
static int locationTestEventCount;
static int locationTestLastStatus = -1;

static void location_test_devicelistener(mUpnpControlPoint* cp, const char* udn, mUpnpDeviceStatus status)
{
  (void)cp;
  if (mupnp_streq(udn, "uuid:1234567890"))
    locationTestLastStatus = (int)status;
}

static void location_test_eventlistener(mUpnpProperty* prop)
{
  const char* sid = mupnp_property_getsid(prop);
  if (sid != NULL && mupnp_streq(mupnp_property_getname(prop), TEST_DEVICE_STATEVARIABLE_STATUS)) {
    locationTestLastSid = sid;
    locationTestEventCount++;
  }
}

#include <vector>

/* Non-loopback IPv4 addresses; mUPnP binds its servers per interface. */
static std::vector<std::string> location_test_addresses(void)
{
  std::vector<std::string> addrs;
  mUpnpNetworkInterfaceList* netIfList = mupnp_net_interfacelist_new();
  if (netIfList == NULL)
    return addrs;
  mupnp_net_gethostinterfaces(netIfList);
  for (mUpnpNetworkInterface* netIf = mupnp_net_interfacelist_gets(netIfList); netIf != NULL; netIf = mupnp_net_interface_next(netIf)) {
    const char* ifAddr = mupnp_net_interface_getaddress(netIf);
    if (ifAddr == NULL || mupnp_net_isipv6address(ifAddr) || strncmp(ifAddr, "127.", 4) == 0)
      continue;
    addrs.push_back(ifAddr);
  }
  mupnp_net_interfacelist_delete(netIfList);
  return addrs;
}

static std::string location_test_localaddress;

/* Feed an ssdp:alive NOTIFY for the test device, as if it arrived on the
   first interface, with the given LOCATION. */
static void location_test_announce(mUpnpControlPoint* cp, const std::string& location)
{
  std::string msg = "NOTIFY * HTTP/1.1\r\n"
                    "HOST: 239.255.255.250:1900\r\n"
                    "CACHE-CONTROL: max-age=1800\r\n"
                    "LOCATION: "
      + location + "\r\n"
                   "NT: upnp:rootdevice\r\n"
                   "NTS: ssdp:alive\r\n"
                   "USN: uuid:1234567890::upnp:rootdevice\r\n"
                   "\r\n";
  std::string buf = msg;

  mUpnpSSDPPacket* pkt = mupnp_ssdp_packet_new();
  BOOST_REQUIRE(pkt != NULL);
  mupnp_ssdp_packet_setheader(pkt, &buf[0]);
  mupnp_socket_datagram_packet_setlocaladdress(mupnp_ssdp_packet_getdatagrampacket(pkt), location_test_localaddress.c_str());
  mupnp_ssdp_packet_settimestamp(pkt, mupnp_getcurrentsystemtime());
  BOOST_REQUIRE(mupnp_streq(mupnp_ssdp_packet_getlocation(pkt), location.c_str()));
  mupnp_controlpoint_adddevicebyssdppacket(cp, pkt);
  mupnp_ssdp_packet_delete(pkt);
}

static std::string location_test_url(const std::string& host, int port, const char* path, bool zeroPaddedPort = false)
{
  char buf[256];
  snprintf(buf, sizeof(buf), zeroPaddedPort ? "http://%s:0%d%s" : "http://%s:%d%s", host.c_str(), port, path);
  return buf;
}

static bool location_test_setup(void)
{
  std::vector<std::string> addrs = location_test_addresses();
  if (addrs.empty())
    return false;
  location_test_localaddress = addrs[0];
  return true;
}

struct LocationTestState {
  mUpnpService* service;
  bool subscribed;
  std::string sid;
};

static LocationTestState location_test_state(mUpnpControlPoint* cp)
{
  LocationTestState st = { NULL, false, "" };
  mupnp_controlpoint_lock(cp);
  mUpnpDevice* dev = mupnp_controlpoint_getdevicebyudn(cp, "uuid:1234567890");
  if (dev != NULL) {
    st.service = mupnp_device_getservicebyexacttype(dev, TEST_DEVICE_SERVICE_TYPE);
    if (st.service != NULL) {
      st.subscribed = mupnp_service_issubscribed(st.service);
      if (st.subscribed)
        st.sid = mupnp_service_getsubscriptionsid(st.service);
      BOOST_CHECK(mupnp_service_getrootdevice(st.service) == dev);
    }
  }
  mupnp_controlpoint_unlock(cp);
  return st;
}

BOOST_AUTO_TEST_CASE(LocationChangeKeepsSubscription)
{
  BOOST_REQUIRE_MESSAGE(location_test_setup(), "needs a non-loopback IPv4 interface");
  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_REQUIRE(testDev);
  BOOST_REQUIRE(mupnp_device_start(testDev));
  int port = mupnp_device_gethttpport(testDev);
  const char* descPath = mupnp_device_getdescriptionuri(testDev);

  mUpnpControlPoint* cp = mupnp_controlpoint_new();
  BOOST_REQUIRE(cp);
  BOOST_REQUIRE(mupnp_controlpoint_start(cp));
  mupnp_controlpoint_addeventlistener(cp, location_test_eventlistener);
  mupnp_controlpoint_setdevicelistener(cp, location_test_devicelistener);

  std::vector<std::string> addrs = location_test_addresses();
  std::string locA = location_test_url(addrs[0], port, descPath);
  /* A second interface gives a real multi-homed LOCATION; otherwise spell the
     same endpoint differently so the LOCATION string still changes. */
  std::string locB = (1 < addrs.size()) ? location_test_url(addrs[1], port, descPath) : location_test_url(addrs[0], port, descPath, true);
  std::string locDead = location_test_url("127.0.0.1", 1, descPath);
  BOOST_TEST_MESSAGE("LOCATION A=" << locA << " B=" << locB);
  BOOST_REQUIRE(locA != locB);

  mUpnpService* devService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(devService != NULL);
  mUpnpStateVariable* devState = mupnp_service_getstatevariablebyname(devService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_REQUIRE(devState != NULL);
  mupnp_statevariable_setvalue(devState, LOCATION_TEST_DEFAULTVALUE);

  // Discover through LOCATION A and subscribe.
  location_test_announce(cp, locA);
  LocationTestState st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.service != NULL);
  BOOST_REQUIRE(mupnp_controlpoint_subscribe(cp, st0.service, 300));
  st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.subscribed);
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  // Same device, other address: the subscription and the service object stay.
  location_test_announce(cp, locB);
  LocationTestState st1 = location_test_state(cp);
  BOOST_REQUIRE(st1.service != NULL);
  BOOST_CHECK(st1.service == st0.service);
  BOOST_CHECK(st1.subscribed);
  BOOST_CHECK_EQUAL(st1.sid, st0.sid);

  // NOTIFY is still matched to the cached service.
  locationTestEventCount = 0;
  locationTestLastSid.clear();
  mupnp_statevariable_setvalue(devState, LOCATION_TEST_UPDATEVALUE);
  for (int n = 0; n < 20 && locationTestEventCount == 0; n++)
    mupnp_sleep(200);
  BOOST_CHECK(0 < locationTestEventCount);
  BOOST_CHECK_EQUAL(locationTestLastSid, st0.sid);

  // Renew keeps working.
  BOOST_CHECK(mupnp_controlpoint_resubscribe(cp, st0.service, 300));

  // An unreachable LOCATION must not wipe the cached device or its SID, and
  // the device stays usable, so it is reported as Updated, not Invalid.
  locationTestLastStatus = -1;
  location_test_announce(cp, locDead);
  BOOST_CHECK_EQUAL(locationTestLastStatus, (int)mUpnpDeviceStatusUpdated);
  LocationTestState st2 = location_test_state(cp);
  BOOST_REQUIRE(st2.service != NULL);
  BOOST_CHECK(st2.service == st0.service);
  BOOST_CHECK(st2.subscribed);

  // Unsubscribe reaches the device.
  BOOST_CHECK(mupnp_controlpoint_unsubscribe(cp, st0.service));

  /* The device sends the initial event from a detached thread about
     MUPNP_SERVICE_NOTIFY_WAITTIME after each SUBSCRIBE; let it finish before
     the device is deleted (same as SubscriptionTest). */
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  BOOST_REQUIRE(mupnp_controlpoint_stop(cp));
  mupnp_controlpoint_delete(cp);
  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
}

BOOST_AUTO_TEST_CASE(LocationChangeWithNewDescriptionCarriesSid)
{
  BOOST_REQUIRE_MESSAGE(location_test_setup(), "needs a non-loopback IPv4 interface");

  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_REQUIRE(testDev);
  BOOST_REQUIRE(mupnp_device_start(testDev));
  int port = mupnp_device_gethttpport(testDev);
  const char* descPath = mupnp_device_getdescriptionuri(testDev);

  mUpnpControlPoint* cp = mupnp_controlpoint_new();
  BOOST_REQUIRE(cp);
  BOOST_REQUIRE(mupnp_controlpoint_start(cp));

  location_test_announce(cp, location_test_url(location_test_localaddress, port, descPath));
  LocationTestState st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.service != NULL);
  BOOST_REQUIRE(mupnp_controlpoint_subscribe(cp, st0.service, 300));
  st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.subscribed);

  // The device now serves a different description (another eventSubURL) and
  // is announced under another LOCATION: services are rebuilt, and the SID
  // must move to the new service object instead of being dropped.
  mUpnpService* devService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(devService != NULL);
  mupnp_service_seteventsuburl(devService, "/service/power/eventSub2");
  location_test_announce(cp, location_test_url(location_test_localaddress, port, descPath, true));

  mupnp_controlpoint_lock(cp);
  mUpnpDevice* cpDev = mupnp_controlpoint_getdevicebyudn(cp, "uuid:1234567890");
  BOOST_REQUIRE(cpDev != NULL);
  mUpnpService* newService = mupnp_device_getservicebyexacttype(cpDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(newService != NULL);
  BOOST_CHECK(mupnp_streq(mupnp_xml_node_getchildnodevalue(mupnp_service_getservicenode(newService), MUPNP_SERVICE_EVENT_SUB_URL), "/service/power/eventSub2"));
  BOOST_CHECK(mupnp_service_getrootdevice(newService) == cpDev);
  BOOST_CHECK(mupnp_service_issubscribed(newService));
  BOOST_CHECK(mupnp_streq(mupnp_service_getsubscriptionsid(newService), st0.sid.c_str()));
  for (mUpnpDevice* child = mupnp_device_getdevices(cpDev); child != NULL; child = mupnp_device_next(child)) {
    BOOST_CHECK(mupnp_device_getparentdevice(child) == cpDev);
    for (mUpnpService* svc = mupnp_device_getservices(child); svc != NULL; svc = mupnp_service_next(svc))
      BOOST_CHECK(mupnp_service_getrootdevice(svc) == cpDev);
  }
  mupnp_controlpoint_unlock(cp);

  /* see the note in LocationChangeKeepsSubscription */
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  BOOST_REQUIRE(mupnp_controlpoint_stop(cp));
  mupnp_controlpoint_delete(cp);
  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
}

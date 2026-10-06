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
#include <cstring>
#include <string>

#include <mupnp/net/url.h>

#include "TestDevice.h"

namespace {

constexpr const char* TEST_UDN = "uuid:1234567890";
constexpr const char* DEFAULT_VALUE = "1234";
constexpr const char* UPDATE_VALUE = "4649";

/* State written by the C listener callbacks, which take no user data. */
struct LocationTestEvents {
  std::string lastSid;
  int eventCount = 0;
  int lastStatus = -1;
  std::string localAddress;
};

LocationTestEvents& events()
{
  static LocationTestEvents state;
  return state;
}

/* The signature is fixed by MUPNP_DEVICE_LISTENER. */
void location_test_devicelistener(mUpnpControlPoint*, const char* udn, mUpnpDeviceStatus status)
{
  if (mupnp_streq(udn, TEST_UDN))
    events().lastStatus = static_cast<int>(status);
}

void location_test_eventlistener(mUpnpProperty* prop)
{
  const char* sid = mupnp_property_getsid(prop);
  if (sid != nullptr && mupnp_streq(mupnp_property_getname(prop), TEST_DEVICE_STATEVARIABLE_STATUS)) {
    events().lastSid = sid;
    events().eventCount++;
  }
}

void location_test_reset()
{
  events() = LocationTestEvents();
}

/* Discover the test device with a real M-SEARCH, as SubscriptionTest does,
   and remember the LOCATION and the local interface address the control
   point actually used. Injected announcements reuse that address, so the test
   does not depend on how many interfaces the host has. */
std::string location_test_discover(mUpnpControlPoint* cp)
{
  std::string location;
  BOOST_REQUIRE(mupnp_controlpoint_search(cp, MUPNP_ST_ROOT_DEVICE));
  for (int n = 0; n < (mupnp_controlpoint_getssdpsearchmx(cp) + 3) * 10 && location.empty(); n++) {
    mupnp_sleep(100);
    mupnp_controlpoint_lock(cp);
    if (mUpnpDevice* dev = mupnp_controlpoint_getdevicebyudn(cp, TEST_UDN); dev != nullptr) {
      mUpnpSSDPPacket* pkt = mupnp_device_getssdppacket(dev);
      const char* loc = mupnp_ssdp_packet_getlocation(pkt);
      const char* localAddr = mupnp_ssdp_packet_getlocaladdress(pkt);
      if (loc != nullptr && localAddr != nullptr) {
        location = loc;
        events().localAddress = localAddr;
      }
    }
    mupnp_controlpoint_unlock(cp);
  }
  BOOST_REQUIRE_MESSAGE(!location.empty(), "test device not discovered");
  return location;
}

/* The LOCATION the device would advertise for host, built by the same
   library helper the device uses. UPnP descriptions are served over plain
   HTTP by the UPnP Device Architecture, so this is an http URL. */
std::string location_test_url(const std::string& host, int port, const char* path)
{
  std::string buf(256, '\0');
  mupnp_net_gethosturl(host.c_str(), port, path, &buf[0], buf.size());
  buf.resize(std::strlen(buf.c_str()));
  return buf;
}

/* The same endpoint spelled differently (zero-padded port), so the LOCATION
   string changes while it still reaches the same server. */
std::string location_test_respelled(const std::string& url, int port)
{
  std::string respelled = url;
  std::string portPart = ":" + std::to_string(port) + "/";
  if (auto pos = respelled.find(portPart); pos != std::string::npos)
    respelled.insert(pos + 1, "0");
  BOOST_REQUIRE(respelled != url);
  return respelled;
}

/* Feed an ssdp:alive NOTIFY for the test device, as if it arrived on the
   first interface, with the given LOCATION. */
void location_test_announce(mUpnpControlPoint* cp, const std::string& location)
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

  mUpnpSSDPPacket* pkt = mupnp_ssdp_packet_new();
  BOOST_REQUIRE(pkt != nullptr);
  mupnp_ssdp_packet_setheader(pkt, &msg[0]);
  mupnp_socket_datagram_packet_setlocaladdress(mupnp_ssdp_packet_getdatagrampacket(pkt), events().localAddress.c_str());
  mupnp_ssdp_packet_settimestamp(pkt, mupnp_getcurrentsystemtime());
  BOOST_REQUIRE(mupnp_streq(mupnp_ssdp_packet_getlocation(pkt), location.c_str()));
  mupnp_controlpoint_adddevicebyssdppacket(cp, pkt);
  mupnp_ssdp_packet_delete(pkt);
}

struct LocationTestState {
  mUpnpService* service = nullptr;
  bool subscribed = false;
  std::string sid;
};

LocationTestState location_test_state(mUpnpControlPoint* cp)
{
  LocationTestState st;
  mupnp_controlpoint_lock(cp);
  if (mUpnpDevice* dev = mupnp_controlpoint_getdevicebyudn(cp, TEST_UDN); dev != nullptr) {
    st.service = mupnp_device_getservicebyexacttype(dev, TEST_DEVICE_SERVICE_TYPE);
    if (st.service != nullptr) {
      st.subscribed = mupnp_service_issubscribed(st.service);
      if (st.subscribed)
        st.sid = mupnp_service_getsubscriptionsid(st.service);
      BOOST_CHECK(mupnp_service_getrootdevice(st.service) == dev);
    }
  }
  mupnp_controlpoint_unlock(cp);
  return st;
}

} // namespace

BOOST_AUTO_TEST_CASE(LocationChangeKeepsSubscription)
{
  location_test_reset();
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

  // Discover through the real LOCATION A; B is the same endpoint spelled
  // differently, so only the LOCATION string changes.
  std::string locA = location_test_discover(cp);
  std::string locB = location_test_respelled(locA, port);
  std::string locDead = location_test_url("127.0.0.1", 1, descPath);
  BOOST_TEST_MESSAGE("LOCATION A=" << locA << " B=" << locB << " local=" << events().localAddress);

  mUpnpService* devService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(devService != nullptr);
  mUpnpStateVariable* devState = mupnp_service_getstatevariablebyname(devService, TEST_DEVICE_STATEVARIABLE_STATUS);
  BOOST_REQUIRE(devState != nullptr);
  mupnp_statevariable_setvalue(devState, DEFAULT_VALUE);

  // Subscribe through LOCATION A.
  LocationTestState st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.service != nullptr);
  BOOST_REQUIRE(mupnp_controlpoint_subscribe(cp, st0.service, 300));
  st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.subscribed);
  mupnp_sleep(MUPNP_SERVICE_NOTIFY_WAITTIME * 2);

  // Same device, other address: the subscription and the service object stay.
  location_test_announce(cp, locB);
  LocationTestState st1 = location_test_state(cp);
  BOOST_REQUIRE(st1.service != nullptr);
  BOOST_CHECK(st1.service == st0.service);
  BOOST_CHECK(st1.subscribed);
  BOOST_CHECK_EQUAL(st1.sid, st0.sid);

  // NOTIFY is still matched to the cached service.
  events().eventCount = 0;
  events().lastSid.clear();
  mupnp_statevariable_setvalue(devState, UPDATE_VALUE);
  for (int n = 0; n < 20 && events().eventCount == 0; n++)
    mupnp_sleep(200);
  BOOST_CHECK(0 < events().eventCount);
  BOOST_CHECK_EQUAL(events().lastSid, st0.sid);

  // Renew keeps working.
  BOOST_CHECK(mupnp_controlpoint_resubscribe(cp, st0.service, 300));

  // An unreachable LOCATION must not wipe the cached device or its SID, and
  // the device stays usable, so it is reported as Updated, not Invalid.
  events().lastStatus = -1;
  location_test_announce(cp, locDead);
  BOOST_CHECK_EQUAL(events().lastStatus, static_cast<int>(mUpnpDeviceStatusUpdated));
  LocationTestState st2 = location_test_state(cp);
  BOOST_REQUIRE(st2.service != nullptr);
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
  location_test_reset();

  mUpnpDevice* testDev = upnp_test_device_new();
  BOOST_REQUIRE(testDev);
  BOOST_REQUIRE(mupnp_device_start(testDev));
  int port = mupnp_device_gethttpport(testDev);

  mUpnpControlPoint* cp = mupnp_controlpoint_new();
  BOOST_REQUIRE(cp);
  BOOST_REQUIRE(mupnp_controlpoint_start(cp));

  std::string locA = location_test_discover(cp);
  LocationTestState st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.service != nullptr);
  BOOST_REQUIRE(mupnp_controlpoint_subscribe(cp, st0.service, 300));
  st0 = location_test_state(cp);
  BOOST_REQUIRE(st0.subscribed);

  // The device now serves a different description (another eventSubURL) and
  // is announced under another LOCATION: services are rebuilt, and the SID
  // must move to the new service object instead of being dropped.
  mUpnpService* devService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(devService != nullptr);
  mupnp_service_seteventsuburl(devService, "/service/power/eventSub2");
  location_test_announce(cp, location_test_respelled(locA, port));

  mupnp_controlpoint_lock(cp);
  mUpnpDevice* cpDev = mupnp_controlpoint_getdevicebyudn(cp, TEST_UDN);
  BOOST_REQUIRE(cpDev != nullptr);
  mUpnpService* newService = mupnp_device_getservicebyexacttype(cpDev, TEST_DEVICE_SERVICE_TYPE);
  BOOST_REQUIRE(newService != nullptr);
  BOOST_CHECK(mupnp_streq(mupnp_xml_node_getchildnodevalue(mupnp_service_getservicenode(newService), MUPNP_SERVICE_EVENT_SUB_URL), "/service/power/eventSub2"));
  BOOST_CHECK(mupnp_service_getrootdevice(newService) == cpDev);
  BOOST_CHECK(mupnp_service_issubscribed(newService));
  BOOST_CHECK(mupnp_streq(mupnp_service_getsubscriptionsid(newService), st0.sid.c_str()));
  for (auto child = mupnp_device_getdevices(cpDev); child != nullptr; child = mupnp_device_next(child)) {
    BOOST_CHECK(mupnp_device_getparentdevice(child) == cpDev);
    for (auto svc = mupnp_device_getservices(child); svc != nullptr; svc = mupnp_service_next(svc))
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

#include "GatewayFixture.h"
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <mupnp/device.h>
#include <mupnp/service.h>
#include <string>

BOOST_AUTO_TEST_CASE(GatewayOwnerQualifiedServices)
{
  for (int version : { 1, 2 }) {
    std::string xml(gatewayDescription);
    if (version == 1) {
      for (std::string::size_type pos = 0; (pos = xml.find(":2<", pos)) != std::string::npos;)
        xml.replace(pos, 3, ":1<");
    }
    auto root = mupnp_device_new();
    BOOST_REQUIRE(mupnp_device_parsedescription(root, xml.data(), xml.size()));
    const char* id = "urn:upnp-org:serviceId:WANIPConn1";
    BOOST_CHECK(!mupnp_device_getownedservicebyserviceid(root, id));
    BOOST_CHECK(!mupnp_device_getservicebyownerudn(root, "uuid:root", id));
    auto one = mupnp_device_getservicebyownerudn(root, "uuid:conn1", id);
    auto two = mupnp_device_getservicebyownerudn(root, "uuid:conn2", id);
    auto deep = mupnp_device_getservicebyownerudn(root, "uuid:deepest", id);
    auto av = mupnp_device_getservicebyownerudn(root, "uuid:av", id);
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);
    BOOST_REQUIRE(deep);
    BOOST_REQUIRE(av);
    BOOST_CHECK_EQUAL(mupnp_service_getservicetype(av), "urn:schemas-upnp-org:service:ContentDirectory:1");
    BOOST_CHECK(one != two);
    BOOST_CHECK(two != deep);
    BOOST_CHECK(one != av);
    BOOST_CHECK_EQUAL(mupnp_device_getservicebyserviceid(root, id), one);
    auto owner = mupnp_service_getdevice(two);
    BOOST_CHECK_EQUAL(mupnp_device_getownedservicebyserviceid(owner, id), two);
    BOOST_CHECK_EQUAL(mupnp_device_getservicebyownerudn(owner, "uuid:conn2", id), two);
    BOOST_CHECK(!mupnp_device_getservicebyownerudn(owner, "uuid:conn1", id));
    BOOST_CHECK(!mupnp_device_getservicebyownerudn(root, "uuid:missing", id));
    BOOST_CHECK(!mupnp_device_getservicebyownerudn(root, nullptr, id));
    BOOST_CHECK(!mupnp_device_getownedservicebyserviceid(nullptr, id));
    BOOST_CHECK_EQUAL(mupnp_device_getrootdevice(mupnp_service_getdevice(deep)), root);
    mupnp_device_delete(root);
  }
}

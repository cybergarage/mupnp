/* Deterministic loopback-only SSDP description-fetch regressions. */
#include "TestDevice.h"
#include <boost/test/unit_test.hpp>
#if !defined(WIN32)
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace {
const char* description = "<root><device><deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType><friendlyName>Loopback</friendlyName><UDN>uuid:fetch-root</UDN><deviceList><device><deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType><UDN>uuid:fetch-child</UDN></device></deviceList></device></root>";
struct FetchServer {
  int socketFd;
  int port;
  std::mutex mutex;
  std::condition_variable cond;
  bool released = false;
  std::atomic<bool> stopped { false };
  std::atomic<int> requests { 0 };
  std::thread worker;
  FetchServer()
  {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    BOOST_REQUIRE(socketFd >= 0);
    sockaddr_in addr { };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    BOOST_REQUIRE(bind(socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0);
    socklen_t len = sizeof(addr);
    BOOST_REQUIRE(getsockname(socketFd, reinterpret_cast<sockaddr*>(&addr), &len) == 0);
    port = ntohs(addr.sin_port);
    BOOST_REQUIRE(listen(socketFd, 8) == 0);
    worker = std::thread([this] {
      while (!stopped) {
        pollfd fd { socketFd, POLLIN, 0 };
        if (poll(&fd, 1, 50) <= 0)
          continue;
        int client = accept(socketFd, nullptr, nullptr);
        if (client < 0)
          continue;
        int noSignal = 1;
#ifdef SO_NOSIGPIPE
        setsockopt(client, SOL_SOCKET, SO_NOSIGPIPE, &noSignal, sizeof(noSignal));
#endif
        requests++;
        {
          std::unique_lock<std::mutex> lock(mutex);
          cond.wait(lock, [this] { return released || stopped; });
        }
        char request[4096];
        recv(client, request, sizeof(request), MSG_DONTWAIT);
        std::string body(description);
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/xml\r\nConnection: close\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
#ifdef MSG_NOSIGNAL
        send(client, response.data(), response.size(), MSG_NOSIGNAL);
#else
        send(client, response.data(), response.size(), 0);
#endif
        close(client);
      }
    });
  }
  void release()
  {
    std::lock_guard<std::mutex> lock(mutex);
    released = true;
    cond.notify_all();
  }
  ~FetchServer()
  {
    stopped = true;
    release();
    worker.join();
    close(socketFd);
  }
  std::string url() { return "http://127.0.0.1:" + std::to_string(port) + "/description.xml"; }
};
struct FetchPacket {
  mUpnpSSDPPacket* packet = mupnp_ssdp_packet_new();
  FetchPacket(const std::string& url, const char* udn = "uuid:fetch-root")
  {
    std::string headers = "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=1800\r\nLOCATION: " + url + "\r\nUSN: " + udn + "::upnp:rootdevice\r\nST: upnp:rootdevice\r\n\r\n";
    mupnp_ssdp_packet_setheader(packet, &headers[0]);
    mupnp_socket_datagram_packet_setlocaladdress(mupnp_ssdp_packet_getdatagrampacket(packet), "127.0.0.1");
    mupnp_ssdp_packet_settimestamp(packet, mupnp_getcurrentsystemtime());
  }
  ~FetchPacket() { mupnp_ssdp_packet_delete(packet); }
};
void waitForRequest(FetchServer& server)
{
  for (int n = 0; n < 200 && server.requests == 0; n++)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  BOOST_CHECK_EQUAL(server.requests.load(), 1);
}
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchDoesNotLockCacheAndDeduplicates)
{
  FetchServer server;
  auto cp = mupnp_controlpoint_new();
  FetchPacket packet(server.url());
  auto fetch = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet); });
  waitForRequest(server);
  auto cacheRead = std::async(std::launch::async, [&] {
    mupnp_controlpoint_lock(cp);
    int count = mupnp_controlpoint_getndevices(cp);
    mupnp_controlpoint_unlock(cp);
    return count;
  });
  auto ready = cacheRead.wait_for(std::chrono::milliseconds(250));
  BOOST_CHECK(ready == std::future_status::ready);
  if (ready != std::future_status::ready)
    server.release();
  BOOST_CHECK_EQUAL(cacheRead.get(), 0);
  FetchPacket duplicate(server.url(), "uuid:fetch-child");
  mupnp_controlpoint_adddevicebyssdppacket(cp, duplicate.packet);
  BOOST_CHECK_EQUAL(server.requests.load(), 1);
  server.release();
  fetch.get();
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 1);
  mupnp_controlpoint_adddevicebyssdppacket(cp, duplicate.packet);
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 1);
  BOOST_CHECK_EQUAL(server.requests.load(), 1);
  BOOST_CHECK(mupnp_controlpoint_getdevicebyudn(cp, "uuid:fetch-child") != nullptr);
  mupnp_controlpoint_delete(cp);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchByebyeDiscardsPendingCandidate)
{
  FetchServer server;
  auto cp = mupnp_controlpoint_new();
  FetchPacket packet(server.url());
  auto fetch = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet); });
  waitForRequest(server);
  mupnp_controlpoint_removedevicebyssdppacket(cp, packet.packet);
  server.release();
  fetch.get();
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 0);
  mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet);
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 1);
  BOOST_CHECK_EQUAL(server.requests.load(), 2);
  mupnp_controlpoint_delete(cp);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchStopJoinsAndRejectsPublication)
{
  FetchServer server;
  auto cp = mupnp_controlpoint_new();
  FetchPacket packet(server.url());
  auto fetch = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet); });
  waitForRequest(server);
  auto stop = std::async(std::launch::async, [&] { mupnp_controlpoint_stop(cp); });
  BOOST_CHECK(stop.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout);
  server.release();
  fetch.get();
  stop.get();
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 0);
  mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet);
  BOOST_CHECK_EQUAL(server.requests.load(), 1);
  mupnp_controlpoint_delete(cp);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchAlternateLocationPreservesOwnedTree)
{
  FetchServer server;
  auto cp = mupnp_controlpoint_new();
  auto cached = mupnp_device_new();
  BOOST_REQUIRE(mupnp_device_parsedescription(cached, description, strlen(description)));
  FetchPacket original("http://127.0.0.1:1/old.xml");
  mupnp_device_setssdppacket(cached, original.packet);
  mupnp_controlpoint_adddevice(cp, cached);
  auto child = mupnp_controlpoint_getdevicebyudn(cp, "uuid:fetch-child");
  FetchPacket alternate(server.url(), "uuid:fetch-child");
  auto fetch = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, alternate.packet); });
  waitForRequest(server);
  auto read = std::async(std::launch::async, [&] {
    mupnp_controlpoint_lock(cp);
    bool retained = mupnp_controlpoint_getdevicebyudn(cp, "uuid:fetch-root") == cached;
    mupnp_controlpoint_unlock(cp);
    return retained;
  });
  auto ready = read.wait_for(std::chrono::milliseconds(250));
  BOOST_CHECK(ready == std::future_status::ready);
  if (ready != std::future_status::ready)
    server.release();
  BOOST_CHECK(read.get());
  server.release();
  fetch.get();
  BOOST_CHECK(mupnp_controlpoint_getdevicebyudn(cp, "uuid:fetch-root") == cached);
  BOOST_CHECK(mupnp_controlpoint_getdevicebyudn(cp, "uuid:fetch-child") == child);
  BOOST_CHECK_EQUAL(std::string(mupnp_ssdp_packet_getlocation(mupnp_device_getssdppacket(cached))), server.url());
  FetchPacket failed("http://127.0.0.1:1/unreachable.xml");
  mupnp_controlpoint_adddevicebyssdppacket(cp, failed.packet);
  BOOST_CHECK_EQUAL(std::string(mupnp_ssdp_packet_getlocation(mupnp_device_getssdppacket(cached))), server.url());
  mupnp_controlpoint_delete(cp);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchOlderLocationCannotReplaceNewerRoot)
{
  FetchServer slow;
  FetchServer newer;
  newer.release();
  auto cp = mupnp_controlpoint_new();
  auto cached = mupnp_device_new();
  BOOST_REQUIRE(mupnp_device_parsedescription(cached, description, strlen(description)));
  FetchPacket original("http://127.0.0.1:1/old.xml");
  mupnp_device_setssdppacket(cached, original.packet);
  mupnp_controlpoint_adddevice(cp, cached);
  FetchPacket first(slow.url());
  auto fetch = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, first.packet); });
  waitForRequest(slow);
  FetchPacket second(newer.url(), "uuid:fetch-child");
  auto update = std::async(std::launch::async, [&] { mupnp_controlpoint_adddevicebyssdppacket(cp, second.packet); });
  /* Native HTTP can finish the second request while the first stalls. Curl's
     existing persistent-connection mutex serializes HTTP, so release the
     first response after giving the newer reservation time to enter. */
  update.wait_for(std::chrono::milliseconds(250));
  slow.release();
  fetch.get();
  update.get();
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 1);
  BOOST_CHECK_EQUAL(std::string(mupnp_ssdp_packet_getlocation(mupnp_device_getssdppacket(cached))), newer.url());
  mupnp_controlpoint_delete(cp);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchRejectsUnrelatedDescription)
{
  FetchServer server;
  server.release();
  auto cp = mupnp_controlpoint_new();
  FetchPacket packet(server.url(), "uuid:unrelated");
  mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet);
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 0);
  mupnp_controlpoint_delete(cp);
}
struct FetchWorkerState {
  mUpnpControlPoint* cp;
  mUpnpSSDPPacket* packet;
};
static void fetchWorker(mUpnpThread* thread)
{
  auto state = static_cast<FetchWorkerState*>(mupnp_thread_getuserdata(thread));
  mupnp_controlpoint_adddevicebyssdppacket(state->cp, state->packet);
}
BOOST_AUTO_TEST_CASE(DiscoveryFetchCooperativeCancellationAndRetry)
{
  FetchServer server;
  auto cp = mupnp_controlpoint_new();
  FetchPacket packet(server.url());
  FetchWorkerState state { cp, packet.packet };
  auto thread = mupnp_thread_new();
  mupnp_thread_setuserdata(thread, &state);
  mupnp_thread_setaction(thread, fetchWorker);
  BOOST_REQUIRE(mupnp_thread_start(thread));
  waitForRequest(server);
  auto start = std::chrono::steady_clock::now();
  BOOST_CHECK(mupnp_thread_stop(thread));
  BOOST_CHECK(std::chrono::steady_clock::now() - start < std::chrono::seconds(3));
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 0);
  mupnp_thread_delete(thread);
  server.release();
  mupnp_controlpoint_adddevicebyssdppacket(cp, packet.packet);
  BOOST_CHECK_EQUAL(mupnp_controlpoint_getndevices(cp), 1);
  BOOST_CHECK_EQUAL(server.requests.load(), 2);
  mupnp_controlpoint_delete(cp);
}

#endif

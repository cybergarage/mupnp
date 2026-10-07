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

#include <mupnp/net/interface.h>

#if !defined(WIN32)
#include <arpa/inet.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mupnp/net/socket.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/util/thread.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#endif

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(NetworkInterface)
{
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
  mUpnpByte macAddr[MUPNP_NET_MACADDR_SIZE];
  mUpnpByte nullMacAddr[MUPNP_NET_MACADDR_SIZE];
  memset(nullMacAddr, 0, MUPNP_NET_MACADDR_SIZE);
#endif

  mUpnpNetworkInterfaceList* netIfList = mupnp_net_interfacelist_new();
  BOOST_REQUIRE(netIfList);
  BOOST_REQUIRE(0 < mupnp_net_gethostinterfaces(netIfList));
  for (mUpnpNetworkInterface* netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    char* ipaddr = mupnp_net_interface_getaddress(netIf);
    BOOST_REQUIRE(0 < mupnp_strlen(ipaddr));
    BOOST_REQUIRE(mupnp_streq(ipaddr, "0.0.0.0") == false);
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
    mupnp_net_interface_getmacaddress(netIf, macAddr);
    BOOST_REQUIRE(memcmp(macAddr, nullMacAddr, MUPNP_NET_MACADDR_SIZE) != 0);
#endif
    // BOOST_REQUIRE(0 < mupnp_strlen(mupnp_net_interface_getname(netIf)));
    // BOOST_REQUIRE(0 < mupnp_strlen(mupnp_net_interface_getnetmask(netIf)));
  }
  mupnp_net_interfacelist_delete(netIfList);
}

#if !defined(WIN32)

BOOST_AUTO_TEST_CASE(DatagramEmptyThenValid)
{
  int descriptors[2];
  BOOST_REQUIRE_EQUAL(socketpair(AF_UNIX, SOCK_DGRAM, 0, descriptors), 0);
  mUpnpSocket* socket = mupnp_socket_dgram_new();
  mupnp_socket_setid(socket, descriptors[0]);
  mUpnpDatagramPacket* packet = mupnp_socket_datagram_packet_new();
  BOOST_REQUIRE_EQUAL(send(descriptors[1], "", 0, 0), 0);
  BOOST_REQUIRE_EQUAL(mupnp_socket_recv(socket, packet), 0);
  BOOST_REQUIRE_EQUAL(send(descriptors[1], "valid", 5, 0), 5);
  BOOST_REQUIRE_EQUAL(mupnp_socket_recv(socket, packet), 5);
  BOOST_CHECK_EQUAL(mupnp_socket_datagram_packet_getdata(packet), "valid");
  mupnp_socket_datagram_packet_delete(packet);
  mupnp_socket_delete(socket);
  close(descriptors[1]);
}

struct ShutdownState {
  std::atomic<bool> entered { false };
  std::atomic<bool> finished { false };
};

static void slow_worker(mUpnpThread* thread)
{
  ShutdownState* state = static_cast<ShutdownState*>(mupnp_thread_getuserdata(thread));
  state->entered = true;
  while (mupnp_thread_isrunnable(thread))
    std::this_thread::yield();
  /* Deliberately outlast the old one-second guessed shutdown delay. */
  auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1100);
  while (std::chrono::steady_clock::now() < deadline)
    std::this_thread::yield();
  state->finished = true;
}

BOOST_AUTO_TEST_CASE(ThreadShutdownWaitsForCompletion)
{
  ShutdownState state;
  mUpnpThread* thread = mupnp_thread_new();
  mupnp_thread_setuserdata(thread, &state);
  mupnp_thread_setaction(thread, slow_worker);
  BOOST_REQUIRE(mupnp_thread_start(thread));
  while (!state.entered)
    std::this_thread::yield();
  BOOST_REQUIRE(mupnp_thread_stop(thread));
  BOOST_CHECK(state.finished.load());
  mupnp_thread_delete(thread);
}

static void self_deleting_worker(mUpnpThread* thread)
{
  ShutdownState* state = static_cast<ShutdownState*>(mupnp_thread_getuserdata(thread));
  mupnp_thread_delete(thread);
  /* The trampoline must retain thread memory until this action returns. */
  BOOST_CHECK(!mupnp_thread_isrunnable(thread));
  state->finished = true;
}

BOOST_AUTO_TEST_CASE(ThreadImmediateStopAndSelfDeletion)
{
  for (int n = 0; n < 50; n++) {
    mUpnpThread* thread = mupnp_thread_new();
    BOOST_REQUIRE(mupnp_thread_start(thread));
    BOOST_REQUIRE(mupnp_thread_stop(thread));
    BOOST_REQUIRE(mupnp_thread_start(thread));
    mupnp_thread_delete(thread);
  }
  ShutdownState state;
  mUpnpThread* thread = mupnp_thread_new();
  mupnp_thread_setuserdata(thread, &state);
  mupnp_thread_setaction(thread, self_deleting_worker);
  BOOST_REQUIRE(mupnp_thread_start(thread));
  while (!state.finished)
    std::this_thread::yield();
}

BOOST_AUTO_TEST_CASE(SSDPEmptyHeaderCleanup)
{
  mUpnpSSDPPacket* packet = mupnp_ssdp_packet_new();
  for (int n = 0; n < 1000; n++) {
    char delimiters[] = "\r\n\r\n";
    mupnp_ssdp_packet_setheader(packet, delimiters);
  }
  char valid[] = "HTTP/1.1 200 OK\r\nST: upnp:rootdevice\r\n\r\n";
  mupnp_ssdp_packet_setheader(packet, valid);
  BOOST_CHECK_EQUAL(mupnp_ssdp_packet_getst(packet), "upnp:rootdevice");
  mupnp_ssdp_packet_delete(packet);
}
#endif

#if !defined(WIN32)
BOOST_AUTO_TEST_CASE(SSDPSearchDelayBound)
{
  mUpnpSSDPPacket* packet = mupnp_ssdp_packet_new();
  const std::array<const char*, 8> values = { "0", "1", "5", "6", "2147483647", "999999999999999999999999", "5x", "-1" };
  const std::array<int, 8> expected = { 0, 1, 5, 5, 5, 5, 0, 0 };
  for (size_t n = 0; n < values.size(); n++) {
    mupnp_http_headerlist_set(packet->headerList, MUPNP_HTTP_MX, values[n]);
    BOOST_CHECK_EQUAL(mupnp_ssdp_packet_getmx(packet), expected[n]);
  }
  mupnp_ssdp_packet_delete(packet);
}
#endif

#if !defined(WIN32)
static void count_ssdp(mUpnpSSDPPacket* packet)
{
  static_cast<std::atomic<int>*>(mupnp_ssdp_packet_getuserdata(packet))->fetch_add(1);
}

BOOST_AUTO_TEST_CASE(SSDPWorkerIgnoresEmptyDatagram)
{
  std::atomic<int> received { 0 };
  mUpnpSSDPResponseServer* server = mupnp_ssdpresponse_server_new();
  char address[] = "127.0.0.1";
  BOOST_REQUIRE(mupnp_ssdpresponse_server_open(server, 38196, address));
  mupnp_ssdpresponse_server_setuserdata(server, &received);
  mupnp_ssdpresponse_server_setlistener(server, count_ssdp);
  BOOST_REQUIRE(mupnp_ssdpresponse_server_start(server));
  int sender = socket(AF_INET, SOCK_DGRAM, 0);
  if (sender < 0) {
    mupnp_ssdpresponse_server_delete(server);
    BOOST_FAIL("Could not create UDP test sender");
    return;
  }
  struct sockaddr_in target = {};
  target.sin_family = AF_INET;
  target.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  target.sin_port = htons(38196);
  BOOST_REQUIRE_EQUAL(sendto(sender, "", 0, 0, (struct sockaddr*)&target, sizeof(target)), 0);
  const char* valid = "HTTP/1.1 200 OK\r\nST: upnp:rootdevice\r\n\r\n";
  BOOST_REQUIRE_EQUAL(sendto(sender, valid, strlen(valid), 0, (struct sockaddr*)&target, sizeof(target)), (ssize_t)strlen(valid));
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (!received.load() && std::chrono::steady_clock::now() < deadline)
    std::this_thread::yield();
  BOOST_CHECK_EQUAL(received.load(), 1);
  close(sender);
  mupnp_ssdpresponse_server_delete(server);
}
#endif

#if !defined(WIN32) && !defined(ESP_PLATFORM)

/* Issue #10: a timeout set before mupnp_socket_connect() creates the
   descriptor must still apply, both to connect() and to later reads. */

static int open_test_listener(int backlog)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return -1;
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0 || listen(fd, backlog) != 0) {
    close(fd);
    return -1;
  }
  return fd;
}

/* Returns the port a listener was bound to, or -1. */
static int listener_port(int fd)
{
  if (fd < 0)
    return -1;
  struct sockaddr_in addr = {};
  socklen_t len = sizeof(addr);
  return (getsockname(fd, (struct sockaddr*)&addr, &len) == 0) ? ntohs(addr.sin_port) : -1;
}

BOOST_AUTO_TEST_CASE(SocketTimeoutAppliesToReadAfterConnect)
{
  int listener = open_test_listener(4);
  BOOST_REQUIRE(0 <= listener);
  int port = listener_port(listener);
  BOOST_REQUIRE(0 < port);

  mUpnpSocket* sock = mupnp_socket_stream_new();
  BOOST_REQUIRE(sock);
  BOOST_CHECK(mupnp_socket_settimeout(sock, 1));
  BOOST_REQUIRE(mupnp_socket_connect(sock, "127.0.0.1", port));

  /* The peer never answers: the read must give up after the timeout
     instead of blocking forever. */
  std::array<char, 16> buf {};
  auto start = std::chrono::steady_clock::now();
  ssize_t n = mupnp_socket_read(sock, buf.data(), buf.size());
  auto elapsed = std::chrono::steady_clock::now() - start;
  BOOST_CHECK(n <= 0);
  BOOST_CHECK(elapsed < std::chrono::seconds(5));

  mupnp_socket_close(sock);
  mupnp_socket_delete(sock);
  close(listener);
}

BOOST_AUTO_TEST_CASE(SocketConnectRefusedFailsFast)
{
  int listener = open_test_listener(1);
  BOOST_REQUIRE(0 <= listener);
  int port = listener_port(listener);
  BOOST_REQUIRE(0 < port);
  close(listener); /* Nothing listens on the port any more. */

  mUpnpSocket* sock = mupnp_socket_stream_new();
  BOOST_REQUIRE(sock);
  BOOST_CHECK(mupnp_socket_settimeout(sock, 3));
  auto start = std::chrono::steady_clock::now();
  BOOST_CHECK(!mupnp_socket_connect(sock, "127.0.0.1", port));
  BOOST_CHECK(std::chrono::steady_clock::now() - start < std::chrono::seconds(2));
  mupnp_socket_close(sock);
  mupnp_socket_delete(sock);
}

BOOST_AUTO_TEST_CASE(SocketConnectTimesOut)
{
  /* A listener that never accepts drops SYNs once its backlog is full, so
     further connects hang in the kernel. Each attempt must give up within
     the requested timeout. */
  int listener = open_test_listener(0);
  BOOST_REQUIRE(0 <= listener);
  int port = listener_port(listener);
  BOOST_REQUIRE(0 < port);

  std::vector<mUpnpSocket*> socks;
  bool timedOut = false;
  for (int n = 0; n < 16; n++) {
    mUpnpSocket* sock = mupnp_socket_stream_new();
    BOOST_REQUIRE(sock);
    socks.push_back(sock);
    BOOST_CHECK(mupnp_socket_settimeout(sock, 1));
    auto start = std::chrono::steady_clock::now();
    bool connected = mupnp_socket_connect(sock, "127.0.0.1", port);
    auto elapsed = std::chrono::steady_clock::now() - start;
    BOOST_CHECK(elapsed < std::chrono::seconds(4));
    if (!connected && std::chrono::milliseconds(800) <= elapsed) {
      timedOut = true;
      break;
    }
  }
  /* Some kernels answer a full backlog with RST; only require the bound. */
  BOOST_TEST_MESSAGE("connect timeout observed: " << timedOut);

  for (mUpnpSocket* sock : socks) {
    mupnp_socket_close(sock);
    mupnp_socket_delete(sock);
  }
  close(listener);
}

#endif

#if !defined(WIN32)

/* mupnp_net_selectaddr() picks the local address advertised in SSDP replies.
   It must always return one of this host's interface addresses (or the
   loopback fallback), whatever the peer's address family. */
static bool is_host_address(const std::string& addr)
{
  if (addr.empty())
    return false;
  if (addr == "127.0.0.1")
    return true;
  bool found = false;
  auto* ifList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(ifList);
  for (auto* netIf = mupnp_net_interfacelist_gets(ifList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    if (addr == mupnp_net_interface_getaddress(netIf))
      found = true;
  }
  mupnp_net_interfacelist_delete(ifList);
  return found;
}

/* Calls mupnp_net_selectaddr() and takes ownership of the returned string. */
static std::string select_addr(const void* remote)
{
  std::unique_ptr<char, void (*)(void*)> addr(mupnp_net_selectaddr((struct sockaddr*)remote), std::free);
  return addr ? std::string(addr.get()) : std::string();
}

BOOST_AUTO_TEST_CASE(SelectAddrReturnsHostAddress)
{
  /* IPv4 peer on an unrelated subnet (TEST-NET-3 documentation range). */
  struct sockaddr_in remote4 = {};
  remote4.sin_family = AF_INET;
  inet_pton(AF_INET, "203.0.113.5", &remote4.sin_addr); // NOSONAR: documentation address
  std::string addr = select_addr(&remote4);
  BOOST_CHECK(is_host_address(addr));
  BOOST_CHECK(!mupnp_net_isipv6address(addr.c_str()));

  /* IPv4 peer on the same subnet as an interface selects that interface. */
  auto* ifList = mupnp_net_interfacelist_new();
  mupnp_net_gethostinterfaces(ifList);
  const auto* netIf = mupnp_net_interfacelist_gets(ifList);
  if (netIf && !mupnp_net_isipv6address(mupnp_net_interface_getaddress(netIf))) {
    struct sockaddr_in same = {};
    same.sin_family = AF_INET;
    inet_pton(AF_INET, mupnp_net_interface_getaddress(netIf), &same.sin_addr);
    BOOST_CHECK_EQUAL(select_addr(&same), std::string(mupnp_net_interface_getaddress(netIf)));
  }
  mupnp_net_interfacelist_delete(ifList);

  /* IPv6 peer: never an IPv4 address decoded from IPv6 bytes. With IPv6
     disabled (the default) an IPv4 host address is used. */
  struct sockaddr_in6 remote6 = {};
  remote6.sin6_family = AF_INET6;
  inet_pton(AF_INET6, "fe80::1234", &remote6.sin6_addr);
  remote6.sin6_scope_id = 1;
  BOOST_CHECK(is_host_address(select_addr(&remote6)));

  /* With IPv6 enabled, an IPv6 address is chosen when the host has one. */
  mupnp_net_setipv6enabled(true);
  addr = select_addr(&remote6);
  BOOST_CHECK(is_host_address(addr));
  BOOST_TEST_MESSAGE("selected for IPv6 peer: " << addr);
  mupnp_net_setipv6enabled(false);
}

#endif

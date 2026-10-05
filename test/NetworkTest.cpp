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
#include <atomic>
#include <chrono>
#include <mupnp/net/socket.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/util/thread.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

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
  const char* values[] = { "0", "1", "5", "6", "2147483647", "999999999999999999999999", "5x", "-1" };
  const int expected[] = { 0, 1, 5, 5, 5, 5, 0, 0 };
  for (size_t n = 0; n < sizeof(values) / sizeof(values[0]); n++) {
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

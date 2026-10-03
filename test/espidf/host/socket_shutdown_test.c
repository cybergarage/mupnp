/* ESP socket path on host sockets: validates cooperative shutdown, not lwIP. */
#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <esp_timer.h>
#include <mupnp/net/interface.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/thread.h>

static atomic_bool entered;
static ssize_t result;
static bool datagram;

/* No packet is received in the UDP cancellation case. */
char* mupnp_net_selectaddr(struct sockaddr* address)
{
  (void)address;
  return mupnp_strdup("127.0.0.1");
}

static void blocked_reader(mUpnpThread* thread)
{
  mUpnpSocket* sock = mupnp_thread_getuserdata(thread);
  char byte;
  atomic_store(&entered, true);
  if (datagram) {
    mUpnpDatagramPacket* packet = mupnp_socket_datagram_packet_new();
    result = mupnp_socket_recv(sock, packet);
    mupnp_socket_datagram_packet_delete(packet);
  }
  else {
    result = mupnp_socket_read(sock, &byte, 1);
  }
}

static void stop_blocked_reader(bool udp)
{
  int pair[2];
  assert(socketpair(AF_UNIX, udp ? SOCK_DGRAM : SOCK_STREAM, 0, pair) == 0);
  mUpnpSocket* sock = mupnp_socket_new(udp ? MUPNP_NET_SOCKET_DGRAM : MUPNP_NET_SOCKET_STREAM);
  assert(sock);
  mupnp_socket_setid(sock, pair[0]);
  mUpnpThread* thread = mupnp_thread_new();
  assert(thread);
  datagram = udp;
  result = 0;
  atomic_store(&entered, false);
  mupnp_thread_setaction(thread, blocked_reader);
  mupnp_thread_setuserdata(thread, sock);
  assert(mupnp_thread_start(thread));
  while (!atomic_load(&entered))
    usleep(1000);
  usleep(20000);
  int64_t start = esp_timer_get_time();
  assert(mupnp_thread_stop(thread));
  assert(esp_timer_get_time() - start < 1000000);
  assert(result < 0);
  assert(mupnp_thread_delete(thread));
  mupnp_socket_delete(sock);
  close(pair[1]);
}

static void timeout_before_descriptor(void)
{
  int pair[2];
  char byte;
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
  mUpnpSocket* sock = mupnp_socket_stream_new();
  assert(mupnp_socket_settimeout(sock, 1));
  mupnp_socket_setid(sock, pair[0]);
  int64_t start = esp_timer_get_time();
  assert(mupnp_socket_read(sock, &byte, 1) < 0);
  assert(errno == ETIMEDOUT);
  int64_t elapsed = esp_timer_get_time() - start;
  assert(elapsed >= 900000 && elapsed < 2000000);
  assert(write(pair[1], "x", 1) == 1);
  assert(mupnp_socket_read(sock, &byte, 1) == 1 && byte == 'x');
  mupnp_socket_delete(sock);
  close(pair[1]);
}

int main(void)
{
  stop_blocked_reader(false);
  stop_blocked_reader(true);
  timeout_before_descriptor();
  puts("ESP socket cancellation and timeout tests passed");
  return 0;
}

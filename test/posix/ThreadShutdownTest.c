/* Offline POSIX lifecycle characterization. No multicast or appliance traffic. */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <mupnp/net/interface.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/cond.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/thread.h>

static atomic_uint quitSent;
static atomic_uint otherSent;
static atomic_uint sendFailed;
static volatile sig_atomic_t quitReceived;
static void owner_quit_handler(int signal)
{
  (void)signal;
  quitReceived++;
}
static int pendingConnect = -1;

/* Deterministic pending-connect case: a full local stream is not writable.
 * All other connect calls are forwarded unchanged. No unreachable LAN target. */
int mupnp_test_connect(int fd, const struct sockaddr* address, socklen_t length)
{
  if (fd == pendingConnect) {
    errno = EINPROGRESS;
    return -1;
  }
  return connect(fd, address, length);
}

/* Only thread.c's call site is renamed when compiling this standalone test.
 * Forward the original signal to the original target. Do not replace the
 * library's handler or configure a debugger to ignore anything. */
int mupnp_test_pthread_kill(pthread_t target, int signal)
{
  if (signal == SIGQUIT)
    atomic_fetch_add(&quitSent, 1);
  else
    atomic_fetch_add(&otherSent, 1);
  int result = pthread_kill(target, signal);
  if (result != 0)
    atomic_fetch_add(&sendFailed, 1);
  return result;
}

/* The blocked UDP case never receives a packet. Do not enumerate host NICs. */
char* mupnp_net_selectaddr(struct sockaddr* remoteAddr)
{
  (void)remoteAddr;
  return mupnp_strdup("127.0.0.1");
}

typedef enum { WaitCondition,
  WaitStream,
  WaitDatagram,
  WaitAccept,
  WaitWrite,
  WaitConnect,
  WaitSleep,
  WaitPartialWrite } WaitKind;
typedef struct {
  WaitKind kind;
  mUpnpSocket* socket;
  mUpnpCond* condition;
  mUpnpMutex* mutex;
  atomic_bool entered;
  atomic_bool finished;
  ssize_t result;
} WaitContext;

static double monotonic_seconds(void)
{
  struct timespec now;
  assert(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}

static void watchdog(int signal)
{
  (void)signal;
  const char message[] = "FAIL: POSIX shutdown watchdog expired\n";
  (void)write(STDERR_FILENO, message, sizeof(message) - 1);
  _exit(1);
}

static void blocked_worker(mUpnpThread* thread)
{
  WaitContext* context = mupnp_thread_getuserdata(thread);
  assert(mupnp_thread_self() == thread);
  if (context->kind == WaitSleep) {
    atomic_store(&context->entered, true);
    mupnp_wait(24L * 60 * 60 * 1000);
  }
  else if (context->kind == WaitCondition) {
    mupnp_mutex_lock(context->mutex);
    atomic_store(&context->entered, true);
    while (mupnp_thread_isrunnable(thread))
      mupnp_cond_wait(context->condition, context->mutex, 0);
    mupnp_mutex_unlock(context->mutex);
  }
  else {
    atomic_store(&context->entered, true);
    if (context->kind == WaitWrite || context->kind == WaitPartialWrite) {
      size_t length = context->kind == WaitPartialWrite ? 1024 * 1024 : 1;
      char* bytes = calloc(1, length);
      assert(bytes);
      context->result = mupnp_socket_write(context->socket, bytes, length) == 0 ? -1 : 1;
      free(bytes);
    }
    else if (context->kind == WaitConnect) {
      context->result = mupnp_socket_connect(context->socket, "127.0.0.1", 1) ? 1 : -1;
    }
    else if (context->kind == WaitStream) {
      char byte;
      context->result = mupnp_socket_read(context->socket, &byte, 1);
    }
    else if (context->kind == WaitDatagram) {
      mUpnpDatagramPacket* packet = mupnp_socket_datagram_packet_new();
      assert(packet);
      context->result = mupnp_socket_recv(context->socket, packet);
      mupnp_socket_datagram_packet_delete(packet);
    }
    else {
      mUpnpSocket* client = mupnp_socket_stream_new();
      assert(client);
      context->result = mupnp_socket_accept(context->socket, client) ? 1 : -1;
      mupnp_socket_delete(client);
    }
    assert(context->result < 0);
  }
  assert(!mupnp_thread_isrunnable(thread));
  atomic_store(&context->finished, true);
}

static void exercise(WaitKind kind, const char* name)
{
  alarm(5);
  WaitContext context = { .kind = kind };
  int peer = -1;
  context.condition = mupnp_cond_new();
  context.mutex = mupnp_mutex_new();
  assert(context.condition && context.mutex);
  if (kind == WaitStream || kind == WaitDatagram || kind == WaitWrite || kind == WaitConnect || kind == WaitPartialWrite) {
    int pair[2];
    assert(socketpair(AF_UNIX, kind == WaitDatagram ? SOCK_DGRAM : SOCK_STREAM, 0, pair) == 0);
    peer = pair[1];
    context.socket = mupnp_socket_new(kind == WaitDatagram ? MUPNP_NET_SOCKET_DGRAM : MUPNP_NET_SOCKET_STREAM);
    assert(context.socket);
    mupnp_socket_setid(context.socket, pair[0]);
    if (kind == WaitPartialWrite) {
      int small = 4096;
      assert(setsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, &small, sizeof(small)) == 0);
    }
    if (kind == WaitWrite || kind == WaitConnect) {
      char bytes[4096] = { 0 };
      int small = 4096;
      assert(setsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, &small, sizeof(small)) == 0);
      int flags = fcntl(pair[0], F_GETFL, 0);
      assert(flags >= 0 && fcntl(pair[0], F_SETFL, flags | O_NONBLOCK) == 0);
      while (send(pair[0], bytes, sizeof(bytes), 0) > 0) { }
      assert(errno == EAGAIN || errno == EWOULDBLOCK);
      assert(fcntl(pair[0], F_SETFL, flags) == 0);
      if (kind == WaitConnect)
        pendingConnect = pair[0];
    }
  }
  else if (kind == WaitAccept) {
    /* Loopback-only ephemeral listener; no client connects or sends bytes. */
    context.socket = mupnp_socket_stream_new();
    assert(context.socket);
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener >= 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    assert(bind(listener, (struct sockaddr*)&address, sizeof(address)) == 0);
    mupnp_socket_setid(context.socket, listener);
    assert(mupnp_socket_listen(context.socket));
  }
  mUpnpThread* thread = mupnp_thread_new();
  assert(thread);
  mupnp_thread_setaction(thread, blocked_worker);
  mupnp_thread_setuserdata(thread, &context);
  unsigned before = atomic_load(&quitSent);
  double maximum = 0;
  /* Reuse the same worker and socket: 25 starts/restarts per wait kind.
   * Stop twice each cycle and delete only after the worker has completed. */
  for (int cycle = 0; cycle < 25; cycle++) {
    alarm(5);
    atomic_store(&context.entered, false);
    atomic_store(&context.finished, false);
    context.result = 0;
    assert(mupnp_thread_start(thread));
    while (!atomic_load(&context.entered))
      usleep(1000);
    if (kind == WaitCondition) {
      mupnp_mutex_lock(context.mutex);
      mupnp_mutex_unlock(context.mutex);
    }
    else
      usleep(20000);
    double started = monotonic_seconds();
    assert(mupnp_thread_stop_with_cond(thread, kind == WaitCondition ? context.condition : NULL));
    assert(atomic_load(&context.finished));
    if (context.socket) {
      assert(!(fcntl(context.socket->id, F_GETFL, 0) & O_NONBLOCK));
      struct timeval timeout;
      socklen_t length = sizeof(timeout);
      assert(getsockopt(context.socket->id, SOL_SOCKET, SO_SNDTIMEO, &timeout, &length) == 0);
      assert(timeout.tv_sec == 0 && timeout.tv_usec == 0);
    }
    unsigned stoppedSignals = atomic_load(&quitSent);
    assert(mupnp_thread_stop(thread));
    assert(atomic_load(&quitSent) == stoppedSignals);
    double elapsed = monotonic_seconds() - started;
    assert(elapsed < 2.0);
    if (elapsed > maximum)
      maximum = elapsed;
    if (kind == WaitPartialWrite) {
      char bytes[4096];
      int flags = fcntl(peer, F_GETFL, 0);
      assert(flags >= 0 && fcntl(peer, F_SETFL, flags | O_NONBLOCK) == 0);
      while (recv(peer, bytes, sizeof(bytes), 0) > 0) { }
      assert(errno == EAGAIN || errno == EWOULDBLOCK);
      assert(fcntl(peer, F_SETFL, flags) == 0);
    }
    alarm(0);
  }
  assert(mupnp_thread_delete(thread));
  pendingConnect = -1;
  if (context.socket)
    mupnp_socket_delete(context.socket);
  if (peer >= 0)
    close(peer);
  mupnp_cond_delete(context.condition);
  mupnp_mutex_delete(context.mutex);
  printf("PASS: %s: 25 start/stop/restart cycles, repeated stop; SIGQUIT sent=%u; maximum stop=%.3fs\n", name, atomic_load(&quitSent) - before, maximum);
}

int main(int argc, char** argv)
{
  bool requireSignalFree = argc == 1 || (argc == 2 && strcmp(argv[1], "--require-signal-free") == 0);
  assert(argc == 1 || requireSignalFree);
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = watchdog;
  sigemptyset(&action.sa_mask);
  assert(sigaction(SIGALRM, &action, NULL) == 0);
  struct sigaction previousQuit;
  action.sa_handler = owner_quit_handler;
  assert(sigaction(SIGQUIT, &action, &previousQuit) == 0);
  exercise(WaitCondition, "condition wait");
  exercise(WaitStream, "TCP-style stream receive (socketpair)");
  exercise(WaitDatagram, "UDP-style datagram receive (socketpair)");
  exercise(WaitAccept, "loopback TCP accept");
  exercise(WaitWrite, "backpressured stream write");
  exercise(WaitConnect, "injected pending connect on a full local stream");
  exercise(WaitSleep, "long advertiser-style wait");
  exercise(WaitPartialWrite, "partial/progressing large stream write");
  assert(sigaction(SIGQUIT, NULL, &action) == 0);
  assert(action.sa_handler == owner_quit_handler && quitReceived == 0);
  assert(sigaction(SIGQUIT, &previousQuit, NULL) == 0);
  assert(atomic_load(&otherSent) == 0);
  assert(atomic_load(&sendFailed) == 0);
  unsigned signals = atomic_load(&quitSent);
  printf("OBSERVED: 200 offline lifecycle cycles; SIGQUIT sent=%u (not app Search presses)\n", signals);
  fflush(stdout);
  if (requireSignalFree && signals > 0) {
    fputs("FAIL: signal-free shutdown contract violated by SIGQUIT delivery\n", stderr);
    return 1;
  }
  return 0;
}

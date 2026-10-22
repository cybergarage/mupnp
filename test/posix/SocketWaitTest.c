/* Loopback-only timeout, TLS continuation and cooperative stop regression. */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <mupnp/net/interface.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/thread.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

static atomic_uint signalsSent;
int mupnp_test_pthread_kill(pthread_t target, int signal)
{
  atomic_fetch_add(&signalsSent, 1);
  return pthread_kill(target, signal);
}
char* mupnp_net_selectaddr(struct sockaddr* address)
{
  (void)address;
  return mupnp_strdup("127.0.0.1");
}
static double seconds(void)
{
  struct timespec now;
  assert(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}
static void watchdog(int signal)
{
  (void)signal;
  const char message[] = "FAIL: socket/TLS wait watchdog expired\n";
  (void)write(2, message, sizeof(message) - 1);
  _exit(1);
}
static int listener(int* port)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(fd >= 0);
  struct sockaddr_in address = { 0 };
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  assert(bind(fd, (struct sockaddr*)&address, sizeof(address)) == 0);
  socklen_t length = sizeof(address);
  assert(getsockname(fd, (struct sockaddr*)&address, &length) == 0);
  *port = ntohs(address.sin_port);
  assert(listen(fd, 1) == 0);
  return fd;
}
static void plain_contract(void)
{
  int pair[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
  mUpnpSocket* sock = mupnp_socket_stream_new();
  assert(sock && mupnp_socket_settimeout(sock, 1));
  mupnp_socket_setid(sock, pair[0]);
  char bytes[16];
  assert(mupnp_socket_read(sock, bytes, 0) == 0);
  double started = seconds();
  assert(mupnp_socket_read(sock, bytes, sizeof(bytes)) < 0);
  assert(errno == EAGAIN || errno == EWOULDBLOCK);
  assert(seconds() - started >= 0.8 && seconds() - started < 2.5);
  int flags = fcntl(pair[0], F_GETFL, 0);
  assert(flags >= 0 && !(flags & O_NONBLOCK));
  assert(fcntl(pair[0], F_SETFL, flags | O_NONBLOCK) == 0);
  started = seconds();
  assert(mupnp_socket_read(sock, bytes, 1) < 0);
  assert(errno == EAGAIN || errno == EWOULDBLOCK);
  assert(seconds() - started < 0.5);
  assert(fcntl(pair[0], F_GETFL, 0) == (flags | O_NONBLOCK));
  assert(fcntl(pair[0], F_SETFL, flags) == 0);
  struct timeval shortTimeout = { 0, 80000 };
  assert(setsockopt(pair[0], SOL_SOCKET, SO_RCVTIMEO, &shortTimeout, sizeof(shortTimeout)) == 0);
  started = seconds();
  assert(mupnp_socket_read(sock, bytes, 1) < 0);
  assert(errno == EAGAIN || errno == EWOULDBLOCK);
  assert(seconds() - started >= 0.04 && seconds() - started < 0.5);
  assert(mupnp_socket_settimeout(sock, 1));
  assert(write(pair[1], "abc", 3) == 3);
  assert(mupnp_socket_read(sock, bytes, sizeof(bytes)) == 3);
  assert(memcmp(bytes, "abc", 3) == 0);
  close(pair[1]);
  ssize_t eof = mupnp_socket_read(sock, bytes, sizeof(bytes));
  if (eof != 0)
    fprintf(stderr, "EOF read returned %zd, errno=%d (%s)\n", eof, errno, strerror(errno));
  assert(eof == 0);
  mupnp_socket_delete(sock);
  int port;
  int reserved = listener(&port);
  close(reserved);
  sock = mupnp_socket_stream_new();
  assert(sock && mupnp_socket_settimeout(sock, 1));
  assert(!mupnp_socket_connect(sock, "127.0.0.1", port));
  assert(errno == ECONNREFUSED);
  mupnp_socket_delete(sock);
  puts("PASS: native timeout/EAGAIN, caller O_NONBLOCK/subsecond timeout, zero length, partial read, EOF, refused connect");
}

static void high_descriptor(void)
{
  int pair[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
  int high = fcntl(pair[0], F_DUPFD, FD_SETSIZE + 1);
  close(pair[0]);
  if (high < 0) {
    close(pair[1]);
    puts("SKIP: descriptor-limit fixture exceeds this host's soft fd limit");
    return;
  }
  mUpnpSocket* sock = mupnp_socket_stream_new();
  assert(sock && mupnp_socket_settimeout(sock, 1));
  mupnp_socket_setid(sock, high);
  assert(write(pair[1], "x", 1) == 1);
  char byte;
  assert(mupnp_socket_read(sock, &byte, 1) == 1 && byte == 'x');
  mupnp_socket_delete(sock);
  close(pair[1]);
  puts("PASS: native descriptor above FD_SETSIZE");
}

typedef struct {
  SSL_CTX* tls;
  int fd;
  int port;
  bool stallHandshake;
  atomic_bool accepted;
  atomic_bool release;
} Server;
static void* serve(void* value)
{
  Server* server = value;
  int peer = accept(server->fd, NULL, NULL);
  assert(peer >= 0);
  atomic_store(&server->accepted, true);
  SSL* ssl = NULL;
  if (!server->stallHandshake) {
    ssl = SSL_new(server->tls);
    assert(ssl && SSL_set_fd(ssl, peer) == 1);
    if (SSL_accept(ssl) == 1) {
      assert(SSL_write(ssl, "abc", 3) == 3);
      while (!atomic_load(&server->release))
        usleep(1000);
      (void)SSL_shutdown(ssl);
    }
  }
  else {
    while (!atomic_load(&server->release))
      usleep(1000);
  }
  SSL_free(ssl);
  close(peer);
  return NULL;
}
static pthread_t start_server(Server* server, const char* cert, const char* key, bool stall)
{
  memset(server, 0, sizeof(*server));
  server->stallHandshake = stall;
  server->tls = SSL_CTX_new(TLS_server_method());
  assert(server->tls);
  assert(SSL_CTX_use_certificate_file(server->tls, cert, SSL_FILETYPE_PEM) == 1);
  assert(SSL_CTX_use_PrivateKey_file(server->tls, key, SSL_FILETYPE_PEM) == 1);
  server->fd = listener(&server->port);
  pthread_t thread;
  assert(pthread_create(&thread, NULL, serve, server) == 0);
  return thread;
}
static void finish_server(Server* server, pthread_t thread)
{
  atomic_store(&server->release, true);
  assert(pthread_join(thread, NULL) == 0);
  close(server->fd);
  SSL_CTX_free(server->tls);
}

typedef struct {
  mUpnpSocket* socket;
  int port;
  int operation;
  atomic_bool entered;
  ssize_t result;
  int error;
} Client;
static void waiting_client(mUpnpThread* thread)
{
  Client* client = mupnp_thread_getuserdata(thread);
  char* bytes = calloc(1, 8 * 1024 * 1024);
  assert(bytes);
  atomic_store(&client->entered, true);
  if (client->operation == 0)
    client->result = mupnp_socket_connect(client->socket, "127.0.0.1", client->port) ? 1 : -1;
  else if (client->operation == 1)
    client->result = mupnp_socket_read(client->socket, bytes, 1);
  else
    client->result = mupnp_socket_write(client->socket, bytes, 8 * 1024 * 1024) == 0 ? -1 : 1;
  client->error = errno;
  free(bytes);
}
static void tls_case(const char* cert, const char* key, int operation)
{
  alarm(10);
  Server server;
  pthread_t serving = start_server(&server, cert, key, operation == 0);
  Client client = { .port = server.port, .operation = operation };
  client.socket = mupnp_socket_ssl_new();
  assert(client.socket && mupnp_socket_settimeout(client.socket, 3));
  if (operation != 0) {
    assert(mupnp_socket_connect(client.socket, "127.0.0.1", server.port));
    char bytes[3];
    assert(mupnp_socket_read(client.socket, bytes, sizeof(bytes)) == 3);
    assert(memcmp(bytes, "abc", 3) == 0);
    int small = 4096;
    assert(setsockopt(client.socket->id, SOL_SOCKET, SO_SNDBUF, &small, sizeof(small)) == 0);
  }
  mUpnpThread* thread = mupnp_thread_new();
  assert(thread);
  mupnp_thread_setuserdata(thread, &client);
  mupnp_thread_setaction(thread, waiting_client);
  assert(mupnp_thread_start(thread));
  while (!atomic_load(&client.entered) || !atomic_load(&server.accepted))
    usleep(1000);
  usleep(30000);
  double started = seconds();
  assert(mupnp_thread_stop(thread));
  assert(seconds() - started < 1.0);
  assert(client.result < 0 && client.error == EINTR);
  assert(mupnp_thread_stop(thread));
  assert(mupnp_thread_delete(thread));
  mupnp_socket_delete(client.socket);
  finish_server(&server, serving);
  printf("PASS: TLS %s cancellation without signal\n", operation == 0 ? "handshake" : operation == 1 ? "read"
                                                                                                     : "backpressured write");
  alarm(0);
}
static void tls_timeout(const char* cert, const char* key)
{
  alarm(5);
  Server server;
  pthread_t serving = start_server(&server, cert, key, true);
  mUpnpSocket* sock = mupnp_socket_ssl_new();
  assert(sock && mupnp_socket_settimeout(sock, 1));
  double started = seconds();
  assert(!mupnp_socket_connect(sock, "127.0.0.1", server.port));
  assert(errno == EAGAIN || errno == EWOULDBLOCK);
  assert(seconds() - started >= 0.8 && seconds() - started < 2.5);
  mupnp_socket_delete(sock);
  finish_server(&server, serving);
  alarm(0);
  puts("PASS: stalled TLS handshake preserves timeout");
}
static void tls_contract(const char* cert, const char* key)
{
  alarm(5);
  Server server;
  pthread_t serving = start_server(&server, cert, key, false);
  mUpnpSocket* sock = mupnp_socket_ssl_new();
  assert(sock && mupnp_socket_settimeout(sock, 1));
  assert(mupnp_socket_connect(sock, "127.0.0.1", server.port));
  char bytes[3];
  assert(mupnp_socket_read(sock, bytes, 0) == 0);
  assert(mupnp_socket_read(sock, bytes, 1) == 1);
  /* Remaining decrypted bytes need no new descriptor readiness. */
  assert(mupnp_socket_read(sock, bytes + 1, 2) == 2);
  assert(memcmp(bytes, "abc", 3) == 0);
  double started = seconds();
  assert(mupnp_socket_read(sock, bytes, 1) < 0);
  assert(errno == EAGAIN || errno == EWOULDBLOCK);
  assert(seconds() - started >= 0.8 && seconds() - started < 2.5);
  atomic_store(&server.release, true);
  assert(mupnp_socket_read(sock, bytes, 1) == 0);
  mupnp_socket_delete(sock);
  finish_server(&server, serving);
  alarm(0);
  puts("PASS: TLS buffered plaintext, read timeout and close-notify EOF");
}
static void tls_verify(const char* cert, const char* key)
{
  alarm(5);
  Server server;
  pthread_t serving = start_server(&server, cert, key, false);
  mUpnpSocket* sock = mupnp_socket_ssl_new();
  assert(sock && mupnp_socket_settimeout(sock, 2));
  assert(!mupnp_socket_connect(sock, "127.0.0.1", server.port));
  mupnp_socket_delete(sock);
  finish_server(&server, serving);
  alarm(0);
  puts("PASS: TLS rejects untrusted or mismatched peer");
}
int main(int argc, char** argv)
{
  assert(argc == 3 || argc == 4);
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = watchdog;
  sigemptyset(&action.sa_mask);
  assert(sigaction(SIGALRM, &action, NULL) == 0);
  if (argc == 4)
    tls_verify(argv[1], argv[2]);
  else {
    alarm(10);
    plain_contract();
    high_descriptor();
    alarm(0);
    tls_timeout(argv[1], argv[2]);
    tls_contract(argv[1], argv[2]);
    for (int i = 0; i < 3; i++)
      tls_case(argv[1], argv[2], i);
  }
  assert(atomic_load(&signalsSent) == 0);
  return 0;
}

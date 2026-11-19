/* Test executable overrides: static-library networking must fail immediately.
 * POSIX-only; no socket is ever created by these functions. */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static void forbidden(const char* operation)
{
  fprintf(stderr, "FAIL: renderer model attempted %s\n", operation);
  abort();
}

int socket(int domain, int type, int protocol)
{
  (void)domain;
  (void)type;
  (void)protocol;
  forbidden("socket");
  return -1;
}
int connect(int fd, const struct sockaddr* address, socklen_t length)
{
  (void)fd;
  (void)address;
  (void)length;
  forbidden("connect");
  return -1;
}
int bind(int fd, const struct sockaddr* address, socklen_t length)
{
  (void)fd;
  (void)address;
  (void)length;
  forbidden("bind");
  return -1;
}
int listen(int fd, int backlog)
{
  (void)fd;
  (void)backlog;
  forbidden("listen");
  return -1;
}
ssize_t send(int fd, const void* buffer, size_t length, int flags)
{
  (void)fd;
  (void)buffer;
  (void)length;
  (void)flags;
  forbidden("send");
  return -1;
}
ssize_t sendto(int fd, const void* buffer, size_t length, int flags, const struct sockaddr* address, socklen_t addressLength)
{
  (void)fd;
  (void)buffer;
  (void)length;
  (void)flags;
  (void)address;
  (void)addressLength;
  forbidden("sendto");
  return -1;
}
int getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** result)
{
  (void)node;
  (void)service;
  (void)hints;
  (void)result;
  forbidden("getaddrinfo");
  return EAI_FAIL;
}

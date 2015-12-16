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

#ifndef _MUPNP_NET_CSOCKET_H_
#define _MUPNP_NET_CSOCKET_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>

#if defined(MUPNP_USE_OPENSSL)
#include <openssl/ssl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_NET_SOCKET_NONE 0

#define MUPNP_NET_SOCKET_STREAM 0x01
#define MUPNP_NET_SOCKET_DGRAM 0x02

#define MUPNP_NET_SOCKET_CLIENT 1
#define MUPNP_NET_SOCKET_SERVER 2

#define MUPNP_NET_SOCKET_MAXHOST 32
#define MUPNP_NET_SOCKET_MAXSERV 32

#if defined(BTRON) || defined(TENGINE)
typedef W SOCKET;
#elif defined(ITRON)
typedef ER SOCKET;
#elif !defined(WIN32) && !defined(__CYGWIN__)
typedef int SOCKET;
#endif

#define MUPNP_SOCKET_LF '\n'

#define MUPNP_NET_SOCKET_DGRAM_RECV_BUFSIZE 512
#define MUPNP_NET_SOCKET_DGRAM_ANCILLARY_BUFSIZE 512
#define MUPNP_NET_SOCKET_MULTICAST_DEFAULT_TTL 4
#define MUPNP_NET_SOCKET_AUTO_IP_NET 0xa9fe0000
#define MUPNP_NET_SOCKET_AUTO_IP_MASK 0xffff0000

#if defined(ITRON)
#define MUPNP_NET_SOCKET_WINDOW_BUFSIZE 4096
#endif

/****************************************
* Define (SocketList)
****************************************/

#if defined(ITRON)
#define MUPNP_NET_USE_SOCKET_LIST 1
#endif

/****************************************
* Data Type
****************************************/

#if defined(MUPNP_NET_USE_SOCKET_LIST)
#include <mupnp/util/list.h>
#endif

typedef struct _mUpnpSocket {
#if defined(MUPNP_NET_USE_SOCKET_LIST)
  MUPNP_LIST_STRUCT_MEMBERS
#endif
  SOCKET id;
  int type;
  int direction;
  mUpnpString* ipaddr;
  int port;
#if defined(ITRON)
  UH* sendWinBuf;
  UH* recvWinBuf;
#endif
#if defined(MUPNP_USE_OPENSSL)
  SSL_CTX* ctx;
  SSL* ssl;
#endif
} mUpnpSocket, mUpnpSocketList;

typedef struct _mUpnpDatagramPacket {
  mUpnpString* data;
  mUpnpString* localAddress;
  int localPort;
  mUpnpString* remoteAddress;
  int remotePort;
} mUpnpDatagramPacket;

/****************************************
* Function (Socket)
****************************************/

void mupnp_socket_startup();
void mupnp_socket_cleanup();

mUpnpSocket* mupnp_socket_new(int type);
#define mupnp_socket_stream_new() mupnp_socket_new(MUPNP_NET_SOCKET_STREAM)
#define mupnp_socket_dgram_new() mupnp_socket_new(MUPNP_NET_SOCKET_DGRAM)
bool mupnp_socket_delete(mUpnpSocket* socket);

void mupnp_socket_setid(mUpnpSocket* socket, SOCKET value);
#define mupnp_socket_getid(socket) (socket->id)

#define mupnp_socket_settype(socket, value) (socket->type = value)
#define mupnp_socket_gettype(socket) (socket->type)
#define mupnp_socket_issocketstream(socket) ((socket->type & MUPNP_NET_SOCKET_STREAM) ? true : false)
#define mupnp_socket_isdatagramstream(socket) ((socket->type & MUPNP_NET_SOCKET_DGRAM) ? true : false)

#define mupnp_socket_setdirection(socket, value) (socket->direction = value)
#define mupnp_socket_getdirection(socket) (socket->direction)
#define mupnp_socket_isclient(socket) ((socket->direction == MUPNP_NET_SOCKET_CLIENT) ? true : false)
#define mupnp_socket_isserver(socket) ((socket->direction == MUPNP_NET_SOCKET_SERVER) ? true : false)

#define mupnp_socket_setaddress(socket, value) mupnp_string_setvalue(socket->ipaddr, value)
#define mupnp_socket_setport(socket, value) (socket->port = value)
#define mupnp_socket_getaddress(socket) mupnp_string_getvalue(socket->ipaddr)
#define mupnp_socket_getport(socket) (socket->port)

bool mupnp_socket_isbound(mUpnpSocket* socket);
bool mupnp_socket_close(mUpnpSocket* socket);

bool mupnp_socket_listen(mUpnpSocket* socket);

bool mupnp_socket_bind(mUpnpSocket* sock, int bindPort, const char* bindAddr, bool bindFlag, bool reuseFlag);
bool mupnp_socket_accept(mUpnpSocket* sock, mUpnpSocket* clientSock);
bool mupnp_socket_connect(mUpnpSocket* sock, const char* addr, int port);
ssize_t mupnp_socket_read(mUpnpSocket* sock, char* buffer, size_t bufferLen);
size_t mupnp_socket_write(mUpnpSocket* sock, const char* buffer, size_t bufferLen);
ssize_t mupnp_socket_readline(mUpnpSocket* sock, char* buffer, size_t bufferLen);
size_t mupnp_socket_skip(mUpnpSocket* sock, size_t skipLen);

size_t mupnp_socket_sendto(mUpnpSocket* sock, const char* addr, int port, const char* data, size_t dataeLen);
ssize_t mupnp_socket_recv(mUpnpSocket* sock, mUpnpDatagramPacket* dgmPkt);

int mupnp_socket_getlasterror();

/****************************************
* Function (Multicast)
****************************************/

bool mupnp_socket_joingroup(mUpnpSocket* sock, const char* mcastAddr, const char* ifAddr);

/****************************************
* Function (Option)
****************************************/

bool mupnp_socket_setreuseaddress(mUpnpSocket* socket, bool flag);
bool mupnp_socket_setmulticastttl(mUpnpSocket* sock, int ttl);
bool mupnp_socket_settimeout(mUpnpSocket* sock, int sec);

/****************************************
* Function (DatagramPacket)
****************************************/

mUpnpDatagramPacket* mupnp_socket_datagram_packet_new();
void mupnp_socket_datagram_packet_delete(mUpnpDatagramPacket* dgmPkt);

#define mupnp_socket_datagram_packet_setdata(dgmPkt, value) mupnp_string_setvalue(dgmPkt->data, value)
#define mupnp_socket_datagram_packet_getdata(dgmPkt) mupnp_string_getvalue(dgmPkt->data)

#define mupnp_socket_datagram_packet_setlocaladdress(dgmPkt, addr) mupnp_string_setvalue(dgmPkt->localAddress, addr)
#define mupnp_socket_datagram_packet_getlocaladdress(dgmPkt) mupnp_string_getvalue(dgmPkt->localAddress)
#define mupnp_socket_datagram_packet_setlocalport(dgmPkt, port) (dgmPkt->localPort = port)
#define mupnp_socket_datagram_packet_getlocalport(dgmPkt) (dgmPkt->localPort)
#define mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, addr) mupnp_string_setvalue(dgmPkt->remoteAddress, addr)
#define mupnp_socket_datagram_packet_getremoteaddress(dgmPkt) mupnp_string_getvalue(dgmPkt->remoteAddress)
#define mupnp_socket_datagram_packet_setremoteport(dgmPkt, port) (dgmPkt->remotePort = port)
#define mupnp_socket_datagram_packet_getremoteport(dgmPkt) (dgmPkt->remotePort)

void mupnp_socket_datagram_packet_copy(mUpnpDatagramPacket* dstDgmPkt, mUpnpDatagramPacket* srcDgmPkt);

/****************************************
* Function (SSLSocket)
****************************************/

#if defined(MUPNP_USE_OPENSSL)
#define MUPNP_NET_SOCKET_SSL 0x0100
#define mupnp_socket_ssl_new() mupnp_socket_new(MUPNP_NET_SOCKET_STREAM | MUPNP_NET_SOCKET_SSL)
#define mupnp_socket_isssl(socket) ((socket->type & MUPNP_NET_SOCKET_SSL) ? true : false)
#endif

/****************************************
* Function (SocketList)
****************************************/

#if defined(MUPNP_NET_USE_SOCKET_LIST)

#define mupnp_socket_next(sock) (mUpnpSocket*) mupnp_list_next((mUpnpList*)sock)

mUpnpSocketList* mupnp_socketlist_new();
void mupnp_socketlist_delete(mUpnpSocketList* sockList);

#define mupnp_socketlist_clear(sockList) mupnp_list_clear((mUpnpList*)sockList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_socket_delete)
#define mupnp_socketlist_size(sockList) mupnp_list_size((mUpnpList*)sockList)
#define mupnp_socketlist_gets(sockList) (mUpnpSocket*) mupnp_list_next((mUpnpList*)sockList)
#define mupnp_socketlist_add(sockList, sock) mupnp_list_add((mUpnpList*)sockList, (mUpnpList*)sock)

#endif

#ifdef __cplusplus
}
#endif

#endif

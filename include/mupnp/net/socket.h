/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csocket.h
*
*	Revision:
*
*	01/17/05
*		- first revision
*	04/03/06 Theo Beisch
*		- added WSAGetLastError support
*		- changed socket_cleanup to return
*		  value to enable final Winsock close 
******************************************************************/

#ifndef _CG_NET_CSOCKET_H_
#define _CG_NET_CSOCKET_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>

#if defined(CG_USE_OPENSSL)
#include <openssl/ssl.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_NET_SOCKET_NONE 0

#define CG_NET_SOCKET_STREAM 0x01
#define CG_NET_SOCKET_DGRAM 0x02

#define CG_NET_SOCKET_CLIENT 1
#define CG_NET_SOCKET_SERVER 2

#define CG_NET_SOCKET_MAXHOST 32
#define CG_NET_SOCKET_MAXSERV 32

#if defined(BTRON) || defined(TENGINE)
typedef W SOCKET;
#elif defined(ITRON)
typedef ER SOCKET;
#elif !defined(WIN32) && !defined(__CYGWIN__)
typedef int SOCKET;
#endif

#define CG_SOCKET_LF '\n'

#define CG_NET_SOCKET_DGRAM_RECV_BUFSIZE 512
#define CG_NET_SOCKET_DGRAM_ANCILLARY_BUFSIZE 512
#define CG_NET_SOCKET_MULTICAST_DEFAULT_TTL 4
#define CG_NET_SOCKET_AUTO_IP_NET 0xa9fe0000
#define CG_NET_SOCKET_AUTO_IP_MASK 0xffff0000 

#if defined(ITRON)
#define CG_NET_SOCKET_WINDOW_BUFSIZE 4096
#endif

/****************************************
* Define (SocketList)
****************************************/

#if defined(ITRON)
#define CG_NET_USE_SOCKET_LIST 1
#endif

/****************************************
* Data Type
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)
#include <mupnp/util/list.h>
#endif

typedef struct _CgSocket {
#if defined(CG_NET_USE_SOCKET_LIST)
	BOOL headFlag;
	struct _CgSocket *prev;
	struct _CgSocket *next;
#endif
	SOCKET id;
	int type;
	int direction;
	CgString *ipaddr;
	int port;
#if defined(ITRON)
	UH *sendWinBuf;
	UH *recvWinBuf;
#endif
#if defined(CG_USE_OPENSSL)
	SSL_CTX* ctx;
	SSL* ssl;
#endif
} CgSocket, CgSocketList;

typedef struct _CgDatagramPacket {
	CgString *data;
	CgString *localAddress;
	int localPort;
	CgString *remoteAddress;
	int remotePort;
} CgDatagramPacket;

/****************************************
* Function (Socket)
****************************************/

void mupnp_socket_startup();
void mupnp_socket_cleanup();

CgSocket *mupnp_socket_new(int type);
#define mupnp_socket_stream_new() mupnp_socket_new(CG_NET_SOCKET_STREAM)
#define mupnp_socket_dgram_new() mupnp_socket_new(CG_NET_SOCKET_DGRAM)
BOOL mupnp_socket_delete(CgSocket *socket);

void mupnp_socket_setid(CgSocket *socket, SOCKET value);
#define mupnp_socket_getid(socket) (socket->id)

#define mupnp_socket_settype(socket, value) (socket->type = value)
#define mupnp_socket_gettype(socket) (socket->type)
#define mupnp_socket_issocketstream(socket) ((socket->type & CG_NET_SOCKET_STREAM) ? TRUE : FALSE)
#define mupnp_socket_isdatagramstream(socket) ((socket->type & CG_NET_SOCKET_DGRAM) ? TRUE : FALSE)

#define mupnp_socket_setdirection(socket, value) (socket->direction = value)
#define mupnp_socket_getdirection(socket) (socket->direction)
#define mupnp_socket_isclient(socket) ((socket->direction == CG_NET_SOCKET_CLIENT) ? TRUE : FALSE)
#define mupnp_socket_isserver(socket) ((socket->direction == CG_NET_SOCKET_SERVER) ? TRUE : FALSE)

#define mupnp_socket_setaddress(socket, value) mupnp_string_setvalue(socket->ipaddr, value)
#define mupnp_socket_setport(socket, value) (socket->port = value)
#define mupnp_socket_getaddress(socket) mupnp_string_getvalue(socket->ipaddr)
#define mupnp_socket_getport(socket) (socket->port)

BOOL mupnp_socket_isbound(CgSocket *socket);
BOOL mupnp_socket_close(CgSocket *socket);

BOOL mupnp_socket_listen(CgSocket *socket);

BOOL mupnp_socket_bind(CgSocket *sock, int bindPort, const char *bindAddr, BOOL bindFlag, BOOL reuseFlag);
BOOL mupnp_socket_accept(CgSocket *sock, CgSocket *clientSock);
BOOL mupnp_socket_connect(CgSocket *sock, const char *addr, int port);
ssize_t mupnp_socket_read(CgSocket *sock, char *buffer, size_t bufferLen);
size_t mupnp_socket_write(CgSocket *sock, const char *buffer, size_t bufferLen);
ssize_t mupnp_socket_readline(CgSocket *sock, char *buffer, size_t bufferLen);
size_t mupnp_socket_skip(CgSocket *sock, size_t skipLen);

size_t mupnp_socket_sendto(CgSocket *sock, const char *addr, int port, const char *data, size_t dataeLen);
ssize_t mupnp_socket_recv(CgSocket *sock, CgDatagramPacket *dgmPkt);

int mupnp_socket_getlasterror();

/****************************************
* Function (Multicast)
****************************************/

BOOL mupnp_socket_joingroup(CgSocket *sock, const char *mcastAddr, const char *ifAddr);

/****************************************
* Function (Option)
****************************************/

BOOL mupnp_socket_setreuseaddress(CgSocket *socket, BOOL flag);
BOOL mupnp_socket_setmulticastttl(CgSocket *sock,  int ttl);
BOOL mupnp_socket_settimeout(CgSocket *sock, int sec);

/****************************************
* Function (DatagramPacket)
****************************************/

CgDatagramPacket *mupnp_socket_datagram_packet_new();
void mupnp_socket_datagram_packet_delete(CgDatagramPacket *dgmPkt);

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

void mupnp_socket_datagram_packet_copy(CgDatagramPacket *dstDgmPkt, CgDatagramPacket *srcDgmPkt);

/****************************************
* Function (SSLSocket)
****************************************/

#if defined(CG_USE_OPENSSL)
#define CG_NET_SOCKET_SSL 0x0100
#define mupnp_socket_ssl_new() mupnp_socket_new(CG_NET_SOCKET_STREAM | CG_NET_SOCKET_SSL)
#define mupnp_socket_isssl(socket) ((socket->type & CG_NET_SOCKET_SSL) ? TRUE : FALSE)
#endif

/****************************************
* Function (SocketList)
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

#define mupnp_socket_next(sock) (CgSocket *)mupnp_list_next((CgList *)sock)

CgSocketList *mupnp_socketlist_new();
void mupnp_socketlist_delete(CgSocketList *sockList);

#define mupnp_socketlist_clear(sockList) mupnp_list_clear((CgList *)sockList, (CG_LIST_DESTRUCTORFUNC)mupnp_socket_delete)
#define mupnp_socketlist_size(sockList) mupnp_list_size((CgList *)sockList)
#define mupnp_socketlist_gets(sockList) (CgSocket *)mupnp_list_next((CgList *)sockList)
#define mupnp_socketlist_add(sockList, sock) mupnp_list_add((CgList *)sockList, (CgList *)sock)

#endif

#ifdef  __cplusplus
}
#endif

#endif

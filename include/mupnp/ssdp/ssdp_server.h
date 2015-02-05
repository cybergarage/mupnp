/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cssdp_server.h
*
*	Revision:
*
*	05/26/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CSSDP_SERVER_H_
#define _CG_UPNP_CSSDP_SERVER_H_

#include <mupnp/typedef.h>

#include <mupnp/ssdp/ssdp.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef mUpnpSocket mUpnpUpnpHttpMuSocket;
typedef mUpnpSocket mUpnpUpnpHttpUSocket;

typedef struct _mUpnpUpnpSSDPPacket {
	mUpnpDatagramPacket *dgmPkt;
	mUpnpHttpHeaderList *headerList;
	void *userData;
	mUpnpTime timeStamp;

	/* These are used when filtering out duplicate
	 * M-SEARCHes
	 */
	mUpnpTime *timestamps;
        int initialized;
} mUpnpUpnpSSDPPacket;

typedef void (*CG_UPNP_SSDP_LISTNER)(mUpnpUpnpSSDPPacket *);

typedef struct _mUpnpUpnpSSDPServer {
	BOOL headFlag;
	struct _mUpnpUpnpSSDPServer *prev;
	struct _mUpnpUpnpSSDPServer *next;
	mUpnpUpnpHttpMuSocket *httpmuSock;
	mUpnpThread *recvThread;
	CG_UPNP_SSDP_LISTNER listener;
	void *userData;
} mUpnpUpnpSSDPServer, mUpnpUpnpSSDPServerList;

typedef void (*CG_UPNP_SSDP_RESPONSE_LISTNER)(mUpnpUpnpSSDPPacket *);

typedef struct _mUpnpUpnpSSDPResponseServer {
	BOOL headFlag;
	struct _mUpnpUpnpSSDPServer *prev;
	struct _mUpnpUpnpSSDPServer *next;
	mUpnpUpnpHttpUSocket *httpuSock;
	mUpnpThread *recvThread;
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;
	void *userData;
} mUpnpUpnpSSDPResponseServer, mUpnpUpnpSSDPResponseServerList;

/****************************************
* Function (HTTPMU)
****************************************/

#define mupnp_upnp_httpmu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpmu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_upnp_httpmu_socket_close(sock) mupnp_socket_close(sock)

BOOL mupnp_upnp_httpmu_socket_bind(mUpnpUpnpHttpMuSocket *sock, const char *mcastAddr, int port, const char *bindAddr);
ssize_t mupnp_upnp_httpmu_socket_recv(mUpnpUpnpHttpMuSocket *sock, mUpnpUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (HTTPU)
****************************************/

#define mupnp_upnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_upnp_httpu_socket_close(sock) mupnp_socket_close(sock)
#define mupnp_upnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpu_socket_bind(sock, bindPort, bindAddr) mupnp_socket_bind(sock, bindPort, bindAddr, FALSE, FALSE)
ssize_t mupnp_upnp_httpu_socket_recv(mUpnpUpnpHttpUSocket *sock, mUpnpUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (SSDPPacket)
****************************************/

mUpnpUpnpSSDPPacket *mupnp_upnp_ssdp_packet_new();
void mupnp_upnp_ssdp_packet_delete(mUpnpUpnpSSDPPacket *ssdpPkt);
void mupnp_upnp_ssdp_packet_clear(mUpnpUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_setdatagrampacket(ssdpPkt, value) (ssdpPkt->dgmPkt = value)
#define mupnp_upnp_ssdp_packet_getdatagrampacket(ssdpPkt) (ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_setdata(ssdpPkt, value) mupnp_string_setvalue(ssdpPkt->dgmPkt->data, value)
#define mupnp_upnp_ssdp_packet_getdata(ssdpPkt) mupnp_string_getvalue(ssdpPkt->dgmPkt->data)
#define mupnp_upnp_ssdp_packet_setuserdata(ssdpPkt, data) (ssdpPkt->userData = data)
#define mupnp_upnp_ssdp_packet_getuserdata(ssdpPkt) (ssdpPkt->userData)
#define mupnp_upnp_ssdp_packet_settimestamp(ssdpPkt, value) (ssdpPkt->timeStamp = value)
#define mupnp_upnp_ssdp_packet_gettimestamp(ssdpPkt) (ssdpPkt->timeStamp)
#define mupnp_upnp_ssdp_packet_getexpiration(ssdpPkt) (mupnp_upnp_ssdp_packet_getmaxage(ssdpPkt) == 0 ? 0 : mupnp_upnp_ssdp_packet_gettimestamp(ssdpPkt) + mupnp_upnp_ssdp_packet_getmaxage(ssdpPkt))

void mupnp_upnp_ssdp_packet_setheader(mUpnpUpnpSSDPPacket *ssdpPkt, char *ssdpMsg);
#define mupnp_upnp_ssdp_packet_addheader(ssdpPkt, header) mupnp_http_headerlist_add(ssdpPkt->headerList, header)
#define mupnp_upnp_ssdp_packet_getheaders(ssdpPkt) mupnp_http_headerlist_gets(ssdpPkt->headerList)

#define mupnp_upnp_ssdp_packet_gethost(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_HOST)
#define mupnp_upnp_ssdp_packet_getserver(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_SERVER)
#define mupnp_upnp_ssdp_packet_getcachecontrol(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_CACHE_CONTROL)
#define mupnp_upnp_ssdp_packet_getdate(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_DATE)
#define mupnp_upnp_ssdp_packet_getext(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_EXT)
#define mupnp_upnp_ssdp_packet_getlocation(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_LOCATION)
#define mupnp_upnp_ssdp_packet_getst(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_ST)
#define mupnp_upnp_ssdp_packet_getusn(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_USN)
#define mupnp_upnp_ssdp_packet_getmx(ssdpPkt) mupnp_str2int(mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_MX))
#define mupnp_upnp_ssdp_packet_getman(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_MAN)
#define mupnp_upnp_ssdp_packet_getnt(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_NT)
#define mupnp_upnp_ssdp_packet_getnts(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, CG_HTTP_NTS)
long mupnp_upnp_ssdp_packet_getmaxage(mUpnpUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_isdiscover(ssdpPkt) mupnp_upnp_man_isdiscover(mupnp_upnp_ssdp_packet_getman(ssdpPkt))
#define mupnp_upnp_ssdp_packet_isalive(ssdpPkt) mupnp_upnp_nts_isalive(mupnp_upnp_ssdp_packet_getnts(ssdpPkt))
#define mupnp_upnp_ssdp_packet_isbyebye(ssdpPkt) mupnp_upnp_nts_isbyebye(mupnp_upnp_ssdp_packet_getnts(ssdpPkt))

BOOL mupnp_upnp_ssdp_packet_isrootdevice(mUpnpUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_getlocaladdress(ssdpPkt) mupnp_socket_datagram_packet_getlocaladdress(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getlocalport(ssdpPkt) mupnp_socket_datagram_packet_getlocalport(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getremoteaddress(ssdpPkt) mupnp_socket_datagram_packet_getremoteaddress(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getremoteport(ssdpPkt) mupnp_socket_datagram_packet_getremoteport(ssdpPkt->dgmPkt)

void mupnp_upnp_ssdp_packet_copy(mUpnpUpnpSSDPPacket *destSsdpPkt, mUpnpUpnpSSDPPacket *srcSsdpPkt);
void mupnp_upnp_ssdp_packet_print(mUpnpUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (SSDPServer)
****************************************/

mUpnpUpnpSSDPServer *mupnp_upnp_ssdp_server_new();
void mupnp_upnp_ssdp_server_delete(mUpnpUpnpSSDPServer *server);

#define mupnp_upnp_ssdp_server_next(server) (mUpnpUpnpSSDPServer *)mupnp_list_next((mUpnpList *)server)

BOOL mupnp_upnp_ssdp_server_open(mUpnpUpnpSSDPServer *server, char *bindAddr);
BOOL mupnp_upnp_ssdp_server_close(mUpnpUpnpSSDPServer *server);

#define mupnp_upnp_ssdp_server_isopened(server) ((server->httpmuSock != NULL) ? TRUE : FALSE)
#define mupnp_upnp_ssdp_server_getsocket(server) (server->httpmuSock)

#define mupnp_upnp_ssdp_server_setlistener(server, func) (server->listener = func)
#define mupnp_upnp_ssdp_server_getlistener(server) (server->listener)
void mupnp_upnp_ssdp_server_performlistener(mUpnpUpnpSSDPServer *server, mUpnpUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_server_setuserdata(server, data) (server->userData = data)
#define mupnp_upnp_ssdp_server_getuserdata(server) (server->userData)

BOOL mupnp_upnp_ssdp_server_start(mUpnpUpnpSSDPServer *server);
BOOL mupnp_upnp_ssdp_server_stop(mUpnpUpnpSSDPServer *server);

/****************************************
* Function (SSDPServerList)
****************************************/

mUpnpUpnpSSDPServerList *mupnp_upnp_ssdp_serverlist_new();
void mupnp_upnp_ssdp_serverlist_delete(mUpnpUpnpSSDPServerList *serverList);

#define mupnp_upnp_ssdp_serverlist_clear(serverList) mupnp_list_clear((mUpnpList *)serverList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_ssdp_server_delete)
#define mupnp_upnp_ssdp_serverlist_size(serverList) mupnp_list_size((mUpnpList *)serverList)
#define mupnp_upnp_ssdp_serverlist_gets(serverList) (mUpnpUpnpSSDPServer *)mupnp_list_next((mUpnpList *)serverList)
#define mupnp_upnp_ssdp_serverlist_add(serverList, server) mupnp_list_add((mUpnpList *)serverList, (mUpnpList *)server)

BOOL mupnp_upnp_ssdp_serverlist_open(mUpnpUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_close(mUpnpUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_start(mUpnpUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_stop(mUpnpUpnpSSDPServerList *ssdpServerList);
void mupnp_upnp_ssdp_serverlist_setlistener(mUpnpUpnpSSDPServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener);
void mupnp_upnp_ssdp_serverlist_setuserdata(mUpnpUpnpSSDPServerList *ssdpServerList, void *data);

/****************************************
* Function (SSDPResponseServer)
****************************************/

mUpnpUpnpSSDPResponseServer *mupnp_upnp_ssdpresponse_server_new();
void mupnp_upnp_ssdpresponse_server_delete(mUpnpUpnpSSDPResponseServer *server);

#define mupnp_upnp_ssdpresponse_server_next(server) (mUpnpUpnpSSDPResponseServer *)mupnp_list_next((mUpnpList *)server)

BOOL mupnp_upnp_ssdpresponse_server_open(mUpnpUpnpSSDPResponseServer *server, int bindPort, char *bindAddr);
BOOL mupnp_upnp_ssdpresponse_server_close(mUpnpUpnpSSDPResponseServer *server);

#define mupnp_upnp_ssdpresponse_server_isopened(server) ((server->httpuSock != NULL) ? TRUE : FALSE)
#define mupnp_upnp_ssdpresponse_server_getsocket(server) (server->httpuSock)

#define mupnp_upnp_ssdpresponse_server_setlistener(server, func) (server->listener = func)
#define mupnp_upnp_ssdpresponse_server_getlistener(server) (server->listener)
void mupnp_upnp_ssdpresponse_server_performlistener(mUpnpUpnpSSDPResponseServer *server, mUpnpUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdpresponse_server_setuserdata(server, data) (server->userData = data)
#define mupnp_upnp_ssdpresponse_server_getuserdata(server) (server->userData)

BOOL mupnp_upnp_ssdpresponse_server_start(mUpnpUpnpSSDPResponseServer *server);
BOOL mupnp_upnp_ssdpresponse_server_stop(mUpnpUpnpSSDPResponseServer *server);

BOOL mupnp_upnp_ssdpresponse_server_post(mUpnpUpnpSSDPResponseServer *server, mUpnpUpnpSSDPRequest *ssdpReq);

/****************************************
* Function (SSDPResponseServerList)
****************************************/

mUpnpUpnpSSDPResponseServerList *mupnp_upnp_ssdpresponse_serverlist_new();
void mupnp_upnp_ssdpresponse_serverlist_delete(mUpnpUpnpSSDPResponseServerList *serverList);

#define mupnp_upnp_ssdpresponse_serverlist_clear(serverList) mupnp_list_clear((mUpnpList *)serverList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_ssdpresponse_server_delete)
#define mupnp_upnp_ssdpresponse_serverlist_size(serverList) mupnp_list_size((mUpnpList *)serverList)
#define mupnp_upnp_ssdpresponse_serverlist_gets(serverList) (mUpnpUpnpSSDPResponseServer *)mupnp_list_next((mUpnpList *)serverList)
#define mupnp_upnp_ssdpresponse_serverlist_add(serverList, server) mupnp_list_add((mUpnpList *)serverList, (mUpnpList *)server)

BOOL mupnp_upnp_ssdpresponse_serverlist_open(mUpnpUpnpSSDPResponseServerList *ssdpServerList, int bindPort);
BOOL mupnp_upnp_ssdpresponse_serverlist_close(mUpnpUpnpSSDPResponseServerList *ssdpServerList);
BOOL mupnp_upnp_ssdpresponse_serverlist_start(mUpnpUpnpSSDPResponseServerList *ssdpServerList);
BOOL mupnp_upnp_ssdpresponse_serverlist_stop(mUpnpUpnpSSDPResponseServerList *ssdpServerList);
void mupnp_upnp_ssdpresponse_serverlist_setlistener(mUpnpUpnpSSDPResponseServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener);
void mupnp_upnp_ssdpresponse_serverlist_setuserdata(mUpnpUpnpSSDPResponseServerList *ssdpServerList, void *data);
BOOL mupnp_upnp_ssdpresponse_serverlist_post(mUpnpUpnpSSDPResponseServerList *ssdpServerList, mUpnpUpnpSSDPRequest *ssdpReq);

#ifdef  __cplusplus
}
#endif

#endif

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

typedef CgSocket CgUpnpHttpMuSocket;
typedef CgSocket CgUpnpHttpUSocket;

typedef struct _CgUpnpSSDPPacket {
	CgDatagramPacket *dgmPkt;
	CgHttpHeaderList *headerList;
	void *userData;
	CgTime timeStamp;

	/* These are used when filtering out duplicate
	 * M-SEARCHes
	 */
	CgTime *timestamps;
        int initialized;
} CgUpnpSSDPPacket;

typedef void (*CG_UPNP_SSDP_LISTNER)(CgUpnpSSDPPacket *);

typedef struct _CgUpnpSSDPServer {
	BOOL headFlag;
	struct _CgUpnpSSDPServer *prev;
	struct _CgUpnpSSDPServer *next;
	CgUpnpHttpMuSocket *httpmuSock;
	CgThread *recvThread;
	CG_UPNP_SSDP_LISTNER listener;
	void *userData;
} CgUpnpSSDPServer, CgUpnpSSDPServerList;

typedef void (*CG_UPNP_SSDP_RESPONSE_LISTNER)(CgUpnpSSDPPacket *);

typedef struct _CgUpnpSSDPResponseServer {
	BOOL headFlag;
	struct _CgUpnpSSDPServer *prev;
	struct _CgUpnpSSDPServer *next;
	CgUpnpHttpUSocket *httpuSock;
	CgThread *recvThread;
	CG_UPNP_SSDP_RESPONSE_LISTNER listener;
	void *userData;
} CgUpnpSSDPResponseServer, CgUpnpSSDPResponseServerList;

/****************************************
* Function (HTTPMU)
****************************************/

#define mupnp_upnp_httpmu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpmu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_upnp_httpmu_socket_close(sock) mupnp_socket_close(sock)

BOOL mupnp_upnp_httpmu_socket_bind(CgUpnpHttpMuSocket *sock, const char *mcastAddr, int port, const char *bindAddr);
ssize_t mupnp_upnp_httpmu_socket_recv(CgUpnpHttpMuSocket *sock, CgUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (HTTPU)
****************************************/

#define mupnp_upnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_upnp_httpu_socket_close(sock) mupnp_socket_close(sock)
#define mupnp_upnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_upnp_httpu_socket_bind(sock, bindPort, bindAddr) mupnp_socket_bind(sock, bindPort, bindAddr, FALSE, FALSE)
ssize_t mupnp_upnp_httpu_socket_recv(CgUpnpHttpUSocket *sock, CgUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (SSDPPacket)
****************************************/

CgUpnpSSDPPacket *mupnp_upnp_ssdp_packet_new();
void mupnp_upnp_ssdp_packet_delete(CgUpnpSSDPPacket *ssdpPkt);
void mupnp_upnp_ssdp_packet_clear(CgUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_setdatagrampacket(ssdpPkt, value) (ssdpPkt->dgmPkt = value)
#define mupnp_upnp_ssdp_packet_getdatagrampacket(ssdpPkt) (ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_setdata(ssdpPkt, value) mupnp_string_setvalue(ssdpPkt->dgmPkt->data, value)
#define mupnp_upnp_ssdp_packet_getdata(ssdpPkt) mupnp_string_getvalue(ssdpPkt->dgmPkt->data)
#define mupnp_upnp_ssdp_packet_setuserdata(ssdpPkt, data) (ssdpPkt->userData = data)
#define mupnp_upnp_ssdp_packet_getuserdata(ssdpPkt) (ssdpPkt->userData)
#define mupnp_upnp_ssdp_packet_settimestamp(ssdpPkt, value) (ssdpPkt->timeStamp = value)
#define mupnp_upnp_ssdp_packet_gettimestamp(ssdpPkt) (ssdpPkt->timeStamp)
#define mupnp_upnp_ssdp_packet_getexpiration(ssdpPkt) (mupnp_upnp_ssdp_packet_getmaxage(ssdpPkt) == 0 ? 0 : mupnp_upnp_ssdp_packet_gettimestamp(ssdpPkt) + mupnp_upnp_ssdp_packet_getmaxage(ssdpPkt))

void mupnp_upnp_ssdp_packet_setheader(CgUpnpSSDPPacket *ssdpPkt, char *ssdpMsg);
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
long mupnp_upnp_ssdp_packet_getmaxage(CgUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_isdiscover(ssdpPkt) mupnp_upnp_man_isdiscover(mupnp_upnp_ssdp_packet_getman(ssdpPkt))
#define mupnp_upnp_ssdp_packet_isalive(ssdpPkt) mupnp_upnp_nts_isalive(mupnp_upnp_ssdp_packet_getnts(ssdpPkt))
#define mupnp_upnp_ssdp_packet_isbyebye(ssdpPkt) mupnp_upnp_nts_isbyebye(mupnp_upnp_ssdp_packet_getnts(ssdpPkt))

BOOL mupnp_upnp_ssdp_packet_isrootdevice(CgUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_packet_getlocaladdress(ssdpPkt) mupnp_socket_datagram_packet_getlocaladdress(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getlocalport(ssdpPkt) mupnp_socket_datagram_packet_getlocalport(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getremoteaddress(ssdpPkt) mupnp_socket_datagram_packet_getremoteaddress(ssdpPkt->dgmPkt)
#define mupnp_upnp_ssdp_packet_getremoteport(ssdpPkt) mupnp_socket_datagram_packet_getremoteport(ssdpPkt->dgmPkt)

void mupnp_upnp_ssdp_packet_copy(CgUpnpSSDPPacket *destSsdpPkt, CgUpnpSSDPPacket *srcSsdpPkt);
void mupnp_upnp_ssdp_packet_print(CgUpnpSSDPPacket *ssdpPkt);

/****************************************
* Function (SSDPServer)
****************************************/

CgUpnpSSDPServer *mupnp_upnp_ssdp_server_new();
void mupnp_upnp_ssdp_server_delete(CgUpnpSSDPServer *server);

#define mupnp_upnp_ssdp_server_next(server) (CgUpnpSSDPServer *)mupnp_list_next((CgList *)server)

BOOL mupnp_upnp_ssdp_server_open(CgUpnpSSDPServer *server, char *bindAddr);
BOOL mupnp_upnp_ssdp_server_close(CgUpnpSSDPServer *server);

#define mupnp_upnp_ssdp_server_isopened(server) ((server->httpmuSock != NULL) ? TRUE : FALSE)
#define mupnp_upnp_ssdp_server_getsocket(server) (server->httpmuSock)

#define mupnp_upnp_ssdp_server_setlistener(server, func) (server->listener = func)
#define mupnp_upnp_ssdp_server_getlistener(server) (server->listener)
void mupnp_upnp_ssdp_server_performlistener(CgUpnpSSDPServer *server, CgUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdp_server_setuserdata(server, data) (server->userData = data)
#define mupnp_upnp_ssdp_server_getuserdata(server) (server->userData)

BOOL mupnp_upnp_ssdp_server_start(CgUpnpSSDPServer *server);
BOOL mupnp_upnp_ssdp_server_stop(CgUpnpSSDPServer *server);

/****************************************
* Function (SSDPServerList)
****************************************/

CgUpnpSSDPServerList *mupnp_upnp_ssdp_serverlist_new();
void mupnp_upnp_ssdp_serverlist_delete(CgUpnpSSDPServerList *serverList);

#define mupnp_upnp_ssdp_serverlist_clear(serverList) mupnp_list_clear((CgList *)serverList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_ssdp_server_delete)
#define mupnp_upnp_ssdp_serverlist_size(serverList) mupnp_list_size((CgList *)serverList)
#define mupnp_upnp_ssdp_serverlist_gets(serverList) (CgUpnpSSDPServer *)mupnp_list_next((CgList *)serverList)
#define mupnp_upnp_ssdp_serverlist_add(serverList, server) mupnp_list_add((CgList *)serverList, (CgList *)server)

BOOL mupnp_upnp_ssdp_serverlist_open(CgUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_close(CgUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_start(CgUpnpSSDPServerList *ssdpServerList);
BOOL mupnp_upnp_ssdp_serverlist_stop(CgUpnpSSDPServerList *ssdpServerList);
void mupnp_upnp_ssdp_serverlist_setlistener(CgUpnpSSDPServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener);
void mupnp_upnp_ssdp_serverlist_setuserdata(CgUpnpSSDPServerList *ssdpServerList, void *data);

/****************************************
* Function (SSDPResponseServer)
****************************************/

CgUpnpSSDPResponseServer *mupnp_upnp_ssdpresponse_server_new();
void mupnp_upnp_ssdpresponse_server_delete(CgUpnpSSDPResponseServer *server);

#define mupnp_upnp_ssdpresponse_server_next(server) (CgUpnpSSDPResponseServer *)mupnp_list_next((CgList *)server)

BOOL mupnp_upnp_ssdpresponse_server_open(CgUpnpSSDPResponseServer *server, int bindPort, char *bindAddr);
BOOL mupnp_upnp_ssdpresponse_server_close(CgUpnpSSDPResponseServer *server);

#define mupnp_upnp_ssdpresponse_server_isopened(server) ((server->httpuSock != NULL) ? TRUE : FALSE)
#define mupnp_upnp_ssdpresponse_server_getsocket(server) (server->httpuSock)

#define mupnp_upnp_ssdpresponse_server_setlistener(server, func) (server->listener = func)
#define mupnp_upnp_ssdpresponse_server_getlistener(server) (server->listener)
void mupnp_upnp_ssdpresponse_server_performlistener(CgUpnpSSDPResponseServer *server, CgUpnpSSDPPacket *ssdpPkt);

#define mupnp_upnp_ssdpresponse_server_setuserdata(server, data) (server->userData = data)
#define mupnp_upnp_ssdpresponse_server_getuserdata(server) (server->userData)

BOOL mupnp_upnp_ssdpresponse_server_start(CgUpnpSSDPResponseServer *server);
BOOL mupnp_upnp_ssdpresponse_server_stop(CgUpnpSSDPResponseServer *server);

BOOL mupnp_upnp_ssdpresponse_server_post(CgUpnpSSDPResponseServer *server, CgUpnpSSDPRequest *ssdpReq);

/****************************************
* Function (SSDPResponseServerList)
****************************************/

CgUpnpSSDPResponseServerList *mupnp_upnp_ssdpresponse_serverlist_new();
void mupnp_upnp_ssdpresponse_serverlist_delete(CgUpnpSSDPResponseServerList *serverList);

#define mupnp_upnp_ssdpresponse_serverlist_clear(serverList) mupnp_list_clear((CgList *)serverList, (CG_LIST_DESTRUCTORFUNC)mupnp_upnp_ssdpresponse_server_delete)
#define mupnp_upnp_ssdpresponse_serverlist_size(serverList) mupnp_list_size((CgList *)serverList)
#define mupnp_upnp_ssdpresponse_serverlist_gets(serverList) (CgUpnpSSDPResponseServer *)mupnp_list_next((CgList *)serverList)
#define mupnp_upnp_ssdpresponse_serverlist_add(serverList, server) mupnp_list_add((CgList *)serverList, (CgList *)server)

BOOL mupnp_upnp_ssdpresponse_serverlist_open(CgUpnpSSDPResponseServerList *ssdpServerList, int bindPort);
BOOL mupnp_upnp_ssdpresponse_serverlist_close(CgUpnpSSDPResponseServerList *ssdpServerList);
BOOL mupnp_upnp_ssdpresponse_serverlist_start(CgUpnpSSDPResponseServerList *ssdpServerList);
BOOL mupnp_upnp_ssdpresponse_serverlist_stop(CgUpnpSSDPResponseServerList *ssdpServerList);
void mupnp_upnp_ssdpresponse_serverlist_setlistener(CgUpnpSSDPResponseServerList *ssdpServerList, CG_UPNP_SSDP_LISTNER listener);
void mupnp_upnp_ssdpresponse_serverlist_setuserdata(CgUpnpSSDPResponseServerList *ssdpServerList, void *data);
BOOL mupnp_upnp_ssdpresponse_serverlist_post(CgUpnpSSDPResponseServerList *ssdpServerList, CgUpnpSSDPRequest *ssdpReq);

#ifdef  __cplusplus
}
#endif

#endif

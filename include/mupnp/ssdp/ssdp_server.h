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

#ifndef _MUPNP_SSDP_SERVER_H_
#define _MUPNP_SSDP_SERVER_H_

#include <mupnp/typedef.h>

#include <mupnp/ssdp/ssdp.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef mUpnpSocket mUpnpHttpMuSocket;
typedef mUpnpSocket mUpnpHttpUSocket;

typedef struct _mUpnpSSDPPacket {
  mUpnpDatagramPacket* dgmPkt;
  mUpnpHttpHeaderList* headerList;
  void* userData;
  mUpnpTime timeStamp;

  /* These are used when filtering out duplicate
	 * M-SEARCHes
	 */
  mUpnpTime* timestamps;
  int initialized;
} mUpnpSSDPPacket;

typedef void (*MUPNP_SSDP_LISTNER)(mUpnpSSDPPacket*);

typedef struct _mUpnpSSDPServer {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpHttpMuSocket* httpmuSock;
  mUpnpThread* recvThread;
  MUPNP_SSDP_LISTNER listener;
  void* userData;
} mUpnpSSDPServer, mUpnpSSDPServerList;

typedef void (*MUPNP_SSDP_RESPONSE_LISTNER)(mUpnpSSDPPacket*);

typedef struct _mUpnpSSDPResponseServer {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpHttpUSocket* httpuSock;
  mUpnpThread* recvThread;
  MUPNP_SSDP_RESPONSE_LISTNER listener;
  void* userData;
} mUpnpSSDPResponseServer, mUpnpSSDPResponseServerList;

/****************************************
* Function (HTTPMU)
****************************************/

#define mupnp_httpmu_socket_new() mupnp_socket_dgram_new()
#define mupnp_httpmu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_httpmu_socket_close(sock) mupnp_socket_close(sock)

bool mupnp_httpmu_socket_bind(mUpnpHttpMuSocket* sock, const char* mcastAddr, int port, const char* bindAddr);
ssize_t mupnp_httpmu_socket_recv(mUpnpHttpMuSocket* sock, mUpnpSSDPPacket* ssdpPkt);

/****************************************
* Function (HTTPU)
****************************************/

#define mupnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_httpu_socket_delete(sock) mupnp_socket_delete(sock)
#define mupnp_httpu_socket_close(sock) mupnp_socket_close(sock)
#define mupnp_httpu_socket_new() mupnp_socket_dgram_new()
#define mupnp_httpu_socket_bind(sock, bindPort, bindAddr) mupnp_socket_bind(sock, bindPort, bindAddr, false, false)
ssize_t mupnp_httpu_socket_recv(mUpnpHttpUSocket* sock, mUpnpSSDPPacket* ssdpPkt);

/****************************************
* Function (SSDPPacket)
****************************************/

mUpnpSSDPPacket* mupnp_ssdp_packet_new();
void mupnp_ssdp_packet_delete(mUpnpSSDPPacket* ssdpPkt);
void mupnp_ssdp_packet_clear(mUpnpSSDPPacket* ssdpPkt);

#define mupnp_ssdp_packet_setdatagrampacket(ssdpPkt, value) (ssdpPkt->dgmPkt = value)
#define mupnp_ssdp_packet_getdatagrampacket(ssdpPkt) (ssdpPkt->dgmPkt)
#define mupnp_ssdp_packet_setdata(ssdpPkt, value) mupnp_string_setvalue(ssdpPkt->dgmPkt->data, value)
#define mupnp_ssdp_packet_getdata(ssdpPkt) mupnp_string_getvalue(ssdpPkt->dgmPkt->data)
#define mupnp_ssdp_packet_setuserdata(ssdpPkt, data) (ssdpPkt->userData = data)
#define mupnp_ssdp_packet_getuserdata(ssdpPkt) (ssdpPkt->userData)
#define mupnp_ssdp_packet_settimestamp(ssdpPkt, value) (ssdpPkt->timeStamp = value)
#define mupnp_ssdp_packet_gettimestamp(ssdpPkt) (ssdpPkt->timeStamp)
#define mupnp_ssdp_packet_getexpiration(ssdpPkt) (mupnp_ssdp_packet_getmaxage(ssdpPkt) == 0 ? 0 : mupnp_ssdp_packet_gettimestamp(ssdpPkt) + mupnp_ssdp_packet_getmaxage(ssdpPkt))

void mupnp_ssdp_packet_setheader(mUpnpSSDPPacket* ssdpPkt, char* ssdpMsg);
#define mupnp_ssdp_packet_addheader(ssdpPkt, header) mupnp_http_headerlist_add(ssdpPkt->headerList, header)
#define mupnp_ssdp_packet_getheaders(ssdpPkt) mupnp_http_headerlist_gets(ssdpPkt->headerList)

#define mupnp_ssdp_packet_gethost(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_HOST)
#define mupnp_ssdp_packet_getserver(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_SERVER)
#define mupnp_ssdp_packet_getcachecontrol(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_CACHE_CONTROL)
#define mupnp_ssdp_packet_getdate(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_DATE)
#define mupnp_ssdp_packet_getext(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_EXT)
#define mupnp_ssdp_packet_getlocation(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_LOCATION)
#define mupnp_ssdp_packet_getst(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_ST)
#define mupnp_ssdp_packet_getusn(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_USN)
#define mupnp_ssdp_packet_getmx(ssdpPkt) mupnp_str2int(mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_MX))
#define mupnp_ssdp_packet_getman(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_MAN)
#define mupnp_ssdp_packet_getnt(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_NT)
#define mupnp_ssdp_packet_getnts(ssdpPkt) mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_NTS)
long mupnp_ssdp_packet_getmaxage(mUpnpSSDPPacket* ssdpPkt);

#define mupnp_ssdp_packet_isdiscover(ssdpPkt) mupnp_man_isdiscover(mupnp_ssdp_packet_getman(ssdpPkt))
#define mupnp_ssdp_packet_isalive(ssdpPkt) mupnp_nts_isalive(mupnp_ssdp_packet_getnts(ssdpPkt))
#define mupnp_ssdp_packet_isbyebye(ssdpPkt) mupnp_nts_isbyebye(mupnp_ssdp_packet_getnts(ssdpPkt))

bool mupnp_ssdp_packet_isrootdevice(mUpnpSSDPPacket* ssdpPkt);

#define mupnp_ssdp_packet_getlocaladdress(ssdpPkt) mupnp_socket_datagram_packet_getlocaladdress(ssdpPkt->dgmPkt)
#define mupnp_ssdp_packet_getlocalport(ssdpPkt) mupnp_socket_datagram_packet_getlocalport(ssdpPkt->dgmPkt)
#define mupnp_ssdp_packet_getremoteaddress(ssdpPkt) mupnp_socket_datagram_packet_getremoteaddress(ssdpPkt->dgmPkt)
#define mupnp_ssdp_packet_getremoteport(ssdpPkt) mupnp_socket_datagram_packet_getremoteport(ssdpPkt->dgmPkt)

void mupnp_ssdp_packet_copy(mUpnpSSDPPacket* destSsdpPkt, mUpnpSSDPPacket* srcSsdpPkt);
void mupnp_ssdp_packet_print(mUpnpSSDPPacket* ssdpPkt);

/****************************************
* Function (SSDPServer)
****************************************/

mUpnpSSDPServer* mupnp_ssdp_server_new();
void mupnp_ssdp_server_delete(mUpnpSSDPServer* server);

#define mupnp_ssdp_server_next(server) (mUpnpSSDPServer*) mupnp_list_next((mUpnpList*)server)

bool mupnp_ssdp_server_open(mUpnpSSDPServer* server, char* bindAddr);
bool mupnp_ssdp_server_close(mUpnpSSDPServer* server);

#define mupnp_ssdp_server_isopened(server) ((server->httpmuSock != NULL) ? true : false)
#define mupnp_ssdp_server_getsocket(server) (server->httpmuSock)

#define mupnp_ssdp_server_setlistener(server, func) (server->listener = func)
#define mupnp_ssdp_server_getlistener(server) (server->listener)
void mupnp_ssdp_server_performlistener(mUpnpSSDPServer* server, mUpnpSSDPPacket* ssdpPkt);

#define mupnp_ssdp_server_setuserdata(server, data) (server->userData = data)
#define mupnp_ssdp_server_getuserdata(server) (server->userData)

bool mupnp_ssdp_server_start(mUpnpSSDPServer* server);
bool mupnp_ssdp_server_stop(mUpnpSSDPServer* server);

/****************************************
* Function (SSDPServerList)
****************************************/

mUpnpSSDPServerList* mupnp_ssdp_serverlist_new();
void mupnp_ssdp_serverlist_delete(mUpnpSSDPServerList* serverList);

#define mupnp_ssdp_serverlist_clear(serverList) mupnp_list_clear((mUpnpList*)serverList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_ssdp_server_delete)
#define mupnp_ssdp_serverlist_size(serverList) mupnp_list_size((mUpnpList*)serverList)
#define mupnp_ssdp_serverlist_gets(serverList) (mUpnpSSDPServer*) mupnp_list_next((mUpnpList*)serverList)
#define mupnp_ssdp_serverlist_add(serverList, server) mupnp_list_add((mUpnpList*)serverList, (mUpnpList*)server)

bool mupnp_ssdp_serverlist_open(mUpnpSSDPServerList* ssdpServerList);
bool mupnp_ssdp_serverlist_close(mUpnpSSDPServerList* ssdpServerList);
bool mupnp_ssdp_serverlist_start(mUpnpSSDPServerList* ssdpServerList);
bool mupnp_ssdp_serverlist_stop(mUpnpSSDPServerList* ssdpServerList);
void mupnp_ssdp_serverlist_setlistener(mUpnpSSDPServerList* ssdpServerList, MUPNP_SSDP_LISTNER listener);
void mupnp_ssdp_serverlist_setuserdata(mUpnpSSDPServerList* ssdpServerList, void* data);

/****************************************
* Function (SSDPResponseServer)
****************************************/

mUpnpSSDPResponseServer* mupnp_ssdpresponse_server_new();
void mupnp_ssdpresponse_server_delete(mUpnpSSDPResponseServer* server);

#define mupnp_ssdpresponse_server_next(server) (mUpnpSSDPResponseServer*) mupnp_list_next((mUpnpList*)server)

bool mupnp_ssdpresponse_server_open(mUpnpSSDPResponseServer* server, int bindPort, char* bindAddr);
bool mupnp_ssdpresponse_server_close(mUpnpSSDPResponseServer* server);

#define mupnp_ssdpresponse_server_isopened(server) ((server->httpuSock != NULL) ? true : false)
#define mupnp_ssdpresponse_server_getsocket(server) (server->httpuSock)

#define mupnp_ssdpresponse_server_setlistener(server, func) (server->listener = func)
#define mupnp_ssdpresponse_server_getlistener(server) (server->listener)
void mupnp_ssdpresponse_server_performlistener(mUpnpSSDPResponseServer* server, mUpnpSSDPPacket* ssdpPkt);

#define mupnp_ssdpresponse_server_setuserdata(server, data) (server->userData = data)
#define mupnp_ssdpresponse_server_getuserdata(server) (server->userData)

bool mupnp_ssdpresponse_server_start(mUpnpSSDPResponseServer* server);
bool mupnp_ssdpresponse_server_stop(mUpnpSSDPResponseServer* server);

bool mupnp_ssdpresponse_server_post(mUpnpSSDPResponseServer* server, mUpnpSSDPRequest* ssdpReq);

/****************************************
* Function (SSDPResponseServerList)
****************************************/

mUpnpSSDPResponseServerList* mupnp_ssdpresponse_serverlist_new();
void mupnp_ssdpresponse_serverlist_delete(mUpnpSSDPResponseServerList* serverList);

#define mupnp_ssdpresponse_serverlist_clear(serverList) mupnp_list_clear((mUpnpList*)serverList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_ssdpresponse_server_delete)
#define mupnp_ssdpresponse_serverlist_size(serverList) mupnp_list_size((mUpnpList*)serverList)
#define mupnp_ssdpresponse_serverlist_gets(serverList) (mUpnpSSDPResponseServer*) mupnp_list_next((mUpnpList*)serverList)
#define mupnp_ssdpresponse_serverlist_add(serverList, server) mupnp_list_add((mUpnpList*)serverList, (mUpnpList*)server)

bool mupnp_ssdpresponse_serverlist_open(mUpnpSSDPResponseServerList* ssdpServerList, int bindPort);
bool mupnp_ssdpresponse_serverlist_close(mUpnpSSDPResponseServerList* ssdpServerList);
bool mupnp_ssdpresponse_serverlist_start(mUpnpSSDPResponseServerList* ssdpServerList);
bool mupnp_ssdpresponse_serverlist_stop(mUpnpSSDPResponseServerList* ssdpServerList);
void mupnp_ssdpresponse_serverlist_setlistener(mUpnpSSDPResponseServerList* ssdpServerList, MUPNP_SSDP_LISTNER listener);
void mupnp_ssdpresponse_serverlist_setuserdata(mUpnpSSDPResponseServerList* ssdpServerList, void* data);
bool mupnp_ssdpresponse_serverlist_post(mUpnpSSDPResponseServerList* ssdpServerList, mUpnpSSDPRequest* ssdpReq);

#ifdef __cplusplus
}
#endif

#endif

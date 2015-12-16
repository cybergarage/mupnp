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

#ifndef _MUPNP_SSDP_H_
#define _MUPNP_SSDP_H_

#include <mupnp/typedef.h>

#include <mupnp/http/http.h>
#include <mupnp/net/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_SSDP_PORT 1900

#define MUPNP_SSDP_ADDRESS "239.255.255.250"

#define MUPNP_SSDP_MULTICAST_ADDRESS "239.255.255.250:1900"

#define MUPNP_AUTO_IP_NET 0xa9fe0000
#define MUPNP_AUTO_IP_MASK 0xffff0000

/* Default time to live specified in UPnP DA spec */
#define MUPNP_SSDP_MULTICAST_DEFAULT_TTL 4

#define MUPNP_SSDP_IPV6_IF_LOCAL_ADDRESS "FF01::C"
#define MUPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS "FF02::C"
#define MUPNP_SSDP_IPV6_SUBNET_ADDRESS "FF03::C"
#define MUPNP_SSDP_IPV6_ADMINISTRATIVE_ADDRESS "FF04::C"
#define MUPNP_SSDP_IPV6_SITE_LOCAL_ADDRESS "FF05::C"
#define MUPNP_SSDP_IPV6_GLOBAL_ADDRESS "FF0E::C"

#define MUPNP_SSDP_DEFAULT_MSEARCH_MX 3
#define MUPNP_SSDP_DEFAULT_ANNOUNCE_COUNT 3

#define MUPNP_SSDP_HEADER_LINE_MAXSIZE 128

#define MUPNP_SSDP_ST "ST"
#define MUPNP_SSDP_MX "MX"
#define MUPNP_SSDP_MAN "MAN"
#define MUPNP_SSDP_NT "NT"
#define MUPNP_SSDP_NTS "NTS"
#define MUPNP_SSDP_NTS_ALIVE "ssdp:alive"
#define MUPNP_SSDP_NTS_BYEBYE "ssdp:byebye"
#define MUPNP_SSDP_NTS_PROPCHANGE "upnp:propchange"
#define MUPNP_SSDP_USN "USN"
#define MUPNP_SSDP_EXT "EXT"
#define MUPNP_SSDP_SID "SID"
#define MUPNP_SSDP_SEQ "SEQ"
#define MUPNP_SSDP_CALBACK "CALLBACK"
#define MUPNP_SSDP_TIMEOUT "TIMEOUT"
#define MUPNP_SSDP_SERVER "SERVER"
#define MUPNP_SSDP_BOOTID_UPNP_ORG "BOOTID.UPNP.ORG"

#define MUPNP_SSDP_FILTER_TABLE_SIZE 101

/****************************************
* Data Type
****************************************/

typedef mUpnpHttpResponse mUpnpSSDPResponse;
typedef mUpnpHttpRequest mUpnpSSDPRequest;

typedef mUpnpSocket mUpnpSSDPSocket;

/****************************************
* Function
****************************************/

void mupnp_ssdp_setipv6address(const char* addr);
const char* mupnp_ssdp_getipv6address();

void mupnp_ssdp_setannouncecount(int count);
int mupnp_ssdp_getannouncecount();

int mupnp_ssdp_getleasetime(const char* cacheCont);

const char* mupnp_ssdp_gethostaddress(const char* ifAddr);

/****************************************
* Function (SSDPRequest)
****************************************/

mUpnpSSDPRequest* mupnp_ssdprequest_new();
void mupnp_ssdprequest_delete(mUpnpSSDPRequest* ssdpReq);

/**** Method ****/
#define mupnp_ssdprequest_setmethod(ssdpReq, value) mupnp_http_request_setmethod(ssdpReq, value)
#define mupnp_ssdprequest_getmethod(ssdpReq) mupnp_http_request_getmethod(ssdpReq)

/**** URI ****/
#define mupnp_ssdprequest_seturi(ssdpReq, value) mupnp_http_request_seturi(ssdpReq, value)
#define mupnp_ssdprequest_geturi(ssdpReq) mupnp_http_request_geturi(ssdpReq)

/**** HOST ****/
#define mupnp_ssdprequest_sethost(ssdpReq, addr, port) mupnp_http_packet_sethost((mUpnpHttpPacket*)ssdpReq, addr, port)
#define mupnp_ssdprequest_gethost(ssdpReq) mupnp_http_packet_gethost((mUpnpHttpPacket*)ssdpReq)

/**** Server ****/
#define mupnp_ssdprequest_setserver(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_SERVER, value)
#define mupnp_ssdprequest_getserver(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_SERVER)

/**** ST ****/
#define mupnp_ssdprequest_setst(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_ST, value)
#define mupnp_ssdprequest_getst(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_ST)

/**** NT ****/
#define mupnp_ssdprequest_setnt(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_NT, value)
#define mupnp_ssdprequest_getnt(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_NT)

/**** NTS ****/
#define mupnp_ssdprequest_setnts(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_NTS, value)
#define mupnp_ssdprequest_getnts(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_NTS)

/**** Location ****/
#define mupnp_ssdprequest_setlocation(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_LOCATION, value)
#define mupnp_ssdprequest_getlocation(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_HTTP_LOCATION)

/**** USN ****/
#define mupnp_ssdprequest_setusn(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_USN, value)
#define mupnp_ssdprequest_getusn(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_USN)

/**** MX ****/
#define mupnp_ssdprequest_setmx(ssdpReq, value) mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_MX, value)
#define mupnp_ssdprequest_getmx(ssdpReq) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_MX)

/**** MAN ****/
#define mupnp_ssdprequest_setman(ssdpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_MAN, value)
#define mupnp_ssdprequest_getman(ssdpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_MAN)

/**** BOOTID.UPNP.ORG ****/
#define mupnp_ssdprequest_setbootid(ssdpReq, value) mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_BOOTID_UPNP_ORG, value)
#define mupnp_ssdprequest_getbootid(ssdpReq) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)ssdpReq, MUPNP_SSDP_BOOTID_UPNP_ORG)

/**** CacheControl ****/
void mupnp_ssdprequest_setleasetime(mUpnpSSDPRequest* ssdpReq, mUpnpTime value);
mUpnpTime mupnp_ssdprequest_getleasetime(mUpnpSSDPRequest* ssdpReq);

/**** toString ****/
char* mupnp_ssdprequest_tostring(mUpnpSSDPRequest* ssdpReq, mUpnpString* ssdpMsg);

/**** Print ****/
#define mupnp_ssdprequest_print(ssdpReq) mupnp_http_request_print(ssdpReq)

/****************************************
* Function (SSDPResponse)
****************************************/

mUpnpSSDPResponse* mupnp_ssdpresponse_new();
void mupnp_ssdpresponse_delete(mUpnpSSDPResponse* ssdpRes);

/**** ST ****/
#define mupnp_ssdpresponse_setst(ssdpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_ST, value)
#define mupnp_ssdpresponse_getst(ssdpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_ST)

/**** EXT ****/
#define mupnp_ssdpresponse_setext(ssdpRes) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_EXT, "")

/**** SERVER ****/
#define mupnp_ssdpresponse_setserver(ssdpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_SERVER, value)

/**** Location ****/
#define mupnp_ssdpresponse_setlocation(ssdpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_HTTP_LOCATION, value)
#define mupnp_ssdpresponse_getlocation(ssdpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_HTTP_LOCATION)

/**** USN ****/
#define mupnp_ssdpresponse_setusn(ssdpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_USN, value)
#define mupnp_ssdpresponse_getusn(ssdpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_USN)

/**** CacheControl ****/
void mupnp_ssdpresponse_setleasetime(mUpnpSSDPResponse* ssdpRes, mUpnpTime value);
int mupnp_ssdpresponse_getleasetime(mUpnpSSDPResponse* ssdpRes);

/**** Date ****/
#define mupnp_ssdpresponse_setdate(ssdpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_HTTP_DATE, value)
#define mupnp_ssdpresponse_getdate(ssdpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpRes, MUPNP_HTTP_DATE)

/**** BOOTID.UPNP.ORG ****/
#define mupnp_ssdpresponse_setbootid(ssdpRes, value) mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_BOOTID_UPNP_ORG, value)
#define mupnp_ssdpresponse_getbootid(ssdpRes) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)ssdpRes, MUPNP_SSDP_BOOTID_UPNP_ORG)

/**** toString ****/
char* mupnp_ssdpresponse_tostring(mUpnpSSDPResponse* ssdpRes, mUpnpString* ssdpMsg);

/**** Print ****/
#define mupnp_ssdpresponse_print(ssdpReq) mupnp_http_response_print(ssdpReq)

/****************************************
* Function (SSDPSocket)
****************************************/

#define mupnp_ssdp_socket_new() mupnp_socket_new(MUPNP_NET_SOCKET_DGRAM)
#define mupnp_ssdp_socket_delete(socket) mupnp_socket_delete(socket)
#define mupnp_ssdp_socket_close(socket) mupnp_socket_close(socket)

bool mupnp_ssdp_socket_notifyfrom(mUpnpSSDPSocket* ssdpSock, mUpnpSSDPRequest* ssdpReq, const char* bindAddr);
bool mupnp_ssdp_socket_postresponse(mUpnpSSDPSocket* ssdpSock, mUpnpSSDPResponse* ssdpReq, const char* host, int port);

#ifdef __cplusplus
}
#endif

#endif

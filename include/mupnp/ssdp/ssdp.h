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
*	File: cssdp.h
*
*	Revision:
*
*	02/13/05
*		- first revision
*
*	10/31/05
*		- Added mupnp_upnp_ssdpresponse_setext and _setserver
*
******************************************************************/

#ifndef _CG_UPNP_CSSDP_H_
#define _CG_UPNP_CSSDP_H_

#include <mupnp/typedef.h>

#include <mupnp/http/http.h>
#include <mupnp/net/socket.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_SSDP_PORT 1900

#define CG_UPNP_SSDP_ADDRESS "239.255.255.250"

#define CG_UPNP_SSDP_MULTICAST_ADDRESS "239.255.255.250:1900"

#define CG_AUTO_IP_NET 0xa9fe0000
#define CG_AUTO_IP_MASK 0xffff0000

/* Default time to live specified in UPnP DA spec */
#define CG_UPNP_SSDP_MULTICAST_DEFAULT_TTL 4
	
#define CG_UPNP_SSDP_IPV6_IF_LOCAL_ADDRESS "FF01::C"
#define CG_UPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS "FF02::C"
#define CG_UPNP_SSDP_IPV6_SUBNET_ADDRESS "FF03::C"
#define CG_UPNP_SSDP_IPV6_ADMINISTRATIVE_ADDRESS "FF04::C"
#define CG_UPNP_SSDP_IPV6_SITE_LOCAL_ADDRESS "FF05::C"
#define CG_UPNP_SSDP_IPV6_GLOBAL_ADDRESS "FF0E::C"

#define CG_UPNP_SSDP_DEFAULT_MSEARCH_MX 3
#define CG_UPNP_SSDP_DEFAULT_ANNOUNCE_COUNT 3

#define CG_UPNP_SSDP_HEADER_LINE_MAXSIZE 128

#define CG_UPNP_SSDP_ST "ST" 
#define CG_UPNP_SSDP_MX "MX"
#define CG_UPNP_SSDP_MAN "MAN"
#define CG_UPNP_SSDP_NT "NT" 
#define CG_UPNP_SSDP_NTS "NTS" 
	#define CG_UPNP_SSDP_NTS_ALIVE "ssdp:alive"
	#define CG_UPNP_SSDP_NTS_BYEBYE "ssdp:byebye"
	#define CG_UPNP_SSDP_NTS_PROPCHANGE "upnp:propchange"
#define CG_UPNP_SSDP_USN "USN" 
#define CG_UPNP_SSDP_EXT "EXT" 
#define CG_UPNP_SSDP_SID "SID" 
#define CG_UPNP_SSDP_SEQ "SEQ" 
#define CG_UPNP_SSDP_CALBACK "CALLBACK" 
#define CG_UPNP_SSDP_TIMEOUT "TIMEOUT" 
#define CG_UPNP_SSDP_SERVER "SERVER"
#define CG_UPNP_SSDP_BOOTID_UPNP_ORG "BOOTID.UPNP.ORG"

#define CG_UPNP_SSDP_FILTER_TABLE_SIZE 101

/****************************************
* Data Type
****************************************/

typedef CgHttpResponse CgUpnpSSDPResponse;
typedef CgHttpRequest CgUpnpSSDPRequest;

typedef CgSocket CgUpnpSSDPSocket;

/****************************************
* Function
****************************************/

void mupnp_upnp_ssdp_setipv6address(const char *addr);
const char *mupnp_upnp_ssdp_getipv6address();

void mupnp_upnp_ssdp_setannouncecount(int count);
int mupnp_upnp_ssdp_getannouncecount();

int mupnp_upnp_ssdp_getleasetime(const char *cacheCont);

const char *mupnp_upnp_ssdp_gethostaddress(const char *ifAddr);

/****************************************
* Function (SSDPRequest)
****************************************/

CgUpnpSSDPRequest *mupnp_upnp_ssdprequest_new();
void mupnp_upnp_ssdprequest_delete(CgUpnpSSDPRequest *ssdpReq);

/**** Method ****/
#define mupnp_upnp_ssdprequest_setmethod(ssdpReq,value) mupnp_http_request_setmethod(ssdpReq,value)
#define mupnp_upnp_ssdprequest_getmethod(ssdpReq) mupnp_http_request_getmethod(ssdpReq)

/**** URI ****/
#define mupnp_upnp_ssdprequest_seturi(ssdpReq,value) mupnp_http_request_seturi(ssdpReq,value)
#define mupnp_upnp_ssdprequest_geturi(ssdpReq) mupnp_http_request_geturi(ssdpReq)

/**** HOST ****/
#define mupnp_upnp_ssdprequest_sethost(ssdpReq,addr,port) mupnp_http_packet_sethost((CgHttpPacket*)ssdpReq, addr, port)
#define mupnp_upnp_ssdprequest_gethost(ssdpReq) mupnp_http_packet_gethost((CgHttpPacket*)ssdpReq)

/**** Server ****/
#define mupnp_upnp_ssdprequest_setserver(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_HTTP_SERVER, value)
#define mupnp_upnp_ssdprequest_getserver(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_HTTP_SERVER)

/**** ST ****/
#define mupnp_upnp_ssdprequest_setst(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_ST, value)
#define mupnp_upnp_ssdprequest_getst(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_ST)

/**** NT ****/
#define mupnp_upnp_ssdprequest_setnt(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_NT, value)
#define mupnp_upnp_ssdprequest_getnt(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_NT)

/**** NTS ****/
#define mupnp_upnp_ssdprequest_setnts(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_NTS, value)
#define mupnp_upnp_ssdprequest_getnts(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_NTS)

/**** Location ****/
#define mupnp_upnp_ssdprequest_setlocation(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_HTTP_LOCATION, value)
#define mupnp_upnp_ssdprequest_getlocation(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_HTTP_LOCATION)

/**** USN ****/
#define mupnp_upnp_ssdprequest_setusn(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_USN, value)
#define mupnp_upnp_ssdprequest_getusn(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_USN)

/**** MX ****/
#define mupnp_upnp_ssdprequest_setmx(ssdpReq,value) mupnp_http_packet_setheaderinteger((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_MX, value)
#define mupnp_upnp_ssdprequest_getmx(ssdpReq) mupnp_http_packet_getheaderinteger((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_MX)

/**** MAN ****/
#define mupnp_upnp_ssdprequest_setman(ssdpReq,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_MAN, value)
#define mupnp_upnp_ssdprequest_getman(ssdpReq) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_MAN)

/**** BOOTID.UPNP.ORG ****/
#define mupnp_upnp_ssdprequest_setbootid(ssdpReq,value) mupnp_http_packet_setheaderinteger((CgHttpPacket*)ssdpReq, CG_UPNP_SSDP_BOOTID_UPNP_ORG, value)
#define mupnp_upnp_ssdprequest_getbootid(ssdpReq) mupnp_http_packet_getheaderinteger((CgHttpPacket*)ssdpReq,CG_UPNP_SSDP_BOOTID_UPNP_ORG)

/**** CacheControl ****/
void mupnp_upnp_ssdprequest_setleasetime(CgUpnpSSDPRequest *ssdpReq, CgTime value);
CgTime mupnp_upnp_ssdprequest_getleasetime(CgUpnpSSDPRequest *ssdpReq);

/**** toString ****/
char *mupnp_upnp_ssdprequest_tostring(CgUpnpSSDPRequest *ssdpReq, CgString *ssdpMsg);

/**** Print ****/
#define mupnp_upnp_ssdprequest_print(ssdpReq) mupnp_http_request_print(ssdpReq)

/****************************************
* Function (SSDPResponse)
****************************************/

CgUpnpSSDPResponse *mupnp_upnp_ssdpresponse_new();
void mupnp_upnp_ssdpresponse_delete(CgUpnpSSDPResponse *ssdpRes);

/**** ST ****/
#define mupnp_upnp_ssdpresponse_setst(ssdpRes,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpRes, CG_UPNP_SSDP_ST, value)
#define mupnp_upnp_ssdpresponse_getst(ssdpRes) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpRes,CG_UPNP_SSDP_ST)

/**** EXT ****/
#define mupnp_upnp_ssdpresponse_setext(ssdpRes) mupnp_http_packet_setheadervalue((CgHttpPacket*) ssdpRes, CG_UPNP_SSDP_EXT, "")

/**** SERVER ****/
#define mupnp_upnp_ssdpresponse_setserver(ssdpRes, value) mupnp_http_packet_setheadervalue((CgHttpPacket*) ssdpRes, CG_UPNP_SSDP_SERVER, value)

/**** Location ****/
#define mupnp_upnp_ssdpresponse_setlocation(ssdpRes,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpRes, CG_HTTP_LOCATION, value)
#define mupnp_upnp_ssdpresponse_getlocation(ssdpRes) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpRes,CG_HTTP_LOCATION)

/**** USN ****/
#define mupnp_upnp_ssdpresponse_setusn(ssdpRes,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpRes, CG_UPNP_SSDP_USN, value)
#define mupnp_upnp_ssdpresponse_getusn(ssdpRes) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpRes,CG_UPNP_SSDP_USN)

/**** CacheControl ****/
void mupnp_upnp_ssdpresponse_setleasetime(CgUpnpSSDPResponse *ssdpRes, CgTime value);
int mupnp_upnp_ssdpresponse_getleasetime(CgUpnpSSDPResponse *ssdpRes);

/**** Date ****/
#define mupnp_upnp_ssdpresponse_setdate(ssdpRes,value) mupnp_http_packet_setheadervalue((CgHttpPacket*)ssdpRes, CG_HTTP_DATE, value)
#define mupnp_upnp_ssdpresponse_getdate(ssdpRes) mupnp_http_packet_getheadervalue((CgHttpPacket*)ssdpRes,CG_HTTP_DATE)

/**** BOOTID.UPNP.ORG ****/
#define mupnp_upnp_ssdpresponse_setbootid(ssdpRes,value) mupnp_http_packet_setheaderinteger((CgHttpPacket*)ssdpRes, CG_UPNP_SSDP_BOOTID_UPNP_ORG, value)
#define mupnp_upnp_ssdpresponse_getbootid(ssdpRes) mupnp_http_packet_getheaderinteger((CgHttpPacket*)ssdpRes,CG_UPNP_SSDP_BOOTID_UPNP_ORG)

/**** toString ****/
char *mupnp_upnp_ssdpresponse_tostring(CgUpnpSSDPResponse *ssdpRes, CgString *ssdpMsg);

/**** Print ****/
#define mupnp_upnp_ssdpresponse_print(ssdpReq) mupnp_http_response_print(ssdpReq)

/****************************************
* Function (SSDPSocket)
****************************************/

#define mupnp_upnp_ssdp_socket_new() mupnp_socket_new(CG_NET_SOCKET_DGRAM)
#define mupnp_upnp_ssdp_socket_delete(socket) mupnp_socket_delete(socket)
#define mupnp_upnp_ssdp_socket_close(socket) mupnp_socket_close(socket)

BOOL mupnp_upnp_ssdp_socket_notifyfrom(CgUpnpSSDPSocket *ssdpSock, CgUpnpSSDPRequest *ssdpReq, const char *bindAddr);
BOOL mupnp_upnp_ssdp_socket_postresponse(CgUpnpSSDPSocket *ssdpSock, CgUpnpSSDPResponse *ssdpReq, const char *host, int port);

#ifdef  __cplusplus
}
#endif

#endif

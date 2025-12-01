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

/**
 * @brief Set the IPv6 multicast address for SSDP
 *
 * @details
 * Configures the IPv6 multicast address used for SSDP discovery.
 * Default is MUPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS ("FF02::C").
 *
 * Common IPv6 SSDP addresses:
 * - FF01::C - Interface-local scope
 * - FF02::C - Link-local scope (default, same subnet)
 * - FF05::C - Site-local scope
 * - FF0E::C - Global scope
 *
 * @param addr The IPv6 multicast address. Must not be NULL.
 *             Should be one of the MUPNP_SSDP_IPV6_* constants.
 *
 * @note Thread-safe: Should be called before starting devices or control points.
 * @note Side effect: Changes the global IPv6 multicast address setting.
 *
 * @see mupnp_ssdp_getipv6address()
 */
void mupnp_ssdp_setipv6address(const char* addr);

/**
 * @brief Get the current IPv6 multicast address for SSDP
 *
 * @return The currently configured IPv6 multicast address.
 *         Default is MUPNP_SSDP_IPV6_LINK_LOCAL_ADDRESS.
 *
 * @note Thread-safe: Can be called from any thread.
 *
 * @see mupnp_ssdp_setipv6address()
 */
const char* mupnp_ssdp_getipv6address(void);

/**
 * @brief Set the number of times to send SSDP announcements
 *
 * @details
 * Configures how many times each SSDP advertisement (alive or byebye) is
 * sent. Multiple announcements increase reliability on unreliable networks.
 * Default is MUPNP_SSDP_DEFAULT_ANNOUNCE_COUNT (3).
 *
 * @param count Number of announcement repetitions. Must be >= 1.
 *              Typical values: 2-4. Higher values increase network traffic.
 *
 * @note Thread-safe: Should be called before starting devices.
 * @note Side effect: Changes the global announcement count setting.
 *
 * @see mupnp_ssdp_getannouncecount()
 */
void mupnp_ssdp_setannouncecount(int count);

/**
 * @brief Get the current SSDP announcement repetition count
 *
 * @return Number of times each announcement is sent.
 *         Default is MUPNP_SSDP_DEFAULT_ANNOUNCE_COUNT (3).
 *
 * @note Thread-safe: Can be called from any thread.
 *
 * @see mupnp_ssdp_setannouncecount()
 */
int mupnp_ssdp_getannouncecount(void);

/**
 * @brief Extract lease time from a Cache-Control header value
 *
 * @details
 * Parses the Cache-Control header (or CACHE-CONTROL) from an SSDP message
 * to extract the max-age directive, which specifies the device's lease time
 * in seconds.
 *
 * Example: "max-age=1800" returns 1800 (30 minutes).
 *
 * @param cacheCont The Cache-Control header value. May be NULL.
 *
 * @return The lease time in seconds, or 0 if cacheCont is NULL or
 *         max-age directive is not found.
 *
 * @note Thread-safe: Can be called from any thread.
 */
int mupnp_ssdp_getleasetime(const char* cacheCont);

/**
 * @brief Get the host address for a given network interface
 *
 * @details
 * Retrieves the IP address of the specified network interface, suitable
 * for use in SSDP messages (Location headers, etc.).
 *
 * @param ifAddr The network interface address. May be NULL to use default.
 *
 * @return The host address string, or NULL if not available.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note The returned string is managed internally and should not be freed.
 */
const char* mupnp_ssdp_gethostaddress(const char* ifAddr);

/****************************************
 * Function (SSDPRequest)
 ****************************************/

/**
 * @brief Create a new SSDP request message
 *
 * @details
 * Allocates and initializes a new SSDP request object, which is used for:
 * - M-SEARCH discovery requests
 * - NOTIFY alive/byebye announcements
 *
 * SSDP requests are HTTP-based messages sent over UDP multicast or unicast.
 *
 * @return A newly-created mUpnpSSDPRequest on success, or NULL if memory
 *         allocation fails.
 *
 * @note The returned request must be freed with mupnp_ssdprequest_delete()
 *       when no longer needed.
 * @note Thread-safe: Can be called concurrently from multiple threads.
 *
 * @see mupnp_ssdprequest_delete()
 */
mUpnpSSDPRequest* mupnp_ssdprequest_new(void);

/**
 * @brief Destroy an SSDP request message and free resources
 *
 * @details
 * Releases all resources associated with the SSDP request, including
 * header fields and message content.
 *
 * @param ssdpReq The SSDP request to destroy. May be NULL (no-op if NULL).
 *
 * @note After calling this function, the ssdpReq pointer is invalid.
 * @note Thread-safe: Must not be called concurrently on the same request.
 *
 * @see mupnp_ssdprequest_new()
 */
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

mUpnpSSDPResponse* mupnp_ssdpresponse_new(void);
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

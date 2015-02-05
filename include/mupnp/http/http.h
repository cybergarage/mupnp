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
*	File: chttp.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
*	10/31/05
*		- Changed CHttpHeader* pointers to CHttpServer* pointers
*		  in CHttpServer struct.
*	
*	16-Jan-06 Aapo Makela
*		- Fixed header comparisons to be case-insensitive
*		- Fixed iskeepaliveconnection to check "Connection:close"
*		- Added M-POST specific stuff
*	04/03/06 Theo Beisch
*		- added (mUpnpHttpHeaderList*) cast (see below)
*		- added LONGLONG macro for platform independence
*		- some re-sorting of get/set macros (see below)
******************************************************************/

#ifndef _CG_HTTP_CHTTP_H_
#define _CG_HTTP_CHTTP_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>
#include <mupnp/util/thread.h>
#include <mupnp/util/time.h>
#include <mupnp/util/mutex.h>
#include <mupnp/net/socket.h>
#include <mupnp/net/url.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/


#define CG_HTTP_READLINE_BUFSIZE 512
#define CG_HTTP_SEVERNAME_MAXLEN 64
#define CG_HTTP_DATE_MAXLEN 128

#define CG_HTTP_REQUESTLINE_DELIM "\r\n "
#define CG_HTTP_STATUSLINE_DELIM "\r\n "
#define CG_HTTP_HEADERLINE_DELIM "\r\n :"

#define CG_HTTP_CR "\r"
#define CG_HTTP_LF "\n"
#define CG_HTTP_CRLF "\r\n"
#define CG_HTTP_SP " "
#define CG_HTTP_COLON ":"

#define CG_HTTP_VER10 "HTTP/1.0"
#define CG_HTTP_VER11 "HTTP/1.1"

#if !defined(CG_HTTP_CURL)
#define CG_HTTP_USERAGENT_DEFAULT "CyberGarage-HTTP/1.0"
#else
#define CG_HTTP_USERAGENT_DEFAULT "libcurl-agent/1.0 (Cyberlink for C)"
#endif

#define CG_HTTP_DEFAULT_PORT 80
#define CG_HTTS_DEFAULT_PORT 443

#define CG_HTTP_STATUS_CONTINUE 100
#define CG_HTTP_STATUS_OK 200
#define CG_HTTP_STATUS_PARTIAL_CONTENT 206
#define CG_HTTP_STATUS_BAD_REQUEST 400
#define CG_HTTP_STATUS_NOT_FOUND 404
#define CG_HTTP_STATUS_METHOD_NOT_ALLOWED 405
#define CG_HTTP_STATUS_PRECONDITION_FAILED 412
#define CG_HTTP_STATUS_INVALID_RANGE 416
#define CG_HTTP_STATUS_INTERNAL_SERVER_ERROR 500

#define CG_HTTP_POST "POST"
#define CG_HTTP_GET "GET"
#define CG_HTTP_HEAD "HEAD"

#define CG_HTTP_HOST "HOST"
#define CG_HTTP_DATE "Date"
#define CG_HTTP_CACHE_CONTROL "Cache-Control"
	#define CG_HTTP_NO_CACHE "no-cache"
	#define CG_HTTP_MAX_AGE "max-age"
#define CG_HTTP_CONNECTION "Connection"
	#define CG_HTTP_CLOSE "close"
	#define CG_HTTP_KEEP_ALIVE "Keep-Alive"
#define CG_HTTP_CONTENT_TYPE "Content-Type"
#define CG_HTTP_CONTENT_LENGTH "Content-Length"
#define CG_HTTP_CONTENT_RANGE "Content-Range"
	#define CG_HTTP_CONTENT_RANGE_BYTES "bytes" 
#define CG_HTTP_LOCATION "Location"
#define CG_HTTP_SERVER "Server"
#define CG_HTTP_RANGE "Range"
#define CG_HTTP_TRANSFER_ENCODING "Transfer-Encoding"
	#define CG_HTTP_CHUNKED "Chunked"
#define CG_HTTP_USERAGENT "User-Agent"

/**** SOAP Extention ****/
#define CG_HTTP_SOAP_ACTION "SOAPACTION"
#define CG_HTTP_SOAP_ACTION_WITH_NS "01-SOAPACTION"
#define CG_HTTP_SOAP_MAN_VALUE "\"http://schemas.xmlsoap.org/soap/envelope/\"; ns=01"

/**** UPnP Extention ****/
#define CG_HTTP_MPOST "M-POST"
#define CG_HTTP_MSEARCH "M-SEARCH"
#define CG_HTTP_NOTIFY "NOTIFY"
#define CG_HTTP_SUBSCRIBE "SUBSCRIBE"
#define CG_HTTP_UNSUBSCRIBE "UNSUBSCRIBE"	

#define CG_HTTP_ST "ST"
#define CG_HTTP_MX "MX"
#define CG_HTTP_MAN "MAN"
#define CG_HTTP_NT "NT"
#define CG_HTTP_NTS "NTS"
#define CG_HTTP_USN "USN"
#define CG_HTTP_EXT "EXT"
#define CG_HTTP_SID "SID"
#define CG_HTTP_SEQ "SEQ"
#define CG_HTTP_CALLBACK "CALLBACK"
#define CG_HTTP_TIMEOUT "TIMEOUT"

/**** CURL connection timeout: 2 seconds ****/
#define CG_HTTP_CURL_CONNECTTIMEOUT 2

/**** CURL total timeout: 30 seconds (UPnP DA: whole transfer: 30sec ****/
#define CG_HTTP_CONN_TIMEOUT 30

/* HTTP server - client thread blocking timeout */
#define CG_HTTP_SERVER_READ_TIMEOUT 120

/**** HTTP Status code reason phrases ****/
#define CG_HTTP_REASON_100 "Continue"
#define CG_HTTP_REASON_101 "Switching Protocols"
#define CG_HTTP_REASON_200 "OK"
#define CG_HTTP_REASON_201 "Created"
#define CG_HTTP_REASON_202 "Accepted"
#define CG_HTTP_REASON_203 "Non-Authoritative Information"
#define CG_HTTP_REASON_204 "No Content"
#define CG_HTTP_REASON_205 "Reset Content"
#define CG_HTTP_REASON_206 "Partial Content"
#define CG_HTTP_REASON_300 "Multiple Choices"
#define CG_HTTP_REASON_301 "Moved Permanently"
#define CG_HTTP_REASON_302 "Found"
#define CG_HTTP_REASON_303 "See Other"
#define CG_HTTP_REASON_304 "Not Modified"
#define CG_HTTP_REASON_305 "Use Proxy"
#define CG_HTTP_REASON_307 "Temporary Redirect"
#define CG_HTTP_REASON_400 "Bad Request"
#define CG_HTTP_REASON_401 "Unauthorized"
#define CG_HTTP_REASON_402 "Payment Required"
#define CG_HTTP_REASON_403 "Forbidden"
#define CG_HTTP_REASON_404 "Not Found"
#define CG_HTTP_REASON_405 "Method Not Allowed"
#define CG_HTTP_REASON_406 "Not Acceptable"
#define CG_HTTP_REASON_407 "Proxy Authentication Required"
#define CG_HTTP_REASON_408 "Request Time-out"
#define CG_HTTP_REASON_409 "Conflict"
#define CG_HTTP_REASON_410 "Gone"
#define CG_HTTP_REASON_411 "Length Required"
#define CG_HTTP_REASON_412 "Precondition Failed"
#define CG_HTTP_REASON_413 "Request Entity Too Large"
#define CG_HTTP_REASON_414 "Request-URI Too Large"
#define CG_HTTP_REASON_415 "Unsupported Media Type"
#define CG_HTTP_REASON_416 "Requested range not satisfiable"
#define CG_HTTP_REASON_417 "Expectation Failed"
#define CG_HTTP_REASON_500 "Internal Server Error"
#define CG_HTTP_REASON_501 "Not Implemented"
#define CG_HTTP_REASON_502 "Bad Gateway"
#define CG_HTTP_REASON_503 "Service Unavailable"
#define CG_HTTP_REASON_504 "Gateway Time-out"
#define CG_HTTP_REASON_505 "HTTP Version not supported"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpHttpHeader {
	BOOL headFlag;
	struct _mUpnpHttpHeader *prev;
	struct _mUpnpHttpHeader *next;
	mUpnpString *name;
	mUpnpString *value;
} mUpnpHttpHeader, mUpnpHttpHeaderList;

typedef struct _mUpnpHttpPacket {
	mUpnpHttpHeaderList *headerList;
	mUpnpString *content;
} mUpnpHttpPacket;

typedef struct _mUpnpHttpResponse {
	mUpnpHttpHeaderList *headerList;
	mUpnpString *content;
	mUpnpString *version;
	int statusCode;
	mUpnpString *reasonPhrase;
	void *userData;
	int timeout;
} mUpnpHttpResponse;

typedef struct _mUpnpHttpRequest {
	mUpnpHttpHeaderList *headerList;
	mUpnpString *content;
	mUpnpString *method;
	mUpnpString *uri;
	mUpnpString *version;
	mUpnpString *userAgent;
	mUpnpHttpResponse *httpRes;
	mUpnpSocket *sock;
	mUpnpNetURL *postURL;
	void *userData;
	int timeout;
} mUpnpHttpRequest;

typedef void (*CG_HTTP_LISTENER)(mUpnpHttpRequest *);

typedef struct _mUpnpHttpServer {
	BOOL headFlag;
	struct _mUpnpHttpServer *prev;
	struct _mUpnpHttpServer *next;
	mUpnpSocket *sock;
	mUpnpThread *acceptThread;
	mUpnpThreadList *clientThreads;
	CG_HTTP_LISTENER listener;
	void *userData;
	int timeout;
	mUpnpMutex *mutex;
} mUpnpHttpServer, mUpnpHttpServerList;

/****************************************
* Function 
****************************************/

/**
 * Convert an HTTP status code to a reason phrase (ex. 200 -> "200 OK")
 *
 * @param code The HTTP status code to convert
 * @return The reason phrase (do not modify the pointer's contents)
 */
const char *mupnp_http_statuscode2reasonphrase(const int code);

/****************************************
* Function (Header)
****************************************/

mUpnpHttpHeader *mupnp_http_header_new();
void mupnp_http_header_delete(mUpnpHttpHeader *header);

#define mupnp_http_header_next(header) (mUpnpHttpHeader *)mupnp_list_next((mUpnpList *)header)
          
void mupnp_http_header_setname(mUpnpHttpHeader *header, const char *name);
const char *mupnp_http_header_getname(mUpnpHttpHeader *header);
void mupnp_http_header_setvalue(mUpnpHttpHeader *header, const char *value);
const char *mupnp_http_header_getvalue(mUpnpHttpHeader *header);

/****************************************
* Function (Header List)
****************************************/

mUpnpHttpHeaderList *mupnp_http_headerlist_new();
void mupnp_http_headerlist_delete(mUpnpHttpHeaderList *headerList);

#define mupnp_http_headerlist_clear(headerList) mupnp_list_clear((mUpnpList *)headerList, (CG_LIST_DESTRUCTORFUNC)mupnp_http_header_delete)
#define mupnp_http_headerlist_size(headerList) mupnp_list_size((mUpnpList *)headerList)
#define mupnp_http_headerlist_gets(headerList) (mUpnpHttpHeader *)mupnp_list_next((mUpnpList *)headerList)
#define mupnp_http_headerlist_add(headerList, header) mupnp_list_add((mUpnpList *)headerList, (mUpnpList *)header)

mUpnpHttpHeader *mupnp_http_headerlist_get(mUpnpHttpHeaderList *headerList, const char *name);
void mupnp_http_headerlist_set(mUpnpHttpHeaderList *headerList, const char *name, const char *value);
const char *mupnp_http_headerlist_getvalue(mUpnpHttpHeaderList *headerList, const char *name);

/****************************************
* Function (Packet)
****************************************/

mUpnpHttpPacket *mupnp_http_packet_new();
void mupnp_http_packet_delete(mUpnpHttpPacket *httpPkt);
void mupnp_http_packet_init(mUpnpHttpPacket *httpPkt);
void mupnp_http_packet_clean(mUpnpHttpPacket *httpPkt);
void mupnp_http_packet_clear(mUpnpHttpPacket *httpPkt);

#define mupnp_http_packet_deleteallheaders(httpPkt) mupnp_http_headerlist_gets(httpPkt->headerList)
#define mupnp_http_packet_getnheaders(httpPkt) mupnp_http_headerlist_size(httpPkt->headerList)
#define mupnp_http_packet_getheaders(httpPkt) mupnp_http_headerlist_gets(httpPkt->headerList)
//Theo Beisch - added (mUpnpHttpHeaderList*) cast - as _get is function call
#define mupnp_http_packet_getheader(httpPkt, name) mupnp_http_headerlist_get((mUpnpHttpHeaderList*)httpPkt->headerList, name)
#define mupnp_http_packet_hasheader(httpPkt, name) ((mupnp_http_headerlist_get((mUpnpHttpHeaderList*)httpPkt->headerList, name) != NULL) ? TRUE : FALSE)

#define mupnp_http_packet_hasheadervalue(httpPkt, header, value) (mupnp_streq(mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpPkt, header),value))

#define mupnp_http_packet_addheader(httpPkt, header) mupnp_http_headerlist_add(httpPkt->headerList, header)

void mupnp_http_packet_setheadervalue(mUpnpHttpPacket *httpPkt, const char* name, const char *value);
void mupnp_http_packet_setheaderinteger(mUpnpHttpPacket *httpPkt, const char* name, int value);
void mupnp_http_packet_setheaderlong(mUpnpHttpPacket *httpPkt, const char* name, long value);
void mupnp_http_packet_setheadersizet(mUpnpHttpPacket *httpPkt, const char* name, size_t value);
void mupnp_http_packet_setheaderssizet(mUpnpHttpPacket *httpPkt, const char* name, ssize_t value);

const char *mupnp_http_packet_getheadervalue(mUpnpHttpPacket *httpPkt, const char* name);
int mupnp_http_packet_getheaderinteger(mUpnpHttpPacket *httpPkt, const char* name);
long mupnp_http_packet_getheaderlong(mUpnpHttpPacket *httpPkt, const char* name);
size_t mupnp_http_packet_getheadersizet(mUpnpHttpPacket *httpPkt, const char* name);
ssize_t mupnp_http_packet_getheaderssizet(mUpnpHttpPacket *httpPkt, const char* name);
size_t mupnp_http_packet_getheadersize(mUpnpHttpPacket *httpPkt);

#define mupnp_http_packet_setcontent(httpPkt, value) mupnp_string_setvalue(httpPkt->content, value)
#define mupnp_http_packet_setncontent(httpPkt, value, len) mupnp_string_setnvalue(httpPkt->content, value, len)
#define mupnp_http_packet_appendncontent(httpPkt, value, len) mupnp_string_naddvalue(httpPkt->content, value, len)
#define mupnp_http_packet_setcontentpointer(httpPkt, value, len) mupnp_string_setpointervalue(httpPkt->content, value, len)
#define mupnp_http_packet_getcontent(httpPkt) mupnp_string_getvalue(httpPkt->content)

void mupnp_http_packet_post(mUpnpHttpPacket *httpPkt, mUpnpSocket *sock);
void mupnp_http_packet_read_headers(mUpnpHttpPacket *httpPkt, mUpnpSocket *sock, char *lineBuf, size_t lineBufSize);
BOOL mupnp_http_packet_read_body(mUpnpHttpPacket *httpPkt, mUpnpSocket *sock, char *lineBuf, size_t lineBufSize);
BOOL mupnp_http_packet_read(mUpnpHttpPacket *httpPkt, mUpnpSocket *sock, BOOL onlyHeader, char *lineBuf, size_t lineBufSize);

#define mupnp_http_packet_setcontentlength(httpPkt,value) mupnp_http_packet_setheaderssizet(httpPkt,CG_HTTP_CONTENT_LENGTH,value)
#define mupnp_http_packet_getcontentlength(httpPkt) mupnp_http_packet_getheaderssizet(httpPkt,CG_HTTP_CONTENT_LENGTH)

/**** Connection ****/
#define mupnp_http_packet_setconnection(httpPkt, value) mupnp_http_packet_setheadervalue(httpPkt,CG_HTTP_CONNECTION, value)
#define mupnp_http_packet_getconnection(httpPkt) mupnp_http_packet_getheadervalue(httpPkt,CG_HTTP_CONNECTION)
#define mupnp_http_packet_iskeepaliveconnection(httpPkt) (mupnp_strcaseeq(mupnp_http_packet_getconnection(httpPkt), CG_HTTP_CLOSE) == FALSE)

/**** Transfer-Encoding ****/
#define mupnp_http_packet_settransferencoding(httpPkt, value) mupnp_http_packet_setheadervalue(httpPkt,CG_HTTP_TRANSFER_ENCODING, value)
#define mupnp_http_packet_gettransferencoding(httpPkt) mupnp_http_packet_getheadervalue(httpPkt,CG_HTTP_TRANSFER_ENCODING)
#define mupnp_http_packet_ischunked(httpPkt) mupnp_strcaseeq(mupnp_http_packet_gettransferencoding(httpPkt), CG_HTTP_CHUNKED)

/**** Host ****/
void mupnp_http_packet_sethost(mUpnpHttpPacket *httpPkt, const char *addr, int port);
#define mupnp_http_packet_gethost(httpPkt) mupnp_http_packet_getheaderlong(httpPkt,CG_HTTP_HOST)

/**** Copy ****/
void mupnp_http_packet_copy(mUpnpHttpPacket *destHttpPkt, mUpnpHttpPacket *srcHttpPkt);

/**** Print ****/
void mupnp_http_packet_print(mUpnpHttpPacket *httpPkt);

/****************************************
* Function (Request)
****************************************/

mUpnpHttpRequest *mupnp_http_request_new();
void mupnp_http_request_delete(mUpnpHttpRequest *httpReq);
void mupnp_http_request_clear(mUpnpHttpRequest *httpReq);
void mupnp_http_request_setmethod(mUpnpHttpRequest *httpReq, const char *method);
char *mupnp_http_request_getmethod(mUpnpHttpRequest *httpReq);
void mupnp_http_request_setversion(mUpnpHttpRequest *httpReq, const char *version);
char *mupnp_http_request_getversion(mUpnpHttpRequest *httpReqest);
void mupnp_http_request_setuseragent(mUpnpHttpRequest *httpReq, const char *version);
char *mupnp_http_request_getuseragent(mUpnpHttpRequest *httpReqest);
void mupnp_http_request_addtouseragent(mUpnpHttpRequest *httpReq, const char *value);
void mupnp_http_request_seturi(mUpnpHttpRequest *httpReq, const char *uri);
char *mupnp_http_request_geturi(mUpnpHttpRequest *httpReq);
void mupnp_http_request_setsocket(mUpnpHttpRequest *httpReq, mUpnpSocket *sock);
mUpnpSocket *mupnp_http_request_getsocket(mUpnpHttpRequest *httpReq);
#define mupnp_http_request_closesocket(httpReq) mupnp_socket_close(mupnp_http_request_getsocket(httpReq))
mUpnpHttpResponse *mupnp_http_request_post(mUpnpHttpRequest *httpReq, const char *ipaddr, int port);
#if defined(CG_USE_OPENSSL)
mUpnpHttpResponse *mupnp_https_request_post(mUpnpHttpRequest *httpReq, const char *ipaddr, int port);
#endif
BOOL mupnp_http_request_read(mUpnpHttpRequest *httpReq, mUpnpSocket *sock);
BOOL mupnp_http_request_postresponse(mUpnpHttpRequest *httpReq, mUpnpHttpResponse *httpRes);
BOOL mupnp_http_request_poststatuscode(mUpnpHttpRequest *httpReq, int httpStatCode);
BOOL mupnp_http_request_postdata(mUpnpHttpRequest *httpReq, void *data, int dataLen);
BOOL mupnp_http_request_postchunkedsize(mUpnpHttpRequest *httpReq, int dataLen);
BOOL mupnp_http_request_postchunkeddata(mUpnpHttpRequest *httpReq, void *data, int dataLen);
BOOL mupnp_http_request_postlastchunk(mUpnpHttpRequest *httpReq);
#define mupnp_http_request_poststring(httpReq, data) mupnp_http_request_postdata(httpReq, data, mupnp_strlen(data))
#define mupnp_http_request_postchunkedstring(httpReq, data) mupnp_http_request_postchunkeddata(httpReq, data, mupnp_strlen(data))

#define mupnp_http_request_postokrequest(httpReq) mupnp_http_request_poststatuscode(httpReq, CG_HTTP_STATUS_OK)
#define mupnp_http_request_postbadrequest(httpReq) mupnp_http_request_poststatuscode(httpReq, CG_HTTP_STATUS_BAD_REQUEST)

#define mupnp_http_request_ismethod(httpReq,value) mupnp_streq(mupnp_http_request_getmethod(httpReq), value)
#define mupnp_http_request_isgetrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_GET)
#define mupnp_http_request_ispostrequest(httpReq) (mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_POST) || mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_MPOST))
#define mupnp_http_request_isheadrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_HEAD)
#define mupnp_http_request_issubscriberequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_SUBSCRIBE)
#define mupnp_http_request_isunsubscriberequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_UNSUBSCRIBE)
#define mupnp_http_request_isnotifyrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_NOTIFY)
#define mupnp_http_request_issoapaction(httpReq) (mupnp_http_packet_hasheader(((mUpnpHttpPacket*)httpReq), CG_HTTP_SOAP_ACTION) || mupnp_streq(mupnp_http_request_getmethod(httpReq), CG_HTTP_MPOST))

#define mupnp_http_request_setcontent(httpReq,value) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpReq),value)
#define mupnp_http_request_setncontent(httpReq,value,len) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpReq),value,len)
#define mupnp_http_request_appendncontent(httpPkt, value, len) mupnp_http_packet_appendncontent(httpPkt, value, len)
#define mupnp_http_request_setcontentpointer(httpReq,value, len) mupnp_http_packet_setcontentpointer(((mUpnpHttpPacket*)httpReq),value, len)
#define mupnp_http_request_getcontent(httpReq) mupnp_http_packet_getcontent(((mUpnpHttpPacket*)httpReq))
#define mupnp_http_request_getheaders(httpReq) mupnp_http_packet_getheaders((mUpnpHttpPacket*)httpReq)
#define mupnp_http_request_getheader(httpReq,name) mupnp_http_packet_getheader((mUpnpHttpPacket*)httpReq,name)
#define mupnp_http_request_setheadervalue(httpReq,name,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq,name,value)
#define mupnp_http_request_setheaderinteger(httpReq,name,value)  mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)httpReq,name,value)
#define mupnp_http_request_setheaderlong(httpReq,name,value)  mupnp_http_packet_setheaderlong((mUpnpHttpPacket*)httpReq,name,value)
#define mupnp_http_request_getheadervalue(httpReq,name) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq,name)
#define mupnp_http_request_getheaderinteger(httpReq,name) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)httpReq,name)
#define mupnp_http_request_getheaderlong(httpReq,name) mupnp_http_packet_getheaderlong((mUpnpHttpPacket*)httpReq,name)

#define mupnp_http_request_setuserdata(httpReq, value) (httpReq->userData = value)
#define mupnp_http_request_getuserdata(httpReq) (httpReq->userData)

/**** Local Address/Port ****/
#define mupnp_http_request_getlocaladdress(httpReq) mupnp_socket_getaddress(httpReq->sock)
#define mupnp_http_request_getlocalport(httpReq) mupnp_socket_getport(httpReq->sock)

/**** Content-Length ****/
#define mupnp_http_request_setcontentlength(httpReq,value) mupnp_http_packet_setcontentlength((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_request_getcontentlength(httpReq) mupnp_http_packet_getcontentlength((mUpnpHttpPacket*)httpReq)

/**** Content-Type ****/
#define mupnp_http_request_setcontenttype(httpReq,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_CONTENT_TYPE,value)
#define mupnp_http_request_getcontenttype(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_CONTENT_TYPE)

/**** Connection ****/
#define mupnp_http_request_setconnection(httpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_CONNECTION, value)
#define mupnp_http_request_getconnection(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_CONNECTION)
#define mupnp_http_request_iskeepaliveconnection(httpReq) mupnp_http_packet_iskeepaliveconnection((mUpnpHttpPacket*)httpReq)

/**** Transfer-Encoding ****/
#define mupnp_http_request_settransferencoding(httpReq, value) mupnp_http_packet_settransferencoding((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_request_gettransferencoding(httpReq) mupnp_http_packet_gettransferencoding((mUpnpHttpPacket*)httpReq)
#define mupnp_http_request_ischunked(httpReq) mupnp_http_packet_ischunked((mUpnpHttpPacket*)httpReq)

/**** Host ****/
#define mupnp_http_request_sethost(httpReq,addr,port) mupnp_http_packet_sethost((mUpnpHttpPacket*)httpReq,addr,port)
#define mupnp_http_request_gethost(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_HOST)

/**** Date ****/
#define mupnp_http_request_setdate(httpReq,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_DATE,value)
#define mupnp_http_request_getdate(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq,CG_HTTP_DATE)

/**** Copy ****/
void mupnp_http_request_copy(mUpnpHttpRequest *destHttpReq, mUpnpHttpRequest *srcHttpReq);

/*** PostURL ****/
#define mupnp_http_request_getposturl(httpReq) (httpReq->postURL)

/**** Print ****/
void mupnp_http_request_print(mUpnpHttpRequest *httpReq);

/**** Timeout ****/
#define mupnp_http_request_settimeout(httpReq,value) (httpReq->timeout = value)
#define mupnp_http_request_gettimeout(httpReq) (httpReq->timeout)

/****************************************
* Function (Response)
****************************************/

mUpnpHttpResponse *mupnp_http_response_new();
void mupnp_http_response_delete(mUpnpHttpResponse *httpRes);
void mupnp_http_response_clear(mUpnpHttpResponse *httpRes);
void mupnp_http_response_setversion(mUpnpHttpResponse *httpRes, const char *version);
char *mupnp_http_response_getversion(mUpnpHttpResponse *httpRes);
void mupnp_http_response_setreasonphrase(mUpnpHttpResponse *httpRes, const char *value);
char *mupnp_http_response_getreasonphrase(mUpnpHttpResponse *httpRes);
void mupnp_http_response_setstatuscode(mUpnpHttpResponse *httpRes, int code);
int mupnp_http_response_getstatuscode(mUpnpHttpResponse *httpRes);
BOOL mupnp_http_response_read(mUpnpHttpResponse *httpRes, mUpnpSocket *sock, BOOL onlyHeader);

/*#define mupnp_http_response_issuccessful(httpRes) ((mupnp_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_OK) ? TRUE : FALSE)*/
#define mupnp_http_response_issuccessful(httpRes) (mupnp_http_response_getstatuscode(httpRes) >= 200 && mupnp_http_response_getstatuscode(httpRes) < 300 )

#define mupnp_http_response_setcontent(httpRes,value) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpRes),value)
#define mupnp_http_response_setncontent(httpRes,value, len) mupnp_http_packet_setncontent(((mUpnpHttpPacket*)httpRes),value, len)
#define mupnp_http_response_appendncontent(httpRes, value, len) mupnp_http_packet_appendncontent(((mUpnpHttpPacket*) httpRes), value, len)

#define mupnp_http_response_setcontentpointer(httpRes,value,len) mupnp_http_packet_setcontentpointer(((mUpnpHttpPacket*)httpRes),value,len)
#define mupnp_http_response_getcontent(httpRes) mupnp_http_packet_getcontent(((mUpnpHttpPacket*)httpRes))
#define mupnp_http_response_getheaders(p) mupnp_http_packet_getheaders((mUpnpHttpPacket*)p)
#define mupnp_http_response_getheader(p,n) mupnp_http_packet_getheader((mUpnpHttpPacket*)p,n)
#define mupnp_http_response_setheadervalue(httpRes,n,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes,n,value)
#define mupnp_http_response_setheaderinteger(httpRes,n,value)  mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)httpRes,n,value)
#define mupnp_http_response_setheaderlong(httpRes,n,value)  mupnp_http_packet_setheaderlong((mUpnpHttpPacket*)httpRes,n,value)
#define mupnp_http_response_getheadervalue(httpRes,n) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes,n)
#define mupnp_http_response_getheaderinteger(httpRes,n) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)httpRes,n)
#define mupnp_http_response_getheaderlong(httpRes,n) mupnp_http_packet_getheaderlong((mUpnpHttpPacket*)httpRes,n)

#define mupnp_http_response_setuserdata(httpRes, value) (httpRes->userData = value)
#define mupnp_http_response_getuserdata(httpRes) (httpRes->userData)

/**** Content-Length ****/
#define mupnp_http_response_setcontentlength(httpRes,value) mupnp_http_packet_setcontentlength((mUpnpHttpPacket*)httpRes, value)
#define mupnp_http_response_getcontentlength(httpRes) mupnp_http_packet_getcontentlength((mUpnpHttpPacket*)httpRes)

/**** Content-Type ****/
#define mupnp_http_response_setcontenttype(httpRes,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_CONTENT_TYPE,value)
#define mupnp_http_response_getcontenttype(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_CONTENT_TYPE)

/**** connection ****/
#define mupnp_http_response_setconnection(httpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_CONNECTION, value)
#define mupnp_http_response_getconnection(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, CG_HTTP_CONNECTION)
#define mupnp_http_response_iskeepaliveconnection(httpRes) mupnp_http_packet_iskeepaliveconnection((mUpnpHttpPacket*)httpRes)

/**** Host ****/
#define mupnp_http_response_sethost(httpRes,addr,port) mupnp_http_packet_sethost((mUpnpHttpPacket*)httpRes,addr,port)
#define mupnp_http_response_gethost(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_HOST)

/**** Date ****/
#define mupnp_http_response_setdate(httpRes,value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_DATE,value)
#define mupnp_http_response_getdate(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes,CG_HTTP_DATE)

/**** Transfer-Encoding ****/
#define mupnp_http_response_settransferencoding(httpReq, value) mupnp_http_packet_settransferencoding((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_response_gettransferencoding(httpReq) mupnp_http_packet_gettransferencoding((mUpnpHttpPacket*)httpReq)
	
/**** Copy ****/
void mupnp_http_response_copy(mUpnpHttpResponse *destHttpRes, mUpnpHttpResponse *srcHttpRes);

/**** Print ****/
void mupnp_http_response_print(mUpnpHttpResponse *httpRes);

/**** Timeout ****/
#define mupnp_http_response_settimeout(httpRes,value) (httpRes->timeout = value)
#define mupnp_http_response_gettimeout(httpRes) (httpRes->timeout)

/****************************************
* Function (Server)
****************************************/

mUpnpHttpServer *mupnp_http_server_new();
void mupnp_http_server_delete(mUpnpHttpServer *httpServer);
BOOL mupnp_http_server_open(mUpnpHttpServer *httpServer, int bindPort, const char *bindAddr);
BOOL mupnp_http_server_close(mUpnpHttpServer *httpServer);
BOOL mupnp_http_server_accept(mUpnpHttpServer *httpServer);
BOOL mupnp_http_server_start(mUpnpHttpServer *httpServer);
BOOL mupnp_http_server_stop(mUpnpHttpServer *httpServer);
void mupnp_http_server_setlistener(mUpnpHttpServer *httpServer, CG_HTTP_LISTENER listener);

#define mupnp_http_server_next(httpServer) (mUpnpHttpServer *)mupnp_list_next((mUpnpList *)httpServer)

#define mupnp_http_server_getsocket(httpServer) (httpServer->sock)
#define mupnp_http_server_isopened(httpServer) ((httpServer->sock != NULL) ? TRUE : FALSE)

#define mupnp_http_server_setuserdata(httpServer, value) (httpServer->userData = value)
#define mupnp_http_server_getuserdata(httpServer) (httpServer->userData)

const char *mupnp_http_getservername(char *buf, size_t bufSize);

/**** Timeout ****/
#define mupnp_http_server_settimeout(httpServer,value) (httpServer->timeout = value)
#define mupnp_http_server_gettimeout(httpServer) (httpServer->timeout)

/**** Mutex ****/
#define mupnp_http_server_lock(httpServer) mupnp_mutex_lock(httpServer->mutex)
#define mupnp_http_server_unlock(httpServer) mupnp_mutex_unlock(httpServer->mutex)

/****************************************
* Function (Server List)
****************************************/

mUpnpHttpServerList *mupnp_http_serverlist_new();
void mupnp_http_serverlist_delete(mUpnpHttpServerList *httpServerList);

#define mupnp_http_serverlist_clear(httpServerList) mupnp_list_clear((mUpnpList *)httpServerList, (CG_LIST_DESTRUCTORFUNC)mupnp_http_server_delete)
#define mupnp_http_serverlist_size(httpServerList) mupnp_list_size((mUpnpList *)httpServerList)
#define mupnp_http_serverlist_gets(httpServerList) (mUpnpHttpServer *)mupnp_list_next((mUpnpList *)httpServerList)
#define mupnp_http_serverlist_add(httpServerList, httpServer) mupnp_list_add((mUpnpList *)httpServerList, (mUpnpList *)httpServer)

BOOL mupnp_http_serverlist_open(mUpnpHttpServerList *httpServerList, int port);
BOOL mupnp_http_serverlist_close(mUpnpHttpServerList *httpServerList);
BOOL mupnp_http_serverlist_start(mUpnpHttpServerList *httpServerList);
BOOL mupnp_http_serverlist_stop(mUpnpHttpServerList *httpServerList);
void mupnp_http_serverlist_setlistener(mUpnpHttpServerList *httpServerList, CG_HTTP_LISTENER listener);
void mupnp_http_serverlist_setuserdata(mUpnpHttpServerList *httpServerList, void *value);

/****************************************
* Function (Date)
****************************************/

const char *mupnp_http_getdate(mUpnpTime sysTime, char *buf, size_t bufSize);

/****************************************
* Persistent connection cache
****************************************/

/** Initialize persistent connection cache. Can be called many times */
BOOL mupnp_http_persistentconnection_init(void);
/** Clear persistent connection cache and free all memory */
void mupnp_http_persistentconnection_clear(void);
/** Lock persistent connection cache. Required for getting, putting and using
    persistent connection from cache */
void mupnp_http_persistentconnection_lock(void);
/** Unlock persistent connection cache. */
void mupnp_http_persistentconnection_unlock(void);
/** Get persistent connection from cache */
void *mupnp_http_persistentconnection_get(char *host, int port);
/** Put connection to persistent connection cache */
BOOL mupnp_http_persistentconnection_put(char *host, int port, void *data);

#ifdef  __cplusplus
}
#endif

#endif

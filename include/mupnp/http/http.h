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

#ifndef _MUPNP_HTTP_HTTP_H_
#define _MUPNP_HTTP_HTTP_H_

#include <mupnp/typedef.h>
#include <mupnp/version.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>
#include <mupnp/util/thread.h>
#include <mupnp/util/time.h>
#include <mupnp/util/mutex.h>
#include <mupnp/net/socket.h>
#include <mupnp/net/url.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_HTTP_READLINE_BUFSIZE 512
#define MUPNP_HTTP_SEVERNAME_MAXLEN 64
#define MUPNP_HTTP_DATE_MAXLEN 128

#define MUPNP_HTTP_REQUESTLINE_DELIM "\r\n "
#define MUPNP_HTTP_STATUSLINE_DELIM "\r\n "
#define MUPNP_HTTP_HEADERLINE_DELIM "\r\n :"

#define MUPNP_HTTP_CR "\r"
#define MUPNP_HTTP_LF "\n"
#define MUPNP_HTTP_CRLF "\r\n"
#define MUPNP_HTTP_SP " "
#define MUPNP_HTTP_COLON ":"

#define MUPNP_HTTP_VER10 "HTTP/1.0"
#define MUPNP_HTTP_VER11 "HTTP/1.1"

#if !defined(MUPNP_HTTP_CURL)
#define MUPNP_HTTP_USERAGENT_DEFAULT "mUPnP-HTTP/" MUPNP_VER
#else
#define MUPNP_HTTP_USERAGENT_DEFAULT "mUPnP-HTTP(libcurl)/" MUPNP_VER
#endif

#define MUPNP_HTTP_DEFAULT_PORT 80
#define MUPNP_HTTS_DEFAULT_PORT 443

#define MUPNP_HTTP_STATUS_CONTINUE 100
#define MUPNP_HTTP_STATUS_OK 200
#define MUPNP_HTTP_STATUS_PARTIAL_CONTENT 206
#define MUPNP_HTTP_STATUS_BAD_REQUEST 400
#define MUPNP_HTTP_STATUS_NOT_FOUND 404
#define MUPNP_HTTP_STATUS_METHOD_NOT_ALLOWED 405
#define MUPNP_HTTP_STATUS_PRECONDITION_FAILED 412
#define MUPNP_HTTP_STATUS_INVALID_RANGE 416
#define MUPNP_HTTP_STATUS_INTERNAL_SERVER_ERROR 500

#define MUPNP_HTTP_POST "POST"
#define MUPNP_HTTP_GET "GET"
#define MUPNP_HTTP_HEAD "HEAD"

#define MUPNP_HTTP_HOST "HOST"
#define MUPNP_HTTP_DATE "Date"
#define MUPNP_HTTP_CACHE_CONTROL "Cache-Control"
#define MUPNP_HTTP_NO_CACHE "no-cache"
#define MUPNP_HTTP_MAX_AGE "max-age"
#define MUPNP_HTTP_CONNECTION "Connection"
#define MUPNP_HTTP_CLOSE "close"
#define MUPNP_HTTP_KEEP_ALIVE "Keep-Alive"
#define MUPNP_HTTP_CONTENT_TYPE "Content-Type"
#define MUPNP_HTTP_CONTENT_LENGTH "Content-Length"
#define MUPNP_HTTP_CONTENT_RANGE "Content-Range"
#define MUPNP_HTTP_CONTENT_RANGE_BYTES "bytes"
#define MUPNP_HTTP_LOCATION "Location"
#define MUPNP_HTTP_SERVER "Server"
#define MUPNP_HTTP_RANGE "Range"
#define MUPNP_HTTP_TRANSFER_ENCODING "Transfer-Encoding"
#define MUPNP_HTTP_CHUNKED "Chunked"
#define MUPNP_HTTP_USERAGENT "User-Agent"

/**** SOAP Extention ****/
#define MUPNP_HTTP_SOAP_ACTION "SOAPACTION"
#define MUPNP_HTTP_SOAP_ACTION_WITH_NS "01-SOAPACTION"
#define MUPNP_HTTP_SOAP_MAN_VALUE "\"http://schemas.xmlsoap.org/soap/envelope/\"; ns=01"

/**** UPnP Extention ****/
#define MUPNP_HTTP_MPOST "M-POST"
#define MUPNP_HTTP_MSEARCH "M-SEARCH"
#define MUPNP_HTTP_NOTIFY "NOTIFY"
#define MUPNP_HTTP_SUBSCRIBE "SUBSCRIBE"
#define MUPNP_HTTP_UNSUBSCRIBE "UNSUBSCRIBE"

#define MUPNP_HTTP_ST "ST"
#define MUPNP_HTTP_MX "MX"
#define MUPNP_HTTP_MAN "MAN"
#define MUPNP_HTTP_NT "NT"
#define MUPNP_HTTP_NTS "NTS"
#define MUPNP_HTTP_USN "USN"
#define MUPNP_HTTP_EXT "EXT"
#define MUPNP_HTTP_SID "SID"
#define MUPNP_HTTP_SEQ "SEQ"
#define MUPNP_HTTP_CALLBACK "CALLBACK"
#define MUPNP_HTTP_TIMEOUT "TIMEOUT"

/**** CURL connection timeout: 2 seconds ****/
#define MUPNP_HTTP_CURL_CONNECTTIMEOUT 2

/**** CURL total timeout: 30 seconds (UPnP DA: whole transfer: 30sec ****/
#define MUPNP_HTTP_CONN_TIMEOUT 30

/* HTTP server - client thread blocking timeout */
#define MUPNP_HTTP_SERVER_READ_TIMEOUT 120

/**** HTTP Status code reason phrases ****/
#define MUPNP_HTTP_REASON_100 "Continue"
#define MUPNP_HTTP_REASON_101 "Switching Protocols"
#define MUPNP_HTTP_REASON_200 "OK"
#define MUPNP_HTTP_REASON_201 "Created"
#define MUPNP_HTTP_REASON_202 "Accepted"
#define MUPNP_HTTP_REASON_203 "Non-Authoritative Information"
#define MUPNP_HTTP_REASON_204 "No Content"
#define MUPNP_HTTP_REASON_205 "Reset Content"
#define MUPNP_HTTP_REASON_206 "Partial Content"
#define MUPNP_HTTP_REASON_300 "Multiple Choices"
#define MUPNP_HTTP_REASON_301 "Moved Permanently"
#define MUPNP_HTTP_REASON_302 "Found"
#define MUPNP_HTTP_REASON_303 "See Other"
#define MUPNP_HTTP_REASON_304 "Not Modified"
#define MUPNP_HTTP_REASON_305 "Use Proxy"
#define MUPNP_HTTP_REASON_307 "Temporary Redirect"
#define MUPNP_HTTP_REASON_400 "Bad Request"
#define MUPNP_HTTP_REASON_401 "Unauthorized"
#define MUPNP_HTTP_REASON_402 "Payment Required"
#define MUPNP_HTTP_REASON_403 "Forbidden"
#define MUPNP_HTTP_REASON_404 "Not Found"
#define MUPNP_HTTP_REASON_405 "Method Not Allowed"
#define MUPNP_HTTP_REASON_406 "Not Acceptable"
#define MUPNP_HTTP_REASON_407 "Proxy Authentication Required"
#define MUPNP_HTTP_REASON_408 "Request Time-out"
#define MUPNP_HTTP_REASON_409 "Conflict"
#define MUPNP_HTTP_REASON_410 "Gone"
#define MUPNP_HTTP_REASON_411 "Length Required"
#define MUPNP_HTTP_REASON_412 "Precondition Failed"
#define MUPNP_HTTP_REASON_413 "Request Entity Too Large"
#define MUPNP_HTTP_REASON_414 "Request-URI Too Large"
#define MUPNP_HTTP_REASON_415 "Unsupported Media Type"
#define MUPNP_HTTP_REASON_416 "Requested range not satisfiable"
#define MUPNP_HTTP_REASON_417 "Expectation Failed"
#define MUPNP_HTTP_REASON_500 "Internal Server Error"
#define MUPNP_HTTP_REASON_501 "Not Implemented"
#define MUPNP_HTTP_REASON_502 "Bad Gateway"
#define MUPNP_HTTP_REASON_503 "Service Unavailable"
#define MUPNP_HTTP_REASON_504 "Gateway Time-out"
#define MUPNP_HTTP_REASON_505 "HTTP Version not supported"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpHttpHeader {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* name;
  mUpnpString* value;
} mUpnpHttpHeader, mUpnpHttpHeaderList;

typedef struct _mUpnpHttpPacket {
  mUpnpHttpHeaderList* headerList;
  mUpnpString* content;
} mUpnpHttpPacket;

typedef struct _mUpnpHttpResponse {
  mUpnpHttpHeaderList* headerList;
  mUpnpString* content;
  mUpnpString* version;
  int statusCode;
  mUpnpString* reasonPhrase;
  void* userData;
  int timeout;
} mUpnpHttpResponse;

typedef struct _mUpnpHttpRequest {
  mUpnpHttpHeaderList* headerList;
  mUpnpString* content;
  mUpnpString* method;
  mUpnpString* uri;
  mUpnpString* version;
  mUpnpString* userAgent;
  mUpnpHttpResponse* httpRes;
  mUpnpSocket* sock;
  mUpnpNetURL* postURL;
  void* userData;
  int timeout;
} mUpnpHttpRequest;

typedef void (*MUPNP_HTTP_LISTENER)(mUpnpHttpRequest*);

typedef struct _mUpnpHttpServer {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpSocket* sock;
  mUpnpThread* acceptThread;
  mUpnpThreadList* clientThreads;
  MUPNP_HTTP_LISTENER listener;
  void* userData;
  int timeout;
  mUpnpMutex* mutex;
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
const char* mupnp_http_statuscode2reasonphrase(const int code);

/****************************************
* Function (Header)
****************************************/

mUpnpHttpHeader* mupnp_http_header_new();
void mupnp_http_header_delete(mUpnpHttpHeader* header);

#define mupnp_http_header_next(header) (mUpnpHttpHeader*) mupnp_list_next((mUpnpList*)header)

void mupnp_http_header_setname(mUpnpHttpHeader* header, const char* name);
const char* mupnp_http_header_getname(mUpnpHttpHeader* header);
void mupnp_http_header_setvalue(mUpnpHttpHeader* header, const char* value);
const char* mupnp_http_header_getvalue(mUpnpHttpHeader* header);

/****************************************
* Function (Header List)
****************************************/

mUpnpHttpHeaderList* mupnp_http_headerlist_new();
void mupnp_http_headerlist_delete(mUpnpHttpHeaderList* headerList);

#define mupnp_http_headerlist_clear(headerList) mupnp_list_clear((mUpnpList*)headerList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_http_header_delete)
#define mupnp_http_headerlist_size(headerList) mupnp_list_size((mUpnpList*)headerList)
#define mupnp_http_headerlist_gets(headerList) (mUpnpHttpHeader*) mupnp_list_next((mUpnpList*)headerList)
#define mupnp_http_headerlist_add(headerList, header) mupnp_list_add((mUpnpList*)headerList, (mUpnpList*)header)

mUpnpHttpHeader* mupnp_http_headerlist_get(mUpnpHttpHeaderList* headerList, const char* name);
void mupnp_http_headerlist_set(mUpnpHttpHeaderList* headerList, const char* name, const char* value);
const char* mupnp_http_headerlist_getvalue(mUpnpHttpHeaderList* headerList, const char* name);

/****************************************
* Function (Packet)
****************************************/

mUpnpHttpPacket* mupnp_http_packet_new();
void mupnp_http_packet_delete(mUpnpHttpPacket* httpPkt);
void mupnp_http_packet_init(mUpnpHttpPacket* httpPkt);
void mupnp_http_packet_clean(mUpnpHttpPacket* httpPkt);
void mupnp_http_packet_clear(mUpnpHttpPacket* httpPkt);

#define mupnp_http_packet_deleteallheaders(httpPkt) mupnp_http_headerlist_gets(httpPkt->headerList)
#define mupnp_http_packet_getnheaders(httpPkt) mupnp_http_headerlist_size(httpPkt->headerList)
#define mupnp_http_packet_getheaders(httpPkt) mupnp_http_headerlist_gets(httpPkt->headerList)
//Theo Beisch - added (mUpnpHttpHeaderList*) cast - as _get is function call
#define mupnp_http_packet_getheader(httpPkt, name) mupnp_http_headerlist_get((mUpnpHttpHeaderList*)httpPkt->headerList, name)
#define mupnp_http_packet_hasheader(httpPkt, name) ((mupnp_http_headerlist_get((mUpnpHttpHeaderList*)httpPkt->headerList, name) != NULL) ? true : false)

#define mupnp_http_packet_hasheadervalue(httpPkt, header, value) (mupnp_streq(mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpPkt, header), value))

#define mupnp_http_packet_addheader(httpPkt, header) mupnp_http_headerlist_add(httpPkt->headerList, header)

void mupnp_http_packet_setheadervalue(mUpnpHttpPacket* httpPkt, const char* name, const char* value);
void mupnp_http_packet_setheaderinteger(mUpnpHttpPacket* httpPkt, const char* name, int value);
void mupnp_http_packet_setheaderlong(mUpnpHttpPacket* httpPkt, const char* name, long value);
void mupnp_http_packet_setheadersizet(mUpnpHttpPacket* httpPkt, const char* name, size_t value);
void mupnp_http_packet_setheaderssizet(mUpnpHttpPacket* httpPkt, const char* name, ssize_t value);

const char* mupnp_http_packet_getheadervalue(mUpnpHttpPacket* httpPkt, const char* name);
int mupnp_http_packet_getheaderinteger(mUpnpHttpPacket* httpPkt, const char* name);
long mupnp_http_packet_getheaderlong(mUpnpHttpPacket* httpPkt, const char* name);
size_t mupnp_http_packet_getheadersizet(mUpnpHttpPacket* httpPkt, const char* name);
ssize_t mupnp_http_packet_getheaderssizet(mUpnpHttpPacket* httpPkt, const char* name);
size_t mupnp_http_packet_getheadersize(mUpnpHttpPacket* httpPkt);

#define mupnp_http_packet_setcontent(httpPkt, value) mupnp_string_setvalue(httpPkt->content, value)
#define mupnp_http_packet_setncontent(httpPkt, value, len) mupnp_string_setnvalue(httpPkt->content, value, len)
#define mupnp_http_packet_appendncontent(httpPkt, value, len) mupnp_string_naddvalue(httpPkt->content, value, len)
#define mupnp_http_packet_setcontentpointer(httpPkt, value, len) mupnp_string_setpointervalue(httpPkt->content, value, len)
#define mupnp_http_packet_getcontent(httpPkt) mupnp_string_getvalue(httpPkt->content)

void mupnp_http_packet_post(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock);
void mupnp_http_packet_read_headers(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, char* lineBuf, size_t lineBufSize);
bool mupnp_http_packet_read_body(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, char* lineBuf, size_t lineBufSize);
bool mupnp_http_packet_read(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, bool onlyHeader, char* lineBuf, size_t lineBufSize);

#define mupnp_http_packet_setcontentlength(httpPkt, value) mupnp_http_packet_setheaderssizet(httpPkt, MUPNP_HTTP_CONTENT_LENGTH, value)
#define mupnp_http_packet_getcontentlength(httpPkt) mupnp_http_packet_getheaderssizet(httpPkt, MUPNP_HTTP_CONTENT_LENGTH)

/**** Connection ****/
#define mupnp_http_packet_setconnection(httpPkt, value) mupnp_http_packet_setheadervalue(httpPkt, MUPNP_HTTP_CONNECTION, value)
#define mupnp_http_packet_getconnection(httpPkt) mupnp_http_packet_getheadervalue(httpPkt, MUPNP_HTTP_CONNECTION)
#define mupnp_http_packet_iskeepaliveconnection(httpPkt) (mupnp_strcaseeq(mupnp_http_packet_getconnection(httpPkt), MUPNP_HTTP_CLOSE) == false)

/**** Transfer-Encoding ****/
#define mupnp_http_packet_settransferencoding(httpPkt, value) mupnp_http_packet_setheadervalue(httpPkt, MUPNP_HTTP_TRANSFER_ENCODING, value)
#define mupnp_http_packet_gettransferencoding(httpPkt) mupnp_http_packet_getheadervalue(httpPkt, MUPNP_HTTP_TRANSFER_ENCODING)
#define mupnp_http_packet_ischunked(httpPkt) mupnp_strcaseeq(mupnp_http_packet_gettransferencoding(httpPkt), MUPNP_HTTP_CHUNKED)

/**** Host ****/
void mupnp_http_packet_sethost(mUpnpHttpPacket* httpPkt, const char* addr, int port);
#define mupnp_http_packet_gethost(httpPkt) mupnp_http_packet_getheaderlong(httpPkt, MUPNP_HTTP_HOST)

/**** Copy ****/
void mupnp_http_packet_copy(mUpnpHttpPacket* destHttpPkt, mUpnpHttpPacket* srcHttpPkt);

/**** Print ****/
void mupnp_http_packet_print(mUpnpHttpPacket* httpPkt);

/****************************************
* Function (Request)
****************************************/

mUpnpHttpRequest* mupnp_http_request_new();
void mupnp_http_request_delete(mUpnpHttpRequest* httpReq);
void mupnp_http_request_clear(mUpnpHttpRequest* httpReq);
void mupnp_http_request_setmethod(mUpnpHttpRequest* httpReq, const char* method);
char* mupnp_http_request_getmethod(mUpnpHttpRequest* httpReq);
void mupnp_http_request_setversion(mUpnpHttpRequest* httpReq, const char* version);
char* mupnp_http_request_getversion(mUpnpHttpRequest* httpReqest);
void mupnp_http_request_setuseragent(mUpnpHttpRequest* httpReq, const char* version);
char* mupnp_http_request_getuseragent(mUpnpHttpRequest* httpReqest);
void mupnp_http_request_addtouseragent(mUpnpHttpRequest* httpReq, const char* value);
void mupnp_http_request_seturi(mUpnpHttpRequest* httpReq, const char* uri);
char* mupnp_http_request_geturi(mUpnpHttpRequest* httpReq);
void mupnp_http_request_setsocket(mUpnpHttpRequest* httpReq, mUpnpSocket* sock);
mUpnpSocket* mupnp_http_request_getsocket(mUpnpHttpRequest* httpReq);
#define mupnp_http_request_closesocket(httpReq) mupnp_socket_close(mupnp_http_request_getsocket(httpReq))
mUpnpHttpResponse* mupnp_http_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port);
#if defined(MUPNP_USE_OPENSSL)
mUpnpHttpResponse* mupnp_https_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port);
#endif
bool mupnp_http_request_read(mUpnpHttpRequest* httpReq, mUpnpSocket* sock);
bool mupnp_http_request_postresponse(mUpnpHttpRequest* httpReq, mUpnpHttpResponse* httpRes);
bool mupnp_http_request_poststatuscode(mUpnpHttpRequest* httpReq, int httpStatCode);
bool mupnp_http_request_postdata(mUpnpHttpRequest* httpReq, void* data, int dataLen);
bool mupnp_http_request_postchunkedsize(mUpnpHttpRequest* httpReq, int dataLen);
bool mupnp_http_request_postchunkeddata(mUpnpHttpRequest* httpReq, void* data, int dataLen);
bool mupnp_http_request_postlastchunk(mUpnpHttpRequest* httpReq);
#define mupnp_http_request_poststring(httpReq, data) mupnp_http_request_postdata(httpReq, data, mupnp_strlen(data))
#define mupnp_http_request_postchunkedstring(httpReq, data) mupnp_http_request_postchunkeddata(httpReq, data, mupnp_strlen(data))

#define mupnp_http_request_postokrequest(httpReq) mupnp_http_request_poststatuscode(httpReq, MUPNP_HTTP_STATUS_OK)
#define mupnp_http_request_postbadrequest(httpReq) mupnp_http_request_poststatuscode(httpReq, MUPNP_HTTP_STATUS_BAD_REQUEST)

#define mupnp_http_request_ismethod(httpReq, value) mupnp_streq(mupnp_http_request_getmethod(httpReq), value)
#define mupnp_http_request_isgetrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_GET)
#define mupnp_http_request_ispostrequest(httpReq) (mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_POST) || mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_MPOST))
#define mupnp_http_request_isheadrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_HEAD)
#define mupnp_http_request_issubscriberequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_SUBSCRIBE)
#define mupnp_http_request_isunsubscriberequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_UNSUBSCRIBE)
#define mupnp_http_request_isnotifyrequest(httpReq) mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_NOTIFY)
#define mupnp_http_request_issoapaction(httpReq) (mupnp_http_packet_hasheader(((mUpnpHttpPacket*)httpReq), MUPNP_HTTP_SOAP_ACTION) || mupnp_streq(mupnp_http_request_getmethod(httpReq), MUPNP_HTTP_MPOST))

#define mupnp_http_request_setcontent(httpReq, value) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpReq), value)
#define mupnp_http_request_setncontent(httpReq, value, len) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpReq), value, len)
#define mupnp_http_request_appendncontent(httpPkt, value, len) mupnp_http_packet_appendncontent(httpPkt, value, len)
#define mupnp_http_request_setcontentpointer(httpReq, value, len) mupnp_http_packet_setcontentpointer(((mUpnpHttpPacket*)httpReq), value, len)
#define mupnp_http_request_getcontent(httpReq) mupnp_http_packet_getcontent(((mUpnpHttpPacket*)httpReq))
#define mupnp_http_request_getheaders(httpReq) mupnp_http_packet_getheaders((mUpnpHttpPacket*)httpReq)
#define mupnp_http_request_getheader(httpReq, name) mupnp_http_packet_getheader((mUpnpHttpPacket*)httpReq, name)
#define mupnp_http_request_setheadervalue(httpReq, name, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq, name, value)
#define mupnp_http_request_setheaderinteger(httpReq, name, value) mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)httpReq, name, value)
#define mupnp_http_request_setheaderlong(httpReq, name, value) mupnp_http_packet_setheaderlong((mUpnpHttpPacket*)httpReq, name, value)
#define mupnp_http_request_getheadervalue(httpReq, name) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq, name)
#define mupnp_http_request_getheaderinteger(httpReq, name) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)httpReq, name)
#define mupnp_http_request_getheaderlong(httpReq, name) mupnp_http_packet_getheaderlong((mUpnpHttpPacket*)httpReq, name)

#define mupnp_http_request_setuserdata(httpReq, value) (httpReq->userData = value)
#define mupnp_http_request_getuserdata(httpReq) (httpReq->userData)

/**** Local Address/Port ****/
#define mupnp_http_request_getlocaladdress(httpReq) mupnp_socket_getaddress(httpReq->sock)
#define mupnp_http_request_getlocalport(httpReq) mupnp_socket_getport(httpReq->sock)

/**** Content-Length ****/
#define mupnp_http_request_setcontentlength(httpReq, value) mupnp_http_packet_setcontentlength((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_request_getcontentlength(httpReq) mupnp_http_packet_getcontentlength((mUpnpHttpPacket*)httpReq)

/**** Content-Type ****/
#define mupnp_http_request_setcontenttype(httpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_TYPE, value)
#define mupnp_http_request_getcontenttype(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_TYPE)

/**** Connection ****/
#define mupnp_http_request_setconnection(httpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONNECTION, value)
#define mupnp_http_request_getconnection(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONNECTION)
#define mupnp_http_request_iskeepaliveconnection(httpReq) mupnp_http_packet_iskeepaliveconnection((mUpnpHttpPacket*)httpReq)

/**** Transfer-Encoding ****/
#define mupnp_http_request_settransferencoding(httpReq, value) mupnp_http_packet_settransferencoding((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_request_gettransferencoding(httpReq) mupnp_http_packet_gettransferencoding((mUpnpHttpPacket*)httpReq)
#define mupnp_http_request_ischunked(httpReq) mupnp_http_packet_ischunked((mUpnpHttpPacket*)httpReq)

/**** Host ****/
#define mupnp_http_request_sethost(httpReq, addr, port) mupnp_http_packet_sethost((mUpnpHttpPacket*)httpReq, addr, port)
#define mupnp_http_request_gethost(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_HOST)

/**** Date ****/
#define mupnp_http_request_setdate(httpReq, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_DATE, value)
#define mupnp_http_request_getdate(httpReq) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_DATE)

/**** Copy ****/
void mupnp_http_request_copy(mUpnpHttpRequest* destHttpReq, mUpnpHttpRequest* srcHttpReq);

/*** PostURL ****/
#define mupnp_http_request_getposturl(httpReq) (httpReq->postURL)

/**** Print ****/
void mupnp_http_request_print(mUpnpHttpRequest* httpReq);

/**** Timeout ****/
#define mupnp_http_request_settimeout(httpReq, value) (httpReq->timeout = value)
#define mupnp_http_request_gettimeout(httpReq) (httpReq->timeout)

/****************************************
* Function (Response)
****************************************/

mUpnpHttpResponse* mupnp_http_response_new();
void mupnp_http_response_delete(mUpnpHttpResponse* httpRes);
void mupnp_http_response_clear(mUpnpHttpResponse* httpRes);
void mupnp_http_response_setversion(mUpnpHttpResponse* httpRes, const char* version);
char* mupnp_http_response_getversion(mUpnpHttpResponse* httpRes);
void mupnp_http_response_setreasonphrase(mUpnpHttpResponse* httpRes, const char* value);
char* mupnp_http_response_getreasonphrase(mUpnpHttpResponse* httpRes);
void mupnp_http_response_setstatuscode(mUpnpHttpResponse* httpRes, int code);
int mupnp_http_response_getstatuscode(mUpnpHttpResponse* httpRes);
bool mupnp_http_response_read(mUpnpHttpResponse* httpRes, mUpnpSocket* sock, bool onlyHeader);

/*#define mupnp_http_response_issuccessful(httpRes) ((mupnp_http_response_getstatuscode(httpRes) == MUPNP_HTTP_STATUS_OK) ? true : false)*/
#define mupnp_http_response_issuccessful(httpRes) (mupnp_http_response_getstatuscode(httpRes) >= 200 && mupnp_http_response_getstatuscode(httpRes) < 300)

#define mupnp_http_response_setcontent(httpRes, value) mupnp_http_packet_setcontent(((mUpnpHttpPacket*)httpRes), value)
#define mupnp_http_response_setncontent(httpRes, value, len) mupnp_http_packet_setncontent(((mUpnpHttpPacket*)httpRes), value, len)
#define mupnp_http_response_appendncontent(httpRes, value, len) mupnp_http_packet_appendncontent(((mUpnpHttpPacket*)httpRes), value, len)

#define mupnp_http_response_setcontentpointer(httpRes, value, len) mupnp_http_packet_setcontentpointer(((mUpnpHttpPacket*)httpRes), value, len)
#define mupnp_http_response_getcontent(httpRes) mupnp_http_packet_getcontent(((mUpnpHttpPacket*)httpRes))
#define mupnp_http_response_getheaders(p) mupnp_http_packet_getheaders((mUpnpHttpPacket*)p)
#define mupnp_http_response_getheader(p, n) mupnp_http_packet_getheader((mUpnpHttpPacket*)p, n)
#define mupnp_http_response_setheadervalue(httpRes, n, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes, n, value)
#define mupnp_http_response_setheaderinteger(httpRes, n, value) mupnp_http_packet_setheaderinteger((mUpnpHttpPacket*)httpRes, n, value)
#define mupnp_http_response_setheaderlong(httpRes, n, value) mupnp_http_packet_setheaderlong((mUpnpHttpPacket*)httpRes, n, value)
#define mupnp_http_response_getheadervalue(httpRes, n) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, n)
#define mupnp_http_response_getheaderinteger(httpRes, n) mupnp_http_packet_getheaderinteger((mUpnpHttpPacket*)httpRes, n)
#define mupnp_http_response_getheaderlong(httpRes, n) mupnp_http_packet_getheaderlong((mUpnpHttpPacket*)httpRes, n)

#define mupnp_http_response_setuserdata(httpRes, value) (httpRes->userData = value)
#define mupnp_http_response_getuserdata(httpRes) (httpRes->userData)

/**** Content-Length ****/
#define mupnp_http_response_setcontentlength(httpRes, value) mupnp_http_packet_setcontentlength((mUpnpHttpPacket*)httpRes, value)
#define mupnp_http_response_getcontentlength(httpRes) mupnp_http_packet_getcontentlength((mUpnpHttpPacket*)httpRes)

/**** Content-Type ****/
#define mupnp_http_response_setcontenttype(httpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_CONTENT_TYPE, value)
#define mupnp_http_response_getcontenttype(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_CONTENT_TYPE)

/**** connection ****/
#define mupnp_http_response_setconnection(httpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_CONNECTION, value)
#define mupnp_http_response_getconnection(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_CONNECTION)
#define mupnp_http_response_iskeepaliveconnection(httpRes) mupnp_http_packet_iskeepaliveconnection((mUpnpHttpPacket*)httpRes)

/**** Host ****/
#define mupnp_http_response_sethost(httpRes, addr, port) mupnp_http_packet_sethost((mUpnpHttpPacket*)httpRes, addr, port)
#define mupnp_http_response_gethost(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_HOST)

/**** Date ****/
#define mupnp_http_response_setdate(httpRes, value) mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_DATE, value)
#define mupnp_http_response_getdate(httpRes) mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)httpRes, MUPNP_HTTP_DATE)

/**** Transfer-Encoding ****/
#define mupnp_http_response_settransferencoding(httpReq, value) mupnp_http_packet_settransferencoding((mUpnpHttpPacket*)httpReq, value)
#define mupnp_http_response_gettransferencoding(httpReq) mupnp_http_packet_gettransferencoding((mUpnpHttpPacket*)httpReq)

/**** Copy ****/
void mupnp_http_response_copy(mUpnpHttpResponse* destHttpRes, mUpnpHttpResponse* srcHttpRes);

/**** Print ****/
void mupnp_http_response_print(mUpnpHttpResponse* httpRes);

/**** Timeout ****/
#define mupnp_http_response_settimeout(httpRes, value) (httpRes->timeout = value)
#define mupnp_http_response_gettimeout(httpRes) (httpRes->timeout)

/****************************************
* Function (Server)
****************************************/

mUpnpHttpServer* mupnp_http_server_new();
void mupnp_http_server_delete(mUpnpHttpServer* httpServer);
bool mupnp_http_server_open(mUpnpHttpServer* httpServer, int bindPort, const char* bindAddr);
bool mupnp_http_server_close(mUpnpHttpServer* httpServer);
bool mupnp_http_server_accept(mUpnpHttpServer* httpServer);
bool mupnp_http_server_start(mUpnpHttpServer* httpServer);
bool mupnp_http_server_stop(mUpnpHttpServer* httpServer);
void mupnp_http_server_setlistener(mUpnpHttpServer* httpServer, MUPNP_HTTP_LISTENER listener);

#define mupnp_http_server_next(httpServer) (mUpnpHttpServer*) mupnp_list_next((mUpnpList*)httpServer)

#define mupnp_http_server_getsocket(httpServer) (httpServer->sock)
#define mupnp_http_server_isopened(httpServer) ((httpServer->sock != NULL) ? true : false)

#define mupnp_http_server_setuserdata(httpServer, value) (httpServer->userData = value)
#define mupnp_http_server_getuserdata(httpServer) (httpServer->userData)

const char* mupnp_http_getservername(char* buf, size_t bufSize);

/**** Timeout ****/
#define mupnp_http_server_settimeout(httpServer, value) (httpServer->timeout = value)
#define mupnp_http_server_gettimeout(httpServer) (httpServer->timeout)

/**** Mutex ****/
#define mupnp_http_server_lock(httpServer) mupnp_mutex_lock(httpServer->mutex)
#define mupnp_http_server_unlock(httpServer) mupnp_mutex_unlock(httpServer->mutex)

/****************************************
* Function (Server List)
****************************************/

mUpnpHttpServerList* mupnp_http_serverlist_new();
void mupnp_http_serverlist_delete(mUpnpHttpServerList* httpServerList);

#define mupnp_http_serverlist_clear(httpServerList) mupnp_list_clear((mUpnpList*)httpServerList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_http_server_delete)
#define mupnp_http_serverlist_size(httpServerList) mupnp_list_size((mUpnpList*)httpServerList)
#define mupnp_http_serverlist_gets(httpServerList) (mUpnpHttpServer*) mupnp_list_next((mUpnpList*)httpServerList)
#define mupnp_http_serverlist_add(httpServerList, httpServer) mupnp_list_add((mUpnpList*)httpServerList, (mUpnpList*)httpServer)

bool mupnp_http_serverlist_open(mUpnpHttpServerList* httpServerList, int port);
bool mupnp_http_serverlist_close(mUpnpHttpServerList* httpServerList);
bool mupnp_http_serverlist_start(mUpnpHttpServerList* httpServerList);
bool mupnp_http_serverlist_stop(mUpnpHttpServerList* httpServerList);
void mupnp_http_serverlist_setlistener(mUpnpHttpServerList* httpServerList, MUPNP_HTTP_LISTENER listener);
void mupnp_http_serverlist_setuserdata(mUpnpHttpServerList* httpServerList, void* value);

/****************************************
* Function (Date)
****************************************/

const char* mupnp_http_getdate(mUpnpTime sysTime, char* buf, size_t bufSize);

/****************************************
* Persistent connection cache
****************************************/

/** Initialize persistent connection cache. Can be called many times */
bool mupnp_http_persistentconnection_init(void);
/** Clear persistent connection cache and free all memory */
void mupnp_http_persistentconnection_clear(void);
/** Lock persistent connection cache. Required for getting, putting and using
    persistent connection from cache */
void mupnp_http_persistentconnection_lock(void);
/** Unlock persistent connection cache. */
void mupnp_http_persistentconnection_unlock(void);
/** Get persistent connection from cache */
void* mupnp_http_persistentconnection_get(char* host, int port);
/** Put connection to persistent connection cache */
bool mupnp_http_persistentconnection_put(char* host, int port, void* data);

#ifdef __cplusplus
}
#endif

#endif

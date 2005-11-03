/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: chttp_request.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*	10/25/05
*		- Added support for libcurl in cg_http_request_post().
*	10/26/05
*		- Changed to use CURLOPT_CUSTOMREQUEST instead of CURLOPT_POST in cg_http_request_post().
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/net/csocket.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef CG_HTTP_CURL
#include <curl/curl.h>
#endif

#ifdef SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

long int cg_total_elapsed_time;
#endif

/****************************************
* cg_http_request_new
****************************************/

CgHttpRequest *cg_http_request_new()
{
	CgHttpRequest *httpReq = (CgHttpRequest *)malloc(sizeof(CgHttpRequest));
	
	cg_http_packet_init((CgHttpPacket *)httpReq);
	httpReq->method = cg_string_new();
	httpReq->version = cg_string_new();
	httpReq->uri = cg_string_new();
	httpReq->httpRes = cg_http_response_new();
	httpReq->postURL = cg_net_url_new();
	
	cg_http_request_setversion(httpReq, CG_HTTP_VER10);
	cg_http_request_setsocket(httpReq, NULL);
	cg_http_request_setuserdata(httpReq, NULL);
	
	return httpReq;
}

/****************************************
* cg_http_request_delete
****************************************/

void cg_http_request_delete(CgHttpRequest *httpReq)
{
	cg_http_packet_clean((CgHttpPacket *)httpReq);
	cg_string_delete(httpReq->method);
	cg_string_delete(httpReq->version);
	cg_string_delete(httpReq->uri);
	cg_http_response_delete(httpReq->httpRes);
	cg_net_url_delete(httpReq->postURL);
	
	free(httpReq);
}

/****************************************
* cg_http_request_clear
****************************************/

void cg_http_request_clear(CgHttpRequest *httpReq)
{
	cg_http_packet_clear((CgHttpPacket *)httpReq);
	cg_http_request_setmethod(httpReq, NULL);
	cg_http_request_seturi(httpReq, NULL);
	cg_http_request_setversion(httpReq, NULL);
	cg_net_url_clear(httpReq->postURL);
	cg_http_request_setuserdata(httpReq, NULL);
}

/****************************************
* cg_http_request_setmethod
****************************************/

void cg_http_request_setmethod(CgHttpRequest *httpReq, char *value)
{
	cg_string_setvalue(httpReq->method, value);
}

/****************************************
* cg_http_request_setversion
****************************************/

void cg_http_request_setversion(CgHttpRequest *httpReq, char *value)
{
	cg_string_setvalue(httpReq->version, value);
}

/****************************************
* cg_http_request_seturi
****************************************/

void cg_http_request_seturi(CgHttpRequest *httpReq, char *value)
{
	cg_string_setvalue(httpReq->uri, value);
}

/****************************************
* cg_http_request_getmethod
****************************************/

char *cg_http_request_getmethod(CgHttpRequest *httpReq)
{
	return cg_string_getvalue(httpReq->method);
}

/****************************************
* cg_http_request_getversion
****************************************/

char *cg_http_request_getversion(CgHttpRequest *httpReq)
{
	return cg_string_getvalue(httpReq->version);
}

/****************************************
* cg_http_request_geturi
****************************************/

char *cg_http_request_geturi(CgHttpRequest *httpReq)
{
	return cg_string_getvalue(httpReq->uri);
}

/****************************************
* cg_http_request_setsocket
****************************************/

void cg_http_request_setsocket(CgHttpRequest *httpReq, CgSocket *sock)
{
	httpReq->sock = sock;
}

/****************************************
* cg_http_request_getsocket
****************************************/

CgSocket *cg_http_request_getsocket(CgHttpRequest *httpReq)
{
	return httpReq->sock;
}

/****************************************
* cg_http_request_post
****************************************/

#if !defined(CG_HTTP_CURL)

CgHttpResponse *cg_http_request_post(CgHttpRequest *httpReq, char *ipaddr, int port)
{

	CgSocket *sock;
	char *method, *uri, *version;
#ifdef SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
#endif		
	cg_http_response_clear(httpReq->httpRes);
	
	sock = cg_socket_stream_new();
	if (cg_socket_connect(sock, ipaddr, port) == FALSE) {
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}

	cg_http_request_sethost(httpReq, ipaddr, port);

	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);	
	version = cg_http_request_getversion(httpReq);

	if (method == NULL || uri == NULL || version == NULL) {
		cg_socket_close(sock);
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}	
	
#ifdef SHOW_TIMINGS	
	printf("\nRequest: %s%s%s:%d%s%s%s\n", method, CG_HTTP_SP, ipaddr, port, uri, CG_HTTP_SP, version);
	gettimeofday(&start_time, NULL);
#endif
	/**** send first line ****/
	cg_socket_write(sock, method, cg_strlen(method));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, uri, cg_strlen(uri));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, version, cg_strlen(version));
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpReq, sock);
	
	/**** read response ****/
	cg_http_response_read(httpReq->httpRes, sock);

#ifdef SHOW_TIMINGS	
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &start_time, &elapsed_time);
	printf("Getting HTTP-response completed. Elapsed time: "
	       "%ld msec\n", ((elapsed_time.tv_sec*1000) + 
			      (elapsed_time.tv_usec/1000)));
	cg_total_elapsed_time += (elapsed_time.tv_sec*1000000)+
				 (elapsed_time.tv_usec);
#endif
	cg_socket_close(sock);
	cg_socket_delete(sock);	
	
  return httpReq->httpRes;
}

#endif

/****************************************
* cg_http_request_post (libcurl)
****************************************/

#if defined(CG_HTTP_CURL)

static size_t cg_http_request_post_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
	CgHttpResponse *httpRes;
	size_t realsize;

	httpRes = (CgHttpResponse *)data;
	realsize = size * nmemb;
	cg_http_response_setncontent(httpRes, ptr, realsize);
	cg_http_response_setcontentlength(httpRes, realsize);

	return realsize;
}

CgHttpResponse *cg_http_request_post(CgHttpRequest *httpReq, char *ipaddr, int port)
{
	CgHttpResponse *httpRes;
	CURL *curl;
	CgHttpHeader *reqHeader;
	struct curl_slist *curlHeaderList; 
	CgString *headerStr;
	CURLcode res;
	char *uri, *method;
	char url[CG_NET_URI_MAXLEN];
	long retcode;

	httpRes = httpReq->httpRes;

	curl = curl_easy_init();
	if (curl == NULL)
		return httpReq->httpRes;		

	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);	

	/**** method ****/
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);

	/**** url ****/
	cg_net_gethosturl(ipaddr, port, uri, url, sizeof(url));
	curl_easy_setopt(curl, CURLOPT_URL, url);

	/**** header ****/
	curlHeaderList = NULL;
	headerStr = cg_string_new();
	for (reqHeader = cg_http_request_getheaders(httpReq); reqHeader; reqHeader = cg_http_header_next(reqHeader)) {
		cg_string_clear(headerStr);
		cg_string_addvalue(headerStr, cg_http_header_getname(reqHeader));
		cg_string_addvalue(headerStr, CG_HTTP_COLON CG_HTTP_SP);
		cg_string_addvalue(headerStr, cg_http_header_getvalue(reqHeader));
		curlHeaderList = curl_slist_append(curlHeaderList, cg_string_getvalue(headerStr));
	}
	cg_string_delete(headerStr);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaderList);

	/**** content ****/
	if (cg_http_request_ispostrequest(httpReq) == TRUE) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cg_http_request_getcontent(httpReq));
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, cg_http_request_getcontentlength(httpReq));
	}

	/**** response callback ****/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cg_http_request_post_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)httpRes);

	/**** useragent ****/
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	res = curl_easy_perform(curl);
    
	curl_slist_free_all(curlHeaderList); 

	curl_easy_getinfo (curl, CURLINFO_HTTP_CODE, &retcode);
	cg_http_response_setstatuscode(httpRes, retcode);

	curl_easy_cleanup(curl);

	return httpReq->httpRes;
}

#endif

/****************************************
* cg_http_request_read
****************************************/

BOOL cg_http_request_read(CgHttpRequest *httpReq, CgSocket *sock)
{
	char lineBuf[CG_HTTP_READLINE_BUFSIZE];
	CgStringTokenizer *strTok;
	int readLen;
	
	cg_http_request_clear(httpReq);

	readLen = cg_socket_readline(sock, lineBuf, sizeof(lineBuf));
	if (readLen <= 0)
		return FALSE;

	strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_STATUSLINE_DELIM);
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_setmethod(httpReq, cg_string_tokenizer_nexttoken(strTok));
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_seturi(httpReq, cg_string_tokenizer_nexttoken(strTok));
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_setversion(httpReq, cg_string_tokenizer_nexttoken(strTok));
	cg_string_tokenizer_delete(strTok);

	/* Read headers */
	cg_http_packet_read_headers((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf));
	
	/* HTTP-request must have Content-Length or Transfer-Encoding header
	   in order to have body */
	if (cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_CONTENT_LENGTH) ||
	    cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_TRANSFER_ENCODING))
		cg_http_packet_read_body((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf));
	
	return TRUE;
}

/****************************************
* cg_http_response_postresponse
****************************************/

BOOL cg_http_request_postresponse(CgHttpRequest *httpReq, CgHttpResponse *httpRes)
{
	CgSocket *sock;
	char httpDate[CG_HTTP_DATE_MAXLEN];
	char *version, *reasonPhrase;
	int statusCode;
	char statusCodeBuf[CG_STRING_INTEGER_BUFLEN];
	
	sock = cg_http_request_getsocket(httpReq);
	
	cg_http_response_setdate(httpRes, cg_http_getdate(cg_getcurrentsystemtime(), httpDate, sizeof(httpDate)));

	version = cg_http_response_getversion(httpRes);
	statusCode = cg_http_response_getstatuscode(httpRes);	
	reasonPhrase = cg_http_response_getreasonphrase(httpRes);

	if (version == NULL || reasonPhrase == NULL)
		return FALSE;		

	cg_int2str(statusCode, statusCodeBuf, sizeof(statusCodeBuf));
		
	/**** send first line ****/
	cg_socket_write(sock, version, cg_strlen(version));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, statusCodeBuf, cg_strlen(statusCodeBuf));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, reasonPhrase, cg_strlen(reasonPhrase));
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpRes, sock);
	
	return TRUE;
}

/****************************************
* cg_http_request_poststatuscode
****************************************/

BOOL cg_http_request_poststatuscode(CgHttpRequest *httpReq, int httpStatCode)
{
	CgHttpResponse *httpRes;
	BOOL postRet;

	httpRes = cg_http_response_new();
	cg_http_response_setstatuscode(httpRes, httpStatCode);
	cg_http_response_setcontentlength(httpRes, 0);
	postRet = cg_http_request_postresponse(httpReq, httpRes);
	cg_http_response_delete(httpRes);

	return postRet;
}

/****************************************
* cg_http_response_copy
****************************************/

void cg_http_request_copy(CgHttpRequest *destHttpReq, CgHttpRequest *srcHttpReq)
{
	cg_http_request_setmethod(destHttpReq, cg_http_request_getmethod(srcHttpReq));
	cg_http_request_seturi(destHttpReq, cg_http_request_geturi(srcHttpReq));
	cg_http_request_setversion(destHttpReq, cg_http_request_getversion(srcHttpReq));

	cg_http_packet_copy((CgHttpPacket *)destHttpReq, (CgHttpPacket *)srcHttpReq);
}

/****************************************
* cg_http_request_print
****************************************/

void cg_http_request_print(CgHttpRequest *httpReq)
{
	printf("%s %s %s\n",
		cg_http_request_getmethod(httpReq),
		cg_http_request_geturi(httpReq),
		cg_http_request_getversion(httpReq));

	cg_http_packet_print((CgHttpPacket *)httpReq);
}

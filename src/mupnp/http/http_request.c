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

#include <mupnp/util/list.h>
#include <mupnp/http/http.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>
#include <mupnp/util/thread.h>

#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef MUPNP_HTTP_CURL
#include <curl/curl.h>
#endif

#ifdef MUPNP_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

long int mupnp_total_elapsed_time;
#endif

/****************************************
* mupnp_http_request_new
****************************************/

mUpnpHttpRequest* mupnp_http_request_new()
{
  mUpnpHttpRequest* httpReq;

  mupnp_log_debug_l4("Entering...\n");

  httpReq = (mUpnpHttpRequest*)malloc(sizeof(mUpnpHttpRequest));

  if (NULL != httpReq) {
    mupnp_http_packet_init((mUpnpHttpPacket*)httpReq);
    httpReq->method = mupnp_string_new();
    httpReq->version = mupnp_string_new();
    httpReq->uri = mupnp_string_new();
    httpReq->userAgent = mupnp_string_new();
    httpReq->httpRes = mupnp_http_response_new();
    httpReq->postURL = mupnp_net_url_new();

    mupnp_http_request_setversion(httpReq, MUPNP_HTTP_VER11);
    mupnp_http_request_setuseragent(httpReq, MUPNP_HTTP_USERAGENT_DEFAULT);

    mupnp_http_request_setsocket(httpReq, NULL);
    mupnp_http_request_setuserdata(httpReq, NULL);

    mupnp_http_request_settimeout(httpReq, MUPNP_HTTP_CONN_TIMEOUT);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return httpReq;
}

/****************************************
* mupnp_http_request_delete
****************************************/

void mupnp_http_request_delete(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clean((mUpnpHttpPacket*)httpReq);
  mupnp_string_delete(httpReq->method);
  mupnp_string_delete(httpReq->version);
  mupnp_string_delete(httpReq->uri);
  /* ADD Fabrice Fontaine Orange 23/04/2007 */
  /* Memory leak correction */
  mupnp_string_delete(httpReq->userAgent);
  /* ADD END Fabrice Fontaine Orange 23/04/2007 */
  mupnp_http_response_delete(httpReq->httpRes);
  mupnp_net_url_delete(httpReq->postURL);

  free(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_clear
****************************************/

void mupnp_http_request_clear(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clear((mUpnpHttpPacket*)httpReq);
  mupnp_http_request_setmethod(httpReq, NULL);
  mupnp_http_request_seturi(httpReq, NULL);
  mupnp_http_request_setversion(httpReq, NULL);
  mupnp_net_url_clear(httpReq->postURL);
  mupnp_http_request_setuserdata(httpReq, NULL);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_setmethod
****************************************/

void mupnp_http_request_setmethod(mUpnpHttpRequest* httpReq, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpReq->method, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_setversion
****************************************/

void mupnp_http_request_setversion(mUpnpHttpRequest* httpReq, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpReq->version, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_setuseragent
****************************************/

void mupnp_http_request_setuseragent(mUpnpHttpRequest* httpReq, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpReq->userAgent, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_setuseragent
****************************************/

void mupnp_http_request_addtouseragent(mUpnpHttpRequest* httpReq, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_addvalue(httpReq->userAgent, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_seturi
****************************************/

void mupnp_http_request_seturi(mUpnpHttpRequest* httpReq, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpReq->uri, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_getmethod
****************************************/

char* mupnp_http_request_getmethod(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(httpReq->method);
}

/****************************************
* mupnp_http_request_getversion
****************************************/

char* mupnp_http_request_getversion(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(httpReq->version);
}

/****************************************
* mupnp_http_request_getversion
****************************************/

char* mupnp_http_request_getuseragent(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(httpReq->userAgent);
}
/****************************************
* mupnp_http_request_geturi
****************************************/

char* mupnp_http_request_geturi(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(httpReq->uri);
}

/****************************************
* mupnp_http_request_setsocket
****************************************/

void mupnp_http_request_setsocket(mUpnpHttpRequest* httpReq, mUpnpSocket* sock)
{
  mupnp_log_debug_l4("Entering...\n");

  httpReq->sock = sock;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_getsocket
****************************************/

mUpnpSocket* mupnp_http_request_getsocket(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

  return httpReq->sock;
}

/****************************************
* mupnp_http_request_post
****************************************/

#if !defined(MUPNP_HTTP_CURL)

mUpnpHttpResponse* mupnp_http_request_post_main(mUpnpHttpRequest* httpReq, const char* ipaddr, int port, bool isSecure)
{
  mUpnpSocket* sock;
  char *method, *uri, *version;
#ifdef MUPNP_SHOW_TIMINGS
  struct timeval start_time, end_time, elapsed_time;
#endif
  mUpnpString* firstLine;

  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&start_time, NULL);
#endif

  mupnp_http_response_clear(httpReq->httpRes);

  mupnp_log_debug_s("(HTTP) Posting:\n");
  mupnp_http_request_print(httpReq);

#if defined(MUPNP_USE_OPENSSL)
  if (isSecure == false)
    sock = mupnp_socket_stream_new();
  else
    sock = mupnp_socket_ssl_new();
#else
  sock = mupnp_socket_stream_new();
#endif

  mupnp_socket_settimeout(sock, mupnp_http_request_gettimeout(httpReq));
  if (mupnp_socket_connect(sock, ipaddr, port) == false) {
    mupnp_socket_delete(sock);
    return httpReq->httpRes;
  }

  mupnp_http_request_sethost(httpReq, ipaddr, port);
  mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_USERAGENT, mupnp_http_request_getuseragent(httpReq));

  method = mupnp_http_request_getmethod(httpReq);
  uri = mupnp_http_request_geturi(httpReq);
  version = mupnp_http_request_getversion(httpReq);

  if (method == NULL || uri == NULL || version == NULL) {
    mupnp_socket_close(sock);
    mupnp_socket_delete(sock);
    return httpReq->httpRes;
  }

#ifdef MUPNP_SHOW_TIMINGS
  mupnp_log_debug_s("\nRequest: %s%s%s:%d%s%s%s\n", method, MUPNP_HTTP_SP, ipaddr, port, uri, MUPNP_HTTP_SP, version);
#endif
  /**** send first line ****/
  firstLine = mupnp_string_new();
  mupnp_string_addvalue(firstLine, method);
  mupnp_string_addvalue(firstLine, MUPNP_HTTP_SP);
  mupnp_string_addvalue(firstLine, uri);
  mupnp_string_addvalue(firstLine, MUPNP_HTTP_SP);
  mupnp_string_addvalue(firstLine, version);
  mupnp_string_addvalue(firstLine, MUPNP_HTTP_CRLF);
  mupnp_socket_write(sock, mupnp_string_getvalue(firstLine), mupnp_string_length(firstLine));
  mupnp_string_delete(firstLine);

  /**** send header and content ****/
  mupnp_http_packet_post((mUpnpHttpPacket*)httpReq, sock);

  /**** read response ****/
  mupnp_http_response_read(httpReq->httpRes, sock, mupnp_http_request_isheadrequest(httpReq));

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&end_time, NULL);
  timersub(&end_time, &start_time, &elapsed_time);
  mupnp_log_debug_s("Getting HTTP-response completed. Elapsed time: "
                    "%ld msec\n",
      ((elapsed_time.tv_sec * 1000) + (elapsed_time.tv_usec / 1000)));
  mupnp_total_elapsed_time += (elapsed_time.tv_sec * 1000000) + (elapsed_time.tv_usec);
#endif
  mupnp_socket_close(sock);
  mupnp_socket_delete(sock);

  mupnp_http_response_print(httpReq->httpRes);

  mupnp_log_debug_l4("Leaving...\n");

  return httpReq->httpRes;
}

mUpnpHttpResponse* mupnp_http_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port)
{
  return mupnp_http_request_post_main(httpReq, ipaddr, port, false);
}

#if defined(MUPNP_USE_OPENSSL)
mUpnpHttpResponse* mupnp_https_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port)
{
  return mupnp_http_request_post_main(httpReq, ipaddr, port, true);
}
#endif

#endif

/****************************************
* mupnp_http_request_post (libcurl)
****************************************/

#if defined(MUPNP_HTTP_CURL)

/**
 * Callback function for CURL to read each HTTP header line
 * mUpnpStringTokenizer might have been a viable choice to do the parsing
 * of various fields. Then again, it would not have read correctly a
 * header line with, for example, the time (because of multiple use of colons):
 * Foo: 12:34:56 EEST DST
 */
static size_t mupnp_http_request_header_callback(void* ptr, size_t size, size_t nmemb, void* stream)
{
  char* headerLine = NULL;
  char* name = NULL;
  char* value = NULL;
  mUpnpStringTokenizer* strTok = NULL;
  mUpnpHttpResponse* httpRes = NULL;
  int head = 0;
  int tail = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (stream == NULL || ptr == NULL) {
    return 0;
  }

  httpRes = (mUpnpHttpResponse*)stream;
  headerLine = (char*)ptr;

  /* Read header items */
  if (mupnp_strncmp(headerLine, MUPNP_HTTP_VER11, mupnp_strlen(MUPNP_HTTP_VER11)) == 0
      || mupnp_strncmp(headerLine, MUPNP_HTTP_VER10, mupnp_strlen(MUPNP_HTTP_VER10)) == 0) {
    /* <HTTP/version> <status code> <reason phrase> */
    strTok = mupnp_string_tokenizer_new(headerLine, MUPNP_HTTP_STATUSLINE_DELIM);
    if (mupnp_string_tokenizer_hasmoretoken(strTok) == true) {
      mupnp_http_response_setversion(httpRes, mupnp_string_tokenizer_nexttoken(strTok));
    }

    if (mupnp_string_tokenizer_hasmoretoken(strTok) == true) {
      mupnp_http_response_setstatuscode(httpRes, atoi(mupnp_string_tokenizer_nexttoken(strTok)));
    }

    if (mupnp_string_tokenizer_hasmoretoken(strTok) == true) {
      value = mupnp_string_tokenizer_nextalltoken(strTok);
      mupnp_strrtrim(value, MUPNP_HTTP_STATUSLINE_DELIM, mupnp_strlen(MUPNP_HTTP_STATUSLINE_DELIM));
      mupnp_http_response_setreasonphrase(httpRes, value);
    }

    mupnp_string_tokenizer_delete(strTok);
  }
  else {
    /* Find the header delimiter */
    for (head = 0; head < size * nmemb; head++) {
      if (headerLine[head] == ':') {
        break;
      }
    }

    /* Unable to find a colon, this is not a valid header line */
    if (head <= 0 || head >= (size * nmemb) - 1) {
      return size * nmemb;
    }

    /* Take the header name */
    name = (char*)malloc(head + 1);

    if (NULL == name) {
      mupnp_log_debug_s("Memory allocation failure!\n");
      return 0;
    }

    memcpy(name, headerLine, head);
    name[head] = '\0';

    /* Skip colon and space(s) */
    for (head++; head < size * nmemb; head++) {
      if (headerLine[head] != 0x20) {
        break;
      }
    }

    /* Unable to find anything sensible anymore */
    if (head >= (size * nmemb) - 1) {
      return size * nmemb;
    }

    /* Find the end of the actual value, without CRLF */
    for (tail = size * nmemb; tail > 0; tail--) {
      if (headerLine[tail] == '\r') {
        break;
      }
      else if (headerLine[tail] == '\n') {
        if (tail > 0 && headerLine[tail - 1] == '\r') {
          tail--;
          break;
        }
      }
    }

    /* Unable to find CRLF */
    if (tail <= head) {
      free(name);
      return size * nmemb;
    }

    /* Take the header value */
    value = (char*)malloc(tail - head + 1);

    if (NULL == value) {
      mupnp_log_debug_s("Memory allocation failure!\n");
      return 0;
    }

    memcpy(value, headerLine + head, tail - head);
    value[tail - head] = '\0';

    /* Set the header value to the response */
    mupnp_http_response_setheadervalue(httpRes, name, value);

    free(name);
    free(value);
  }

  return size * nmemb;

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Callback function for CURL to read each HTTP header line
 */
static size_t mupnp_http_request_content_callback(void* ptr, size_t size, size_t nmemb, void* data)
{
  mupnp_log_debug_l4("Entering...\n");

  /* Append the data */
  mupnp_http_response_appendncontent((mUpnpHttpResponse*)data, ptr, size * nmemb);

  return size * nmemb;

  mupnp_log_debug_l4("Leaving...\n");
}

static int mupnp_http_request_progress_callback(void* ptr, double dltotal, double dlnow, double ultotal, double ulnow)
{
  mUpnpThread* self = mupnp_thread_self();

  mupnp_log_debug_s("Curl call progressing... Got thread %p\n", self);

  if ((NULL != self) && !mupnp_thread_isrunnable(self)) {
    mupnp_log_debug_s("Thread is not runnable anymore! Informing libcurl to abort\n");
    return true;
  }

  return false;
}

mUpnpHttpResponse* mupnp_http_request_post(mUpnpHttpRequest* httpReq, char* ipaddr, int port)
{
  mUpnpHttpResponse* httpRes;
  bool newCurl = false;
  CURL* curl;
  mUpnpHttpHeader* reqHeader;
  struct curl_slist* curlHeaderList;
  mUpnpString* headerStr;
  CURLcode res;
  char *uri, *method;
  char url[MUPNP_NET_URI_MAXLEN];
  long retcode;
#ifdef MUPNP_SHOW_TIMINGS
  struct timeval start_time, end_time, elapsed_time;
#endif

  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&start_time, NULL);
#endif

  httpRes = httpReq->httpRes;

  /* Clear the response data because new data will not
	 * overwrite it, but it is appended to the end */
  mupnp_string_clear(httpRes->content);

  mupnp_log_debug_s("Posting HTTP request (Curl)\n");
  mupnp_http_request_print(httpReq);

  mupnp_http_persistentconnection_lock();
#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
  mupnp_log_debug_s("Looking for persistent connection to %s, port %d\n", ipaddr, port);
  curl = (CURL*)mupnp_http_persistentconnection_get(ipaddr, port);

  if (curl == NULL) {
    mupnp_log_debug_s("Persistent connection not found...\n");
#endif
    curl = curl_easy_init();
    if (curl == NULL) {
      mupnp_http_persistentconnection_unlock();
      return httpReq->httpRes;
    }
#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
    newCurl = true;
  }
#endif
  method = mupnp_http_request_getmethod(httpReq);
  uri = mupnp_http_request_geturi(httpReq);

  /**** method ****/
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);

  /**** url ****/
  if (uri && mupnp_strstr(uri, MUPNP_NET_URI_PROTOCOL_DELIM) > 0) {
    curl_easy_setopt(curl, CURLOPT_URL, uri);
  }
  else {
    mupnp_net_gethosturl(ipaddr, port, uri, url, sizeof(url));
    curl_easy_setopt(curl, CURLOPT_URL, url);
    mupnp_log_debug_s("\n\nCURL: %s\n\n", url);
  }

  /**** header ****/
  curlHeaderList = NULL;
  headerStr = mupnp_string_new();
  for (reqHeader = mupnp_http_request_getheaders(httpReq); reqHeader; reqHeader = mupnp_http_header_next(reqHeader)) {
    mupnp_string_setvalue(headerStr, mupnp_http_header_getname(reqHeader));
    if (mupnp_string_addvalue(headerStr, MUPNP_HTTP_COLON MUPNP_HTTP_SP) && mupnp_string_addvalue(headerStr, mupnp_http_header_getvalue(reqHeader)))
      curlHeaderList = curl_slist_append(curlHeaderList, mupnp_string_getvalue(headerStr));
  }
  mupnp_string_delete(headerStr);
  /* Disable Expect header because it causes IOP issues */
  curlHeaderList = curl_slist_append(curlHeaderList, "Expect:");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaderList);

  /**** content ****/
  /*if (mupnp_http_request_ispostrequest(httpReq) == true) {*/
  if (mupnp_http_request_getcontentlength(httpReq) > 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mupnp_http_request_getcontent(httpReq));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, mupnp_http_request_getcontentlength(httpReq));
  }
  else {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
  }

  /* This has to be enabled for progress callback to be called */
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);

  /* Used for checking stack state during curl easy perform */
  curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, mupnp_http_request_progress_callback);

  /**** response header callback ****/
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, mupnp_http_request_header_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void*)httpRes);

  /**** response content callback ****/
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mupnp_http_request_content_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)httpRes);

  /**** useragent ****/
  curl_easy_setopt(curl, CURLOPT_USERAGENT, mupnp_http_request_getuseragent(httpReq));

  /**** Prohibit curl from using signals ****/
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

  /**** Set the connection timeout so we don't wait forever ****/
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, MUPNP_HTTP_CURL_CONNECTTIMEOUT);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, MUPNP_HTTP_CONN_TIMEOUT);
#ifdef MUPNP_SHOW_TIMINGS
  mupnp_log_debug_s("\nRequest: %s%s%s\n", method, MUPNP_HTTP_SP, url);
#endif

  /* Get the XML document with CURL */
  res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    mupnp_log_debug_s("curl_easy_perform: %s\n",
        curl_easy_strerror(res));

  /* Set the content length, if it wasn't said in the header */
  if (mupnp_http_response_getcontentlength(httpRes) <= 0) {
    mupnp_http_response_setcontentlength(httpRes,
        mupnp_string_length(httpRes->content));
  }

  curl_slist_free_all(curlHeaderList);

  curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &retcode);
  mupnp_http_response_setstatuscode(httpRes, retcode);

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&end_time, NULL);
  timersub(&end_time, &start_time, &elapsed_time);
  mupnp_log_debug_s("Getting HTTP-response completed. Elapsed time: "
                    "%ld msec\n",
      ((elapsed_time.tv_sec * 1000) + (elapsed_time.tv_usec / 1000)));
  mupnp_total_elapsed_time += (elapsed_time.tv_sec * 1000000) + (elapsed_time.tv_usec);
#endif

#ifdef MUPNP_HTTP_USE_PERSISTENT_CONNECTIONS
  if (newCurl) {
    mupnp_log_debug_s("Putting new connection into cache: %s %d\n", ipaddr, port);
    mupnp_http_persistentconnection_put(ipaddr, port, curl);
  }
#else
  curl_easy_cleanup(curl);
#endif
  mupnp_http_persistentconnection_unlock();

  mupnp_log_debug_s("Response for HTTP request (Curl)\n");
  mupnp_http_response_print(httpReq->httpRes);

  return httpReq->httpRes;

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* mupnp_http_request_read
****************************************/

bool mupnp_http_request_read(mUpnpHttpRequest* httpReq, mUpnpSocket* sock)
{
  char lineBuf[MUPNP_HTTP_READLINE_BUFSIZE];
  mUpnpStringTokenizer* strTok;
  ssize_t readLen;
  mUpnpNetURI* uri = NULL;
  bool failed = false;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_clear(httpReq);

  /* If first character(s) is \n or \r\n we ignore it(them) and read second line. */
  do {
    readLen = mupnp_socket_readline(sock, lineBuf, sizeof(lineBuf));
  } while (readLen >= 1 && readLen <= 2);

  if (readLen <= 0)
    return false;

  strTok = mupnp_string_tokenizer_new(lineBuf, MUPNP_HTTP_STATUSLINE_DELIM);
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
    mupnp_http_request_setmethod(httpReq, mupnp_string_tokenizer_nexttoken(strTok));
  else
    failed = true;
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
    mupnp_http_request_seturi(httpReq, mupnp_string_tokenizer_nexttoken(strTok));
  else
    failed = true;
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
    mupnp_http_request_setversion(httpReq, mupnp_string_tokenizer_nexttoken(strTok));
  else
    failed = true;
  mupnp_string_tokenizer_delete(strTok);

  if (failed == true)
    return false;

  /* We could do some further validation for the HTTP-request? */

  /* Change URI to be relative (absolute not needed anymore) */
  uri = mupnp_net_uri_new();
  if (uri != NULL) {
    mupnp_net_uri_set(uri, mupnp_http_request_geturi(httpReq));
    if (mupnp_net_uri_isabsolute(uri) == true && mupnp_net_uri_getrequest(uri) != NULL) {
      mupnp_http_request_seturi(httpReq,
          mupnp_net_uri_getrequest(uri));
    }
    mupnp_net_uri_delete(uri);
    uri = NULL;
  }

  /* Read headers */
  mupnp_http_packet_clear((mUpnpHttpPacket*)httpReq);
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
	   in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_response_postresponse
****************************************/

bool mupnp_http_request_postresponse(mUpnpHttpRequest* httpReq, mUpnpHttpResponse* httpRes)
{
  mUpnpSocket* sock;
  char httpDate[MUPNP_HTTP_DATE_MAXLEN];
  char *version, *reasonPhrase;
  int statusCode;
  char statusCodeBuf[MUPNP_STRING_INTEGER_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  sock = mupnp_http_request_getsocket(httpReq);

  mupnp_log_debug_s("Got request:\n");
  mupnp_http_request_print(httpReq);

  mupnp_http_response_setdate(httpRes, mupnp_http_getdate(mupnp_getcurrentsystemtime(), httpDate, sizeof(httpDate)));

  version = mupnp_http_response_getversion(httpRes);
  statusCode = mupnp_http_response_getstatuscode(httpRes);
  reasonPhrase = mupnp_http_response_getreasonphrase(httpRes);

  if (version == NULL || reasonPhrase == NULL)
    return false;

  mupnp_int2str(statusCode, statusCodeBuf, sizeof(statusCodeBuf));

  /**** send first line ****/
  mupnp_socket_write(sock, version, mupnp_strlen(version));
  mupnp_socket_write(sock, MUPNP_HTTP_SP, sizeof(MUPNP_HTTP_SP) - 1);
  mupnp_socket_write(sock, statusCodeBuf, mupnp_strlen(statusCodeBuf));
  mupnp_socket_write(sock, MUPNP_HTTP_SP, sizeof(MUPNP_HTTP_SP) - 1);
  mupnp_socket_write(sock, reasonPhrase, mupnp_strlen(reasonPhrase));
  mupnp_socket_write(sock, MUPNP_HTTP_CRLF, sizeof(MUPNP_HTTP_CRLF) - 1);

  mupnp_log_debug_s("Posting response:\n");
  mupnp_http_response_print(httpRes);

  /**** send header and content ****/
  mupnp_http_packet_post((mUpnpHttpPacket*)httpRes, sock);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_request_poststatuscode
****************************************/

bool mupnp_http_request_poststatuscode(mUpnpHttpRequest* httpReq, int httpStatCode)
{
  mUpnpHttpResponse* httpRes;
  bool postRet;

  mupnp_log_debug_l4("Entering...\n");

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, httpStatCode);
  mupnp_http_response_setcontentlength(httpRes, 0);
  postRet = mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);

  mupnp_log_debug_l4("Leaving...\n");

  return postRet;
}

/****************************************
 * mupnp_http_request_postdata
 ****************************************/

bool mupnp_http_request_postdata(mUpnpHttpRequest* httpReq, void* data, int dataLen)
{
  if (dataLen <= 0)
    return true;

  mupnp_socket_write(mupnp_http_request_getsocket(httpReq), data, dataLen);

  return true;
}

/****************************************
* mupnp_http_request_postchunkedsize
****************************************/

bool mupnp_http_request_postchunkedsize(mUpnpHttpRequest* httpReq, int dataLen)
{
  mUpnpSocket* sock;
  char chunkedChar[MUPNP_STRING_LONG_BUFLEN + 2];

  sock = mupnp_http_request_getsocket(httpReq);
#if defined(HAVE_SNPRINTF)
  snprintf(chunkedChar, sizeof(chunkedChar), "%x%s", dataLen, MUPNP_HTTP_CRLF);
#else
  sprintf(chunkedChar, "%x%s", dataLen, MUPNP_HTTP_CRLF);
#endif
  mupnp_socket_write(sock, chunkedChar, mupnp_strlen(chunkedChar));

  return true;
}

/****************************************
* mupnp_http_request_postchunkeddata
****************************************/

bool mupnp_http_request_postchunkeddata(mUpnpHttpRequest* httpReq, void* data, int dataLen)
{
  mUpnpSocket* sock;

  if (dataLen <= 0)
    return true;

  mupnp_http_request_postchunkedsize(httpReq, dataLen);
  sock = mupnp_http_request_getsocket(httpReq);
  mupnp_socket_write(sock, data, dataLen);
  mupnp_socket_write(sock, MUPNP_HTTP_CRLF, sizeof(MUPNP_HTTP_CRLF) - 1);

  return true;
}

/****************************************
 * mupnp_http_request_postlastchunk
 ****************************************/

bool mupnp_http_request_postlastchunk(mUpnpHttpRequest* httpReq)
{
  mUpnpSocket* sock;

  mupnp_http_request_postchunkedsize(httpReq, 0);
  sock = mupnp_http_request_getsocket(httpReq);
  mupnp_socket_write(sock, MUPNP_HTTP_CRLF, sizeof(MUPNP_HTTP_CRLF) - 1);

  return true;
}

/****************************************
* mupnp_http_response_copy
****************************************/

void mupnp_http_request_copy(mUpnpHttpRequest* destHttpReq, mUpnpHttpRequest* srcHttpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_request_setmethod(destHttpReq, mupnp_http_request_getmethod(srcHttpReq));
  mupnp_http_request_seturi(destHttpReq, mupnp_http_request_geturi(srcHttpReq));
  mupnp_http_request_setversion(destHttpReq, mupnp_http_request_getversion(srcHttpReq));

  mupnp_http_packet_copy((mUpnpHttpPacket*)destHttpReq, (mUpnpHttpPacket*)srcHttpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_request_print
****************************************/

void mupnp_http_request_print(mUpnpHttpRequest* httpReq)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_s("%s %s %s\n",
      mupnp_http_request_getmethod(httpReq),
      mupnp_http_request_geturi(httpReq),
      mupnp_http_request_getversion(httpReq));

  mupnp_http_packet_print((mUpnpHttpPacket*)httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

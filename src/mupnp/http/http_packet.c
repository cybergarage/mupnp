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

#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/net/interface.h>

#include <limits.h>

#define READBUF_LENGTH 1024

/****************************************
* mupnp_http_packet_new
****************************************/

mUpnpHttpPacket* mupnp_http_packet_new()
{
  mUpnpHttpPacket* httpPkt;

  mupnp_log_debug_l4("Entering...\n");

  httpPkt = (mUpnpHttpPacket*)malloc(sizeof(mUpnpHttpPacket));

  if (NULL != httpPkt) {
    mupnp_http_packet_init(httpPkt);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return httpPkt;
}

/****************************************
* mupnp_http_packet_delete
****************************************/

void mupnp_http_packet_delete(mUpnpHttpPacket* httpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clean(httpPkt);
  free(httpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_init
****************************************/

void mupnp_http_packet_init(mUpnpHttpPacket* httpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  httpPkt->headerList = mupnp_http_headerlist_new();
  httpPkt->content = mupnp_string_new();

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_clean
****************************************/

void mupnp_http_packet_clean(mUpnpHttpPacket* httpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_headerlist_delete(httpPkt->headerList);
  mupnp_string_delete(httpPkt->content);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_clear
****************************************/

void mupnp_http_packet_clear(mUpnpHttpPacket* httpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_headerlist_clear(httpPkt->headerList);
  mupnp_string_setvalue(httpPkt->content, NULL);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_setheader
****************************************/

void mupnp_http_packet_setheadervalue(mUpnpHttpPacket* httpPkt, const char* name, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_headerlist_set(httpPkt->headerList, name, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_setheaderinteger
****************************************/

void mupnp_http_packet_setheaderinteger(mUpnpHttpPacket* httpPkt, const char* name, int value)
{
  char svalue[MUPNP_STRING_INTEGER_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_setheadervalue(httpPkt, name, mupnp_int2str(value, svalue, sizeof(svalue)));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_setheaderlong
****************************************/

void mupnp_http_packet_setheaderlong(mUpnpHttpPacket* httpPkt, const char* name, long value)
{
  char svalue[MUPNP_STRING_LONG_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_setheadervalue(httpPkt, name, mupnp_long2str(value, svalue, sizeof(svalue)));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_http_packet_setheadersizet
 ****************************************/

void mupnp_http_packet_setheadersizet(mUpnpHttpPacket* httpPkt, const char* name, size_t value)
{
  char svalue[MUPNP_STRING_LONG_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_setheadervalue(httpPkt, name, mupnp_sizet2str(value, svalue, sizeof(svalue)));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_http_packet_setheaderssizet
 ****************************************/

void mupnp_http_packet_setheaderssizet(mUpnpHttpPacket* httpPkt, const char* name, ssize_t value)
{
  char svalue[MUPNP_STRING_LONG_BUFLEN];

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_setheadervalue(httpPkt, name, mupnp_ssizet2str(value, svalue, sizeof(svalue)));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_getheadervalue
****************************************/

const char* mupnp_http_packet_getheadervalue(mUpnpHttpPacket* httpPkt, const char* name)
{
  return mupnp_http_headerlist_getvalue(httpPkt->headerList, name);
}

/****************************************
* mupnp_http_packet_getheadervalue
****************************************/

int mupnp_http_packet_getheaderinteger(mUpnpHttpPacket* httpPkt, const char* name)
{
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  value = mupnp_http_packet_getheadervalue(httpPkt, name);

  mupnp_log_debug_l4("Leaving...\n");

  return (value != NULL) ? atoi(value) : 0;
}

/****************************************
* mupnp_http_packet_getheadervalue
****************************************/

long mupnp_http_packet_getheaderlong(mUpnpHttpPacket* httpPkt, const char* name)
{
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  value = mupnp_http_packet_getheadervalue(httpPkt, name);

  mupnp_log_debug_l4("Leaving...\n");

  return (value != NULL) ? atol(value) : 0;
}

/****************************************
 * mupnp_http_packet_getheadersizet
 ****************************************/

size_t mupnp_http_packet_getheadersizet(mUpnpHttpPacket* httpPkt, const char* name)
{
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  value = mupnp_http_packet_getheadervalue(httpPkt, name);

  mupnp_log_debug_l4("Leaving...\n");

  return (value != NULL) ? atol(value) : 0;
}

/****************************************
 * mupnp_http_packet_getheadersizet
 ****************************************/

ssize_t mupnp_http_packet_getheaderssizet(mUpnpHttpPacket* httpPkt, const char* name)
{
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  value = mupnp_http_packet_getheadervalue(httpPkt, name);

  mupnp_log_debug_l4("Leaving...\n");

  return (value != NULL) ? atol(value) : 0;
}

/****************************************
* mupnp_http_packet_getheadervalue
****************************************/

void mupnp_http_packet_sethost(mUpnpHttpPacket* httpPkt, const char* addr, int port)
{
  char* host;
  size_t hostMaxLen;

  mupnp_log_debug_l4("Entering...\n");

  if (addr == NULL)
    return;

  hostMaxLen = mupnp_strlen(addr) + MUPNP_NET_IPV6_ADDRSTRING_MAXSIZE + MUPNP_STRING_INTEGER_BUFLEN;
  host = malloc(sizeof(char) * hostMaxLen);

  if (host == NULL)
    /* Memory allocation failure */
    return;

#if defined(HAVE_SNPRINTF)
  if (0 < port && port != MUPNP_HTTP_DEFAULT_PORT) {
    if (mupnp_net_isipv6address(addr) == true)
      snprintf(host, hostMaxLen, "[%s]:%d", addr, port);
    else
      snprintf(host, hostMaxLen, "%s:%d", addr, port);
  }
  else {
    if (mupnp_net_isipv6address(addr) == true)
      snprintf(host, hostMaxLen, "[%s]", addr);
    else
      snprintf(host, hostMaxLen, "%s", addr);
  }
#else
  if (0 < port && port != MUPNP_HTTP_DEFAULT_PORT) {
    if (mupnp_net_isipv6address(addr) == true)
      sprintf(host, "[%s]:%d", addr, port);
    else
      sprintf(host, "%s:%d", addr, port);
  }
  else {
    if (mupnp_net_isipv6address(addr) == true)
      sprintf(host, "[%s]", addr);
    else
      sprintf(host, "%s", addr);
  }
#endif

  mupnp_http_packet_setheadervalue(httpPkt, MUPNP_HTTP_HOST, host);

  free(host);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_post
****************************************/

void mupnp_http_packet_post(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock)
{
  mUpnpHttpHeader* header;
  const char *name, *value;
  char* content;
  size_t contentLen;

  mupnp_log_debug_l4("Entering...\n");

  /**** send headers ****/
  for (header = mupnp_http_packet_getheaders(httpPkt); header != NULL; header = mupnp_http_header_next(header)) {
    name = mupnp_http_header_getname(header);
    if (name == NULL)
      continue;
    mupnp_socket_write(sock, name, mupnp_strlen(name));
    mupnp_socket_write(sock, MUPNP_HTTP_COLON, sizeof(MUPNP_HTTP_COLON) - 1);
    mupnp_socket_write(sock, MUPNP_HTTP_SP, sizeof(MUPNP_HTTP_SP) - 1);
    value = mupnp_http_header_getvalue(header);
    if (value != NULL)
      mupnp_socket_write(sock, value, mupnp_strlen(value));
    mupnp_socket_write(sock, MUPNP_HTTP_CRLF, sizeof(MUPNP_HTTP_CRLF) - 1);
  }
  mupnp_socket_write(sock, MUPNP_HTTP_CRLF, sizeof(MUPNP_HTTP_CRLF) - 1);

  /**** send content ****/
  content = mupnp_http_packet_getcontent(httpPkt);
  contentLen = mupnp_http_packet_getcontentlength(httpPkt);
  if (content != NULL && 0 < contentLen)
    mupnp_socket_write(sock, content, contentLen);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_read_headers
****************************************/

void mupnp_http_packet_read_headers(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, char* lineBuf, size_t lineBufSize)
{
  mUpnpStringTokenizer* strTok;
  mUpnpHttpHeader* header;
  ssize_t readLen;
  char *name, *value;

  mupnp_log_debug_l4("Entering...\n");

  while (1) {
    readLen = mupnp_socket_readline(sock, lineBuf, lineBufSize);
    if (readLen <= 2)
      break;
    name = NULL;
    value = NULL;
    strTok = mupnp_string_tokenizer_new(lineBuf, MUPNP_HTTP_HEADERLINE_DELIM);
    if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
      name = mupnp_string_tokenizer_nexttoken(strTok);
    if (mupnp_string_tokenizer_hasmoretoken(strTok) == true) {
      value = mupnp_string_tokenizer_nextalltoken(strTok);
      mupnp_strrtrim(value, MUPNP_HTTP_HEADERLINE_DELIM, mupnp_strlen(MUPNP_HTTP_HEADERLINE_DELIM));
    }
    if (0 < mupnp_strlen(name)) {
      if (mupnp_strlen(value) == 0)
        value = "";
      header = mupnp_http_header_new();
      mupnp_http_header_setname(header, name);
      mupnp_http_header_setvalue(header, value);
      mupnp_http_packet_addheader(httpPkt, header);
    }
    mupnp_string_tokenizer_delete(strTok);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_read_chunk
****************************************/

size_t mupnp_http_packet_read_chunk(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, char* lineBuf, size_t lineBufSize)
{
  ssize_t readLen = 0;
  ssize_t conLen = 0;
  int tries = 0;
  char* content = NULL;

  mupnp_log_debug_l4("Entering...\n");

  /* Read chunk header */
  readLen = mupnp_socket_readline(sock, lineBuf, lineBufSize);

  conLen = mupnp_strhex2long(lineBuf);
  if (conLen < 1)
    return 0;

  content = (char*)malloc(conLen + 1);

  if (content == NULL) {
    mupnp_log_debug_s("Memory allocation problem!\n");
    return 0;
  }

  content[conLen] = 0;

  readLen = 0;
  /* Read content until conLen is reached, or tired of trying */
  while (readLen < conLen && tries < 20) {
    readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
    tries++;
  }

  /* Append content to packet */
  mupnp_http_packet_appendncontent(httpPkt, content, readLen);
  free(content);
  content = NULL;

  if (readLen == conLen) {
    /* Read CRLF bytes */
    mupnp_socket_readline(sock, lineBuf, lineBufSize);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return readLen;
}

/****************************************
* mupnp_http_packet_read_body
****************************************/

bool mupnp_http_packet_read_body(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, char* lineBuf, size_t lineBufSize)
{
  ssize_t readLen;
  ssize_t conLen;
  char* content;
  char readBuf[READBUF_LENGTH + 1];
  int tries = 0;

  mupnp_log_debug_l4("Entering...\n");

  conLen = mupnp_http_packet_getcontentlength(httpPkt);
  content = NULL;
  if (0 < conLen) {
    content = (char*)malloc(conLen + 1);
    if (content == NULL) {
      mupnp_log_debug_s("Memory allocation problem!\n");
      return false;
    }

    content[0] = '\0';
    readLen = 0;

    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }

    if (readLen <= 0)
      return true;
    content[readLen] = '\0';
    mupnp_http_packet_setcontentpointer(httpPkt, content, readLen);
  }
  else if (mupnp_http_packet_getheadervalue(httpPkt,
               MUPNP_HTTP_CONTENT_LENGTH)
      == NULL) {
    /* header existance must be checked! otherwise packets which
		   rightly report 0 as content length, will jam the http */

    /* Check if we read chunked encoding */
    if (mupnp_http_packet_ischunked(httpPkt) == true) {
      conLen = 0;
      do {
        readLen = mupnp_http_packet_read_chunk(httpPkt, sock, lineBuf, lineBufSize);
        conLen += readLen;
      } while (readLen > 0);

      mupnp_http_packet_setcontentlength(httpPkt, conLen);
    }
    else {
      readLen = 0;
      conLen = 0;
      while ((readLen = mupnp_socket_read(sock, readBuf, READBUF_LENGTH)) > 0) {
        mupnp_http_packet_appendncontent(httpPkt, readBuf, readLen);
        conLen += readLen;
      }

      mupnp_http_packet_setcontentlength(httpPkt, conLen);
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_packet_read
****************************************/

bool mupnp_http_packet_read(mUpnpHttpPacket* httpPkt, mUpnpSocket* sock, bool onlyHeader, char* lineBuf, size_t lineBufSize)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clear(httpPkt);
  mupnp_http_packet_read_headers(httpPkt, sock, lineBuf, lineBufSize);

  mupnp_log_debug_l4("Leaving...\n");

  if (onlyHeader)
    return true;

  return mupnp_http_packet_read_body(httpPkt, sock, lineBuf, lineBufSize);
}

/****************************************
* mupnp_http_packet_getheadersize
****************************************/

size_t mupnp_http_packet_getheadersize(mUpnpHttpPacket* httpPkt)
{
  mUpnpHttpHeader* header;
  size_t headerSize;
  const char* name;
  const char* value;

  mupnp_log_debug_l4("Entering...\n");

  headerSize = 0;
  for (header = mupnp_http_packet_getheaders(httpPkt); header != NULL; header = mupnp_http_header_next(header)) {
    name = mupnp_http_header_getname(header);
    value = mupnp_http_header_getvalue(header);
    headerSize += mupnp_strlen(name);
    headerSize += sizeof(MUPNP_HTTP_COLON) - 1;
    headerSize += sizeof(MUPNP_HTTP_SP) - 1;
    headerSize += mupnp_strlen(value);
    headerSize += sizeof(MUPNP_HTTP_CRLF) - 1;
  }
  headerSize += sizeof(MUPNP_HTTP_CRLF) - 1;

  mupnp_log_debug_l4("Leaving...\n");

  return headerSize;
}

/****************************************
* mupnp_http_packet_copy
****************************************/

void mupnp_http_packet_copy(mUpnpHttpPacket* destHttpPkt, mUpnpHttpPacket* srcHttpPkt)
{
  mUpnpHttpHeader* srcHeader;
  mUpnpHttpHeader* destHeader;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clear(destHttpPkt);

  /**** copy headers ****/
  for (srcHeader = mupnp_http_packet_getheaders(srcHttpPkt); srcHeader != NULL; srcHeader = mupnp_http_header_next(srcHeader)) {
    destHeader = mupnp_http_header_new();
    mupnp_http_header_setname(destHeader, mupnp_http_header_getname(srcHeader));
    mupnp_http_header_setvalue(destHeader, mupnp_http_header_getvalue(srcHeader));
    mupnp_http_packet_addheader(destHttpPkt, destHeader);
  }

  /**** copy content ****/
  mupnp_http_packet_setcontent(destHttpPkt, mupnp_http_packet_getcontent(srcHttpPkt));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_packet_print
****************************************/

void mupnp_http_packet_print(mUpnpHttpPacket* httpPkt)
{
  mUpnpHttpHeader* header;
  char* content;
  long contentLen;

  mupnp_log_debug_l4("Entering...\n");

  /**** print headers ****/
  for (header = mupnp_http_packet_getheaders(httpPkt); header != NULL; header = mupnp_http_header_next(header)) {
    mupnp_log_debug_s("%s: %s\n",
        mupnp_http_header_getname(header),
        mupnp_http_header_getvalue(header));
  }
  mupnp_log_debug_s("\n");

  /**** print content ****/
  content = mupnp_http_packet_getcontent(httpPkt);
  contentLen = mupnp_http_packet_getcontentlength(httpPkt);

  if (content != NULL && 0 < contentLen)
    mupnp_log_debug_s("%s\n", content);

  mupnp_log_debug_l4("Leaving...\n");
}

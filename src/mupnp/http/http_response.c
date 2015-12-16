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
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_response_new
****************************************/

mUpnpHttpResponse* mupnp_http_response_new()
{
  mUpnpHttpResponse* httpRes;

  mupnp_log_debug_l4("Entering...\n");

  httpRes = (mUpnpHttpResponse*)malloc(sizeof(mUpnpHttpResponse));

  if (NULL != httpRes) {
    mupnp_http_packet_init((mUpnpHttpPacket*)httpRes);
    httpRes->version = mupnp_string_new();
    httpRes->reasonPhrase = mupnp_string_new();

    mupnp_http_response_setversion(httpRes, MUPNP_HTTP_VER11);
    mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_BAD_REQUEST);
    mupnp_http_response_setuserdata(httpRes, NULL);

    mupnp_http_response_settimeout(httpRes, MUPNP_HTTP_CONN_TIMEOUT);
  }

  return httpRes;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_delete
****************************************/

void mupnp_http_response_delete(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clean((mUpnpHttpPacket*)httpRes);
  mupnp_string_delete(httpRes->version);
  mupnp_string_delete(httpRes->reasonPhrase);
  free(httpRes);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_clear
****************************************/

void mupnp_http_response_clear(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_packet_clear((mUpnpHttpPacket*)httpRes);
  mupnp_http_response_setversion(httpRes, NULL);
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_INTERNAL_SERVER_ERROR);
  mupnp_http_response_setreasonphrase(httpRes, NULL);
  mupnp_http_response_setuserdata(httpRes, NULL);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setversion
****************************************/

void mupnp_http_response_setversion(mUpnpHttpResponse* httpRes, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpRes->version, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getversion
****************************************/

char* mupnp_http_response_getversion(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(httpRes->version);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setstatuscode
****************************************/

void mupnp_http_response_setstatuscode(mUpnpHttpResponse* httpRes, int value)
{
  mupnp_log_debug_l4("Entering...\n");

  httpRes->statusCode = value;
  mupnp_http_response_setreasonphrase(httpRes,
      (char*)mupnp_http_statuscode2reasonphrase(value));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getstatuscode
****************************************/

int mupnp_http_response_getstatuscode(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  return httpRes->statusCode;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setreasonphrase
****************************************/

void mupnp_http_response_setreasonphrase(mUpnpHttpResponse* httpRes, const char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(httpRes->reasonPhrase, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getreasonphrase
****************************************/

char* mupnp_http_response_getreasonphrase(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(httpRes->reasonPhrase);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_read
****************************************/

bool mupnp_http_response_read(mUpnpHttpResponse* httpRes, mUpnpSocket* sock, bool onlyHeader)
{
  char lineBuf[MUPNP_HTTP_READLINE_BUFSIZE];
  mUpnpStringTokenizer* strTok;
  char* token;
  ssize_t readLen;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_response_clear(httpRes);

  readLen = mupnp_socket_readline(sock, lineBuf, sizeof(lineBuf));
  if (readLen <= 0)
    return false;

  strTok = mupnp_string_tokenizer_new(lineBuf, MUPNP_HTTP_STATUSLINE_DELIM);
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
    mupnp_http_response_setversion(httpRes, mupnp_string_tokenizer_nexttoken(strTok));
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true)
    mupnp_http_response_setstatuscode(httpRes, atoi(mupnp_string_tokenizer_nexttoken(strTok)));
  if (mupnp_string_tokenizer_hasmoretoken(strTok) == true) {
    token = mupnp_string_tokenizer_nextalltoken(strTok);
    mupnp_strrtrim(token, MUPNP_HTTP_STATUSLINE_DELIM, mupnp_strlen(MUPNP_HTTP_STATUSLINE_DELIM));
    mupnp_http_response_setreasonphrase(httpRes, token);
  }
  mupnp_string_tokenizer_delete(strTok);

  mupnp_http_packet_read((mUpnpHttpPacket*)httpRes, sock, onlyHeader, lineBuf, sizeof(lineBuf));

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_response_copy
****************************************/

void mupnp_http_response_copy(mUpnpHttpResponse* destHttpRes, mUpnpHttpResponse* srcHttpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_response_setversion(destHttpRes, mupnp_http_response_getversion(srcHttpRes));
  mupnp_http_response_setstatuscode(destHttpRes, mupnp_http_response_getstatuscode(srcHttpRes));
  mupnp_http_response_setreasonphrase(destHttpRes, mupnp_http_response_getreasonphrase(srcHttpRes));

  mupnp_http_packet_copy((mUpnpHttpPacket*)destHttpRes, (mUpnpHttpPacket*)srcHttpRes);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_print
****************************************/

void mupnp_http_response_print(mUpnpHttpResponse* httpRes)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_s("%s %d %s\n",
      mupnp_http_response_getversion(httpRes),
      mupnp_http_response_getstatuscode(httpRes),
      mupnp_http_response_getreasonphrase(httpRes));

  mupnp_http_packet_print((mUpnpHttpPacket*)httpRes);

  mupnp_log_debug_l4("Leaving...\n");
}

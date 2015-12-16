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

#include <mupnp/net/uri.h>
#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

#if defined(MUPNP_HTTP_CURL)
#include <curl/curl.h>
#endif

/****************************************
* mupnp_net_uri_new
****************************************/

mUpnpNetURI* mupnp_net_uri_new()
{
  mUpnpNetURI* uri;

  mupnp_log_debug_l4("Entering...\n");

  uri = (mUpnpNetURI*)malloc(sizeof(mUpnpNetURI));

  if (NULL != uri) {
    uri->uri = mupnp_string_new();
    uri->protocol = mupnp_string_new();
    uri->user = mupnp_string_new();
    uri->password = mupnp_string_new();
    uri->host = mupnp_string_new();
    uri->port = 0;
    uri->path = mupnp_string_new();
    uri->query = mupnp_string_new();
    uri->fragment = mupnp_string_new();
    uri->request = NULL;
    uri->queryDictionary = NULL;

    /**** Thanks for Theo Beisch (2005/08/25) ****/
    mupnp_string_setvalue(uri->path, MUPNP_NET_URI_DEFAULT_PATH);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return uri;
}

/****************************************
* mupnp_net_uri_delete
****************************************/

void mupnp_net_uri_delete(mUpnpNetURI* uri)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_delete(uri->uri);
  mupnp_string_delete(uri->protocol);
  mupnp_string_delete(uri->user);
  mupnp_string_delete(uri->password);
  mupnp_string_delete(uri->host);
  mupnp_string_delete(uri->path);
  mupnp_string_delete(uri->query);
  mupnp_string_delete(uri->fragment);

  if (uri->request != NULL)
    mupnp_string_delete(uri->request);
  if (uri->queryDictionary != NULL)
    mupnp_dictionary_delete(uri->queryDictionary);

  free(uri);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_uri_clear
****************************************/

void mupnp_net_uri_clear(mUpnpNetURI* uri)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_clear(uri->uri);
  mupnp_string_clear(uri->protocol);
  mupnp_string_clear(uri->user);
  mupnp_string_clear(uri->password);
  mupnp_string_clear(uri->host);
  uri->port = 0;
  mupnp_string_clear(uri->path);
  mupnp_string_clear(uri->query);
  mupnp_string_clear(uri->fragment);
  if (uri->request != NULL) {
    mupnp_string_delete(uri->request);
    uri->request = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_uri_set
****************************************/

void mupnp_net_uri_setvalue(mUpnpNetURI* uri, const char* value)
{
  char* protocol;
  size_t uriLen;
  size_t currIdx;
  ssize_t protoIdx;
  ssize_t atIdx;
  ssize_t colonIdx;
  ssize_t shashIdx;
  char* host;
  ssize_t eblacketIdx;
  mUpnpString* hostStr;
  mUpnpString* portStr;
  size_t hostLen;
  ssize_t sharpIdx;
  ssize_t questionIdx;
  size_t queryLen;

  mupnp_log_debug_l4("Entering...\n");

  uriLen = mupnp_strlen(value);
  mupnp_net_uri_clear(uri);
  mupnp_net_uri_seturi(uri, value);

  currIdx = 0;

  /*** Protocol ****/
  protoIdx = mupnp_strstr(value, MUPNP_NET_URI_PROTOCOL_DELIM);
  if (0 < protoIdx) {
    mupnp_string_setnvalue(uri->protocol, value, protoIdx);
    currIdx += protoIdx + mupnp_strlen(MUPNP_NET_URI_PROTOCOL_DELIM);
  }

  /*** User (Password) ****/
  atIdx = mupnp_strstr(value + currIdx, MUPNP_NET_URI_USER_DELIM);
  if (0 < atIdx) {
    colonIdx = mupnp_strstr(value + currIdx, MUPNP_NET_URI_COLON_DELIM);
    /**** Thanks for Theo Beisch (2005/08/25) ****/
    if (0 < colonIdx && colonIdx < atIdx) {
      mupnp_string_setnvalue(uri->user, value + currIdx, colonIdx);
      mupnp_string_setnvalue(uri->password, value + currIdx + colonIdx + 1, atIdx - (colonIdx + 1));
    }
    else
      mupnp_string_setnvalue(uri->user, value + currIdx, atIdx - currIdx);
    currIdx += atIdx + 1;
  }

  /*** Host (Port) ****/
  shashIdx = mupnp_strstr(value + currIdx, MUPNP_NET_URI_SLASH_DELIM);
  if (0 < shashIdx) {
    mupnp_string_setnvalue(uri->host, value + currIdx, shashIdx);
    currIdx += shashIdx;
  }
  else if (mupnp_net_uri_isabsolute(uri) == true) {
    mupnp_string_setnvalue(uri->host, value + currIdx, mupnp_strlen(value) - currIdx);
    currIdx += mupnp_strlen(value) - currIdx;
  }
  host = mupnp_net_uri_gethost(uri);
  colonIdx = mupnp_strrchr(host, MUPNP_NET_URI_COLON_DELIM, 1);
  eblacketIdx = mupnp_strrchr(host, MUPNP_NET_URI_EBLACET_DELIM, 1);
  if (0 < colonIdx && eblacketIdx < colonIdx) {
    hostStr = mupnp_string_new();
    mupnp_string_setvalue(hostStr, host);
    hostLen = mupnp_string_length(hostStr);
    /**** host ****/
    mupnp_string_setnvalue(uri->host, mupnp_string_getvalue(hostStr), colonIdx);
    host = mupnp_net_uri_gethost(uri);
    if (0 < hostLen) {
      if (host[0] == '[' && host[hostLen - 1] == ']')
        mupnp_string_setnvalue(uri->host, mupnp_string_getvalue(hostStr) + 1, colonIdx - 2);
    }
    /**** port ****/
    portStr = mupnp_string_new();
    mupnp_string_setnvalue(portStr, mupnp_string_getvalue(hostStr) + colonIdx + 1, hostLen - colonIdx - 1);
    uri->port = atoi(mupnp_string_getvalue(portStr));
    mupnp_string_delete(portStr);
    mupnp_string_delete(hostStr);
  }
  else {
    uri->port = MUPNP_NET_URI_KNKOWN_PORT;
    protocol = mupnp_net_uri_getprotocol(uri);
    if (mupnp_strcmp(protocol, MUPNP_NET_URI_PROTOCOL_HTTP) == 0)
      uri->port = MUPNP_NET_URI_DEFAULT_HTTP_PORT;
    if (mupnp_strcmp(protocol, MUPNP_NET_URI_PROTOCOL_FTP) == 0)
      uri->port = MUPNP_NET_URI_DEFAULT_FTP_PORT;
  }

  /*
		Handle relative URL
	*/
  if (mupnp_net_uri_isabsolute(uri) == false) {
    mupnp_string_addvalue(uri->path, value);
  }
  else {
    /* First set path simply to the rest of URI */
    mupnp_string_setnvalue(uri->path, value + currIdx, uriLen - currIdx);
  }

  /**** Path (Query/Fragment) ****/
  sharpIdx = mupnp_strstr(value + currIdx, MUPNP_NET_URI_SHARP_DELIM);
  if (0 < sharpIdx) {
    mupnp_string_setnvalue(uri->path, value + currIdx, sharpIdx);
    mupnp_string_setnvalue(uri->fragment, value + currIdx + sharpIdx + 1, uriLen - (currIdx + sharpIdx + 1));
  }
  questionIdx = mupnp_strstr(value + currIdx, MUPNP_NET_URI_QUESTION_DELIM);
  if (0 < questionIdx) {
    mupnp_string_setnvalue(uri->path, value + currIdx, questionIdx);
    queryLen = uriLen - (currIdx + questionIdx + 1);
    if (0 < sharpIdx)
      queryLen -= uriLen - (currIdx + sharpIdx);
    mupnp_string_setnvalue(uri->query, value + currIdx + questionIdx + 1, queryLen);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_net_uri_rebuild
 ****************************************/

void mupnp_net_uri_rebuild(mUpnpNetURI* uri)
{
  char portStr[32];
  char* path;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(uri->uri, mupnp_net_uri_getprotocol(uri));
  mupnp_string_addvalue(uri->uri, MUPNP_NET_URI_PROTOCOL_DELIM);
  mupnp_string_addvalue(uri->uri, mupnp_net_uri_gethost(uri));
  mupnp_string_addvalue(uri->uri, MUPNP_NET_URI_COLON_DELIM);
  mupnp_string_addvalue(uri->uri, mupnp_int2str(mupnp_net_uri_getport(uri), portStr, sizeof(portStr)));
  if (0 < mupnp_strlen(mupnp_net_uri_getpath(uri))) {
    path = mupnp_net_uri_getpath(uri);
    if (path[0] != '/')
      mupnp_string_addvalue(uri->uri, MUPNP_NET_URI_SLASH_DELIM);
    mupnp_string_addvalue(uri->uri, mupnp_net_uri_getpath(uri));
    if (mupnp_strchr(mupnp_net_uri_getpath(uri), "?", 1) == -1 && 0 < mupnp_strlen(mupnp_net_uri_getquery(uri))) {
      mupnp_string_addvalue(uri->uri, MUPNP_NET_URI_QUESTION_DELIM);
      mupnp_string_addvalue(uri->uri, mupnp_net_uri_getquery(uri));
    }
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_net_uri_getvalue
 ****************************************/

const char* mupnp_net_uri_getvalue(mUpnpNetURI* uri)
{
  mupnp_net_uri_rebuild(uri);
  return mupnp_string_getvalue(uri->uri);
}

/****************************************
* mupnp_net_uri_getrequest
****************************************/

char* mupnp_net_uri_getrequest(mUpnpNetURI* uri)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_net_uri_hasquery(uri) == false) {
    return mupnp_net_uri_getpath(uri);
  }

  if (uri->request == NULL)
    uri->request = mupnp_string_new();

  mupnp_string_setvalue(uri->request, mupnp_net_uri_getpath(uri));
  mupnp_string_addvalue(uri->request, MUPNP_NET_URI_QUESTION_DELIM);
  mupnp_string_addvalue(uri->request, mupnp_net_uri_getquery(uri));

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(uri->request);
}

/****************************************
* mupnp_net_uri_isequivalent
****************************************/

bool mupnp_net_uri_isequivalent(const char* url, const char* relative_url)
{
  mUpnpNetURI* u;
  char* path;
  bool ret;

  u = mupnp_net_uri_new();
  mupnp_net_uri_set(u, url);

  path = mupnp_net_uri_getpath(u);
  ret = mupnp_strcmp(path, relative_url) == 0;

  mupnp_net_uri_delete(u);
  return ret;
}

/****************************************
* mupnp_net_uri_isescapedstring
****************************************/

bool mupnp_net_uri_isescapedstring(char* buf, size_t bufSize)
{
  ssize_t idx;

  mupnp_log_debug_l4("Entering...\n");

  if (bufSize < 1)
    bufSize = mupnp_strlen(buf) + 1;

  /* We assume that the URI string is escaped, if it has at least one
	   escaped character */

  /* First check that there is escaping character */
  idx = mupnp_strstr(buf, MUPNP_NET_URI_ESCAPING_CHAR);
  if (idx < 0 || idx > bufSize - 4)
    return false;

  /* Check that the next two characters are HEX */
  if (((buf[idx + 1] >= '0' && buf[idx + 1] <= '9') || (buf[idx + 1] >= 'a' && buf[idx + 1] <= 'f') || (buf[idx + 1] >= 'A' && buf[idx + 1] <= 'F')) && ((buf[idx + 2] >= '0' && buf[idx + 2] <= '9') || (buf[idx + 2] >= 'a' && buf[idx + 2] <= 'f') || (buf[idx + 2] >= 'A' && buf[idx + 2] <= 'F')))
    return true;

  mupnp_log_debug_l4("Leaving...\n");

  return false;
}

/****************************************
* mupnp_net_uri_unescapestring
****************************************/

char* mupnp_net_uri_escapestring(char* buf, size_t bufSize, mUpnpString* retBuf)
{
#if defined(MUPNP_HTTP_CURL)
  char* tmp;
#else
  int n;
  unsigned char c;
  char hexChar[4];
#endif

  mupnp_log_debug_l4("Entering...\n");

  if (!retBuf)
    return NULL;

#if defined(MUPNP_HTTP_CURL)
  tmp = (bufSize < 1) ? curl_escape(buf, 0) : curl_escape(buf, bufSize);
  if (tmp == NULL) {
    mupnp_log_debug_s("Memory allocation problem!\n");
    return NULL;
  }
  mupnp_string_addvalue(retBuf, tmp);
  curl_free(tmp);
#else
  if (bufSize < 1)
    bufSize = mupnp_strlen(buf) + 1;

  for (n = 0; n < bufSize; n++) {
    c = (unsigned char)buf[n];
    if (!mupnp_net_uri_isalphanumchar(c)) {
#if defined(HAVE_SNPRINTF)
      snprintf(hexChar, sizeof(hexChar), "%%%02X", c);
#else
      sprintf(hexChar, "%%%02X", c);
#endif
      mupnp_string_naddvalue(retBuf, hexChar, 3);
    }
    else
      mupnp_string_naddvalue(retBuf, buf + n, 1);
  }

#endif

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(retBuf);
}

/****************************************
* mupnp_net_uri_escapestring
****************************************/

char* mupnp_net_uri_unescapestring(char* buf, size_t bufSize, mUpnpString* retBuf)
{
#if defined(MUPNP_HTTP_CURL)
  char* tmp;
#else
  int n;
  char hexStr[3];
  long hex;
  unsigned char c;
#endif
  int idx = 0;
#if defined(MUPNP_USE_NET_URI_ESCAPESTRING_SKIP)
  int tmpIdx = 0;
#endif

  mupnp_log_debug_l4("Entering...\n");

  if (!retBuf)
    return NULL;

  /* Check if URI is already escaped */
  if (mupnp_net_uri_isescapedstring(buf + idx, bufSize) == true)
    return buf;

/* We can safely assume that the non-path part is already escaped */
#if defined(MUPNP_USE_NET_URI_ESCAPESTRING_SKIP)
  idx = mupnp_strstr(buf, MUPNP_NET_URI_PROTOCOL_DELIM);
  if (idx > 0) {
    idx = idx + mupnp_strlen(MUPNP_NET_URI_PROTOCOL_DELIM);
    tmpIdx = mupnp_strstr(buf + idx, MUPNP_NET_URI_SLASH_DELIM);
    if (tmpIdx > 0)
      idx += tmpIdx + mupnp_strlen(MUPNP_NET_URI_SLASH_DELIM);
  }
  else {
    idx = 0;
  }
#endif

  if (bufSize < 1)
    bufSize = mupnp_strlen(buf) + 1;

#if defined(MUPNP_HTTP_CURL)
  tmp = curl_unescape(buf + idx, 0);
  if (tmp == NULL)
    return NULL;
  mupnp_string_addvalue(retBuf, tmp);
  mupnp_log_debug_s("%s ==> %s\n", buf + idx, tmp);
  curl_free(tmp);
#else
  for (n = 0; n < bufSize;) {
    c = (unsigned char)buf[n];
    if (buf[n] == '%' && mupnp_net_uri_isalphanumchar(buf[n + 1]) && mupnp_net_uri_isalphanumchar(buf[n + 2])) {
      hexStr[0] = buf[n + 1];
      hexStr[1] = buf[n + 2];
      hexStr[2] = '\0';
      hex = strtol(hexStr, NULL, 16);
      c = (unsigned char)hex;
      n += 3;
    }
    else
      n++;
    mupnp_string_naddvalue(retBuf, (char*)&c, 1);
  }
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_string_getvalue(retBuf);
}

/****************************************
* mupnp_net_uri_getupnpbasepath
****************************************/

char* mupnp_net_uri_getupnpbasepath(mUpnpNetURI* locationURL)
{
  char *path, *c;
  ssize_t i;

  path = mupnp_strdup(mupnp_net_uri_getpath(locationURL));

  mupnp_log_debug_s("Mangling url string: %s\n", path);

  i = mupnp_strlen(path);

  if (0 >= i) {
    mupnp_log_debug("No base path, doing nothing.\n");
    return NULL;
  }

  /* Truncating out the "file name" from path */
  for (c = (path + --i); 0 <= i; c = (path + --i))
    if ('/' == *c) {
      *(path + i + 1) = '\0';
      mupnp_log_debug_s("Truncating string from place %d\n", i);
      break;
    }

  mupnp_log_debug_s("url string after mangling: %s\n", path);

  return path;
}

/****************************************
* mupnp_net_uri_isreservedchar
****************************************/

bool mupnp_net_uri_isreservedchar(char c)
{
  if (c == ';' || c == '/' || c == '?' || c == ':' || c == '@' || c == '&' || c == '=' || c == '+' || c == '$' || c == ',')
    return true;
  return false;
}

/****************************************
* mupnp_net_uri_isalphanumchar
****************************************/

bool mupnp_net_uri_isalphanumchar(char c)
{
  if ('a' <= c && c <= 'z')
    return true;
  if ('A' <= c && c <= 'Z')
    return true;
  if ('0' <= c && c <= '9')
    return true;
  return false;
}

/****************************************
* mupnp_net_uri_isunreservedchar
****************************************/

bool mupnp_net_uri_isunreservedchar(char c)
{
  if (mupnp_net_uri_isalphanumchar(c))
    return true;
  if (c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*' || c == '^' || c == '(' || c == ')')
    return true;
  return false;
}

/****************************************
* mupnp_net_uri_isescapedchar
****************************************/

bool mupnp_net_uri_isescapechar(char c)
{
  if (mupnp_net_uri_isreservedchar(c))
    return false;
  if (mupnp_net_uri_isunreservedchar(c))
    return false;
  return true;
}

/****************************************
* mupnp_net_uri_getquerydictionary
****************************************/

mUpnpDictionary* mupnp_net_uri_getquerydictionary(mUpnpNetURI* uri)
{
  char* query;
  size_t queryOffset;
  ssize_t eqIdx, ampIdx;
  mUpnpString* paramName;
  mUpnpString* paramValue;

  if (NULL == uri->queryDictionary)
    uri->queryDictionary = mupnp_dictionary_new();

  paramName = mupnp_string_new();
  paramValue = mupnp_string_new();

  query = mupnp_net_uri_getquery(uri);
  queryOffset = 0;

  eqIdx = mupnp_strstr(query, "=");
  while (0 < eqIdx) {
    ampIdx = mupnp_strstr(query + queryOffset, "&");
    if (ampIdx <= 0) {
      ampIdx = mupnp_strstr(query + queryOffset, "#");
      if (ampIdx <= 0)
        ampIdx = mupnp_strlen(query + queryOffset);
    }
    if (ampIdx <= eqIdx)
      break;
    mupnp_string_setnvalue(paramName, query + queryOffset, eqIdx);
    mupnp_string_setnvalue(paramValue, query + queryOffset + eqIdx + 1, (ampIdx - eqIdx - 1));
    mupnp_dictionary_setvalue(uri->queryDictionary, mupnp_string_getvalue(paramName), mupnp_string_getvalue(paramValue));
    queryOffset += ampIdx + 1;
    eqIdx = mupnp_strstr(query + queryOffset, "=");
  }

  mupnp_string_delete(paramName);
  mupnp_string_delete(paramValue);

  return uri->queryDictionary;
}

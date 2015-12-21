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

#ifndef _MUPNP_NET_CURI_H_
#define _MUPNP_NET_CURI_H_

#include <mupnp/typedef.h>

#include <mupnp/util/string.h>
#include <mupnp/util/dictionary.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_NET_URI_KNKOWN_PORT (-1)
#define MUPNP_NET_URI_DEFAULT_HTTP_PORT 80
#define MUPNP_NET_URI_DEFAULT_FTP_PORT 21
#define MUPNP_NET_URI_DEFAULT_PATH "/"
#define MUPNP_NET_URI_MAXLEN 256

#define MUPNP_NET_URI_PROTOCOL_DELIM "://"
#define MUPNP_NET_URI_USER_DELIM "@"
#define MUPNP_NET_URI_COLON_DELIM ":"
#define MUPNP_NET_URI_SLASH_DELIM "/"
#define MUPNP_NET_URI_SBLACET_DELIM "["
#define MUPNP_NET_URI_EBLACET_DELIM "]"
#define MUPNP_NET_URI_SHARP_DELIM "#"
#define MUPNP_NET_URI_QUESTION_DELIM "?"
#define MUPNP_NET_URI_ESCAPING_CHAR "%"
#define MUPNP_NET_URI_AMP_DELIM "&"
#define MUPNP_NET_URI_EQ_DELIM "="

#define MUPNP_NET_URI_PROTOCOL_HTTP "http"
#define MUPNP_NET_URI_PROTOCOL_FTP "ftp"

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpNetURI {
  mUpnpString* uri;
  mUpnpString* protocol;
  mUpnpString* user;
  mUpnpString* password;
  mUpnpString* host;
  int port;
  mUpnpString* path;
  mUpnpString* query;
  mUpnpString* fragment;
  mUpnpString* request;
  mUpnpDictionary* queryDictionary;
} mUpnpNetURI;

/****************************************
* Function
****************************************/

mUpnpNetURI* mupnp_net_uri_new();
void mupnp_net_uri_delete(mUpnpNetURI* uri);
void mupnp_net_uri_clear(mUpnpNetURI* uri);
void mupnp_net_uri_setvalue(mUpnpNetURI* uri, const char* value);
void mupnp_net_uri_rebuild(mUpnpNetURI* uri);
const char* mupnp_net_uri_getvalue(mUpnpNetURI* uri);

#define mupnp_net_uri_set(urip, value) mupnp_net_uri_setvalue(urip, value)

#define mupnp_net_uri_seturi(urip, value) mupnp_string_setvalue(urip->uri, value)
#define mupnp_net_uri_setprotocol(urip, value) mupnp_string_setvalue(urip->protocol, value)
#define mupnp_net_uri_setuser(urip, value) mupnp_string_setvalue(urip->user, value)
#define mupnp_net_uri_setpassword(urip, value) mupnp_string_setvalue(urip->password, value)
#define mupnp_net_uri_sethost(urip, value) mupnp_string_setvalue(urip->host, value)
#define mupnp_net_uri_setport(urip, value) (urip->port = value)
#define mupnp_net_uri_setpath(urip, value) mupnp_string_setvalue(urip->path, value)
#define mupnp_net_uri_addpath(urip, value) mupnp_string_addvalue(urip->path, value)
#define mupnp_net_uri_setquery(urip, value) mupnp_string_setvalue(urip->query, value)
#define mupnp_net_uri_setfragment(urip, value) mupnp_string_setvalue(urip->fragment, value)

#define mupnp_net_uri_geturi(urip) mupnp_string_getvalue(urip->uri)
#define mupnp_net_uri_getprotocol(urip) mupnp_string_getvalue(urip->protocol)
#define mupnp_net_uri_getuser(urip) mupnp_string_getvalue(urip->user)
#define mupnp_net_uri_getpassword(urip) mupnp_string_getvalue(urip->password)
#define mupnp_net_uri_gethost(urip) mupnp_string_getvalue(urip->host)
#define mupnp_net_uri_getport(urip) (urip->port)
#define mupnp_net_uri_getpath(urip) mupnp_string_getvalue(urip->path)
#define mupnp_net_uri_getquery(urip) mupnp_string_getvalue(urip->query)
#define mupnp_net_uri_getfragment(urip) mupnp_string_getvalue(urip->fragment)

char* mupnp_net_uri_getrequest(mUpnpNetURI* uri);
char* mupnp_net_uri_getupnpbasepath(mUpnpNetURI* locationURL);

#define mupnp_net_uri_hasuri(urip) ((0 < mupnp_string_length(urip->uri)) ? true : false)
#define mupnp_net_uri_hasprotocol(urip) ((0 < mupnp_string_length(urip->protocol)) ? true : false)
#define mupnp_net_uri_hasuser(urip) ((0 < mupnp_string_length(urip->user)) ? true : false)
#define mupnp_net_uri_haspassword(urip) ((0 < mupnp_string_length(urip->password)) ? true : false)
#define mupnp_net_uri_hashost(urip) ((0 < mupnp_string_length(urip->host)) ? true : false)
#define mupnp_net_uri_hasport(urip) ((0 < urip->port) ? true : false)
#define mupnp_net_uri_haspath(urip) ((0 < mupnp_string_length(urip->path)) ? true : false)
#define mupnp_net_uri_hasquery(urip) ((0 < mupnp_string_length(urip->query)) ? true : false)
#define mupnp_net_uri_hasfragment(urip) ((0 < mupnp_string_length(urip->fragment)) ? true : false)
#define mupnp_net_uri_hasrequest(urip) mupnp_net_uri_haspath(urip)

#define mupnp_net_uri_ishttpprotocol(urip) mupnp_streq(mupnp_string_getvalue(urip->protocol), MUPNP_NET_URI_PROTOCOL_HTTP)

#define mupnp_net_uri_isabsolute(urip) mupnp_net_uri_hasprotocol(urip)
#define mupnp_net_uri_isabsolutepath(uripath) ((*uripath == '/') ? true : false)
#define mupnp_net_uri_isrelative(urip) ((mupnp_net_uri_hasprotocol(urip) == true) ? false : true)

bool mupnp_net_uri_isreservedchar(char c);
bool mupnp_net_uri_isunreservedchar(char c);
bool mupnp_net_uri_isescapechar(char c);
bool mupnp_net_uri_isalphanumchar(char c);

bool mupnp_net_uri_isescapedstring(char* buf, size_t bufSize);
char* mupnp_net_uri_escapestring(char* buf, size_t bufSize, mUpnpString* retBuf);
char* mupnp_net_uri_unescapestring(char* buf, size_t bufSize, mUpnpString* retBuf);
bool mupnp_net_uri_isequivalent(const char* url, const char* relative_url);

mUpnpDictionary* mupnp_net_uri_getquerydictionary(mUpnpNetURI* uri);

#ifdef __cplusplus
}
#endif

#endif

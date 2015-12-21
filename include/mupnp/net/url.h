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

#ifndef _MUPNP_NET_CURL_H_
#define _MUPNP_NET_CURL_H_

#include <mupnp/typedef.h>

#include <mupnp/util/string.h>
#include <mupnp/net/uri.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpNetURI mUpnpNetURL;

/****************************************
* Function
****************************************/

#define mupnp_net_url_new() mupnp_net_uri_new()
#define mupnp_net_url_delete(urip) mupnp_net_uri_delete(urip);
#define mupnp_net_url_clear(urip) mupnp_net_uri_clear(urip);

#define mupnp_net_url_set(urip, value) mupnp_net_uri_set(urip, value)
#define mupnp_net_url_setvalue(urip, value) mupnp_net_uri_setvalue(urip, value)
#define mupnp_net_url_rebuild(urip) mupnp_net_uri_rebuild(urip)
#define mupnp_net_url_getvalue(urip) mupnp_net_uri_getvalue(urip)

#define mupnp_net_url_seturi(urip, value) mupnp_string_setvalue(urip->uri, value)
#define mupnp_net_url_setprotocol(urip, value) mupnp_string_setvalue(urip->protocol, value)
#define mupnp_net_url_setuser(urip, value) mupnp_string_setvalue(urip->user, value)
#define mupnp_net_url_setpassword(urip, value) mupnp_string_setvalue(urip->password, value)
#define mupnp_net_url_sethost(urip, value) mupnp_string_setvalue(urip->host, value)
#define mupnp_net_url_setport(urip, value) (urip->port = value)
#define mupnp_net_url_setpath(urip, value) mupnp_string_setvalue(urip->path, value)
#define mupnp_net_url_addpath(urip, value) mupnp_net_uri_addpath(urip, value)
#define mupnp_net_url_setquery(urip, value) mupnp_string_setvalue(urip->query, value)
#define mupnp_net_url_setfragment(urip, value) mupnp_string_setvalue(urip->fragment, value)

#define mupnp_net_url_geturi(urip) mupnp_string_getvalue(urip->uri)
#define mupnp_net_url_getprotocol(urip) mupnp_string_getvalue(urip->protocol)
#define mupnp_net_url_getuser(urip) mupnp_string_getvalue(urip->user)
#define mupnp_net_url_getpassword(urip) mupnp_string_getvalue(urip->password)
#define mupnp_net_url_gethost(urip) mupnp_string_getvalue(urip->host)
#define mupnp_net_url_getport(urip) (urip->port)
#define mupnp_net_url_getpath(urip) mupnp_string_getvalue(urip->path)
#define mupnp_net_url_getquery(urip) mupnp_string_getvalue(urip->query)
#define mupnp_net_url_getfragment(urip) mupnp_string_getvalue(urip->fragment)
#define mupnp_net_url_getrequest(urip) mupnp_net_uri_getrequest(urip)

#define mupnp_net_url_hasuri(urip) ((0 < mupnp_string_length(urip->uri)) ? true : false)
#define mupnp_net_url_hasprotocol(urip) ((0 < mupnp_string_length(urip->protocol)) ? true : false)
#define mupnp_net_url_hasuser(urip) ((0 < mupnp_string_length(urip->user)) ? true : false)
#define mupnp_net_url_haspassword(urip) ((0 < mupnp_string_length(urip->password)) ? true : false)
#define mupnp_net_url_hashost(urip) ((0 < mupnp_string_length(urip->host)) ? true : false)
#define mupnp_net_url_hasport(urip) ((0 < urip->port) ? true : false)
#define mupnp_net_url_haspath(urip) ((0 < mupnp_string_length(urip->path)) ? true : false)
#define mupnp_net_url_hasquery(urip) ((0 < mupnp_string_length(urip->query)) ? true : false)
#define mupnp_net_url_hasfragment(urip) ((0 < mupnp_string_length(urip->fragment)) ? true : false)
#define mupnp_net_url_hasrequest(urip) mupnp_net_uri_hasrequest(urip)

#define mupnp_net_url_ishttpprotocol(urip) mupnp_streq(mupnp_string_getvalue(urip->protocol), MUPNP_NET_URI_PROTOCOL_HTTP)

#define mupnp_net_url_isabsolute(uripath) mupnp_net_uri_hasprotocol(uripath)
#define mupnp_net_url_isrelative(urip) ((mupnp_net_uri_hasprotocol(urip) == true) ? false : true)
#define mupnp_net_url_isabsolutepath(uripath) mupnp_net_uri_isabsolutepath(uripath)

#define mupnp_net_url_getupnpbasepath(urip) mupnp_net_uri_getupnpbasepath(urip);

const char* mupnp_net_gethosturl(const char* host, int port, const char* uri, char* buf, size_t bufSize);
const char* mupnp_net_getmodifierhosturl(const char* host, int port, const char* uri, const char* begin, const char* end, char* buf, size_t bufSize);

#define mupnp_net_url_getquerydictionary(urip) mupnp_net_uri_getquerydictionary(urip);

#ifdef __cplusplus
}
#endif

#endif
